

int BOMLine_add_fn(METHOD_message_t * msg, va_list args)
{
	int ifail = ITK_ok;
	ResultStatus stat;
	try
	{	 
		/**** va_list for BOMLine_add_msg ***/
		va_list largs;
		va_copy( largs, args );
		tag_t parent_bomline_tag = va_arg(largs, tag_t);
		tag_t item_tag = va_arg(largs, tag_t);
		tag_t item_rev_tag = va_arg(largs, tag_t);
		tag_t bv_tag = va_arg(largs, tag_t);
		char *occType = va_arg(largs, char *);
		tag_t *new_bomline_tag = va_arg(largs, tag_t *);
		va_end( largs );
			
		cout << "\t\t parent_bomline_tag: " << parent_bomline_tag << endl;
		cout << "\t\t item_tag: " << item_tag << endl;
		cout << "\t\t item_rev_tag: " << item_rev_tag << endl;
		cout << "\t\t bv_tag: " << bv_tag << endl;
		cout << "\t\t occType: " << occType << endl;
		cout << "\t\t *new_bomline_tag: " << *new_bomline_tag << endl;
	}
    catch(IFail & ex)
    {
        cout << "error " << ex.ifail() << endl;
        cout << ex.getMessage() << endl;
	}
    return ifail;
 }
 
 