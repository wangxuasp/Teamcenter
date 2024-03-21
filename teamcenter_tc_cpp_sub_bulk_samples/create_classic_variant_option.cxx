//#include "gtac.hxx"

#include <bom/bom.h>
#include <tc/tc.h>
#include <tccore/aom.h>

#include <iostream>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;
using Teamcenter::Main::logger;

int passVoid()
{
	int ifail = ITK_ok;
	ResultStatus stat;
	try
	{
		tag_t tRev = NULLTAG;
		stat = ITEM_find_rev("1234567", "A", &tRev);
		if (tRev != NULLTAG)
		{
			stat = AOM_load(tRev);
			stat = AOM_lock(tRev);

			tag_t tNewOpt = NULLTAG;
			tag_t tNewOptRev = NULLTAG;
			tag_t tVarExp = NULLTAG;
			tag_t tVarExpBlk = NULLTAG;
			stat = BOM_new_option(tRev, "WBASE", NULL, BOM_option_mode_enum, &tNewOpt, &tNewOptRev, &tVarExp, &tVarExpBlk);

			int iIndex = 0;
			stat = BOM_add_option_rev_value(tNewOptRev, "242IN", &iIndex);
			stat = BOM_add_option_rev_value(tNewOptRev, "260IN", &iIndex);
			stat = BOM_add_option_rev_value(tNewOptRev, "281IN", &iIndex);

			stat = AOM_save(tNewOptRev);
			stat = AOM_save(tNewOpt);
			stat = AOM_save(tVarExp);
			stat = AOM_save(tVarExpBlk);
			stat = AOM_save(tRev);
			stat = AOM_unlock(tRev);
			stat = AOM_unload(tRev);
		}
		else
			cout << "Item Revision not found!\n" << endl;
	}
	catch (const IFail &ex)
	{
		logger()->error(ex.ifail(), ex.getMessage());
		cout << ex.getMessage() << endl;
		cout << "error " << ex.ifail() << endl;
	}
	return ifail;
}
