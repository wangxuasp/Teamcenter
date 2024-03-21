
#include <sa/user.h>
#include <tccore/aom.h>
#include <tc/folder.h>
#include <tc/tc.h>
#include <tc/tc_startup.h>

#include <iostream>
#include <base_utils/TcResultStatus.hxx>
#include <base_utils/IFail.hxx>
#include <metaframework/BusinessObjectRef.hxx>
#include <tc/Folder.hxx>
#include <tc/LoggedInUser.hxx>

using namespace Teamcenter;
using namespace Core;
using namespace std;

int createFolder()
{
    int ifail = ITK_ok;
	ResultStatus status;
	try
	{
		char typeName[] = "Folder";
		CreateInput* pFoldCreInput = static_cast<CreateInput*>
			(BusinessObjectRegistry::instance().
					createInputObject(typeName, OPERATIONINPUT_CREATE));

		bool isNull = false;
		status = pFoldCreInput->setString("object_name", "MyFolder", isNull);
		
	    Folder* pFolder = static_cast<Folder*> 
	    	(BusinessObjectRegistry::instance().
	    			createBusinessObject(pFoldCreInput));

	    tag_t folderTag = pFolder->getTag();

	    status = AOM_save_with_extensions(folderTag);

	    LoggedInUser* loggedInUser = LoggedInUser::getInstance();
	    tag_t userTag = loggedInUser->getCurrentUserTag();

	    tag_t homeFolderTag = NULLTAG;
	    status = SA_ask_user_home_folder(userTag, &homeFolderTag);

	    status = FL_insert(homeFolderTag, folderTag, 999);
	    status = AOM_save_with_extensions(homeFolderTag);
	}
	catch( const IFail &e )
	{
		ifail = e.ifail();
		cout << "\n    error " << ifail << endl;
		cout << "    "<< e.getMessage() <<endl;
	}	
    return ifail;
}
