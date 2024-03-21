#include <tc/emh.h>
#include <tc/tc.h>
#include <stdarg.h>
#include <epm/aliaslist.h>
#include <epm/distributionlist.h>

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

/*
    Tested in:
        V10.0.3.8 on 7/15/10
        TC8.1     on 7/15/10

    Note: After creating an "Address List" named "6380411", this code prints:
        a_list = 33840  (valid tag)
        d_list = 0      (NULLTAG)

    So if you're trying to find an Address List use MAIL_find_alias_list.
    If you're not sure what you have, try this example with the name of
    your list to see which returns non-NULLTAG.

*/
static void do_it(void)
{
    tag_t
        a_list = NULLTAG,
        d_list = NULLTAG;

    IFERR_REPORT(MAIL_find_alias_list("6380411", &a_list));
    ECHO("a_list = %d\n", a_list);

    IFERR_REPORT(MAIL_find_distribution_list("6380411", &d_list));
    ECHO("d_list = %d\n", d_list);
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
