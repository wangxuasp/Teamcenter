
#include <cxpom/attributeaccessor.hxx>
#include <form/form.h>

/**
 * Setter for a Tag Array Property
 * @param values - Values to be set for the parameter
 * @param isNull - If array element is true, set the parameter value at that location as null
 * @return - Status. 0 if successful
 */
int  A2FormImpl::setA2_persisent_tagsBase( const std::vector<tag_t> & values, const std::vector<int> * isNull )
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
    get_class_attr_id_by_instance_tag(inst_tag, "a2_persisent_tags", &attr_tag); ;
    
    IFERR_ABORT(AOM_refresh(inst_tag, TRUE));
    IFERR_ABORT(AOM_refresh(form_tag, TRUE));
    
    ifail = AttributeAccessor::resetTagList(inst_tag, attr_tag, values); 
    return ifail;
}
