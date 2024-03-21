
#include <schmgt/schmgt_bridge_itk.h>
#include <schmgt/schmgt_itk.h>
#include <tccore/aom.h>
#include <tccore/workspaceobject.h>
#include <tc/tc.h>
#include <tc/tc_startup.h>

#include <iostream>
#include <base_utils/TcResultStatus.hxx>
#include <base_utils/IFail.hxx>

using namespace std;

int updateSchedule(tag_t tSchedule)
{
	int ifail = ITK_ok;
	ResultStatus status;
	try
	{
		char *name = NULL;
		status = WSOM_ask_name2(tSchedule, &name);
		cout << "\n    Schedule Name:  " << name << "\n" << endl;
		
		ObjectUpdateContainer_t objCnt;
		objCnt.object = tSchedule;
		objCnt.updatesSize = 1;   
		objCnt.updates[0].attrName = "fnd0allowExecUpdates";
		objCnt.updates[0].attrValue = "true";
		objCnt.updates[0].attrType = 5;  

		status = AOM_refresh(tSchedule, TRUE);
		int numUpdatedTasks = 0;
		tag_t *updatedTasks = NULL;  
		status = SCHMGT_update_schedule_non_interactive(objCnt, &numUpdatedTasks, &updatedTasks);
		status = AOM_save(tSchedule);

	}
    catch( const IFail &e )
    {
        ifail = e.ifail();
        cout << "\n    error " << ifail << endl;
        cout << "    "<< e.getMessage() <<endl;
    }	
    return ifail;
}

