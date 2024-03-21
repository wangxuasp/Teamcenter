/*HEAD LIST_ALL_DATASET_NAMED_REFERENCES CCC UFUN */
#include <stdlib.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/workspaceobject.h>
#include <ae/ae.h>
#include <user_exits/user_exits.h>
#include <property/prop.h>
#include <tccore/aom.h>

#define ITK_CALL(X) (report_error( __FILE__, __LINE__, #X, (X)))

static void report_error( char *file, int line, char *function, int return_code)
{
    if (return_code != ITK_ok)
    {
        int
            ii,
            n_errors,
            *severities,
            *statuses;
        char
            **messages;

        printf("\n");

        EMH_ask_errors( &n_errors, &severities, &statuses, &messages );
        for (ii = 0; ii < n_errors; ii++) printf( "%s\n", messages[ii] );
        EMH_clear_errors();

        printf( "error %d at line %d in %s\n", return_code, line, file);
        printf( "%s\n", function);
        printf( "\nExiting program!\n");

        exit (!ITK_ok);
    }
}

static void free_string_array(int n_elements, char **string_array)
{
    int ii;

    for (ii = 0; ii < n_elements; ii++) MEM_free(string_array[ii]);

    MEM_free(string_array);
}

static char **alloc_string_array(int n_elements, int n_chars)
{
    int ii;
    char **where = MEM_alloc(n_elements * sizeof(char *));

    for (ii = 0; ii < n_elements; ii++)
        where[ii] = MEM_alloc(n_chars * sizeof(char));

    return where;
}

static int ask_all_dataset_named_references(tag_t dataset,
    char ***reference_names, tag_t **reference_objects, logical **is_imanfile)
{
    int
        ii,
        reference_count = 0;
    tag_t
        prop_tag = NULLTAG;
    char
        *type = NULL;
    AE_reference_type_t
        reference_type;

/*  Make sure no one messes with it while we're reading it */
    ITK_CALL(AOM_refresh(dataset, TRUE));

    ITK_CALL(AE_ask_dataset_ref_count(dataset, &reference_count));
    if (reference_count > 0)
    {
        *reference_names = alloc_string_array(reference_count,
            AE_reference_size_c + 1);
        *reference_objects = MEM_alloc(reference_count * sizeof(tag_t));
        *is_imanfile = MEM_alloc(reference_count * sizeof(logical));

        for (ii = 0; ii < reference_count; ii++)
        {
            ITK_CALL(AE_find_dataset_named_ref(dataset, ii,
                (*reference_names)[ii], &reference_type,
                &(*reference_objects)[ii]));
            ITK_CALL(PROP_UIF_ask_property_by_name((*reference_objects)[ii],
                "Type", &prop_tag));
            ITK_CALL(PROP_UIF_ask_value(prop_tag, &type));
            if (!strcmp(type, "ImanFile"))
                (*is_imanfile)[ii] = TRUE;
            else
                (*is_imanfile)[ii] = FALSE;
            if (type != NULL) MEM_free(type);
        }
    }

    ITK_CALL(AOM_unload(dataset));

    return reference_count;
}

static void report_dataset_named_references(int n, char **reference_names,
    tag_t *reference_objects, logical *is_imanfile)
{
    int ii;
    char name[IMF_filename_size_c + 1];

    for (ii = 0; ii < n; ii++)
    {
        if (is_imanfile[ii])
        {
            ITK_CALL(IMF_ask_original_file_name(reference_objects[ii], name));
            printf("%d.  %s - %s\n", ii+1, reference_names[ii], name);
        }
        else
            printf("%d.  %s\n", ii+1, reference_names[ii]);
    }
}

static void do_it(void)
{
    logical
        *is_imanfile;
    int
        n_ref;
    char
        dataset_name[WSO_name_size_c +1],
        **reference_names;
    tag_t
        dataset = NULLTAG,
        *reference_objects = NULL;

    do
    {
        printf("Enter Dataset Name: ");
        gets(dataset_name);
        if (!strcmp(dataset_name, "")) return;

        /* AE_find_dataset finds latest revision of dataset */
        ITK_CALL(AE_find_dataset(dataset_name, &dataset));
        if (dataset == NULLTAG) continue;

        if ((n_ref = ask_all_dataset_named_references(dataset,
                &reference_names, &reference_objects, &is_imanfile)) > 0)
        {
            report_dataset_named_references(n_ref, reference_names,
                reference_objects, is_imanfile);
            free_string_array(n_ref, reference_names);
            MEM_free(reference_objects);
            MEM_free(is_imanfile);
        }
        else
            printf("0 named references found");

        printf("\n");

    } while (TRUE);

}

int ITK_user_main(int argc, char* argv[])
{
    ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
    ITK_CALL(ITK_auto_login());
    ITK_CALL(ITK_set_journalling(TRUE));
    do_it();
    ITK_exit_module(TRUE);
    return ITK_ok;
}
