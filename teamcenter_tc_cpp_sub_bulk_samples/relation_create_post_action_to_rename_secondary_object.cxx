/*HEAD RELATION_CREATE_POST_ACTION_TO_RENAME_SECONDARY_OBJECT.CXX CCC ITK */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ug_va_copy.h>
#include <ict/ict_userservice.h>
#include <tc/emh.h>
#include <user_exits/user_exits.h>
#include <tccore/custom.h>
#include <tccore/method.h>
#include <tccore/tctype.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/workspaceobject.h>

#define IFERR_ABORT(X)  (report_error( __FILE__, __LINE__, #X, X, TRUE))
#define IFERR_REPORT(X) (report_error( __FILE__, __LINE__, #X, X, FALSE))
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

static int report_error(char *file, int line, char *call, int status,
    logical exit_on_error)
{
    if (status != ITK_ok)
    {
        int
            n_errors = 0;
        const int
            *severities = NULL,
            *statuses = NULL;
        const char
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

extern DLLAPI int gtac_extension_rule(METHOD_message_t *m, va_list args)
{
    ECHO(("\n\n gtac_extension_rule \n\n"));
    int ifail = ITK_ok;

    va_list largs;
    va_copy( largs, args );

    tag_t  primary_object = va_arg(largs, tag_t);
    tag_t  secondary_object = va_arg(largs, tag_t);
    tag_t  relation_type = va_arg(largs, tag_t);
    tag_t  user_data = va_arg(largs, tag_t);
    tag_t  *new_relation = va_arg(largs, tag_t *);

    va_end( largs );

    ECHO("   primary_object: %u\n", primary_object);
    ECHO("   secondary_object: %u\n", secondary_object);
    ECHO("   relation_type: %u\n", relation_type);
    ECHO("   user_data: %u\n", user_data);
    ECHO("   new_relation: %u\n", *new_relation);

    tag_t type = NULLTAG;
    IFERR_REPORT(TCTYPE_ask_object_type (secondary_object, &type));

    char type_name[TCTYPE_name_size_c+1] = "";
    IFERR_REPORT(TCTYPE_ask_name(type, type_name));
    if  (strcmp(type_name, "ECO Sample Form") == 0)
    {
        IFERR_REPORT(AOM_refresh(secondary_object, TRUE));
        IFERR_REPORT(AOM_set_value_string(secondary_object, "object_name", "New Name"));
        IFERR_REPORT(AOM_save(secondary_object));
        IFERR_REPORT(AOM_refresh(secondary_object, FALSE));
        IFERR_REPORT(AOM_unload(secondary_object));
    }    
    return ifail;
}

#ifdef __cplusplus
}
#endif
