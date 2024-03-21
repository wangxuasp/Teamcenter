

#ifdef __cplusplus
extern "C" {
#endif


#include <time.h>
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

extern DLLAPI int gtac_user_register_properties(int *decision );
extern DLLAPI int gtac_init_user_A2ItemRevision_property(METHOD_message_t* m, va_list args);
extern DLLAPI int a2_runtime_string_base_action_ask(METHOD_message_t *m, va_list args);
static void get_current_date_and_time(char *date_time);
static void ECHO(char *format, ...);

static int report_error(char *file, int line, char *call, int status,
    logical exit_on_error);

extern DLLAPI int libgtac_register_callbacks()
{
    ECHO("\n\n libgtac_register_callbacks \n\n");

    IFERR_REPORT(CUSTOM_register_exit("libgtac", "USER_register_properties", (CUSTOM_EXIT_ftn_t) gtac_user_register_properties));

    return ITK_ok;
}

extern DLLAPI int gtac_user_register_properties(int *decision )
{
    ECHO("\n\n gtac_user_register_properties \n\n");
    *decision = ALL_CUSTOMIZATIONS;
    
    int ifail = ITK_ok;
        
    USER_prop_init_entry_t user_types_methods[] = 
    {
        { "A2ItemRevision" , gtac_init_user_A2ItemRevision_property , NULL },
        { ""                , 0                   , NULL }
    };
 
    int n_types = sizeof(user_types_methods)/sizeof(USER_prop_init_entry_t);
    ifail = TCTYPE_register_properties(user_types_methods, n_types);

    return ifail;
}

extern DLLAPI int gtac_init_user_A2ItemRevision_property(METHOD_message_t* m, va_list args)
{
    ECHO("\n\n gtac_init_user_A2ItemRevision_property \n\n");
    int ifail = ITK_ok;
    tag_t type_tag = va_arg(args, tag_t);
    METHOD_id_t  method;
    char type_name[TCTYPE_name_size_c+1] = "";
    
    if ((ifail = TCTYPE_ask_name(type_tag, type_name)) != ITK_ok) return ifail;

    if ((ifail = METHOD_register_prop_method((const char*)type_name, "a2_runtime_string", PROP_ask_value_string_msg,
        a2_runtime_string_base_action_ask, 0, &method )) != ITK_ok )
    {
        return ifail;
    }


    return(ITK_ok);
}

extern DLLAPI int a2_runtime_string_base_action_ask(METHOD_message_t *m, va_list args)
{
    ECHO("\n\n a2_runtime_string_base_action_ask \n\n");
    int ifail = ITK_ok;

    va_list largs;
    va_copy( largs, args );
    tag_t prop_tag = va_arg( largs, tag_t );

    char** value = va_arg( largs, char**); /* PROP_ask_value_string */

    va_end( largs );
    
    /* get unique const char* for prop value*/
    char current_date_time_str[128 + 1] = {"\0"};
    get_current_date_and_time(current_date_time_str);
    
    *value = (char *)MEM_alloc(sizeof(char) * (strlen(current_date_time_str) + 1));
    strcpy(*value, current_date_time_str);

    return ifail;
}

static void get_current_date_and_time(char *date_time)
{
    time_t the_time;
    struct tm *time_ptr;
    char *time_format = "%Y%m%d-%H%M%S";

    the_time = time((time_t *)0);
    time_ptr = localtime (&the_time);
    strftime(date_time, 128, time_format, time_ptr);
}

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

#ifdef __cplusplus
}
#endif
