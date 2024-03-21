/*

Build the ITK:
    %TC_ROOT%\sample\compile -DIPLIB=none report_dataset_imanfile_paths_before_delete.c
    %TC_ROOT%\sample\link_custom_exits libgtac

In the Rich Client interface set the option
    BMF_CUSTOM_IMPLEMENTOR_PATH=<the folder containing libgtac.dll>

Start Business Modeler IDE:
- Define the Extension Rule.
    Select the "ExtensionS View" tab
         Expand "Rules"
         Select "Extensions Definitions"
         RMB-> New Extensions Definitions
             Name: GTAC_method_2
             Language Type: ANSI_C
             Library: libgtac

             Select The "+" in the Availability section
                Business Object Name: Dataset
                Operation Name: AE_delete_dataset
                Entension Point: Pre-Action
              Finish
          Finish

 - Assign the Extension Rule.
     Select "Business Objects" View
         Find Busines Object: Dataset
         RMB-> "Open Extension Rules"
         Expand "Operations" Folder
         Select "AE_delete_dataset"
         In the "Pre-Action" section select "Add"
               Extension Rule: GTAC_method_2
         Finish

- Deploy the extension rule to Teamcenter
     File-> save DataModel-> Ok
     Select the Project-> RMB-> Deploy Template

     Note: there must be a TAO ImR window open to deploy the template.
     The TAO ImR window will display the progress.
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

extern DLLAPI int GTAC_method_2(METHOD_message_t *m, va_list args)
{
    tag_t datasetTag = va_arg(args, tag_t);

    ECHO("In GTAC_method_2:  datasetTag = %d\n", datasetTag);

    report_dataset_named_references_paths(datasetTag);

    return ITK_ok;
}

