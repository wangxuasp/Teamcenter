

/**
 * Setter for "a2_string", a string Property
 * @param value - Value to be set for the parameter
 * @param isNull - If true, set the parameter value to null
 * @return - Status. 0 if successful
 */
int  A2_FormImpl::setA2_stringBase( const std::string & value, bool  isNull )
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
	get_class_attr_id_by_instance_tag(inst_tag, "a2_string", &attr_tag);

	ifail = AttributeAccessor::setStringValue(inst_tag, attr_tag, value, isNull );
    return ifail;
}
