/*HEAD REGISTER_BOMLINE_PROPERTY_METHODS CCC ITK */

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
#include <itk/mem.h>
#include <tccore/aom_prop.h>
#include <tccore/workspaceobject.h>
#include <tc/folder.h>
#include <tccore/tctype.h>

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
            EMH_ask_error_text (status, &error_message_string);
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

extern DLLAPI int A2_set_unit_of_measure(METHOD_message_t *m, va_list args)
{
    int ifail = ITK_ok;

    va_list largs;
    va_copy( largs, args );
    tag_t prop_tag      = va_arg( largs, tag_t ); 
    char *value =  va_arg( largs, char*); 
    va_end( largs );
    
	tag_t bomline = m->object_tag;

    /* get property name */
    const char *prop_name = NULL;
    METHOD_PROP_MESSAGE_PROP_NAME(m, prop_name);

    tag_t tag_value= NULLTAG;
    IFERR_REPORT(AOM_ask_value_tag(bomline, prop_name, &tag_value));
    ECHO("    %s: %s (%u)\n", prop_name, value, tag_value);
    /* if uom symbol is "ar" the quantity must be 1 */
    if (strcmp(value, "ar") == 0)
    {
        IFERR_REPORT(AOM_set_value_string(bomline, "bl_quantity", "1"));
    }
    return ifail;
}

extern DLLAPI int A2_set_quantity(METHOD_message_t *m, va_list args)
{
    int ifail = ITK_ok;
    va_list largs;
    va_copy( largs, args );
    tag_t prop_tag      = va_arg( largs, tag_t );
    char *value =  va_arg( largs, char*); 
    va_end( largs );
      	    
    tag_t bomline = m->object_tag;

    const char *property_name = NULL;
    METHOD_PROP_MESSAGE_PROP_NAME(m, property_name);
    ECHO ("    %s: %s \n",  property_name, value);

	/* if uom symbol name is "ar" the quantity must be 1 */
    char *uom_value = NULL;
    IFERR_REPORT(AOM_UIF_ask_value(bomline, "bl_uom",  &uom_value));
    ECHO("    uom_value: %s\n", uom_value);
    if (strcmp(uom_value, "ar") == 0)
    {
        ECHO("uom_value is ar\n");
        strcpy ( value,  "1" );
        ifail = 919000;
        EMH_store_error_s1( EMH_severity_error, ifail, "Quantity must me 1 if uom is \"ar\"!"); 
    }    
    return ifail;
}

extern DLLAPI int gtac_user_init_module(int *decision, va_list args)
{
    ECHO("\n\n gtac_user_init_module \n\n");
    *decision = ALL_CUSTOMIZATIONS;

    int ifail = ITK_ok;
    METHOD_id_t  method;

    int num_custom_methods = 2;
    char *type_name[] = {"BOMLine", "BOMLine"};
    char *property_name[] = {"bl_quantity", "bl_uom"};
    char *message[] = {"PROP_UIF_set_value", "PROP_UIF_set_value"};

    METHOD_function_t custom_method[] = {A2_set_quantity, A2_set_unit_of_measure};

    for(int ii = 0; ii <num_custom_methods; ii++)
    {
        IFERR_REPORT(METHOD_find_prop_method(type_name[ii], property_name[ii],message[ii], &method));
        if (method.id != NULLTAG)
        {
            IFERR_REPORT( METHOD_add_action(method, METHOD_post_action_type, custom_method[ii], NULL));
            ECHO("\t Registered %s - %s custom method\n", type_name[ii], message[ii]);
        }
        else  ECHO("\t Method not found for %s - %s \n", type_name[ii], message[ii]);
    }
    return ifail;
}

extern DLLAPI int gtac_custom_exits_register_callbacks()
{
    ECHO("\n gtac_custom_exits_register_callback \n");

    IFERR_REPORT(CUSTOM_register_exit("gtac_custom_exits", "USER_init_module", (CUSTOM_EXIT_ftn_t) gtac_user_init_module));
    return ITK_ok;
}

#ifdef __cplusplus
}
#endif
