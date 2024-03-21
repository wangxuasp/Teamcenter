

int fnd0RemoveOccurrence_fn(METHOD_message_t * msg, va_list args)
{
	int ifail = ITK_ok;
	ResultStatus stat;
	try
	{	 
		/**** va_list for fnd0ReplaceOccurrence_msg ***/
		va_list largs;
		va_copy( largs, args );
		tag_t  occ_tag  = va_arg(largs, tag_t);
		tag_t  child_item_tag  = va_arg(largs, tag_t);
		tag_t  childBomView_tag  = va_arg(largs, tag_t);
		va_end(largs);

		cout << "\t\t occ_tag: " << occ_tag << endl;
		cout << "\t\t child_item_tag: " << child_item_tag << endl;
		cout << "\t\t childBomView_tag: " << childBomView_tag << endl;
	}
    catch(IFail & ex)
    {
        cout << "error " << ex.ifail() << endl;
        cout << ex.getMessage() << endl;
	}
    return ifail;
 }
 
 