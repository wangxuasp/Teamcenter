/* 
*  Create custom Item A3_NRDummy/A3_NRDummyRevision.
*
*  Create A3_NamingRule with a counter
*       Pattern: nnnnnn
*       Initial Value: 200000
*       Maxium Value: 999999
*
*  Attached the naming rule to A3_NRDummy.item_id
*
*  Create A3Item/A3ItemRevision.
*
*  Add four custom required properties.
*       A3Item.a3_Prefix1
*       A3Item.a3_Prefix2
*       A3ItemRevision.a3_Prefix3
*       A3ItemRevision.a3_Prefix4
*
*  Add all four properties are added to the Create Operation for the
*  corresponding business object type.
*
*  Define and assign a A3_Item:create#Teamcenter::CreateInput,*:PreAction.
*
*  SaveAs ItemCreate XMLRenderingStylesheet to A3ItemCreate.
*  To this XMLRenderingStylesheet I add these lines:
*       <property name="a3_Prefix1" />
*       <property name="a3_Prefix2" />
*       <property name="revision:a3_Prefix3" />
*       <property name="revision:a3_Prefix4" />
*
*/
#include <A3_extension_rules/A3_A3Item_create_PreAction.hxx>
#include <ug_va_copy.h>
#include <iostream>
#include <property/nr.h>
#include <tc/emh.h>
#include <tc/tc.h>
#include <tc/tc_startup.h>
#include <tccore/aom_prop.h>

#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <metaframework/CreateInput.hxx>
#include <mld/logging/TcMainLogger.hxx>

#pragma warning(disable: 4189)

using namespace std;
using namespace Teamcenter;
using Teamcenter::Main::logger;

int A3_A3Item_create_PreAction( METHOD_message_t * msg, va_list args)
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
        va_list largs;
        va_copy( largs, args );
        Teamcenter::CreateInput *itemCreInput = va_arg(largs, Teamcenter::CreateInput*);
        va_end( largs );

        /* Get the new ItemRevision CreateInput tag */
        tag_t revCreInputTag = NULLTAG;
        bool isNull = false;
        stat = itemCreInput->getTag("revision", revCreInputTag, isNull);

        string sPrefix1;
        stat = itemCreInput->getString("a3_Prefix1", sPrefix1, isNull);

        string sPrefix2;
        stat = itemCreInput->getString("a3_Prefix2", sPrefix2, isNull);

        scoped_smptr<char> spPrefix3;
        stat = AOM_ask_value_string(revCreInputTag, "a3_Prefix3", &spPrefix3);
        string sPrefix3 = spPrefix3.get();

        scoped_smptr<char> spcPrefix4;
        stat = AOM_ask_value_string(revCreInputTag, "a3_Prefix4", &spcPrefix4);
        string sPrefix4 = spcPrefix4.get();

        scoped_smptr<char> spcNextID;
        stat = NR_next_value("A3_NRDummy", "item_id", NULLTAG,"", "", "", NULLTAG, ""," ", &spcNextID);
        string sNextID = spcNextID.get();

        string sItem_id = sPrefix1 + sPrefix2 + sPrefix3 + sPrefix4 + sNextID;
        itemCreInput->setString("item_id", sItem_id, false);
    }
    catch( const IFail &e )
    {
        cout << "error " << e.ifail() << endl;
        cout << e.getMessage() << endl;
    }
    return ifail;
}
