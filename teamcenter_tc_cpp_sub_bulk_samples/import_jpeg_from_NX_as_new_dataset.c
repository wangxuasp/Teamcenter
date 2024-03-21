/*
  For the NX client side code, see the NX-> open article:
    <a id="u_link" href="http://solutions.industrysoftware.automation.siemens.com/view.php?si=nx_api4216"> Sample Open C API program : create jpeg of current view and import to teamcenter </a>

  Build and "deploy" instructions:

    In a shell set up like this:
        set MSDEV_HOME=C:\Program Files\Microsoft Visual Studio 8\VC
        set TC_ROOT=<as appropriate>
        set TC_DATA=<as appropriate>
        call %tc_data%\tc_profilevars

    Compile:
        %TC_ROOT%\sample\compile -DIPLIB=none <any_name>.c
    Link (the .obj from the compile must be the ONLY .obj in the folder):
        %TC_ROOT%\sample\link_custom_exits libgtac
    Move File:
        copy libgtac.dll %TC_BIN%

    In Portal Client:
        Edit-> Options - create/edit multi-string site preference
            TC_customization_libraries=
            libgtac

*/
#include <ae/ae.h>
#include <ae/dataset.h>
#include <tccore/aom.h>
#include <tc/emh.h>
#include <ict/ict_userservice.h>
#include <tc/tc.h>
#include <tccore/tctype.h>
#include <sa/tcfile.h>
#include <tccore/grm.h>
#include <tccore/item.h>
#include <ss/ss_errors.h>
#include <tccore/custom.h>
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

#define IFERR_REPORT(X) (report_error( __FILE__, __LINE__, #X, X, FALSE))
#define IFERR_RETURN(X) if (IFERR_REPORT(X)) return
#define IFERR_RETURN_IT(X) if (IFERR_REPORT(X)) return X

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
            ECHO("Exiting program!\n");
            exit (status);
        }
    }

    return status;
}

static int ask_file_format(char *path)
{
    IMF_file_status
        stats;

    IFERR_RETURN_IT(IMF_stat(path, &stats));

    return stats.fmt;
}

static void ask_imanfile_path(tag_t fileTag, char *path)
{
    int
        machine_type;
    tag_t
        volume;

    IFERR_RETURN(IMF_ask_volume(fileTag, &volume));
    IFERR_RETURN(VM_ask_machine_type(volume, &machine_type));
    IFERR_RETURN(IMF_ask_file_pathname(fileTag, machine_type, path));
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

    IFERR_RETURN_IT(IMF_import_file(os_path_name, new_os_name, file_type,
                 &file_tag, &file_descriptor));
    ask_imanfile_path(file_tag, pathname);
    ECHO("imported %s to %s\n", os_path_name, pathname);

    if (strcmp(new_os_name, ""))
        IFERR_REPORT(IMF_set_original_file_name(file_tag, new_os_name));

    IFERR_RETURN_IT(AOM_save(file_tag));
    IFERR_RETURN_IT(AOM_unlock(file_tag));

    return file_tag;
}

