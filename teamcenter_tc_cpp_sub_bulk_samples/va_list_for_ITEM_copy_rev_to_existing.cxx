
int ITEM_copy_rev_to_existing_fn(METHOD_message_t * msg, va_list args)
{
	int ifail = ITK_ok;
	ResultStatus stat;
	try
	{	 
		/**** va_list for ITEM_copy_rev_to_existing_msg ***/
		va_list largs;
		va_copy( largs, args );
		tag_t  revTag = va_arg(args, tag_t);
		char   *new_rev_id = va_arg(args, char *);
		char   *name = va_arg(args, char *);
		char   *description = va_arg(args, char *);
		tag_t  new_item = va_arg(args, tag_t );
		tag_t  *new_rev = va_arg(args, tag_t *);
		tag_t  item_rev_master_form = va_arg(args, tag_t );
		va_end( largs );

		cout << "\t\t revTag: " << revTag << endl;
		cout << "\t\t new_rev_id: " << new_rev_id << endl;
		cout << "\t\t name: " << name<< endl;
		cout << "\t\t description: " << description << endl;		
		cout << "\t\t new_item: " << new_item << endl;
		cout << "\t\t *new_rev: " << *new_rev << endl;
		cout << "\t\t item_rev_master_form: " << item_rev_master_form << endl;
	
	}
    catch(IFail & ex)
    {
        cout << "error " << ex.ifail() << endl;
        cout << ex.getMessage() << endl;
	}
    return ifail;
 }
 
