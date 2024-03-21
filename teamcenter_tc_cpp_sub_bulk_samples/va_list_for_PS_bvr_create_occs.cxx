
int PS_bvr_create_occs_fn(METHOD_message_t * msg, va_list args)
{
	int ifail = ITK_ok;
	ResultStatus stat;
	try
	{
		/* va_list for PS_bvr_create_occs_msg */
		va_list largs;
		va_copy( largs, args );
		tag_t parent_bvr_tag = va_arg(largs, tag_t);
		tag_t child_item = va_arg(largs, tag_t);
		tag_t child_bom_view = va_arg(largs, tag_t);
		int n_occ_tags = va_arg(largs, int);
		tag_t** occ_thread_tags = va_arg(largs, tag_t**);
		va_end( largs );

		cout << "\t\t parent_bvr_tag: " << parent_bvr_tag << endl;
		cout << "\t\t child_item: " << child_item << endl;
		cout << "\t\t child_bom_view: " << child_bom_view << endl;
		cout << "\t\t n_occ_tags: " << n_occ_tags << endl;
		for (int ii = 0; ii < *count; ii++)
		{
			cout << "\t\t\t *occ_thread_tags: " << *occ_thread_tags<< endl;
		}
	}
	catch(IFail & ex)
	{
		return ex.ifail();
	}
	return ifail;
}

