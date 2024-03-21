/*HEAD GET_PROBLEM_OF_AFFECTED_ITEMS CCC ITK */
#include <tc/emh.h>
#include <tc/tc.h>
#include <stdarg.h>
#include <ecm/ecm.h>
#include <tccore/workspaceobject.h>

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

static void GTAC_free(void *what)
{
    if (what != NULL)
    {
        MEM_free(what);
        what = NULL;
    }
}

static void do_it(void)
{
    int
        ii = 0,
        n_affected_revs = 0,
        n_problem;
    tag_t
        ec_rev = NULLTAG,
        *affected_revs = NULL,
        problem_rev = NULLTAG;
    char
        *object_id = NULL;

    IFERR_ABORT( ECM_find_ec_revision("EC00002", "B", &ec_rev));
    ECHO("ec_rev: %d\n", ec_rev);
    if (ec_rev == NULLTAG) return;

    IFERR_ABORT(ECM_get_affected_items(ec_rev, &n_affected_revs, &affected_revs ));
    for (ii = 0; ii < n_affected_revs; ii++)
    {
        IFERR_REPORT(WSOM_ask_object_id_string(affected_revs[ii], &object_id));
        ECHO("    affected: %s\n", object_id);
        GTAC_free(object_id);

        IFERR_ABORT(ECM_get_problem_of_affected(ec_rev, affected_revs[ii], &problem_rev));
        if (problem != NULLTAG)
        {
            IFERR_REPORT(WSOM_ask_object_id_string(problem_rev, &object_id));
            ECHO("    problem: %s\n", object_id);
            GTAC_free(object_id);
        }
        else
            ECHO("    missing problem (NULLTAG)\n");
    }
    GTAC_free(affected_revs);
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
