/*HEAD SET_USER_STATUS CCC ITK */
/*
     %TC_ROOT%\sample\compile -DIPLIB=none set_user_status.c
     %TC_ROOT%\sample\linkitk -o set_user_status set_user_status.obj
*/
#include <tccore/aom.h>
#include <sa/user.h>
#include <sa/person.h>
#include <tc/emh.h>
#include <tc/tc.h>
#include <stdarg.h>

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
            n_errors = 0,
            *severities = NULL,
            *statuses = NULL;
        char
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

#define ACTIVE 0
#define INACTIVE 1

static void do_it(void)
{
    int 
        status = 0, 
        new_status = 0;
    tag_t 
        user = NULLTAG;
    char 
        user_id[SA_user_size_c+1] = "smith",
        status_string[2][10] = {"active", "inactive"},
        person_name[SA_person_name_size_c+1] = "";

    IFERR_REPORT(SA_find_user(user_id, &user));
    IFERR_REPORT(AOM_load(user));
    IFERR_REPORT(SA_ask_user_person_name(user, person_name)); 
    IFERR_REPORT(SA_get_user_status(user, &status));
    ECHO("Person Name: %s - %s\n", person_name, status_string[status]);

    /* reverse it */
    if (status == ACTIVE) new_status = INACTIVE;
    if (status == INACTIVE) new_status = ACTIVE;

    IFERR_REPORT(AOM_lock(user));
    IFERR_REPORT(SA_set_user_status(user, new_status));
    IFERR_REPORT(AOM_save(user));
    IFERR_REPORT(AOM_unlock(user));
    IFERR_REPORT(AOM_unload(user));
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

