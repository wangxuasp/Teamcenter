/*****************************************************************************

In the Business Modeler IDE (BMIDE)
	Create Custom ListOfValuesString
		Name: A7_ListOfValuesString
		
	Create Custom Classic LOV
		Name: A7_LegacyDynamicGroupNames
		Type: A7_ListOfValuesString
		
	Create Custom Form
		A7_User_Data (Custom Form)
			Add Perstistent Property 
				Name: a7_group_names_legacy
				Display Name: Group Names (Legacy LOV)
				Attribute Type: String
				String Length: 128	
				
*****************************************************************************/

#include <stdlib.h>
#include <stdarg.h>

#include <tccore/aom.h>
#include <tccore/custom.h>
#include <tc/emh.h>
#include <ict/ict_userservice.h>
#include <tc/tc.h>
#include <tccore/tctype.h>
#include <sa/tcfile.h>
#include <lov/lov.h>
#include <lov/lov_msg.h>
#include <itk/mem.h>
#include <ss/ss_errors.h>
#include <sa/user.h>

#define ECHO(X)  printf X; TC_write_syslog X

#define IFERR_ABORT(X)  (report_error( __FILE__, __LINE__, #X, X, TRUE))
#define IFERR_REPORT(X) (report_error( __FILE__, __LINE__, #X, X, FALSE))
#define IFERR_RETURN(X) if (IFERR_REPORT(X)) return

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

        EMH_ask_errors( &n_errors, &severities, &statuses, &messages );
        if (n_errors > 0)
        {
            ECHO(("\n%s\n", messages[n_errors-1]));
            EMH_clear_errors();
        }
        else
        {
            char *error_message_string;
            EMH_get_error_string (NULLTAG, status, &error_message_string);
            ECHO(("\n%s\n", error_message_string));
        }

        ECHO(("error %d at line %d in %s\n", status, line, file));
        ECHO(("%s\n", call));

        if (exit_on_error)
        {
            ECHO(("\nExiting program!\n"));
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




static int GTAC_LOV_ask_num_of_values( METHOD_message_t *msg, va_list  args ) 
{
    /* va_list for LOV_ask_num_of_values_msg */
    tag_t  lov_tag = va_arg(args, const tag_t); 
    int    *length = va_arg(args, int*); 

    int  
        error_code= ITK_ok,  
        n_entries = 2, 
        n_found = 0;
    tag_t 
        query = NULLTAG, 
        *dba_users = NULL;
    char
        *qry_entries[2] = {"Group", "Role"},
        *qry_values[2] =  {"dba", "DBA"};

    ECHO(("\n\n GTAC_LOV_ask_num_of_values\n\n"));

    IFERR_REPORT(QRY_find("Admin - Group/Role Membership", &query));
    IFERR_REPORT(QRY_execute(query, n_entries, qry_entries, qry_values, &n_found, &dba_users));

    *length = n_found;

    GTAC_free(dba_users);

    return error_code; 
}

static int GTAC_LOV_ask_values( METHOD_message_t *msg, va_list  args ) 
{
    /* va_list for LOV_ask_values_msg */
    tag_t lov_tag  = va_arg(args, const tag_t); 
    int*  n_values = va_arg(args, int*); 
    char  ***values = va_arg(args, char***); 
  
    int  
        error_code = ITK_ok, 
        n_entries = 2, 
        n_found = 0,
        ii = 0;
    tag_t 
        query = NULLTAG, 
        *dba_users = NULL,
        user = NULLTAG;
    char
        *qry_entries[2] = {"Group", "Role"},
        *qry_values[2] =  {"dba", "DBA"},
        person_name[SA_name_size_c+1] = "";

    
    IFERR_REPORT(QRY_find("Admin - Group/Role Membership", &query));
	query = NULLTAG;
    IFERR_REPORT(QRY_execute(query, n_entries, qry_entries, qry_values, 
		&n_found, &dba_users));
    ECHO(("\t n_found = %d\n", n_found));

    *n_values = n_found; 
    ECHO(("\t n_values = %d\n", *n_values));

    *values = (char **)MEM_alloc (*n_values * sizeof(char*));
    memset( *values, 0,  *n_values * sizeof(char*));

    for (ii = 0; ii < n_found; ii++) 
    { 
        IFERR_REPORT(SA_ask_groupmember_user(dba_users[ii], &user));
        IFERR_REPORT(SA_ask_user_person_name(user, person_name));
        (*values)[ii] = (char *) MEM_alloc (strlen(person_name) + 1);
        strcpy((*values)[ii], person_name);
    } 
    GTAC_free(dba_users);

    return error_code; 
}

extern DLLAPI int GTAC_register_method(int *decision)
{
    int          ifail = ITK_ok;
    METHOD_id_t  method;

	*decision = ALL_CUSTOMIZATIONS;

    ECHO(("\n GTAC_register_method \n"));

    if( ifail == ITK_ok )
    {
        ifail = METHOD_register_method( "GTAC_ListOfValuesString",
            LOV_ask_values_msg,
            &GTAC_LOV_ask_values,  0,
            &method);
    }

    if( ifail == ITK_ok )
    {
        ifail = METHOD_register_method("GTAC_ListOfValuesString", 
            LOV_ask_num_of_values_msg, &GTAC_LOV_ask_num_of_values,
			0, &method);
    }

    if( ifail != ITK_ok )
    {
        EMH_store_error( EMH_severity_error, ifail );
    }
    return ifail;
}


extern DLLAPI int GTAC_register_callbacks ()
{
    ECHO(("\n\n---- GTAC_register_callbacks -----\n\n"));
    IFERR_ABORT(CUSTOM_register_exit ( "GTAC", "USER_init_module", 
        (CUSTOM_EXIT_ftn_t)  GTAC_register_method));

	return ( ITK_ok );
}
