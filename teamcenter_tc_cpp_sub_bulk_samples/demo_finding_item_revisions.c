#include <stdlib.h>
#include <tc/emh.h>
#include <tc/tc.h>
#include <tccore/item.h>

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

static void do_it(void)
{
    logical
        result = FALSE;
    tag_t
        part_rev = NULLTAG;
    char
        item_id[ITEM_id_size_c +1],
        rev_id[ITEM_id_size_c + 1];

    do
    {
        printf("Enter Item ID: ");
        gets(item_id);
        if (!strcmp(item_id, "")) return;

        printf("Enter %s Rev: ", item_id);
        gets(rev_id);
        if (!strcmp(rev_id, "")) return;

        part_rev = NULLTAG;
        ECHO("Searching for Item ID %s with Rev ID %s...\n", item_id, rev_id);
        IFERR_REPORT(ITEM_find_rev(item_id, rev_id, &part_rev));
        if (part_rev == NULL_TAG)
        {
            ECHO("ItemRev %s/%s was NOT found!\n", item_id, rev_id);
        }
        else
        {
            ECHO("ItemRev %s/%s was found\n", item_id, rev_id);
        }

    } while (TRUE);
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
