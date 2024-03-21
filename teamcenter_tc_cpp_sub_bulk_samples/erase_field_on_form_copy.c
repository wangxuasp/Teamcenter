/*HEAD ERASE_FIELD_ON_FORM_COPY CCC UFUN */
/*
    Setting up External Extension Rules in Business Modeler

    To build:
    %TC_ROOT%\sample\compile -DIPLIB=none erase_field_on_form_copy.c
    %TC_ROOT%\sample\link_custom_exits erase_field_on_form_copy

    Set up in Portal:
    Admin-> Business Modeler. (Login)
        Edit-> Options... -> Index-> New.
            Name: BMF_CUSTOM_IMPLEMENTOR_PATH
            Scope:  Site
            Values: C:\path\to\executable\folder
        Create.
        Cancel.

    Extension Rules
        Define Extension
            Extension Details
                Name: GTAC_copy_itemRevision_master
                External
                Language Type: C
                Library: erase_field_on_form_copy
                Availablity: TYPE:ItemRevision Master:Workspace Object Copy:Post-Action
        Assign Extensions
            Extension Point Selection
                Form
                    ItemRevision Master
                Type
                Operation: Workspace Object Copy
                Extension Point: Post-Action
                +
                Extension Name: GTAC_copy_itemRevision_master
            Apply
*/
#include <stdlib.h>
#include <tc/tc.h>
#include <tccore/aom.h>
#include <tccore/method.h>
#include <bmf/libuserext_exports.h>

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

extern USER_EXT_DLL_API int GTAC_copy_itemRevision_master(METHOD_message_t* msg, va_list args)
{

    tag_t  wso = va_arg(args, tag_t);
    char   *new_name = va_arg(args, char *);
    tag_t  *item_rev_master_form = va_arg(args, tag_t *);

    ECHO(("In GTAC_copy_itemRevision_master\n"));
    ECHO(("item_rev_master_form = %u", *item_rev_master_form));

    IFERR_ABORT(AOM_refresh(*item_rev_master_form, TRUE));
    IFERR_REPORT(AOM_set_value_string(*item_rev_master_form, "serial_number",
        ""));
    IFERR_REPORT(AOM_save(*item_rev_master_form));
    IFERR_REPORT(AOM_unload(*item_rev_master_form));

    return  ITK_ok;
}
