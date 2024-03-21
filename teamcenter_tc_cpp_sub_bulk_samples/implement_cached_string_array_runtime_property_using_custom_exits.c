/*
    Instructions:
        Save this file as gtac_register_callbacks.c
        The naming format is very important.
            <any_name>.c
            XXXX_register_callbacks() - function name
            XXXX - the name of the executable

        Compile
            %TC_ROOT%\sample\compile -DIPLIB=none test.c
        Link
            %TC_ROOT%\sample\link_custom_exits GTAC
        Move File
            copy GTAC.dll %TC_BIN%

        Edit-> Options - create/edit multi-string site preference
            TC_customization_libraries=
            GTAC
*/
#include <stdarg.h>
#include <ict/ict_userservice.h>
#include <tccore/custom.h>
#include <tc/tc.h>
#include <user_exits/user_exits.h>
#include <tccore/method.h>
#include <property/prop.h>
#include <tccore/aom.h>
#include <tccore/tctype.h>
#include <tc/emh.h>

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
#define IFERR_RETURN(X) if (IFERR_REPORT(X)) return X

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

static char **allocate_string_array(int n_elements, int n_chars)
{
    int ii;
    char **where = MEM_alloc(n_elements * sizeof(char *));

    for (ii = 0; ii < n_elements; ii++)
        where[ii] = MEM_alloc(n_chars * sizeof(char));

    return where;
}

extern int GTAC_ask_gtacStringArray_value(METHOD_message_t *m, va_list args)
{
    tag_t prop_tag = va_arg(args, tag_t);
    int  *num = va_arg(args, int *);
    char ***values = va_arg(args, char ***);

    tag_t object = NULLTAG;
    const char *propname = NULL;
    int ii;
    char **myValues = NULL;

    METHOD_PROP_MESSAGE_OBJECT(m, object);
    METHOD_PROP_MESSAGE_PROP_NAME(m, propname);

/*  Use previously cached value if it exists */
    IFERR_REPORT(AOM_get_value_strings(object, propname, num, values));
    if (*num > 0) return ITK_ok;

/*  Else calculate and cache the new value */
    myValues = allocate_string_array(4, 20);
    for (ii = 0; ii < 4; ii++) sprintf(myValues[ii], "Sample value %d", ii+1);

    *num = 4;
    *values = myValues;

    IFERR_REPORT(AOM_assign_strings(object, propname, *num, *values));

    ECHO("GTAC_ask_gtacStringArray_value\n");
    ECHO("  object = %d, propname = %s, num = %d\n", object, propname, *num);
    for (ii = 0; ii < *num; ii++)
        ECHO("    values[%d] = %s\n", ii, (*values)[ii]);

    return ITK_ok;
}

static logical once = FALSE;

extern int GTAC_prop_init_module(METHOD_message_t* m, va_list args)
{
    tag_t
        prt_tag = NULLTAG,
        new_property = NULLTAG,
        type_tag = va_arg(args, tag_t);
    METHOD_id_t
        method;
    const char
        typename[TCTYPE_name_size_c+1] = "";

    if (once) return ITK_ok;

    ECHO("GTAC_prop_init_module\n");

    IFERR_RETURN(TCTYPE_ask_name(type_tag, (char *)typename));

    IFERR_RETURN(TCTYPE_add_runtime_array_property(type_tag, "gtacStringArray",
        PROP_string, 128, -1, &new_property));

    IFERR_RETURN(PROPDESC_set_display_name(new_property, "GTAC Strings"));

    IFERR_RETURN(METHOD_register_prop_method(typename,
        "gtacStringArray", PROP_ask_value_strings_msg,
        GTAC_ask_gtacStringArray_value, 0, &method));

    once = TRUE;

    return ITK_ok;
}

extern int GTAC_register_runtime_properties(int *decision)
{
    USER_prop_init_entry_t
        user_types_methods[] =
            { { "ItemRevision", GTAC_prop_init_module, NULL } };

    ECHO("GTAC_register_runtime_properties\n");

    *decision = ALL_CUSTOMIZATIONS;

    return (IFERR_REPORT(TCTYPE_register_properties(user_types_methods, 1)));
}

extern DLLAPI int GTAC_register_callbacks ()
{
    ECHO("GTAC_register_callbacks\n");

    CUSTOM_register_exit("GTAC", "USER_register_properties",
       (CUSTOM_EXIT_ftn_t) GTAC_register_runtime_properties );

  return (ITK_ok);
}
