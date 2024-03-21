
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

int is_object_descendant_by_type_tag(tag_t object_1, tag_t object_2)
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
        tag_t type_1 = NULLTAG;
        stat = TCTYPE_ask_object_type(object_1, &type_1);

        tag_t type_2 = NULLTAG;
        stat = TCTYPE_ask_object_type(object_2, &type_2);

        logical verdict = FALSE;
        stat = TCTYPE_is_type_of(type_1, type_2, &verdict);     
        if (verdict == TRUE)
        {
            cout << "object_1 is a descendant of object_2"  << endl;
        }
        else
        {
            cout << "object_1 is NOT a descendant of object_2" << endl;
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
