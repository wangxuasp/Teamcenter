#include <stdlib.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <sa/tcvolume.h>
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

    for (ii = 0; ii < n_elements; ii++)
        if (string_array[ii]) MEM_free(string_array[ii]);

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

    *prop_value = NULL;

    if (PROP_UIF_ask_property_by_name(object, prop_name, &prop_tag) == ITK_ok)
        IFERR_REPORT(PROP_UIF_ask_value(prop_tag, prop_value));
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
            if ((type != NULL) && !strcmp(type, "ImanFile"))
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

static void ask_imanfile_stats(tag_t fileTag, IMF_file_status *stats)
{
    int
        machine_type;
    tag_t
        volume;
    char
        path[SS_MAXPATHLEN];

    ask_imanfile_path(fileTag, path);
    IFERR_ABORT(IMF_stat(path, stats));
}

static void ask_imanfile_file_size(tag_t fileTag, char **file_size)
{
    int
        n_props = 0,
        ii = 0;
    char
        **props = NULL;

    IFERR_REPORT(AOM_ask_prop_names(fileTag, &n_props, &props));

    for (ii = 0; ii < n_props; ii++)
        if (!strcmp(props[ii], "file_size"))
            IFERR_REPORT(AOM_UIF_ask_value(fileTag, props[ii], file_size));

    free_string_array(n_props, props);

}

static void ask_imanfile_byte_size(tag_t fileTag, char **byte_size)
{
    int
        n_props = 0,
        ii = 0;
    char
        **props = NULL;

    IFERR_REPORT(AOM_ask_prop_names(fileTag, &n_props, &props));

    for (ii = 0; ii < n_props; ii++)
        if (!strcmp(props[ii], "byte_size"))
            IFERR_REPORT(AOM_UIF_ask_value(fileTag, props[ii], byte_size));

    free_string_array(n_props, props);

}

static void report_imanfile_reference_sizes(int n, char **reference_names,
    tag_t *reference_objects)
{
    int
        ii;
    IMF_file_status
        stats;
    char
        *byte_size = NULL,
        *file_size = NULL;

/*  This is obviously not efficient, it just demonstrates 3 different ways
    to get the file size information.
*/
    for (ii = 0; ii < n; ii++)
    {
        ECHO(("%d.  %s\n", ii+1, reference_names[ii]));

        ask_imanfile_stats(reference_objects[ii], &stats);
        ECHO(("\t stats.size = %ld\n", stats.size));

        ask_imanfile_file_size(reference_objects[ii], &file_size);
        if (file_size != NULL)
        {
            ECHO(("\t file_size = %s\n", file_size));
            MEM_free(file_size);
        }

        ask_imanfile_byte_size(reference_objects[ii], &byte_size);
        if (byte_size != NULL)
        {
            ECHO(("\t byte_size = %s\n", byte_size));
            MEM_free(byte_size);
        }
    }
}

static void do_it(void)
{
    int
        n_ref;
    char
        dataset_name[WSO_name_size_c +1],
        **reference_names,
        reference_name[AE_reference_size_c+1];
    tag_t
        dataset = NULLTAG,
        *reference_objects = NULL;

    do
    {
        printf("Enter Dataset Name: ");
        gets(dataset_name);
        if (!strcmp(dataset_name, "")) return;

     /* AE_find_dataset finds latest revision of dataset */
        IFERR_ABORT(AE_find_dataset(dataset_name, &dataset));
        if (dataset == NULLTAG) continue;

        n_ref = ask_dataset_imanfile_named_references(dataset,
            &reference_names, &reference_objects);

        report_imanfile_reference_sizes(n_ref, reference_names,
            reference_objects);

        free_string_array(n_ref, reference_names);
        MEM_free(reference_objects);

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
