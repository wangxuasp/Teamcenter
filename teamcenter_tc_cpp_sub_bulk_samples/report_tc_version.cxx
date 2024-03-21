
#include <iostream>
#include <me/me.h>
#include <tc/emh.h>
#include <tc/tc.h>
#include <tc/tc_startup.h>

#include <tccore\aom_prop.h>

#include <tccore/ItemRevision.hxx>
#include <metaframework/BusinessObjectRef.hxx>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;
using Teamcenter::Main::logger;

int ITK_user_main(int argc, char* argv[])
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
        stat = ITK_initialize_text_services(ITK_BATCH_TEXT_MODE);         
        stat = ITK_auto_login();                                          
        stat = ITK_set_journalling(TRUE);
		
		scoped_smptr<char> full_version;
		stat = ITK_ask_full_version (&full_version);
		cout << endl << " Version: " <<  full_version.get() << endl;   
		
    }
    catch( const IFail &ex )
    {
		logger()->error( ex.ifail(), ex.getMessage());
		cout << ex.getMessage()  << endl;
        cout << "error " << ex.ifail() << endl;        
    }
	
	const char* syslog_path;
	syslog_path = EMH_ask_system_log();
	cout << endl << endl;
	cout << syslog_path << endl; 
	
    stat = ITK_exit_module(FALSE);                                        
    return ITK_ok;
}
