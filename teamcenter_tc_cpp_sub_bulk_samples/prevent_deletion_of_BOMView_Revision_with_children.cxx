
/* 
 *  Business Modeler Extension Rule Definition:
 *      Business Object Name: ItemRevision
 *      Business Object or Property: Property
 *      Property Name: structure_revisions
 *      Operation Name: setStructure_revisions
 *      Extension Point: PreCondition
 */
#include <A2gtac/A2_prevent_deletion_of_BOMView_Revision_with_children.hxx>
#include <A4gtac/A4customerrors_error.h>
#include <iostream>

#include <bom/bom_errors.h>
#include <tc/tc.h>
#include <tc/tc_startup.h>
#include <tccore/aom_prop.h>

#include <base_utils/IFail.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <base_utils/TcResultStatus.hxx>

using namespace Teamcenter;
using namespace std;

int A2_prevent_deletion_of_BOMView_Revision_with_children( METHOD_message_t * msg, va_list /*args*/)
{
    printf("\n %s \n", __FUNCTION__);
    int ifail = ITK_ok;
    ResultStatus rstat;
    try
    {
        tag_t rev_tag  = msg->object_tag;

        int n_children = 0;
        scoped_smptr<tag_t> children;
        rstat = AOM_ask_value_tags(rev_tag, "view", &n_children, &children);
        if (n_children > 0)
        {
            EMH_store_error_s1(EMH_severity_error, 950001, 
                "Cannot delete BVR with children");
            ifail = 950001;
        }
    }
    catch( const IFail &e )
    {
        printf("\n\n\n");
        cout << "error " << e.ifail() << endl;
        cout << e.getMessage() << endl;
    }
    return ifail;
}
