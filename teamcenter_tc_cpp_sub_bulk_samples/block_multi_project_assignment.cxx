/*
 *	Define and Assign Extension Rule
 *    WorkspaceObject - fnd0assignObjectsToProjects - PreCondition
 */

#include <tccore/project_errors.h>
#include <tccore/project.h>
#include <tccore/workspaceobject.h>
#include <Error_Exception.hxx>
#include <base_utils/IFail.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <metaframework/BulkInput.hxx>
#include <metaframework/BulkData.hxx>

#pragma warning(disable: 4189)

using namespace std;
using namespace Teamcenter;

int block_multi_project_assignment(METHOD_message_t * /*msg*/, va_list args)
{
    int ifail = ITK_ok;
    
    try
    { 
		ResultStatus stat;
		
        va_list largs;
        va_copy( largs, args );
        vector<BulkInput*> *pInputs = va_arg(largs, vector<BulkInput*>*);
        BulkData* pBulkDatas = va_arg(largs, BulkData* );
        bool alwaysPropagate = va_arg( largs, logical);      
        va_end( largs );
		
		for(std::vector<Teamcenter::BulkInput*>::iterator iter = pInputs->begin(); iter != pInputs->end(); ++ iter)
		{
			BulkInput* inputObj = *iter;
			
			vector<tag_t> projectTagArray;
			vector<int> isNullArray;
			ifail = inputObj->getTagArray("fnd0projectTagArray", projectTagArray, isNullArray);
			
			int nProjects =  projectTagArray.size();
			TC_write_syslog("\n\n No. Projects: %d \n\n", nProjects);
			if (nProjects == 1)  
			{	
				/*** Must only be one to get here ***/
				tag_t project_tag = projectTagArray[0];
				
				scoped_smptr<char> scpName;
				stat = PROJ_ask_name2(project_tag, &scpName);
				TC_write_syslog("    Project Name: %s \n", scpName.get());
				
				tag_t objectTag = NULLTAG;
				bool isNull = false;
				if( ifail == ITK_ok && !isNull)
				{
					stat = inputObj->getTag( "fnd0ObjectTag", objectTag, isNull );
					
					scoped_smptr<char> scpObjectId;
					stat = WSOM_ask_object_id_string(objectTag, &scpObjectId);
					TC_write_syslog("    Object ID: %s \n", scpObjectId.get());
				}

				if(ifail != ITK_ok)
				{
					stat = inputObj->setInt( "fnd0ErrorCode", ifail, false);
				}
			}
			else
			{
				ifail = PROJECT_assign_to_proj_denied;
				EMH_store_error_s1(EMH_severity_error, ifail, "ONLY ONE PROJECT ALLOWED");
			}
		}
    }
    catch( const IFail& ex )
    { 
        ifail = ex.ifail();
    } 
    return ifail;
}
