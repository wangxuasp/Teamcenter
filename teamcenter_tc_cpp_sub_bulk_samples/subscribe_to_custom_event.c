/*HEAD SUBSCRIBE_TO_CUSTOM_EVENT.CXX CCC ITK */
/*
     %TC_ROOT%\sample\compile -DIPLIB=none subscribe_to_custom_event.cxx
     %TC_ROOT%\sample\linkitk -o subscribe_to_custom_event subscribe_to_custom_event.obj
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdarg.h>

#include <tc/emh.h>
#include <tccore/item.h>
#include <sub_mgr/standardtceventtypes.h>
#include <sub_mgr/subscription.h>
#include <tc/tc.h>
#include <sub_mgr/tcactionhandler.h>
#include <sub_mgr/tceventmgr.h>
#include <sub_mgr/tceventtype.h>
#include <tccore/tctype.h>

static void ECHO(char *format, ...)
{
    char msg[1000];
    va_list args;
    va_start(args, format);
    vsprintf(msg, format, args);
    va_end(args);
    printf(msg);
    TC_write_syslog(msg);
}

#define IFERR_ABORT(X)  (report_error( __FILE__, __LINE__, #X, X, TRUE))
#define IFERR_REPORT(X) (report_error( __FILE__, __LINE__, #X, X, FALSE))

static int report_error(char *file, int line, char *call, int status,
    logical exit_on_error)
{
    if (status != ITK_ok)
    {
        int
            n_errors = 0;
        const int
            *severities = NULL,
            *statuses = NULL;
        const char
            **messages;

        EMH_ask_errors(&n_errors, &severities, &statuses, &messages);
        if (n_errors > 0)
        {
            ECHO("\n%s\n", messages[n_errors-1]);
            EMH_clear_errors();
        }
        else
        {
            char *error_message_string;
            EMH_get_error_string (NULLTAG, status, &error_message_string);
            ECHO("\n%s\n", error_message_string);
        }

        ECHO("error %d at line %d in %s\n", status, line, file);
        ECHO("%s\n", call);
        if (exit_on_error)
        {
            ECHO("%s", "Exiting program!\n");
            exit (status);
        }
    }
    return status;
}

static void do_it(void)
{
    tag_t event_type = NULLTAG;
    IFERR_ABORT(TCEVENTTYPE_find("CustomEventName", &event_type));
    if (event_type == NULLTAG)
    {
        ECHO("CustomEventName not found!\n Exiting Program\n");
        return;
    }

    tag_t handler = NULLTAG;
    IFERR_ABORT(TCACTIONHANDLER_find_handler("CustomHanderId", &handler));
    if (handler == NULLTAG)
    {
        ECHO("CustomHanderId not found!\n Exiting Program\n");
        return;
    }

    char *user_name_string = NULL;
    tag_t user = NULLTAG;;
    IFERR_ABORT(POM_get_user(&user_name_string, &user));

    tag_t item = NULLTAG;
    tag_t rev = NULLTAG;
    IFERR_ABORT(ITEM_create_item("", "Subscribe Test", "G9Item", "", &item, &rev));

    tag_t subscription = NULLTAG;
    IFERR_REPORT(SCM_find_subscription( item, user, event_type, handler, &subscription));
    if (subscription != NULLTAG)
    {
        ECHO("Object Subscription already exists!");
    }
    else
    {
        IFERR_REPORT(SCM_subscribe(item, user, event_type, handler, NULLDATE, NULLDATE, 0, NULL, &subscription));
        ECHO("Created new object subscription!\n");
    }

    int ifail = ITK_ok;
    IFERR_REPORT(ifail = TCEVENTMGR_post_event(item, "CustomEventName"));
    if (ifail == ITK_ok) ECHO("\n\n\t EVENT POSTED! \n\n");
    if (ifail != ITK_ok) ECHO("\n\n\t Something is wrong!! \n\n");
}

int ITK_user_main(int argc, char* argv[])
{
    IFERR_REPORT(ITK_initialize_text_services(ITK_BATCH_TEXT_MODE));
    IFERR_ABORT(ITK_auto_login());
    IFERR_REPORT(ITK_set_journalling(TRUE));

    do_it();

    IFERR_REPORT(ITK_exit_module(FALSE));

    return ITK_ok;
}

#ifdef __cplusplus
}
#endif
