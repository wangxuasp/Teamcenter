/*HEAD ACTION_HANDLER_TO_ASSIGN_RESOURCE_POOL_AS_REVIEWER CCC ITK */
/*
    Instructions:
        Save this file as GTAC_register_action_handler_callbacks.c
        The naming format is very important.
            GTAC_register_action_handler_callbacks.c
            GTAC_register_callbacks() - function name
            GTAC - the name of the executable

        Compile
            %TC_ROOT%\sample\compile -DIPLIB=none
                 GTAC_register_action_handler_callbacks.c
        Link
            %TC_ROOT%\sample\link_custom_exits GTAC
        Move File
            copy GTAC.dll %TC_USER_LIB%

        Set the following multi valued string Site Preference
        TC_customization_libraries=
        GTAC

    Handler Placement:
        "Assign" Task Action on the select-signoff-team of an Review Task
*/
#include <ict/ict_userservice.h>
#include <tccore/custom.h>
#include <itk/mem.h>
#include <tc/tc.h>
#include <user_exits/user_exits.h>
#include <tc/emh.h>
#include <epm/epm.h>
#include <sa/sa.h>
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
#define ERROR_CHECK(X) if (IFERR_REPORT(X)) return (X)

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

extern DLLAPI int GTAC_action_handler(EPM_action_message_t message)
{
    int
        count,
        n_attachments = 0;
    tag_t
        *attachments = NULL,
        group = NULLTAG,
        job = NULLTAG,
        resourcePool = NULLTAG,
        role = NULLTAG,
        root_task = NULLTAG,
        signoff = NULLTAG,
        *unassigned_profiles = NULL;

    ECHO("GTAC_action_handler\n");

    ERROR_CHECK(EPM_ask_root_task( message.task, &root_task));
    ECHO("  root_task = %d\n", root_task);

    ERROR_CHECK(EPM_ask_attachments(root_task, EPM_target_attachment,
        &n_attachments, &attachments));
    if (n_attachments == 0) return 1;  // fail

    ERROR_CHECK(AOM_ask_group(attachments[0], &group));
    ECHO("  group = %d\n", group);

    if (attachments) MEM_free(attachments);

    ERROR_CHECK(SA_ask_group_default_role(group, &role) );
    ECHO("  role = %d\n", role);

    ERROR_CHECK(EPM_get_resource_pool(group, role, FALSE, &resourcePool));
    ECHO("  resourcePool = %d\n", resourcePool);

    ERROR_CHECK(EPM_ask_job(message.task, &job));
    ECHO("  job = %d\n", job);

    ERROR_CHECK(EPM_ask_unassigned_profiles(job, message.task, &count,
        &unassigned_profiles));
    ECHO("  count = %d\n", count);
    if (count == 0) return 1;  // fail

    ERROR_CHECK(EPM_create_profile_signoff(message.task, resourcePool,
        unassigned_profiles[0], &signoff));
    ECHO("  signoff = %d\n", signoff);

    if (unassigned_profiles) MEM_free(unassigned_profiles);

    ECHO("  Done returning ITK_ok\n");

    return ITK_ok;
}

extern DLLAPI int GTAC_register_action_handler(int *decision, va_list args)
{
    *decision = ALL_CUSTOMIZATIONS;

    ECHO("GTAC_register_action_handler\n");

    ERROR_CHECK(EPM_register_action_handler("GTAC-action-handler",
        "Placement: Assign Task Action of select-signoff-team task",
        GTAC_action_handler));

    return ITK_ok;
}

extern DLLAPI int GTAC_register_callbacks ()
{
    ECHO("GTAC_register_callbacks\n");

    ERROR_CHECK(CUSTOM_register_exit ( "GTAC", "USER_gs_shell_init_module",
       (CUSTOM_EXIT_ftn_t) GTAC_register_action_handler ));

    return ITK_ok;
}
