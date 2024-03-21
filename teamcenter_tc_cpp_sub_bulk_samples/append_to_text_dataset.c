#include <stdlib.h>
#include <time.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <user_exits/user_exits.h>
#include <ae/ae.h>
#include <tccore/aom.h>
#include <ss/ss_const.h>

#define ECHO(X)  printf X; TC_write_syslog X

#define IFERR_ABORT(X)  (report_error( __FILE__, __LINE__, #X, X, TRUE))
#define IFERR_REPORT(X) (report_error( __FILE__, __LINE__, #X, X, FALSE))

static int report_error(char *file, int line, char *call, int status,
    logical exit_on_error)
{
    if (status != ITK_ok)
    {
        int
            n_errors,
            *severities,
            *statuses;
        char
            **messages;

        EMH_ask_errors( &n_errors, &severities, &statuses, &messages );
        ECHO(("\n%s\n", messages[n_errors-1]));
        EMH_clear_errors();

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

static void do_it(void)
{
    char
        dataset_name[WSO_name_size_c +1],
        new_text[SS_MAXLLEN + 1];
    tag_t
        dataset = NULLTAG,
        new_text_file = NULL_TAG,
        text_file = NULLTAG;
    IMF_file_t
        file_descriptor = NULL;
    time_t
        the_time;
    struct tm
        *time_ptr;
    AE_reference_type_t
        ref_type;

    do
    {
        printf("Enter Dataset Name: ");
        gets(dataset_name);
        if (!strcmp(dataset_name, "")) return;

        IFERR_ABORT(AE_find_dataset(dataset_name, &dataset));
        if (dataset == NULLTAG) continue;
        IFERR_ABORT(AOM_refresh(dataset, TRUE));

        IFERR_ABORT(AE_ask_dataset_named_ref(dataset, "Text",
            &ref_type, &text_file));
        if (text_file == NULLTAG) continue;
        IFERR_ABORT(AOM_refresh(text_file, TRUE));

        IFERR_REPORT(IMF_export_file(text_file, "C:\\TEMP\\before.txt"));
        ECHO(("exported copy C:\\TEMP\\before.txt\n"));

        the_time = time((time_t *)0);
        time_ptr = localtime (&the_time);
        strftime(new_text, SS_MAXLLEN*sizeof(char), "%d %b %Y %H:%M:%S",
            time_ptr);
        ECHO(("appending new_text = %s\n", new_text));

        IFERR_ABORT(IMF_ask_file_descriptor(text_file, &file_descriptor));
        IFERR_ABORT(IMF_open_file(file_descriptor, SS_APPEND));
        IFERR_ABORT(IMF_write_file_line(file_descriptor, new_text));
        IFERR_ABORT(IMF_close_file(file_descriptor));
        IFERR_ABORT(AOM_save(text_file));

        IFERR_REPORT(IMF_export_file(text_file, "C:\\TEMP\\after.txt"));
        ECHO(("exported copy C:\\TEMP\\after.txt\n"));

/*  In order for the changes to be seen in the RAC, a new tag needs to be
    generated.  See PR 1666280.
*/
        IFERR_REPORT(IMF_replace_file_and_get_new_tag(text_file,
            "C:\\TEMP\\after.txt", TRUE, &new_text_file));
        IFERR_ABORT(AOM_unload(text_file));

        IFERR_REPORT(AE_replace_dataset_named_ref(dataset, text_file,
            "Text", ref_type, new_text_file));
        IFERR_REPORT(AE_save_myself(dataset));
        IFERR_ABORT(AOM_unload(dataset));

        IFERR_REPORT(AOM_lock_for_delete(text_file));
        IFERR_REPORT(AOM_delete(text_file));

        printf("\n");

    } while (TRUE);

}

int ITK_user_main(int argc, char* argv[])
{
    IFERR_REPORT(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
    IFERR_ABORT(ITK_auto_login());
    IFERR_REPORT(ITK_set_journalling(TRUE));
    do_it();
    IFERR_REPORT(ITK_exit_module(FALSE));
    return ITK_ok;
}
