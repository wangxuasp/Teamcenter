

#include <tc/tc.h>
#include <user_exits/user_exits.h>
#include <tccore/tctype.h>
#include <ict/ict_userservice.h>
#include <tccore/method.h>
#include <tccore/item_msg.h>
#include <tccore/item_errors.h>
#include <itk/bmf.h>

#define ECHO(X)  printf X; TC_write_syslog X

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

        EMH_ask_errors( &n_errors, &severities, &statuses, &messages );
        if (n_errors > 0)
        {
            ECHO(("\n%s\n", messages[n_errors-1]));
            EMH_clear_errors();
        }
        else
        {
            char *error_message_string;
            EMH_get_error_string (NULLTAG, status, &error_message_string);
            ECHO(("\n%s\n", error_message_string));
        }

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

extern DLLAPI int gtac_extension_rule(METHOD_message_t *m, va_list args)
{
    int n_bmf_params = 0, ii = 0;
    BMF_extension_arguments_t *ext_params = NULL;

    ECHO(( "\n gtac_extension_rule -- \n"));

    if (!IFERR_REPORT(BMF_get_user_params(m, &n_bmf_params, &ext_params)))
    {
        for (ii = 0; ii < n_bmf_params; ii++)
        {
            ECHO(( "  %s - %s\n",
                ext_params[ii].paramName,
                ext_params[ii].arg_val.str_value));
        }
    }

    return ITK_ok;
}
