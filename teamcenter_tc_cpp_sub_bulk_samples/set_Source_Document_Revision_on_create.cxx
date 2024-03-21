
#include <ug_va_copy.h>
#include <tccore/aom_prop.h>

#include <metaframework/BusinessObjectRef.hxx>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;
using Teamcenter::Main::logger;

int A3_ADSPart_finalizeCreateInput_PreCondition( METHOD_message_t * msg, va_list args)
{
	int ifail = ITK_ok;
    ResultStatus stat;

    try
    {
        va_list largs;
        va_copy( largs, args );
        Teamcenter::CreateInput *boInput = va_arg(largs, Teamcenter::CreateInput*);
        va_end( largs );

        tag_t tRevInput = NULLTAG;
        bool isNull = false;
        stat = boInput->getTag("revision", tRevInput, isNull);

        scoped_smptr<char> revid;
        stat = AOM_ask_value_string(tRevInput, "item_revision_id", &revid);

        stat = boInput->setString("adssource_doc_revision", revid.getString(), isNull);
    }
    catch(IFail & ex)
    {
        return ex.ifail();
    }
    return ifail;
}
