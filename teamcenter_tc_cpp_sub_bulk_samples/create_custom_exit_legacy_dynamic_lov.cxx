/*****************************************************************************

Execute in a Teamcenter commandline shell:
    preferences_manager -u=infodba -p=infodba -mode=import -action=merge -preference=TC_customization_libraries -scope=SITE -values=libgtac
    
In the Business Modeler IDE (BMIDE)

    Create Custom ListOfValuesString
        Name: A7_CustomExitLOVString
        
    Create Custom Classic LOV
        Name: A7_CustomExitLegacyDynUserNames
        Type: A7_CustomExitLOVString
        
    Create Custom Form
        Name: A7_User_Data
            Add Persistent Property 
                Name: a7_user_names_legacy
                Display Name: User Names (Custom Exit Legacy Dynamic LOV)
                Attribute Type: String
                String Length: 128
            Attach A7_CustomExitLegacyDynUserNames to Property
                
*****************************************************************************/
#include <stdarg.h>
#include <stdio.h>
#include <iostream>
#include <ug_va_copy.h>
#include <ict/ict_userservice.h>
#include <lov/lov.h>
#include <lov/lov_msg.h>
#include <qry/qry.h>
#include <tc/emh.h>
#include <tc/tc.h>
#include <tc/tc_startup.h>
#include <tccore/aom_prop.h>
#include <tccore/custom.h>
#include <tccore/method.h>

#include <metaframework/BusinessObjectRef.hxx>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;

static int register_LOV_methods(int *decision, va_list args);
static int ask_dba_user_names(METHOD_message_t *m, va_list args);

extern "C" DLLAPI int libgtac_register_callbacks()
{ 
	printf("\n %s \n", __FUNCTION__);
    CUSTOM_register_exit("libgtac", "USER_gs_shell_init_module", (CUSTOM_EXIT_ftn_t) register_LOV_methods);
    return ITK_ok;
}

int register_LOV_methods(int *decision, va_list args)
{
	printf("\n %s \n", __FUNCTION__);
    int          ifail = ITK_ok;
    METHOD_id_t  method;

    *decision = ALL_CUSTOMIZATIONS;

    if( ifail == ITK_ok )
    {
        ifail = METHOD_register_method( "A7_CustomExitLOVString", LOV_ask_values_msg, &ask_dba_user_names, 0, &method);
    }

    if( ifail != ITK_ok )
    {
        EMH_store_error( EMH_severity_error, ifail );
    }

    return ifail;
}

int ask_dba_user_names(METHOD_message_t *m, va_list args)
{
	printf("\n %s \n", __FUNCTION__);
    int ifail = ITK_ok;
    ResultStatus stat;
    
    /* va_list for LOV_ask_values_msg */
    va_list largs;
    va_copy( largs, args );
    tag_t lov_tag  = va_arg(largs, const tag_t); 
    int*  n_values = va_arg(largs, int*); 
    char  ***values = va_arg(largs, char***); 
    va_end( largs );
    
	scoped_smptr<char> lov_name;
	stat = LOV_ask_name2(lov_tag, &lov_name);
	

	tag_t query = NULLTAG;
	stat = QRY_find2("Admin - Group/Role Membership", &query);
	QRY_find2("Admin - Group/Role Membership", &query);
	
	int n_entries = 2;
	char *qry_entries[2] = {"Group", "Role"}, *qry_values[2] = {"dba", "DBA"};  
	int n_found = 0;
	scoped_smptr<tag_t> dba_mbrs;
	stat = QRY_execute(query, n_entries, qry_entries, qry_values, &n_found, &dba_mbrs);

	*n_values = n_found; 
	*values = (char **)MEM_alloc (*n_values * sizeof(char*));
	memset( *values, 0,  *n_values * sizeof(char*));

	for (int ii = 0; ii < n_found; ii++) 
	{
		scoped_smptr<char> name;
		stat = AOM_ask_value_string(dba_mbrs[ii], "user_name", &name);
		(*values)[ii] = (char *) MEM_alloc (strlen(name.get()) + 1);
		strcpy((*values)[ii], name.get());
	}
    return ifail;
}