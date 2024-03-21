/*
	%TC_ROOT%\sample\compile -DIPLIB=none gtac_custom_exits.cxx
	%TC_ROOT%\sample\link_custom_exits libgtac

	Create or edit site preference TC_customization_libraries=libgtac 
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ug_va_copy.h>

#include <tc/emh.h>
#include <epm/epm.h>
#include <ict/ict_userservice.h>
#include <pom/pom/pom.h>
#include <property/prop_errors.h>
#include <tc/emh.h>
#include <tc/folder.h>
#include <tc/tc.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/custom.h>
#include <tccore/grm.h>
#include <tccore/grmtype.h>
#include <tccore/grm.h>
#include <tccore/item.h>
#include <tccore/method.h>
#include <tccore/tctype.h>
#include <tccore/workspaceobject.h>
#include <user_exits/user_exits.h>

extern DLLAPI int libgtac_register_callbacks();
extern DLLAPI int gtac_user_validate_item_rev_id(int *decision, va_list args);
static void get_current_date_and_time(char *date_time);

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


extern DLLAPI int libgtac_register_callbacks()
{
    ECHO("\n\n libgtac_register_callbacks \n\n");
	IFERR_REPORT(CUSTOM_register_exit("libgtac", "USER_validate_item_rev_id", gtac_user_validate_item_rev_id));
    return ITK_ok;
}

extern DLLAPI int gtac_user_validate_item_rev_id(int *decision, va_list args)
{
    ECHO("\n\n gtac_user_validate_item_rev_id  \n\n");

    int ifail = ITK_ok; 
    *decision  = ALL_CUSTOMIZATIONS;

    va_list largs;
    va_copy( largs, args );
    /***********  va_list for USER_validate_item_rev_id ***********/
    const char *item_id = va_arg(largs, const char *);  
    const char *rev_id = va_arg(largs, const char *); 
    const char *item_type = va_arg(largs, const char *);  
    char **modified_item_id = va_arg(largs, char **); 
    char **modified_rev_id = va_arg(largs, char **); 
    IDValid_e *status = va_arg(largs, IDValid_e *);  
    /*****************************************************/
    va_end( largs );
    
    if(stricmp(item_id,"changeme") == 0)
    { 
        char current_date_time_str[128 + 1] = {"\0"};
        get_current_date_and_time(current_date_time_str);

        char id_str[1024] = {"\0"};
        sprintf(id_str, "%s-%s", item_type, current_date_time_str);

        *modified_item_id = (char*) MEM_alloc( (strlen(id_str) + 1) * sizeof(char) );
        strcpy(*modified_item_id, id_str);

        *modified_rev_id = (char*) MEM_alloc( (strlen("--") + 1) * sizeof(char) );
        strcpy(*modified_rev_id, rev_id);

        *status = USER_override_id;
	}
    return ifail;
}

static void get_current_date_and_time(char *date_time)
{
    time_t
        the_time;
    struct tm
        *time_ptr;
    char
        *time_format = "%Y%m%d-%H%M%S";

    the_time = time((time_t *)0);
    time_ptr = localtime (&the_time);
    strftime(date_time, 128, time_format, time_ptr);
}

#ifdef __cplusplus
}
#endif
