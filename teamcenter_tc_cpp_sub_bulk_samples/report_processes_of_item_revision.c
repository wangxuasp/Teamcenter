/*
    If you set EPM_multiple_processes_targets = ON, an object can be a target
    of multiple workflow processes at once.
*/

#include <tc/emh.h>
#include <tc/tc.h>
#include <tccore/item.h>
#include <epm/cr.h>
#include <tccore/aom.h>
#include <epm/epm.h>
#include <tccore/tctype.h>
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
*************************************************
* WARNING - ITEM_find_rev is deprecated in
* Teamcenter version Teamcenter 8.1
* and no longer supported in Teamcenter 10
* Replace this call to ITEM_find_rev with ITEM_find_item_revs_by_key_attributes
*************************************************

    So for everyone's convenience - here is a wrapper replacement
*/

#define ITEM_find_rev GTAC_find_rev

static int GTAC_find_rev(char *item_id, char *rev_id, tag_t *rev)
{
    int
        n = 0;
    tag_t
        *items;
    const char
        *names[1] = { "item_id" },
        *values[1] = { item_id };

    IFERR_REPORT(ITEM_find_item_revs_by_key_attributes(1, names, values, rev_id,
        &n, &items));
    if (n > 0) *rev = items[0];
    if (items) MEM_free(items);

    return 0;
}

static tag_t prompt_for_existing_item_revision(void)
{
    tag_t
        rev = NULLTAG;
    char
        item_id[ITEM_id_size_c +1],
        rev_id[ITEM_id_size_c + 1];

    do
    {
        printf("\nEnter Item ID: ");
        gets(item_id);
        if (!strcmp(item_id, "")) return NULLTAG;

        printf("Enter %s Rev: ", item_id);
        gets(rev_id);
        if (!strcmp(rev_id, "")) return NULLTAG;

        ECHO("Searching for Item ID %s with Rev ID %s...\n", item_id, rev_id);
        IFERR_REPORT(ITEM_find_rev(item_id, rev_id, &rev));
        if (rev == NULL_TAG)
            ECHO("ItemRev %s/%s was NOT found!\n", item_id, rev_id);
        else
            return rev;

    } while (TRUE);

}

static void report_epm_state(EPM_state_t state)
{
    switch (state)
    {
        case EPM_unassigned:
            ECHO("EPM_unassigned");
        break;
        case EPM_pending:
            ECHO("EPM_pending");
        break;
        case EPM_started:
            ECHO("EPM_started");
        break;
        case EPM_completed:
            ECHO("EPM_completed");
        break;
        case EPM_skipped:
            ECHO("EPM_skipped");
        break;
        case EPM_aborted:
            ECHO("EPM_aborted");
        break;
        case EPM_suspended:
            ECHO("EPM_suspended");
        break;
    }
}

static void report_task_state(tag_t task)
{
    char
        task_name[WSO_name_size_c+1];
    EPM_state_t
        state;

    IFERR_RETURN(EPM_ask_name(task, task_name));
    IFERR_REPORT(EPM_ask_state(task, &state));
    ECHO("%s is ", task_name);
    report_epm_state(state);
    ECHO("\n");
}

static void report_referencing_jobs(tag_t object)
{
    int
        cnt = 0,
        ii,
        n_levels,
        n_instances,
        *instance_levels,
        *instance_where_found,
        n_classes,
        *class_levels,
        *class_where_found;
    tag_t
        *ref_instances,
        *ref_classes,
        type = NULLTAG;
    char
        *class_name,
        *name,
        type_name[TCTYPE_name_size_c+1] = "";

    IFERR_REPORT(POM_referencers_of_instance(object, 1, POM_in_ds_and_db,
        &n_instances, &ref_instances, &instance_levels,
        &instance_where_found, &n_classes, &ref_classes, &class_levels,
        &class_where_found));

    if (n_instances > 0)
    {
        for (ii = 0; ii < n_instances; ii++)
        {
            IFERR_REPORT(AOM_ask_name(ref_instances[ii], &name));
            IFERR_REPORT(TCTYPE_ask_object_type(ref_instances[ii], &type));
            IFERR_REPORT(TCTYPE_ask_name(type, type_name));
            if (!strcmp(type_name, "EPMTask"))
            {
                ECHO("%d.  ", ++cnt);
                report_task_state(ref_instances[ii]);
            }

            if (name) MEM_free(name);
        }
        MEM_free(ref_instances);
        MEM_free(instance_levels);
        MEM_free(instance_where_found);
    }

    if (n_classes > 0)
    {
        MEM_free(ref_classes);
        MEM_free(class_levels);
        MEM_free(class_where_found);
    }

}

static void do_it(void)
{
    int
        ii,
        n_jobs = 0;
    tag_t
        itemRev = prompt_for_existing_item_revision(),
        *jobs = NULL,
        root_task = NULLTAG;

    while (itemRev != NULLTAG)
    {
        IFERR_ABORT(AOM_refresh(itemRev, FALSE));

    /*  This only returns 0 or 1 - see PR 6438851 */
        ECHO("Using CR_ask_job:\n");
        IFERR_ABORT(CR_ask_job(itemRev, &n_jobs, &jobs));
        ECHO("n_jobs = %d\n", n_jobs);
        for (ii = 0; ii < n_jobs; ii++)
        {
            ECHO("%d.  ", ii+1);
            IFERR_REPORT(EPM_ask_root_task(jobs[ii], &root_task));
            if (root_task != NULLTAG) report_task_state(root_task);
        }
        if (n_jobs > 0) MEM_free(jobs);

        ECHO("\nUsing POM_referencers_of_instance:\n");
        report_referencing_jobs(itemRev);

        itemRev = prompt_for_existing_item_revision();
    }

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
