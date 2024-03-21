/*
    Call NR_next_rev_options is for next id generated for a particular 
    pattern in the Revision Naming Rule.  

    Call NR_next_value for next id generated for a particular pattern
    in the Naming Rule.  Which can be for item_id OR item_revison_id.
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

int get_revision_naming_rule_next_rev_id(tag_t item_tag, char **next_rev_id)
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
        tag_t item_type_tag = NULLTAG;
        stat = TCTYPE_ask_object_type(item_tag, &item_type_tag);

        char *item_type_name = NULL;
        stat = TCTYPE_ask_name2(item_type_tag, &item_type_name);

        tag_t latest_rev_tag = NULLTAG;
        stat = ITEM_ask_latest_rev(item_tag, &latest_rev_tag);

        char *sec_rev_option = NULL;
        char *suppl_rev_option = NULL;

        stat = NR_next_rev_options(item_type_name, latest_rev_tag, next_rev_id, &sec_rev_option, &suppl_rev_option);
        
        /* Workaround for PR-9636836 */
        *next_rev_id = (char *)MEM_alloc(sizeof(char) * (tc_strlen(init_rev_option) + 1));
        tc_strcpy(*next_rev_id, strtok(init_rev_option, ",") );

        if (item_type_name)  MEM_free(item_type_name);
        if (sec_rev_option)  MEM_free(sec_rev_option);
        if (suppl_rev_option)  MEM_free(suppl_rev_option);
    }
    catch (const IFail &ex)
    {
        logger()->error(ex.ifail(), ex.getMessage());
        cout << ex.getMessage() << endl;
        cout << "error " << ex.ifail() << endl;
    }
    return ITK_ok;
}
