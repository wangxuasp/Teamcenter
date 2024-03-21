#include <stdlib.h>
#include <tccore/aom.h>
#include <tc/emh.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <epm/epm.h>

#define ECHO(X)  printf X; TC_write_syslog X

#define IFERR_ABORT(X)  (report_error( __FILE__, __LINE__, #X, X, TRUE))
#define IFERR_REPORT(X) (report_error( __FILE__, __LINE__, #X, X, FALSE))
#define IFERR_RETURN(X) if (IFERR_REPORT(X)) return

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

        EMH_ask_errors( &n_errors, &severities, &statuses, &messages );
        if (n_errors > 0)
        {
            ECHO(("\n%s\n", messages[n_errors-1]));
            EMH_clear_errors();
        }
        else
        {
            char *error_message_string;
            EMH_get_error_string (NULLTAG, status, &error_message_string);
            ECHO(("\n%s\n", error_message_string));
        }

        ECHO(("error %d at line %d in %s\n", status, line, file));
        ECHO(("%s\n", call));

        if (exit_on_error)
        {
            ECHO(("\nExiting program!\n"));
            exit (status);
        }
    }

    return status;
}

#define INDENT(X)  { int yy; for (yy = 0; yy < X; yy++) ECHO(("  ")); }

static void report_epm_state(EPM_state_t state)
{
    switch (state)
    {
        case EPM_unassigned:
            ECHO(("EPM_unassigned"));
        break;
        case EPM_pending:
            ECHO(("EPM_pending"));
        break;
        case EPM_started:
            ECHO(("EPM_started"));
        break;
        case EPM_completed:
            ECHO(("EPM_completed"));
        break;
        case EPM_skipped:
            ECHO(("EPM_skipped"));
        break;
        case EPM_aborted:
            ECHO(("EPM_aborted"));
        break;
        case EPM_suspended:
            ECHO(("EPM_suspended"));
        break;
    }
}

static void report_task_and_subtasks(tag_t task, int indent)
{
    int
        ii,
        n_subtasks;
    tag_t
        *subtasks = NULL;
    char
        task_name[WSO_name_size_c+1];
    EPM_state_t
        state;

    IFERR_RETURN(EPM_ask_name(task, task_name));
    IFERR_REPORT(EPM_ask_state(task, &state));
    INDENT(indent);
    ECHO(("%s is ", task_name));
    report_epm_state(state);

    IFERR_RETURN(EPM_ask_sub_tasks(task, &n_subtasks, &subtasks));
    ECHO((" and has %d subtasks\n", n_subtasks));
    for (ii = 0; ii < n_subtasks; ii++)
        report_task_and_subtasks(subtasks[ii], indent + 1);
    if (n_subtasks > 0) MEM_free(subtasks);
}

static void do_it(void)
{
    int
        ii,
        n_hits = 0;
    tag_t
        *hits = NULL,
        root_task = NULLTAG;
    char
        process_name[WSO_name_size_c +1];

    printf("Enter Process Name: ");
    gets(process_name);

    IFERR_ABORT(WSOM_find(process_name, &n_hits, &hits));
    ECHO(("n_hits = %d\n", n_hits));
    for (ii = 0; ii < n_hits; ii++)
    {
        IFERR_ABORT(EPM_ask_root_task(hits[ii], &root_task));
        if (root_task != NULLTAG) report_task_and_subtasks(root_task, 0);
    }
    if (n_hits > 0) MEM_free(hits);
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
