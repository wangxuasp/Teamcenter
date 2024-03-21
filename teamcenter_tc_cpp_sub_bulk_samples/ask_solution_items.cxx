
#include <cm/cm.h>
#include <tccore\workspaceobject.h>

#include <iostream>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;
using Teamcenter::Main::logger;

int ask_solution_items(tag_t tCNRevs)
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
        int nItemRevs = 0;
        scoped_smptr<tag_t> sptItemRevs; 
        stat = CM_ask_solution_items_for_change_item_revision(tCNRevs, &nItemRevs, &sptItemRevs); 
        cout << endl << " Solution Items" << endl;
        for (int ii = 0; ii < nItemRevs; ii++)
        {
            scoped_smptr<char> spcName;
            stat = WSOM_ask_object_id_string(sptItemRevs[ii], &spcName);
            cout << "    " << spcName.get() << endl;
        }
    }
    catch (const IFail &e)
    {
        cout << "error " << e.ifail() << endl;
        cout << e.getMessage() << endl;
    }
    return ifail;
}
