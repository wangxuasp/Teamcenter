
#include <cxpom/attributeaccessor.hxx>
#include <form/form.h>

/**
 * Getter for a Tag Array Property
 * @param values - Parameter value
 * @param isNull - Returns true for an array element if the parameter value at that location is null
 * @return - Status. 0 if successful
 */
int  A2FormImpl::getA2_persisent_tagsBase( std::vector<tag_t> & values, std::vector<int> & isNull) const
{
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
    get_class_attr_id_by_instance_tag(inst_tag, "a2_persisent_tags", &attr_tag);  
    
    ifail = AttributeAccessor::getTagValues( inst_tag, attr_tag, values, isNull );      
    return ifail;
}
