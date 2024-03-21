/*
     %TC_ROOT%\sample\compile -DIPLIB=none test.c
     %TC_ROOT%\sample\linkitk -o test test.obj
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <tccore/aom.h>
#include <tccore/item.h>
#include <tccore/workspaceobject.h>

#ifdef __cplusplus
}
#endif

#include <tccore/ItemRevision.hxx> 
#include <metaframework/BusinessObjectRef.hxx>
#include <metaframework/BusinessObjectRegistry.hxx>
#include <metaframework/ReviseInput.hxx>

using namespace std;

static tag_t demo_ItemRevision_revise_operation(tag_t item_tag)
{  
    tag_t latestRevTag  = NULLTAG;
    IFERR_REPORT(ITEM_ask_latest_rev(item_tag, &latestRevTag));
    
    char revTypeName[WSO_name_size_c + 1] =""; 
    IFERR_REPORT(WSOM_ask_object_type(latestRevTag, revTypeName));
    
    std::string reviseOp = OPEARTIONINPUT_REVISE;
    
    Teamcenter::ReviseInput* reviseInput = 
        dynamic_cast<Teamcenter::ReviseInput*>
            (Teamcenter::BusinessObjectRegistry::instance().
                createInputObject(revTypeName, reviseOp.c_str()));

    BusinessObjectRef<Teamcenter::ItemRevision > latestRev(latestRevTag);
    std::vector<Teamcenter::DeepCopyData* > deepCopyData;
    IFERR_REPORT(latestRev->getDeepCopyData(&reviseOp, &deepCopyData));

    logical performDeepCopyVal =  true;
    reviseInput->setLogical("performDeepCopy", performDeepCopyVal, false );
    
    tag_t newRevTag = NULLTAG;
    IFERR_REPORT(latestRev->revise(reviseInput, &deepCopyData, &newRevTag));  
    IFERR_REPORT(AOM_save_with_extensions(newRevTag)); 
    
    return newRevTag;
}
