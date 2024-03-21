/*
    edit the option TC_customization_libraries to add GTAC
    %TC_ROOT%\sample\compile -DIPLIB=none test.c
    %TC_ROOT%\sample\link_custom_exits GTAC
    copy GTAC.dll %TC_BIN% or %TC_USER_LIB%
*/

#include <tc/tc.h>
#include <ict/ict_userservice.h>
#include <tccore/custom.h>
#include <tccore/method.h>
#include <ae/ae.h>
#include <ae/dataset_msg.h>
#include <stdarg.h>

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
            ECHO("Exiting program!\n");
            exit (status);
        }
    }

    return status;
}

extern DLLAPI int GTAC_save_dataset(METHOD_message_t *m, va_list args)
{
    /* Object Type: Dataset - Operation: AE_save_dataset */

    /* va_list for AE_save_dataset_msg */
    tag_t datasetTag = va_arg (args, tag_t);
    logical isNew = va_arg (args, logical);

    ECHO("\nGTAC_save_dataset\n");
    ECHO("\tdatasetTag: %u - isNew: %d\n", datasetTag, isNew);

    return ITK_ok;
}

extern int GTAC_register_method(int *decision, va_list args)
{
    METHOD_id_t  method;

    *decision = ALL_CUSTOMIZATIONS;

    IFERR_REPORT(METHOD_find_method("Dataset", AE_save_dataset_msg, &method));
    if (method.id != NULLTAG)
    {
        IFERR_REPORT( METHOD_add_action(method, METHOD_post_action_type,
            (METHOD_function_t) GTAC_save_dataset, NULL));
        ECHO("Registered GTAC_save_dataset as Post Action for save dataset\n");
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
