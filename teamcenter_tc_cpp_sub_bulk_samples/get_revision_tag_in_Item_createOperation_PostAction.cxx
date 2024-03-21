
#include <A2gtac/A2_7672959.hxx>
#include <ug_va_copy.h>
#include <metaframework/CreateInput.hxx>

using namespace Teamcenter;

int A2Item_createOperation_PostAction( METHOD_message_t * msg, va_list args)
{
    int ifail = ITK_ok;
    
    va_list largs;
    va_copy( largs, args );
    Teamcenter::CreateInput *createInput = va_arg(largs, Teamcenter::CreateInput*);
    va_end( largs );
     
    tag_t revTag = NULLTAG;
    std::string revision = "revision";
    createInput->getCompoundObjectTag(revision, revTag);

    return ifail;
}

