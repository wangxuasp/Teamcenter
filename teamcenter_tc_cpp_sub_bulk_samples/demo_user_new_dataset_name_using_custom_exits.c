/*
Instructions:
    Compile
        %TC_ROOT%\sample\compile -DIPLIB=none <program_name>.c
    Link
        %TC_ROOT%\sample\link_custom_exits <shared library name>

    Move File
        copy <shared library name>.dll %TC_BIN% or %TC_USER_LIB%

    Set SITE preference TC_customization_libraries the shared library.
        TC_customization_libraries=
        <shared library name>
*/

#include <stdarg.h>
#include <ict/ict_userservice.h>
#include <tc/tc.h>
#include <tc/emh.h>
#include <user_exits/user_exits.h>
#include <tccore/custom.h>
#include <tccore/item.h>
#include <tccore/tctype.h>
#include <ae/dataset.h>

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

extern DLLAPI int GTAC_new_dataset_name(int *decision, va_list args)
{
    tag_t
        owner = va_arg(args, tag_t),
        dataset_type = va_arg(args, tag_t),
        relation_type = va_arg(args, tag_t);
    const char
        *basis_name = va_arg(args, const char *);
    char
        **dataset_name = va_arg(args, char **);
    logical
        *modifiable = va_arg(args, logical *);

    char
        datasettype_name[AE_datasettype_name_size_c+1],
        type_name[TCTYPE_name_size_c+1],
        rev_name[ITEM_name_size_c+1];

    ECHO("GTAC_new_dataset_name\n");

    *decision  = NO_CUSTOMIZATION;

    if (owner)
    {
        IFERR_REPORT(ITEM_ask_rev_name(owner, rev_name));
        ECHO(" owner = %d (%s)\n", owner, rev_name);
    }
    else
        ECHO(" owner = NULLTAG\n");

    IFERR_REPORT(AE_ask_datasettype_name (dataset_type, datasettype_name));
    ECHO(" dataset_type = %d (%s)\n", dataset_type, datasettype_name);

    IFERR_REPORT(TCTYPE_ask_name(relation_type, type_name));
    ECHO(" relation_type = %d (%s)\n", relation_type, type_name);

    ECHO(" basis_name = %s\n", basis_name);

    if (modifiable)
        ECHO(" modifiable = TRUE\n");
    else
        ECHO(" modifiable = FALSE\n");

    return ITK_ok;
}

/* This function name must be <library_name>_register_callbacks */
extern DLLAPI int GTAC_register_callbacks()
{
    ECHO("GTAC_register_callbacks\n");

    IFERR_REPORT(CUSTOM_register_exit( "GTAC", "USER_new_dataset_name",
        (CUSTOM_EXIT_ftn_t) GTAC_new_dataset_name));

    return ITK_ok;
}
