/*
    edit the option TC_customization_libraries to add GTAC
    %TC_ROOT%\sample\compile -DIPLIB=none register_delete_dataset_preaction.c
    %TC_ROOT%\sample\link_custom_exits GTAC
    copy GTAC.dll %TC_BIN% or %TC_USER_LIB%
*/

#include <tc/tc.h>
#include <ict/ict_userservice.h>
#include <tccore/custom.h>
#include <tccore/method.h>
#include <ae/ae.h>
#include <tccore/aom.h>
#include <tccore/workspaceobject.h>
#include <ae/dataset_msg.h>
#include <ss/ss_const.h>
#include <sa/tcfile.h>
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

static void ask_property_value_by_name(tag_t object, char *prop_name,
    char **prop_value)
{
    tag_t
        prop_tag = NULLTAG;

    *prop_value = NULL;

    if (PROP_UIF_ask_property_by_name(object, prop_name, &prop_tag) == ITK_ok)
        IFERR_REPORT(PROP_UIF_ask_value(prop_tag, prop_value));
}

static void report_dataset_named_references_paths(tag_t dataset)
{
    int
        ii,
        reference_count = 0;
    tag_t
        reference_object;
    char
        dataset_name[WSO_name_size_c+1],
        path[SS_MAXPATHLEN],
        reference_name[AE_reference_size_c + 1],
        *type;
    AE_reference_type_t
        reference_type;

/*  Make sure no one messes with it while we're reading it */
    IFERR_REPORT(AOM_refresh(dataset, TRUE));

    IFERR_REPORT(WSOM_ask_name(dataset, dataset_name));
    ECHO("dataset = %s\n", dataset_name);

    IFERR_REPORT(AE_ask_dataset_ref_count(dataset, &reference_count));
    if (reference_count > 0)
    {
        for (ii = 0; ii < reference_count; ii++)
        {
            IFERR_REPORT(AE_find_dataset_named_ref(dataset, ii,
                reference_name, &reference_type, &reference_object));
            ask_property_value_by_name(reference_object, "Type", &type);
            if (!strcmp(type, "ImanFile"))
            {
                ask_imanfile_path(reference_object, path);
                ECHO("  imanfile named reference path = %s\n", path);
            }
            if (type != NULL) MEM_free(type);
        }
    }

    IFERR_REPORT(AOM_unload(dataset));
}

extern DLLAPI int GTAC_method_1(METHOD_message_t *m, va_list args)
{
    tag_t datasetTag = va_arg(args, tag_t);

    ECHO("In GTAC_method_1:  datasetTag = %d\n", datasetTag);

    report_dataset_named_references_paths(datasetTag);

    return ITK_ok;
}

extern int GTAC_register_method(int *decision, va_list args)
{
    METHOD_id_t  method;

    *decision = ALL_CUSTOMIZATIONS;

    IFERR_REPORT(METHOD_find_method("Dataset", AE_delete_dataset_msg, &method));
    if (method.id != NULLTAG)
    {
        IFERR_REPORT( METHOD_add_action(method, METHOD_pre_action_type,
            (METHOD_function_t) GTAC_method_1, NULL));
        ECHO("Registered GTAC_method_1 as Pre-Action for delete dataset\n");
    }
    else
        ECHO("Method not found!\n");

    return ITK_ok;
}

extern DLLAPI int GTAC_register_callbacks ()
{
    ECHO("\n\nGTAC_register_callbacks\n\n");
    IFERR_ABORT(CUSTOM_register_exit ( "GTAC", "USER_init_module",
       (CUSTOM_EXIT_ftn_t)  GTAC_register_method));

  return ( ITK_ok );
}
