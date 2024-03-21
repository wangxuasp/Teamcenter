#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ug_va_copy.h>
#include <bom/bom.h>
#include <cfm/cfm.h>
#include <ict_userservice.h>
#include <tccore/item.h>
#include <emh.h>
#include <user_exits.h>
#include <custom.h>
#include <method.h>
#include <lov/lov.h>
#include <lov/lov_msg.h>
#include <itk/mem.h>
#include <tccore/aom_prop.h>
#include <tccore/workspaceobject.h>
#include <tc/folder.h>
#include <tccore/tctype.h>
#include <sa/sa.h>

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

static int ask_LOV_length( METHOD_message_t *msg, va_list  args ) 
{
    ECHO("\n ask_LOV_length  \n");
    int ifail = ITK_ok;
	
	/* va_list for LOV_ask_num_of_values_msg */
    tag_t  lov_tag = va_arg(args, const tag_t); 
    int    *length = va_arg(args, int*); 

	*length = 5;

    return ifail; 
}

static int ask_LOV_description_values( METHOD_message_t *msg, va_list  args ) 
{
    ECHO("\n ask_LOV_description_values  \n" ); 

    int ifail = ITK_ok;

    va_list largs;
    va_copy( largs, args );
    tag_t lov_tag = va_arg( largs, tag_t);	
    int *n_desc_strings = va_arg( largs, int*);
    char ***desc_strings = va_arg( largs, char***);
    logical **is_null = va_arg( largs, logical**);
    logical ** is_empty = va_arg( largs, logical**);
    va_end( largs );

	char *value_list[5] =  {"Description A", "Description B", "Description C", 
		"Description D", "Description E"};
	
	*n_desc_strings = 0;
    *desc_strings = NULL;  
    *is_null  = 0; 
    *is_empty = 0;

    *n_desc_strings = 5;
    (*desc_strings) = (char**) MEM_alloc( 5 * sizeof(char*) );  
    *is_null = (logical*) MEM_alloc( 5 * sizeof( logical)); 
    *is_empty = (logical*) MEM_alloc( 5 * sizeof( logical)); 

    for (int ii = 0; ii < *n_desc_strings; ii++)
    {
	    (*desc_strings)[ii] = (char *)
			MEM_alloc( (strlen(value_list[ii]) * sizeof(char) ) + 1);

        tc_strcpy( (*desc_strings)[ii], value_list[ii] );
		(*is_null)[ii] = FALSE; 
        (*is_empty)[ii] = FALSE; 
    } 
    return ifail; 
}

static int ask_LOV_string_values( METHOD_message_t *msg, va_list  args) 
{
    ECHO("\n ask_LOV_string_values  \n");  
    int  ifail = ITK_ok;

    /* va_list for LOV_ask_values_msg */
    tag_t lov_tag = va_arg(args, const tag_t);  
    int*  n_values = va_arg(args, int*); 
    char  ***values = va_arg(args, char***);
  
	char *value_list[5] =  {"Value A", "Value B", "Value C", "Value D", 
		"Value E"};  
	
	*n_values = 0;   
    *values = NULL;   

    *n_values = 5;   
    (*values) = (char**) MEM_alloc( 5 * sizeof(char*) );    

    for (int ii = 0; ii < *n_values; ii++)
    {
	    (*values)[ii] = (char *)
			MEM_alloc( (strlen(value_list[ii]) * sizeof(char) ) + 1);   
        tc_strcpy( (*values)[ii], value_list[ii] );    
    } 
    return ifail; 
}

extern DLLAPI int gtac_custom_exits_user_init_module(int *decision)
{
    ECHO("\n gtac_user_init_module \n");

	*decision = ALL_CUSTOMIZATIONS;
    int ifail = ITK_ok;
	char lov_type[] = "A2ListOfValuesString";
    METHOD_id_t  method;


    if( ifail == ITK_ok )
    {
        ifail = METHOD_register_method(lov_type, LOV_ask_values_msg, 
			&ask_LOV_string_values,  0,&method);
    }

    if( ifail == ITK_ok )
    {
        ifail = METHOD_register_method(lov_type, LOV_ask_num_of_values_msg, 
			&ask_LOV_length,  0, &method);
    }
	
	if( ifail == ITK_ok )
    {
        ifail = METHOD_register_method(lov_type, LOV_ask_value_desc_msg, 
			&ask_LOV_description_values, 0, &method);
    }

    if( ifail != ITK_ok )
    {
        EMH_store_error( EMH_severity_error, ifail );
    }

    return ifail;
}

extern DLLAPI int gtac_custom_exits_register_callbacks()
{
    ECHO("\n gtac_custom_exits_register_callback \n");

    IFERR_REPORT(CUSTOM_register_exit("gtac_custom_exits", "USER_init_module", (CUSTOM_EXIT_ftn_t) gtac_custom_exits_user_init_module));
    return ITK_ok;
}

#ifdef __cplusplus
}
#endif
