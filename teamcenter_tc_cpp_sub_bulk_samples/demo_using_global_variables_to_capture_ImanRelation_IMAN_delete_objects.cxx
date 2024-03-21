
/*****************************************************************************/
 * ImanRelation IMAN_delete PreAction  extension rule code
/*****************************************************************************/

#include <A2gtac/A2_ImanRelation_IMAN_delete_PreAction.hxx>
#include <iostream>
#include <ug_va_copy.h>
#include <tccore/grm.h>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>

using namespace std;

/* Global Variables */
tag_t primary_object;
tag_t secondary_object;

int A2_ImanRelation_IMAN_delete_PreAction( METHOD_message_t * /*msg*/, va_list args)
{
    int ifail = ITK_ok;
    ResultStatus rstat;

    va_list largs;
    va_copy( largs, args );
    tag_t relation  = va_arg(largs, tag_t);
    va_end( largs );

    try
    {
        rstat = GRM_ask_primary (relation , &primary_object);
        rstat = GRM_ask_secondary(relation , &secondary_object);
    }
    catch( const IFail &e )
    {
        cout << "error " << e.ifail() << endl;
        cout << e.getMessage() << endl;
    }
    return ifail;
}

/*****************************************************************************/
 * ImanRelation IMAN_delete PostAction extension rule code
/*****************************************************************************/

#include <A2gtac/A2_ImanRelation_IMAN_delete_PostAction.hxx>
#include <ug_va_copy.h>
#include <iostream>
#include <ug_va_copy.h>
#include <itk/mem.h>
#include <tccore/workspaceobject.h>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>

using namespace std;

/* Global Variables */
extern tag_t primary_object;
extern tag_t secondary_object;

int A2_ImanRelation_IMAN_delete_PostAction( METHOD_message_t * /*msg*/, va_list args)
{
    int ifail = ITK_ok;
    ResultStatus rstat;

    va_list largs;
    va_copy( largs, args );
    tag_t relation  = va_arg(largs, tag_t);
    va_end( largs );

    try
    {
        char *object_string = NULL;
        rstat = WSOM_ask_object_id_string(primary_object, &object_string);
        printf("    primary_object: %s    \n", object_string);

        rstat = WSOM_ask_object_id_string(secondary_object, &object_string);
        printf("    secondary_object: %s    \n", object_string);

        MEM_free(object_string);
    }
    catch( const IFail &e )
    {
        cout << "error " << e.ifail() << endl;
        cout << e.getMessage() << endl;
    }
    return ifail;
}