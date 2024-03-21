//@<COPYRIGHT>@
//==================================================
//Copyright $2016.
//Siemens Product Lifecycle Management Software Inc.
//All Rights Reserved.
//==================================================
//@<COPYRIGHT>@

/* 
 * @file 
 *
 *   This file contains the implementation for the Extension A2validateReviseInput
 *
 */
#include <A2irs/A2validateReviseInput.hxx>
#include <A2irs/gtac_utils.hxx>
#include <string>
#include <iostream>
 
using namespace std;

int A2validateReviseInput( METHOD_message_t * msg, va_list args )
{
    int ifail = ITK_ok; 

    va_list largs;
    va_copy( largs, args );
    Teamcenter::ReviseInput *reviseInput = va_arg(largs, Teamcenter::ReviseInput*);
    std::vector<Teamcenter::DeepCopyData*> *vecDeepCopyData = 
        va_arg(largs, std::vector<Teamcenter::DeepCopyData*>*);
    va_end( largs );
    
    bool isNull = false;
    tag_t newTargetObject = NULLTAG;
    reviseInput->getTag("newTargetObject", newTargetObject, isNull);

    tag_t srcObjectTag = NULLTAG;
    reviseInput->getTag("srcObjectTag", srcObjectTag, isNull);
   
    bool performDeepCopy = true;
    reviseInput->getLogical( "performDeepCopy", performDeepCopy, isNull);
    
    tag_t item_master_tag = NULLTAG;
    reviseInput->getTag("item_master_tag", item_master_tag, isNull);

    std::string revid;
    reviseInput->getString("item_revision_id", revid, isNull);
    
    if ( vecDeepCopyData != 0 )
    {        
        std::vector<Teamcenter::DeepCopyData *>::iterator it;
        for ( it = (*vecDeepCopyData).begin(); it != (*vecDeepCopyData).end(); ++it )
        {
            bool isNull = false;
            tag_t targetObject = NULLTAG;
            (*it)->getTag("targetObject", targetObject, isNull);
                                 
            tag_t attachedObject = NULLTAG;
            (*it)->getTag("attachedObject", attachedObject, isNull);
           
            tag_t attachedObjectCopy = NULLTAG;
            (*it )->getTag("attachedObjectCopy", attachedObjectCopy, isNull);
             
            std::string relRefTypeName;
            (*it)->getString("propertyName", relRefTypeName, isNull);
            
            std::vector<tag_t> childDeepCopyData;
            std::vector<int> nullVals;            
            (*it)->getTagArray("childDeepCopyData", childDeepCopyData, nullVals );
            if (!childDeepCopyData.empty() )
            {
                for(std::vector< tag_t >::const_iterator childIt = childDeepCopyData.begin(); childIt != childDeepCopyData.end(); ++childIt )
                {
                    tag_t childTargetObject = NULLTAG;
                    AOM_ask_value_tag(*childIt, "targetObject", &childTargetObject);
                    
                    tag_t childAttachedObject = NULLTAG;
                    AOM_ask_value_tag(*childIt, "attachedObject", &childAttachedObject);
                    
                    tag_t childAttachedObjectCopy = NULLTAG;
                    AOM_ask_value_tag(*childIt, "attachedObjectCopy", &childAttachedObjectCopy);
                }

            }
        }
    }
    return ifail;
}