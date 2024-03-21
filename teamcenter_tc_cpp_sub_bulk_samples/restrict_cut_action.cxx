/***
*
    Define and Assign Business Modeler IDE extension rule for
        ImanRelation - IMAN_delete - PreCondition
*
*/
#include <iostream>
#include <stdio.h>
#include <tc/emh.h>
#include <tccore/grm.h>
#include <tccore/tctype.h>

#include <Error_Exception.hxx>
#include <base_utils/IFail.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;

int A3_restrict_cut_action( METHOD_message_t * /*msg*/, va_list args)
{
    int ifail = ITK_ok;   
    try
    {   
        ResultStatus stat;

        va_list largs; 
        va_copy( largs, args );
        tag_t relation  = va_arg(largs, tag_t);
        va_end( largs );

        tag_t secondary = NULLTAG;
        stat = GRM_ask_secondary(relation, &secondary);
        
        tag_t type = NULLTAG;
        stat = TCTYPE_ask_object_type(secondary, &type);

        scoped_smptr<char> type_name;
        stat = TCTYPE_ask_name2(type, &type_name);

        logical verdict = FALSE;
        stat = TCTYPE_is_type_of_as_str(type, "ABC Form", &verdict);
        if (verdict == TRUE)
        {
            ifail = PLMS_custom_error;  
            EMH_store_error_s2(EMH_severity_error, ifail, "ABC Form", "Item");
        }
    }
    catch(IFail & ex)
    {
        return ex.ifail();
    }
    return ifail;
}