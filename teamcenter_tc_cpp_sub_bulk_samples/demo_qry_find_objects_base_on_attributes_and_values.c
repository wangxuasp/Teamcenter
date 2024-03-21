#include <tc/emh.h>
#include <tc/tc.h>
#include <stdarg.h>
#include <qry/qry.h>
#include <tccore/item.h>

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

static void do_it(void)
{
    int
        ii,
        n = 0;
    char
        item_name[ITEM_name_size_c+1],
        *names[1] = { "object_name" },
        *values[1] = { "6*" };
    tag_t
        *results = NULL;

    IFERR_REPORT(QRY_find_objects_based_on_attributes_and_values(
        "Item", 1, names, NULL, values, NULL, &n, &results));
    ECHO("n = %d\n", n);
    for (ii = 0; ii < n; ii++)
    {
        IFERR_REPORT(ITEM_ask_name(results[ii], item_name));
        ECHO("results[%d] = %s (%d)\n", ii, item_name, results[ii]);
    }
    if (n > 0) MEM_free(results);

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
