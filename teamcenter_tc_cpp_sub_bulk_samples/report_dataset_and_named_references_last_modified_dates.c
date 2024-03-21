/*HEAD LIST_ALL_DATASET_NAMED_REFERENCES CCC UFUN */
#include <stdlib.h>
#include <tc/tc.h>
#include <tccore/workspaceobject.h>
#include <ae/ae.h>
#include <user_exits/user_exits.h>
#include <tccore/aom.h>

#define ECHO(X)  printf X; TC_write_syslog X

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

static void report_object_modified_date(tag_t object)
{
    date_t
        the_date;
    char
        *last_modified;

    if (IFERR_REPORT(AOM_ask_modified_date(object, &the_date))) return;
    if (IFERR_REPORT(ITK_date_to_string(the_date, &last_modified))) return;
    if (last_modified)
    {
        ECHO((" last modified = %s\n", last_modified));
        MEM_free(last_modified);
    }
}

static void report_dataset_named_references(tag_t dataset)
{
    int
        ii,
        reference_count = 0;
    tag_t
        reference_object;
    AE_reference_type_t
        reference_type;
    char
        reference_name[AE_reference_size_c + 1];

/*  Make sure no one messes with it while we're reading it */
    IFERR_ABORT(AOM_refresh(dataset, TRUE));

    IFERR_ABORT(AE_ask_dataset_ref_count(dataset, &reference_count));
    for (ii = 0; ii < reference_count; ii++)
    {
        IFERR_REPORT(AE_find_dataset_named_ref(dataset, ii,
            reference_name, &reference_type, &reference_object));
        ECHO(("%d.  %s - ", ii+1, reference_name));
        report_object_modified_date(reference_object);
    }

    IFERR_REPORT(AOM_unload(dataset));
}

static void do_it(void)
{
    tag_t
        dataset = NULLTAG;
    char
        dataset_name[WSO_name_size_c +1];

    do
    {
        printf("Enter Dataset Name: ");
        gets(dataset_name);
        if (!strcmp(dataset_name, "")) return;

        /* AE_find_dataset finds latest revision of dataset */
        IFERR_REPORT(AE_find_dataset(dataset_name, &dataset));
        if (dataset == NULLTAG) continue;

        ECHO(("%s", dataset_name));
        report_object_modified_date(dataset);

        report_dataset_named_references(dataset);
    } while (TRUE);

}

int ITK_user_main(int argc, char* argv[])
{
    IFERR_REPORT(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
    IFERR_REPORT(ITK_auto_login());
    IFERR_REPORT(ITK_set_journalling(TRUE));
    do_it();
    ITK_exit_module(TRUE);
    return ITK_ok;
}
