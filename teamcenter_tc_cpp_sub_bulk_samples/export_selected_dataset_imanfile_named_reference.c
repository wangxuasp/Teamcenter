#include <stdlib.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/workspaceobject.h>
#include <ae/ae.h>
#include <user_exits/user_exits.h>
#include <property/prop.h>
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

static void free_string_array(int n_elements, char **string_array)
{
    int ii;

    for (ii = 0; ii < n_elements; ii++) MEM_free(string_array[ii]);

    MEM_free(string_array);
}

static char **allocate_string_array(int n_elements, int n_chars)
{
    int ii;
    char **where = MEM_alloc(n_elements * sizeof(char *));

    for (ii = 0; ii < n_elements; ii++)
        where[ii] = MEM_alloc(n_chars * sizeof(char));

    return where;
}

static void ask_property_value_by_name(tag_t object, char *prop_name,
    char **prop_value)
{
    tag_t
        prop_tag = NULLTAG;

    IFERR_ABORT(PROP_UIF_ask_property_by_name(object, prop_name, &prop_tag));
    IFERR_ABORT(PROP_UIF_ask_value(prop_tag, prop_value));
}

static int ask_dataset_imanfile_named_references(tag_t dataset,
    char ***reference_names, tag_t **reference_objects)
{
    int
        ii,
        imf_count = 0,
        reference_count = 0;
    tag_t
        prop_tag = NULLTAG,
        reference_object = NULLTAG;
    char
        reference_name[AE_reference_size_c + 1],
        *type;
    AE_reference_type_t
        reference_type;

/*  Make sure no one messes with it while we're reading it */
    IFERR_ABORT(AOM_refresh(dataset, TRUE));

    IFERR_ABORT(AE_ask_dataset_ref_count(dataset, &reference_count));
    if (reference_count > 0)
    {
        *reference_names = allocate_string_array(reference_count,
            AE_reference_size_c + 1);
        *reference_objects = MEM_alloc(reference_count * sizeof(tag_t));

        for (ii = 0; ii < reference_count; ii++)
        {
            IFERR_ABORT(AE_find_dataset_named_ref(dataset, ii,
                reference_name, &reference_type, &reference_object));
            ask_property_value_by_name(reference_object, "Type", &type);
            if (!strcmp(type, "ImanFile"))
            {
                strcpy((*reference_names)[imf_count], reference_name);
                (*reference_objects)[imf_count] = reference_object;
                imf_count++;
            }
            if (type != NULL) MEM_free(type);
        }
    }

    IFERR_ABORT(AOM_unlock(dataset));

    return imf_count;
}

static void report_imanfile_references(int n, char **reference_names,
    tag_t *reference_objects)
{
    int ii;
    char name[IMF_filename_size_c + 1];

    for (ii = 0; ii < n; ii++)
    {
        IFERR_ABORT(IMF_ask_original_file_name(reference_objects[ii], name));
        printf("%d.  %s - %s\n", ii+1, reference_names[ii], name);
    }
}

static void build_similar_temp_filespec(char *filespec, char *tempspec)
{
    char *temp_dir = getenv("TC_TMP_DIR");

    if (SS_MACHINE_TYPE == SS_WNT_MACHINE)
        sprintf(tempspec, "%s\\%s", temp_dir, filespec);
    else
        sprintf(tempspec, "%s/%s", temp_dir, filespec);
}

static int ask_file_format(char *path)
{
    IMF_file_status
        stats;

    IFERR_ABORT(IMF_stat(path, &stats));

    return stats.fmt;
}

static void export_backup_imanfile(tag_t imanfile)
{
    char
        backup[SS_MAXPATHLEN],
        name[IMF_filename_size_c + 1],
        *type;

    IFERR_ABORT(AOM_refresh(imanfile, TRUE));

    ask_property_value_by_name(imanfile, "Type", &type);
    if (!strcmp(type, "ImanFile"))
    {
        IFERR_ABORT(IMF_ask_original_file_name(imanfile, name));
        build_similar_temp_filespec(name, backup);
        unlink(backup);
        IFERR_ABORT(IMF_export_file(imanfile, backup));
        printf("Exported backup copy: %s\n", backup);
    }
    if (type != NULL) MEM_free(type);

    IFERR_ABORT(AOM_unlock(imanfile));
}

static void do_it(void)
{
    int
        ii,
        n_ref;
    char
        dataset_name[WSO_name_size_c +1],
        **reference_names,
        reference_name[AE_reference_size_c+1],
        new_path[IMF_filename_size_c + 1],
        which[10];
    tag_t
        dataset = NULLTAG,
        imported_file = NULLTAG,
        *reference_objects = NULL;

    do
    {
        printf("Enter Dataset Name: ");
        gets(dataset_name);
        if (!strcmp(dataset_name, "")) return;

     /* AE_find_dataset finds latest revision of dataset */
        IFERR_ABORT(AE_find_dataset(dataset_name, &dataset));
        if (dataset == NULLTAG) continue;

        do
        {
            n_ref = ask_dataset_imanfile_named_references(dataset,
                &reference_names, &reference_objects);

            report_imanfile_references(n_ref, reference_names,
                reference_objects);
            printf("Enter # to export: ", n_ref+1);
            gets(which);
            if (!strcmp(which, "")) break;

            ii = atoi(which) - 1;
            if (ii < n_ref)
            {
                export_backup_imanfile(reference_objects[ii]);
            }

            free_string_array(n_ref, reference_names);
            MEM_free(reference_objects);
            printf("\n");
        } while (TRUE);
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
