
#include <A3_extension_rules/A3_PreAction.hxx>
#include <ug_va_copy.h>
#include <tccore/aom_prop.h>
#include <base_utils/ScopedSmPtr.hxx>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <metaframework/CreateInput.hxx>

#pragma warning(disable: 4100)

using namespace std;
using namespace Teamcenter;

int A3_ManufacturerPart_finalizeCreateInput_PreAction( METHOD_message_t * msg, va_list args)
{
    int ifail = ITK_ok;
    ResultStatus stat;

    try
    {
        va_list largs;
        va_copy( largs, args );
        CreateInput *boInput = va_arg(largs, CreateInput*);
        va_end( largs );

        bool isNull = true;
        tag_t tVendor = NULLTAG;
        stat = boInput->getTag("vm0vendor_ref", tVendor, isNull);

        scoped_smptr<char> spcVendorId;
        stat = AOM_ask_value_string(tVendor, "item_id", &spcVendorId);

        string sItem_id;
        stat = boInput->getString("item_id", sItem_id, isNull);

        string sNewId = sItem_id + "-" + spcVendorId.getString();
        stat = boInput->setString("item_id", sNewId, false);
    }
    catch(IFail & ex)
    {
        return ex.ifail();
    }
    return ifail;
}