static void build_similar_temp_filespec(char *filespec, char *tempspec)
{
    char *temp_dir = getenv("TC_TMP_DIR");

    if (SS_MACHINE_TYPE == SS_WNT_MACHINE)
        sprintf(tempspec, "%s\\%s", temp_dir, filespec);
    else
        sprintf(tempspec, "%s/%s", temp_dir, filespec);
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

static void export_backup_imanfile(tag_t imanfile)
{
    char
        backup[SS_MAXPATHLEN],
        name[IMF_filename_size_c + 1],
        *type;

    IFERR_REPORT(AOM_refresh(imanfile, TRUE));

    ask_property_value_by_name(imanfile, "Type", &type);
    if (!strcmp(type, "ImanFile"))
    {
        IFERR_REPORT(IMF_ask_original_file_name(imanfile, name));
        build_similar_temp_filespec(name, backup);
        unlink(backup);
        IFERR_REPORT(IMF_export_file(imanfile, backup));
        ECHO("Exported backup copy: %s\n", backup);
    }
    if (type != NULL) MEM_free(type);

    IFERR_REPORT(AOM_unlock(imanfile));
}

static void delete_dataset_named_reference(tag_t dataset, char *reference_name)
{
    tag_t
        reference_object = NULLTAG;
    char
        backup[SS_MAXPATHLEN],
        name[IMF_filename_size_c + 1],
        pathname[SS_MAXPATHLEN],
        *type;
    AE_reference_type_t
        refType = AE_PART_OF;

    IFERR_REPORT(AOM_refresh(dataset, TRUE));

    IFERR_REPORT(AE_ask_dataset_named_ref(dataset, reference_name, &refType,
        &reference_object));

    export_backup_imanfile(reference_object);
    IFERR_REPORT(AE_purge_dataset_revs(dataset));
    IFERR_REPORT(AE_remove_dataset_named_ref_by_tag(dataset, reference_name,
        reference_object));
    IFERR_REPORT(AE_save_myself(dataset));
    IFERR_REPORT(AOM_unlock(dataset));

    IFERR_REPORT(AOM_lock_for_delete(reference_object));
    IFERR_REPORT(IMF_ask_file_pathname(reference_object, SS_MACHINE_TYPE,
        pathname));
    ECHO("Deleted: %s\n", pathname);
    IFERR_REPORT(AOM_delete(reference_object));
}

static void add_dataset_named_reference(tag_t dataset, char *reference_name,
    tag_t imported_file)
{

    IFERR_RETURN(AOM_refresh(dataset, TRUE));

    IFERR_RETURN(AE_add_dataset_named_ref(dataset, reference_name, AE_PART_OF,
        imported_file));

    IFERR_RETURN(AE_save_myself(dataset));
    IFERR_RETURN(AOM_unlock(dataset));
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

static int add_step_file_to_dataset(char *inputString)
{
    tag_t
        item = NULLTAG,
        rev = NULLTAG,
        dataset = NULLTAG,
        ds_type = NULLTAG,
        relation_type = NULLTAG,
        relation = NULLTAG,
        tool = NULLTAG,
        imported_file = NULLTAG;
    char
        ds_name[WSO_name_size_c+1],
        item_id[ITEM_id_size_c+1],
        new_path[IMF_filename_size_c + 1],
        rev_id[ITEM_id_size_c+1],
        view_name[50];
    IMF_file_t
        descriptor;

    strcpy(item_id, strtok(inputString, "~"));
    if (!strcmp(item_id, "")) return 1;

    strcpy(rev_id, strtok(NULL, "~"));
    if (!strcmp(rev_id, "")) return 1;

    strcpy(view_name, strtok(NULL, "~"));
    if (!strcmp(view_name, "")) return 1;

    strcpy(new_path, strtok(NULL, "~"));
    if (!strcmp(new_path, "")) return 1;

    ECHO("Searching for Item ID %s with Rev ID %s...\n", item_id, rev_id);
    IFERR_REPORT(ITEM_find_rev(item_id, rev_id, &rev));
    if (rev == NULL_TAG)
    {
        ECHO("ItemRev %s/%s was NOT found!\n", item_id, rev_id);
        return 1;
    }

    IFERR_REPORT(GRM_find_relation_type("IMAN_specification", &relation_type) );
    IFERR_REPORT(GRM_list_secondary_objects_only(rev, relation_type,  &n_objects, &objects));
    for (ii = 0; ii < n_objects
    (
	    IFERR_REPORT( WSOM_ask_object_type(objects[ii], type) );
        if (strcmp(type, "UGMASTER") == 0)
        {
            dataset = objects[ii]
            IFERR_ABORT(AOM_refresh(dataset, TRUE));
            sprintf(file_name, "%s/%s-Step", item_id, rev_id);
            IFERR_ABORT(AE_import_named_ref(dataset, "UGPART", inputString, file_name,  SS_TEXT));
            IFERR_ABORT(AOM_save(dataset)); 
            IFERR_ABORT(AOM_refresh(dataset, FALSE));
            IFERR_ABORT(AOM_unload(dataset));

        }
    }
    return 0;
}

extern int GTAC_invoke_pdm_server(int *decision, va_list args)
{
    /***********  va_list for USER_invoke_pdm_server ***********/
    int  input_code = va_arg(args, int);          /* args 1 */
    char *input_string = va_arg(args, char *);    /* args 2 */
    int  *output_code = va_arg(args, int *);      /* args 3 */
    char **output_string = va_arg(args, char **); /* args 4 */
    /***********************************************************/

    ECHO("\n In GTAC_invoke_pdm_server \n");
    ECHO("incoming:\n");
    ECHO(" *decision = %d\n", *decision);
    ECHO(" input_code = %d\n", input_code);
    ECHO(" input_string = %s\n", input_string);

    switch (input_code)
    {
        case 6973825:
            *decision = ONLY_CURRENT_CUSTOMIZATION;
            *output_code = add_step_file_to_dataset(input_string);
            break;

        default:
            *decision = ALL_CUSTOMIZATIONS;
            break;
    }

    ECHO("outgoing:\n");
    ECHO(" *decision = %d\n", *decision);
    ECHO(" output_code = %d\n", output_code);
    ECHO(" *output_string = %s\n", *output_string);

    return ITK_ok;
}

/* This function name must be <library_name>_register_callbacks */
extern DLLAPI int libgtac_register_callbacks()
{
    ECHO("\n libgtac_register_callbacks \n");

    IFERR_REPORT(CUSTOM_register_exit( "libgtac", "USER_invoke_pdm_server",
        (CUSTOM_EXIT_ftn_t) gtac_invoke_pdm_server));

    return ITK_ok;
}
