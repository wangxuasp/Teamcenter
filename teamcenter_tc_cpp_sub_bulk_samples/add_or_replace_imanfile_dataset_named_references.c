/*HEAD ADD_OR_REPLACE_IMANFILE_DATASET_NAMED_REFERENCES CCC UFUN */
#include <stdlib.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/workspaceobject.h>
#include <ae/ae.h>
#include <user_exits/user_exits.h>
#include <property/prop.h>
#include <sa/tcvolume.h>
#include <tccore/aom.h>
#include <ss/ss_const.h>
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

static void free_string_array(int n_elements, char **string_array)
{
    int ii;

    for (ii = 0; ii < n_elements; ii++) MEM_free(string_array[ii]);

    if ((n_elements > 0) && (string_array != NULL)) MEM_free(string_array);
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
        ECHO("%d.  %s - %s\n", ii+1, reference_names[ii], name);
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
        ECHO("Exported backup copy: %s\n", backup);
    }
    if (type != NULL) MEM_free(type);

    IFERR_ABORT(AOM_unlock(imanfile));
}

static void ask_imanfile_path(tag_t fileTag, char *path)
{
    int
        machine_type;
    tag_t
        volume;

    IFERR_ABORT(IMF_ask_volume(fileTag, &volume));
    IFERR_ABORT(VM_ask_machine_type(volume, &machine_type));
    IFERR_ABORT(IMF_ask_file_pathname(fileTag, machine_type, path));
}

static void delete_dataset_named_reference(tag_t dataset, char *reference_name,
    tag_t reference_object)
{
    char
        pathname[SS_MAXPATHLEN];

    IFERR_ABORT(AOM_refresh(dataset, TRUE));

    export_backup_imanfile(reference_object);
	IFERR_REPORT(AE_purge_dataset_revs(dataset));
    IFERR_ABORT(AE_remove_dataset_named_ref_by_tag(dataset, reference_name,
        reference_object));
    IFERR_ABORT(AE_save_myself(dataset));
    IFERR_ABORT(AOM_unlock(dataset));

    IFERR_ABORT(AOM_lock_for_delete(reference_object));
    ask_imanfile_path(reference_object, pathname);
    ECHO("Deleted: %s\n", pathname);
    IFERR_ABORT(AOM_delete(reference_object));
}

static tag_t import_os_file(char *os_path_name, char *new_os_name)
{
    int
        file_type;
    tag_t
        file_tag = NULLTAG;
    char
        pathname[SS_MAXPATHLEN];
    IMF_file_t
        file_descriptor;

    file_type = ask_file_format(os_path_name);

    IFERR_ABORT(IMF_import_file(os_path_name, new_os_name, file_type,
                 &file_tag, &file_descriptor));
    ask_imanfile_path(file_tag, pathname);
    ECHO("imported %s to %s\n", os_path_name, pathname);

    if (strcmp(new_os_name, ""))
        IFERR_REPORT(IMF_set_original_file_name(file_tag, new_os_name));

    IFERR_ABORT(AOM_save(file_tag));
    IFERR_ABORT(AOM_unlock(file_tag));

    return file_tag;
}

static void add_dataset_named_reference(tag_t dataset, char *reference_name,
    tag_t imported_file)
{

    IFERR_ABORT(AOM_refresh(dataset, TRUE));

    IFERR_ABORT(AE_add_dataset_named_ref(dataset, reference_name, AE_PART_OF,
        imported_file));

    IFERR_ABORT(AE_save_myself(dataset));
    IFERR_ABORT(AOM_unlock(dataset));
}

static void do_it(void)
{
    int
        ii,
        n_ref;
    char
        dataset_name[WSO_name_size_c +1],
        **reference_names = NULL,
        reference_name[AE_reference_size_c+1],
        new_path[IMF_filename_size_c + 1],
        new_name[IMF_filename_size_c + 1],
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

    /*  AE_find_dataset finds latest revision of dataset */
        IFERR_ABORT(AE_find_dataset(dataset_name, &dataset));
        if (dataset == NULLTAG) continue;

        do
        {
            n_ref = ask_dataset_imanfile_named_references(dataset,
                &reference_names, &reference_objects);

            report_imanfile_references(n_ref, reference_names,
                reference_objects);
            printf("Enter # to replace or %d to add new: ", n_ref+1);
            gets(which);
            if (!strcmp(which, "")) break;

            ii = atoi(which) - 1;
            if (ii >= n_ref)
            {
                printf("Enter reference type to add: ");
                gets(reference_name);
                printf("Enter path to new %s file: ", reference_name);
                gets(new_path);
                printf("Enter new desired OS filename: ");
                gets(new_name);
            }
            else
            {
                strcpy(reference_name, reference_names[ii]);
                printf("Enter path to replacement %s file: ", reference_name);
                gets(new_path);
                printf("Enter new desired OS filename: ");
                gets(new_name);

                delete_dataset_named_reference(dataset, reference_name,
                    reference_objects[ii]);
            }

            imported_file = import_os_file(new_path, new_name);
            add_dataset_named_reference(dataset, reference_name, imported_file);
            if (n_ref > 0)
            {
                free_string_array(n_ref, reference_names);
                MEM_free(reference_objects);
            }
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
