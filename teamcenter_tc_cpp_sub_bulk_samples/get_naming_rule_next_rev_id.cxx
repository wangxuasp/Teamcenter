/*
    Call NR_next_value for next id generated for a particular pattern
    in the Naming Rule.  Which can be for item_id OR item_revison_id.

    Call NR_next_rev_options is for next id generated for a particular 
    pattern in the Revision Naming Rule.  

*/

#include <iostream>

#include <property/nr.h>
#include <tc/tc.h>
#include <tc/tc_startup.h>
#include <tccore/item.h>
#include <tccore/tctype.h>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using Teamcenter::Main::logger;

int get_naming_rule_next_rev_id(tag_t item_tag, char **next_rev_id)
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
        tag_t item_type_tag = NULLTAG;
        stat = TCTYPE_ask_object_type(item_tag, &item_type_tag);

        char *item_type_name = NULL;
        stat = TCTYPE_ask_name2(item_type_tag, &item_type_name);

        stat = NR_next_value(item_type_name, "item_revision_id", item_tag, "", "", "", NULLTAG, "", "", next_rev_id);
        if (item_type_name)  MEM_free(item_type_name);
    }
    catch (const IFail &ex)
    {
        logger()->error(ex.ifail(), ex.getMessage());
        cout << ex.getMessage() << endl;
        cout << "error " << ex.ifail() << endl;
    }
    return ITK_ok;
}
