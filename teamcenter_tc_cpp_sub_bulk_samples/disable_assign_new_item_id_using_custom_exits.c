/*
Instructions:
    Compile
        %TC_ROOT%\sample\compile -DIPLIB=none 6332971.c
    Link
        %TC_ROOT%\sample\link_custom_exits GTAC

    Move File
        copy GTAC.dll %TC_BIN%

    Set SITE preference TC_customization_libraries the shared library.
        TC_customization_libraries=
        GTAC
*/

#include <stdarg.h>
#include <ict/ict_userservice.h>
#include <tc/tc.h>
#include <tc/emh.h>
#include <user_exits/user_exits.h>
#include <tccore/custom.h>

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

extern int GTAC_new_item_id(int *decision, va_list args)
{
    /***********  va_list for USER_new_item_id ***********/
    tag_t old_item = va_arg(args, tag_t);      /* args 1 */
    tag_t item_type = va_arg(args, tag_t);     /* args 2 */
    logical *mod = va_arg(args, logical *);    /* args 3 */
    char **id = va_arg(args, char **);         /* args 4 */
    /*****************************************************/

    ECHO("GTAC_new_item_id\n");

    *decision = ONLY_CURRENT_CUSTOMIZATION;
    *mod = TRUE;
    *id = (char *)MEM_alloc(sizeof(char) * 1);
    strcpy(*id, "");

    return ITK_ok;
}

/* This function name must be <library_name>_register_callbacks */
extern DLLAPI int GTAC_register_callbacks()
{
    ECHO("GTAC_register_callbacks\n");

    IFERR_REPORT(CUSTOM_register_exit( "GTAC", "USER_new_item_id",
        (CUSTOM_EXIT_ftn_t) GTAC_new_item_id));

    return ITK_ok;
}
