
#include <iostream>

#include <tc/tc.h>
#include <tc/tc_startup.h>
#include <tccore/tctype.h>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;
using Teamcenter::Main::logger;

int is_object_descendant_by_type_string(tag_t object_tag)
{
	int ifail = ITK_ok;
	ResultStatus stat;
	try
	{
		tag_t type_tag = NULLTAG;
		stat = TCTYPE_ask_object_type(object_tag, &type_tag);

		scoped_smptr<char> type_name;
		stat = TCTYPE_ask_name2(type_tag, &type_name);

		logical verdict = FALSE;
		stat = TCTYPE_is_type_of_as_str(type_tag, "WorkspaceObject", &verdict);
		cout << "\n\t" << type_name;
		if (verdict == TRUE)
		{
			cout << " is a descendant of WorkspaceObject"  << endl;
		}
		else
		{
			cout << " is NOT a descendant of WorkspaceObject" << endl;
		}			
	}
	catch (const IFail &ex)
	{
		logger()->error(ex.ifail(), ex.getMessage());
		cout << ex.getMessage() << endl;
		cout << "error " << ex.ifail() << endl;
	}

	return ITK_ok;
}
