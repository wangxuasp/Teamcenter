#include <iostream>
#include <stdio.h>
#include <sa/group.h>
#include <sa/role.h>
#include <tc/emh.h>
#include <tcinit/tcinit.h>
#include <tccore/aom.h>

#include <Error_Exception.hxx>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;

static int add_role_to_group()
{
	int ifail = ITK_ok;
    try
    {   
		ResultStatus stat;
	
		tag_t tGroup  = NULLTAG;
		stat = SA_find_group("system", &tGroup);
		
		tag_t tRole  = NULLTAG;
		stat = SA_find_role2("Designer", &tRole);
		
		stat = AOM_refresh(tGroup, true);
		stat = SA_add_role_to_group(tGroup, tRole);
		stat = AOM_save(tGroup);
		stat = AOM_unlock(tGroup);			
	}
    catch(IFail & ex)
    {
        return ex.ifail();
    }		
	return ifail;
}
