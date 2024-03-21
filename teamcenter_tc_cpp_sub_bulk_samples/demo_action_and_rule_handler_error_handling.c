/*HEAD DEMO_ACTION_AND_RULE_HANDLER_ERROR_HANDLING CCC UFUN */
/*
    Instructions:
        Save this file as GTAC_register_action_handler_callbacks.c
        The naming format is very important.
            GTAC_register_callbacks.c
            GTAC_register_callbacks() - function name
            GTAC - the name of the executable

        Compile
            %TC_ROOT%\sample\compile -DIPLIB=none
                 GTAC_register_callbacks.c
        Link
            %TC_ROOT%\sample\link_custom_exits GTAC
        Move File
            copy GTAC.dll %TC_BIN%

        Set the following multi valued string Site Preference
        TC_customization_libraries=
        GTAC

*/
#include <ict/ict_userservice.h>
#include <tccore/custom.h>
#include <user_exits/user_exits.h>
#include <tc/tc.h>
#include <tc/emh.h>
#include <epm/epm.h>
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
#define ERROR_CHECK(X) if (IFERR_REPORT(X)) return (X)

static int report_error(char *file, int line, char *call, int status,
    logical exit_on_error)
{
    if (status != ITK_ok)
    {
        int
            ii,
            n_errors = 0,
            *severities = NULL,
            *statuses = NULL;
        char
            **messages;

        EMH_ask_errors(&n_errors, &severities, &statuses, &messages);
        if (n_errors > 0)
        {
            for (ii = 0; ii < n_errors; ii++)
                ECHO("\n%s\n", messages[ii]);
            // EMH_clear_errors();
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

#define ITK_err 919002

extern DLLAPI int GTAC_action_raise_error_stack(EPM_action_message_t message)
{
    ECHO("GTAC_action_raise_error_stack\n");

    return ITK_ok;
}

extern DLLAPI int GTAC_action_info_ok(EPM_action_message_t message)
{
    ECHO("GTAC_action_info_ok\n");

    EMH_store_error_s2(EMH_severity_information, ITK_err,
        "EMH_severity_information", "ITK_ok");

    return ITK_ok;
}

extern DLLAPI int GTAC_action_warn_ok(EPM_action_message_t message)
{
    ECHO("GTAC_action_warn_ok\n");

    EMH_store_error_s2(EMH_severity_warning, ITK_err,
        "EMH_severity_warning", "ITK_ok");

    return ITK_ok;
}

extern DLLAPI int GTAC_action_error_ok(EPM_action_message_t message)
{
    ECHO("GTAC_action_error_ok\n");

    EMH_store_error_s2(EMH_severity_error, ITK_err,
        "EMH_severity_error", "ITK_ok");

    return ITK_ok;
}

extern DLLAPI int GTAC_action_info_err(EPM_action_message_t message)
{
    ECHO("GTAC_action_info_err\n");

    EMH_store_error_s2(EMH_severity_information, ITK_err,
        "EMH_severity_information", "error code");

    return ITK_err;
}

extern DLLAPI int GTAC_action_warn_err(EPM_action_message_t message)
{
    ECHO("GTAC_action_warn_err\n");

    EMH_store_error_s2(EMH_severity_warning, ITK_err,
        "EMH_severity_warning", "ITK_err");

    return ITK_err;
}

extern DLLAPI int GTAC_action_error_err(EPM_action_message_t message)
{
    ECHO("GTAC_action_error_err\n");

    EMH_store_error_s2(EMH_severity_error, ITK_err,
        "EMH_severity_error", "ITK_err");

    return ITK_err;
}

extern DLLAPI EPM_decision_t GTAC_rule_info_go(EPM_rule_message_t message)
{
    ECHO("GTAC_rule_info_go\n");

    EMH_store_error_s2(EMH_severity_information, ITK_err,
        "EMH_severity_information", "EPM_go");

    return EPM_go;
}

extern DLLAPI EPM_decision_t GTAC_rule_warn_go(EPM_rule_message_t message)
{
    ECHO("GTAC_rule_warn_go\n");

    EMH_store_error_s2(EMH_severity_warning, ITK_err,
        "EMH_severity_warning", "EPM_go");

    return EPM_go;
}

extern DLLAPI EPM_decision_t GTAC_rule_error_go(EPM_rule_message_t message)
{
    ECHO("GTAC_rule_error_go\n");

    EMH_store_error_s2(EMH_severity_error, ITK_err,
        "EMH_severity_error", "EPM_go");

    return EPM_go;
}

extern DLLAPI EPM_decision_t GTAC_rule_info_nogo(EPM_rule_message_t message)
{
    ECHO("GTAC_rule_info_nogo\n");

    EMH_store_error_s2(EMH_severity_information, ITK_err,
        "EMH_severity_information", "EPM_nogo");

    return EPM_nogo;
}

extern DLLAPI EPM_decision_t GTAC_rule_warn_nogo(EPM_rule_message_t message)
{
    ECHO("GTAC_rule_warn_nogo\n");

    EMH_store_error_s2(EMH_severity_warning, ITK_err,
        "EMH_severity_warning", "EPM_nogo");

    return EPM_nogo;
}

extern DLLAPI EPM_decision_t GTAC_rule_error_nogo(EPM_rule_message_t message)
{
    ECHO("GTAC_rule_error_nogo\n");

    EMH_store_error_s2(EMH_severity_error, ITK_err,
        "EMH_severity_error", "EPM_nogo");

    return EPM_nogo;
}

extern DLLAPI int GTAC_register_action_handlers(int *decision, va_list args)
{
    *decision = ALL_CUSTOMIZATIONS;

    ECHO("GTAC_register_action_handlers\n");

    ERROR_CHECK(EPM_register_action_handler("GTAC-action-raise-error-stack",
        "Placement: Anywhere", GTAC_action_raise_error_stack));

    ERROR_CHECK(EPM_register_action_handler("GTAC-action-info-ok",
        "Placement: Anywhere", GTAC_action_info_ok));

    ERROR_CHECK(EPM_register_action_handler("GTAC-action-warn-ok",
        "Placement: Anywhere", GTAC_action_warn_ok));

    ERROR_CHECK(EPM_register_action_handler("GTAC-action-error-ok",
        "Placement: Anywhere", GTAC_action_error_ok));

    ERROR_CHECK(EPM_register_action_handler("GTAC-action-info-err",
        "Placement: Anywhere", GTAC_action_info_err));

    ERROR_CHECK(EPM_register_action_handler("GTAC-action-warn-err",
        "Placement: Anywhere", GTAC_action_warn_err));

    ERROR_CHECK(EPM_register_action_handler("GTAC-action-error-err",
        "Placement: Anywhere", GTAC_action_error_err));

    ERROR_CHECK(EPM_register_rule_handler("GTAC-rule-info-go",
        "Placement: Anywhere", GTAC_rule_info_go));

    ERROR_CHECK(EPM_register_rule_handler("GTAC-rule-warn-go",
        "Placement: Anywhere", GTAC_rule_warn_go));

    ERROR_CHECK(EPM_register_rule_handler("GTAC-rule-error-go",
        "Placement: Anywhere", GTAC_rule_error_go));

    ERROR_CHECK(EPM_register_rule_handler("GTAC-rule-info-nogo",
        "Placement: Anywhere", GTAC_rule_info_nogo));

    ERROR_CHECK(EPM_register_rule_handler("GTAC-rule-warn-nogo",
        "Placement: Anywhere", GTAC_rule_warn_nogo));

    ERROR_CHECK(EPM_register_rule_handler("GTAC-rule-error-nogo",
        "Placement: Anywhere", GTAC_rule_error_nogo));

    return ITK_ok;
}

extern DLLAPI int GTAC_register_callbacks ()
{
    ECHO("GTAC_register_callbacks\n");

    ERROR_CHECK(CUSTOM_register_exit ( "GTAC", "USER_gs_shell_init_module",
       (CUSTOM_EXIT_ftn_t) GTAC_register_action_handlers ));

    return ITK_ok;
}
