
#include <cxpom/attributeaccessor.hxx>
#include <form/form.h>

/**
 * Getter for a string Property
 * @param value - Parameter value
 * @param isNull - Returns true if the Parameter value is null
 * @return - Status. 0 if successful
 */
int  A2FormImpl::getA2_persistent_stringBase( std::string & value, bool & isNull ) const
{
    ECHO("\n A2FormImpl::getA2_persistent_stringBase \n");
    int ifail = ITK_ok;
    
    tag_t inst_tag = this->getA2Form()->getTag();
    
    /* 
        If owning object inherits from form the inst_tag must be form 
        storage class instance.
    */     
    if(is_descendant_of_Form(inst_tag))
    {
        tag_t form_tag = inst_tag;
        inst_tag = NULLTAG;
        IFERR_REPORT(FORM_ask_pom_instance(form_tag, &inst_tag));
    } 
    
    tag_t attr_tag = NULLTAG;
    get_class_attr_id_by_instance_tag(inst_tag, "a2_persistent_string", &attr_tag); 
    
    ifail = AttributeAccessor::getStringValue(inst_tag, attr_tag, value, isNull );
    return ifail;
}
