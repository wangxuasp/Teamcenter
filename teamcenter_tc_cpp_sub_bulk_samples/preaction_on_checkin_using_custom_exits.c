/*
    edit the option TC_customization_libraries to add GTAC
    %TC_ROOT%\sample\compile -DIPLIB=none preaction_on_checkin_using_custom_exits.c
    %TC_ROOT%\sample\link_custom_exits GTAC
    copy GTAC.dll %TC_BIN%
*/

#include <tc/tc.h>
#include <ict/ict_userservice.h>
#include <tccore/custom.h>
#include <tccore/method.h>
#include <tccore/workspaceobject.h>
#include <res/reservation.h>
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

static void report_object_description(tag_t object)
{
    WSO_description_t
        desc;

    if (IFERR_REPORT(WSOM_describe(object, &desc))) return;

    ECHO("     Object Name:     %s\n", desc.object_name);
    ECHO("     Object Type:     %s\n", desc.object_type);
    ECHO("     Owner's Name:    %s\n", desc.owners_name);
    ECHO("     Owning Group:    %s\n", desc.owning_group_name);
    ECHO("     Description:     %s\n", desc.description);
    ECHO("     Date Created:    %s\n", desc.date_created);
    ECHO("     Date Modified:   %s\n", desc.date_modified);
    ECHO("     Application:     %s\n", desc.application);
    ECHO("     Date Released:   %s\n", desc.date_released);
    ECHO("     Released For:    %s\n", desc.released_for);
    ECHO("     ID String:       %s\n", desc.id_string);
    ECHO("     Revision Number: %d\n", desc.revision_number);
    ECHO("     Revision Limit:  %d\n", desc.revision_limit);
    ECHO("     Last Modifier    %s\n", desc.last_modifying_user_name);
    ECHO("     Archive Date:    %s\n", desc.archive_date);
    ECHO("     Backup Date:     %s\n", desc.backup_date);
    ECHO("     Is Frozen:       %d\n", desc.is_frozen);
    ECHO("     Is Reserved:     %d\n", desc.is_reserved);
    ECHO("     Revision ID:     %s\n", desc.revision_id);
    ECHO("     Owning Site:     %s\n", desc.owning_site_name);
}

extern DLLAPI int GTAC_method_1(METHOD_message_t *m, va_list args)
{
    tag_t objTag = va_arg(args, tag_t);

    ECHO("In GTAC_method_1:  checking in = %d\n", objTag);

    report_object_description(objTag);

    return ITK_ok;
}

extern int GTAC_register_method(int *decision, va_list args)
{
    METHOD_id_t  method;

    *decision = ALL_CUSTOMIZATIONS;

    IFERR_REPORT(METHOD_find_method(RES_Type, RES_checkin_msg, &method));
    if (method.id != NULLTAG)
    {
        IFERR_REPORT( METHOD_add_action(method, METHOD_pre_action_type,
            (METHOD_function_t) GTAC_method_1, NULL));
        ECHO("Registered GTAC_method_1 as Pre-Action for check-in\n");
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
