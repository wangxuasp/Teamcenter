
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
#include <res/res_itk.h>
#include <tccore/grm.h>

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

static void GTAC_free(void *what)
{
    if (what != NULL)
    {
        MEM_free(what);
        what = NULL;
    }
}

extern DLLAPI int A2_RES_checkout(METHOD_message_t *m, va_list args)
{
    int ifail = ITK_ok;

    va_list largs;
    va_copy( largs, args );
    tag_t checked_out_object = va_arg(largs, tag_t);  
    va_end( largs );

    tag_t object_type_tag = NULLTAG;
    IFERR_REPORT(TCTYPE_ask_object_type(checked_out_object, &object_type_tag));

    char object_type_name[TCTYPE_name_size_c+1] = "";
    IFERR_REPORT(TCTYPE_ask_name(object_type_tag, object_type_name)); 

    if (strcmp(object_type_name, "PDF") == 0)
    {
        char *object_string = NULL;
        IFERR_REPORT(WSOM_ask_object_id_string(checked_out_object, &object_string));
        ECHO("       Checked out %s <%s>\n", object_string, object_type_name);
        MEM_free(object_string);
    }
    return ifail;
}

extern DLLAPI int A2_RES_checkin(METHOD_message_t *m, va_list args)
{
    int ifail = ITK_ok;

    va_list largs;
    va_copy( largs, args );
    tag_t checked_in_object = va_arg(args, tag_t);  
    va_end( largs );

    tag_t object_type_tag = NULLTAG;
    IFERR_REPORT(TCTYPE_ask_object_type(checked_in_object, &object_type_tag));

    char object_type_name[TCTYPE_name_size_c+1] = "";
    IFERR_REPORT(TCTYPE_ask_name(object_type_tag, object_type_name));  

    if (strcmp(object_type_name, "PDF") == 0)
    {
        char *object_string = NULL;
        IFERR_REPORT(WSOM_ask_object_id_string(checked_in_object, &object_string));
        ECHO("       Checked in %s <%s>\n", object_string, object_type_name);
        MEM_free(object_string);    
    }
    return ifail;
}

extern DLLAPI int A2_RES_cancel_checkout(METHOD_message_t *m, va_list args)
{
    int ifail = ITK_ok;

    tag_t checked_out_object = va_arg(args, tag_t);

    tag_t object_type_tag = NULLTAG;
    IFERR_REPORT(TCTYPE_ask_object_type(checked_out_object, &object_type_tag));

    char object_type_name[TCTYPE_name_size_c+1] = "";
    IFERR_REPORT(TCTYPE_ask_name(object_type_tag, object_type_name));  

    if (strcmp(object_type_name, "PDF") == 0)
    {
        char *object_string = NULL;
        IFERR_REPORT(WSOM_ask_object_id_string(checked_out_object, &object_string));
        ECHO("       Cancel check out of %s <%s>\n", object_string, object_type_name);
        MEM_free(object_string);
    }
    return ifail;
}

extern DLLAPI int gtac_user_init_module(int *decision, va_list args)
{
    ECHO("\n\n gtac_user_init_module \n\n");
    *decision = ALL_CUSTOMIZATIONS;

    int ifail = ITK_ok;
    METHOD_id_t  method;
	
	RES_init_module();

    int num_custom_methods = 3;
    char *type_name[] = {"ImanReservation", "ImanReservation", "ImanReservation"};
    char *message[] = {"RES_checkout", "RES_checkin", "RES_cancel_checkout"};

    METHOD_function_t custom_method[] = {A2_RES_checkout, A2_RES_checkin, A2_RES_cancel_checkout};

    for(int ii = 0; ii <num_custom_methods; ii++)
    {
        IFERR_REPORT(METHOD_find_method(type_name[ii], message[ii], &method));
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
