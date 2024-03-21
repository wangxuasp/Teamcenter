/* 
    Notable ponts:
    - The include of the .hxx files must be outside the extern C linkage
    - %TC_ROOT%\include_cpp should be added to the list of includes
    - libmetaframework.lib should be added to the link list
    
*/
#ifdef __cplusplus
extern "C" {
#endif

#include <gtac.hxx>
#include <tccore/aom.h>
#include <tccore/item.h>
#include <tccore/workspaceobject.h>

#ifdef __cplusplus
}

#endif
#include <tccore/Item.hxx>
#include <metaframework/BusinessObjectRef.hxx>

using namespace std;

static int set_item_description(tag_t itemTag)
{
    int ifail = ITK_ok;
    BusinessObjectRef<Teamcenter::Item> item(itemTag);
    try
    {
        if ((ifail = item->setObject_desc("Modified Description", true)) != ITK_ok )
        {
            return ifail;
        }  
    }
    catch ( IFail &ex )
    {
        return ex;
    }
    ifail = ITEM_save_item(itemTag);
    return ifail;
}


