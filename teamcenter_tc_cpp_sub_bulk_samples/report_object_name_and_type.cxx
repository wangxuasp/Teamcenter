#include <iostream>

#include <tc/tc.h>
#include <tc/tc_startup.h>
#include <tccore/aom.h>
#include <tccore/tctype.h>
#include <tccore/workspaceobject.h>

#include <base_utils/IFail.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;

using Teamcenter::Main::logger;

int report_object_name_and_type(tag_t tObject)
{
	int ifail = ITK_ok;
	ResultStatus stat;
	try	
	{
		tag_t tType = NULLTAG;
		stat = TCTYPE_ask_object_type(tObject, &tType);

		scoped_smptr<char> spcType;
		stat = TCTYPE_ask_name2(tType, &spcType);

		logical is_WorkspaceObject = FALSE;
		stat = TCTYPE_is_type_of_as_str(tType, "WorkspaceObject", &is_WorkspaceObject);

		scoped_smptr<char> spcName;
		if (is_WorkspaceObject == TRUE)
		{
			stat = WSOM_ask_object_id_string(tObject, &spcName);
		}
		else
		{
			stat = AOM_ask_name(tObject, &spcName);
		}
		printf("\t Name: %s - Type: %s \n", spcName.get(), spcType.get() );
	}
	catch (const IFail &ex)
	{
		logger()->error(ex.ifail(), ex.getMessage());
		cout << ex.getMessage() << endl;
		cout << "error " << ex.ifail() << endl;
	}
	return ifail;
}
