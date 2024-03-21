/*
 *     Extension Name="A3_ITEM_create_rev_Refactored"
 *     ItemRevision - IMAN_save - PostAction
 */
#include <ug_va_copy.h>
#include <epm/epm.h>
#include <epm/epm_task_template_itk.h>
#include <form/form.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/grm.h>
#include <tccore/item.h>
#include <tccore/item_errors.h>
#include <tccore/workspaceobject.h>
#include <base_utils/IFail.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <tccore/ItemRevision.hxx>
#include <metaframework/BusinessObjectRef.hxx>

using namespace std;
using namespace Teamcenter;

int A3_ITEM_create_rev_Refactored( METHOD_message_t * /*msg*/, va_list args)
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
        /* va_list for IMAN_save_msg */
        va_list largs;
        va_copy( largs, args );
        tag_t  object_tag  = va_arg(largs, tag_t);
        logical  isNew = va_arg(largs, logical);
        va_end( largs );

        /* Parameters for ITEM_create_rev_msg */
        tag_t item_tag = NULLTAG;
        char  rev_id[32] = ""; /* length limit defined in BMIDE */
        tag_t rev_tag = object_tag;
        tag_t rev_master_form_tag = NULLTAG;

        if(isNew == TRUE) /* only execute on create */
        {
            BusinessObjectRef<ItemRevision> boRev(rev_tag);
            string sName;
            bool isNull = false;
            boRev->getObject_name(sName, isNull);

            if (strcmp(sName.c_str(), "return error") != 0)
            {
                boRev->getItems_tag(item_tag, isNull);

                string sValue;
                boRev->getItem_revision_id(sValue, isNull);
                strcpy(rev_id, sValue.c_str());

                stat = boRev->getTag("item_master_tag", rev_master_form_tag, isNull);
            }
            else
            {
                ifail = PLMS_custom_error;
                EMH_store_error_s1(EMH_severity_error, ifail, "Custom Error Message");
            }
        }
    }
    catch(IFail & ex)
    {
        return ex.ifail();
    }
    return ifail;
}
