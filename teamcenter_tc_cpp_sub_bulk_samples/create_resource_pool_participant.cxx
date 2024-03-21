#include <iostream>
#include <tc/tc.h>
#include <tc/tc_startup.h>

#include <epm/epm.h>
#include <tccore/item.h>

#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;
using Teamcenter::Main::logger;


int create_resource_pool_participant(tag_t rev_tag, tag_t group, tag_t role)
{
	int ifail = ITK_ok;
	ResultStatus stat;
	try
	{
		int ifail = ITK_ok;
		ResultStatus stat;
		try
		{
			tag_t resource_pool = NULLTAG;
			stat = EPM_get_resource_pool(group, role, FALSE, &resource_pool);

			char type_name[] = "ProposedResponsibleParty";
			tag_t type;
			stat = EPM_get_participanttype(type_name, &type);

			tag_t participant = NULLTAG;
			stat = EPM_create_participant(resource_pool, type, &participant);

			stat = ITEM_rev_set_participants(rev_tag, 1, &participant);
		}
		catch (const IFail &ex)
		{
			logger()->error(ex.ifail(), ex.getMessage());
			cout << ex.getMessage() << endl;
			cout << "error " << ex.ifail() << endl;
		}
		return ifail;
	}
	catch (const IFail &ex)
	{
		logger()->error(ex.ifail(), ex.getMessage());
		cout << ex.getMessage() << endl;
		cout << "error " << ex.ifail() << endl;
	}

	return ifail;
}
