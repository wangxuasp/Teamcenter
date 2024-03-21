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

extern int  gtac_ask_date_value( METHOD_message_t *  m, va_list  args )
{
    ECHO("\t\t gtac_ask_date_value \n");

	int ifail = ITK_ok;
    tag_t prop_tag = va_arg( args, tag_t );
    date_t  *value = va_arg(args, date_t*);

	tag_t task = m->object_tag;
	date_t test_date = NULLDATE;  
	IFERR_REPORT(ITK_string_to_date( "14-Feb-2006 00:00", &test_date) );

   *value = test_date;

   return ifail;
}

extern int gtac_init_runtime_property(METHOD_message_t* m, va_list args)
{
    ECHO( "\t\t gtac_init_runtime_property \n   "); 

    int ifail = ITK_ok;
    METHOD_id_t  method;

    tag_t type_tag = va_arg(args, tag_t);
    char type_name[TCTYPE_name_size_c+1] = ""; 
    if ((ifail = TCTYPE_ask_name(type_tag, type_name)) != ITK_ok) 
		return ifail;

    if ((ifail = METHOD_register_prop_method((const char*)type_name, 
		"a2_runtime_date",  PROP_ask_value_date_msg, gtac_ask_date_value, 0, 
		&method )) != ITK_ok )
    {
	    return ifail;
    }
    return ifail;
}

extern int gtac_register_runtime_properties(int *decision )
{

    int ifail = ITK_ok;
        
    USER_prop_init_entry_t user_types_methods[] = 
    {
        { "EPMTask" , gtac_init_runtime_property , NULL },
        { ""          	    , 0 			  	  , NULL }
    };
 
    int n_types = sizeof(user_types_methods)/sizeof(USER_prop_init_entry_t);

    printf( "\t gtac_register_properties\n");

    *decision = ALL_CUSTOMIZATIONS;  /* remove this line if not custom exits */ 
    /*
     * Do not edit this call. This call is not to be used elsewhere.
     */
    ifail = TCTYPE_register_properties(user_types_methods, n_types);

    return ifail;
}

extern DLLAPI int gtac_custom_exits_register_callbacks()
{
    ECHO("\n gtac_custom_exits_register_callback \n");

    IFERR_REPORT(CUSTOM_register_exit("gtac_custom_exits", 
		"USER_register_properties", 
		(CUSTOM_EXIT_ftn_t) gtac_register_runtime_properties));
    return ITK_ok;
}

#ifdef __cplusplus
}
#endif
