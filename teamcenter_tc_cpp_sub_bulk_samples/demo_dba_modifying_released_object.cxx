#include <iostream>

#include <tc/tc.h>
#include <tc/tc_startup.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/item.h>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;
using Teamcenter::Main::logger;

int set_desc_on_releashed_wso(tag_t tObj)
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
        /* Must be dba member to use this API */
        stat = POM_set_env_info( POM_bypass_access_check, TRUE, 0, 0.0, NULLTAG, "" );
        
        stat = AOM_lock(tObj);
        stat = AOM_set_value_string(tObj, "object_desc", "New Value");
        stat = AOM_save(tObj);
        stat = AOM_unlock(tObj);
        
        /* Must be dba member to use this API */
        stat = POM_set_env_info( POM_bypass_access_check, FALSE, 0, 0.0, NULLTAG, "" );
    }
    catch (const IFail &ex)
    {
        logger()->error(ex.ifail(), ex.getMessage());
        cout << ex.getMessage() << endl;
        cout << "error " << ex.ifail() << endl;
    }

    return ITK_ok;
}

