
int ITEM_create_fn(METHOD_message_t * msg, va_list args)
{
	int ifail = ITK_ok;
	ResultStatus stat;
	try
	{	 
		/**** va_list for ITEM_create_msg ***/
		va_list largs;
		va_copy( largs, args );
		char   *item_id = va_arg(largs, char *);
		char   *item_name = va_arg(largs, char *);
		char   *type_name = va_arg(largs, char *);
		char   *rev_id = va_arg(largs, char *);
		tag_t  *new_item = va_arg(largs, tag_t *);
		tag_t  *new_rev = va_arg(largs, tag_t *);
		tag_t  item_master_form = va_arg(largs, tag_t);
		tag_t  item_rev_master_form = va_arg(largs, tag_t);
		va_end( largs );
   
		cout << "\t\t item_id: " << item_id << endl;
		cout << "\t\t item_name: " << item_name << endl;
		cout << "\t\t type_name: " << type_name << endl;
		cout << "\t\t rev_id:  " << rev_id << endl;
		cout << "\t\t *new_item: " << *new_item << endl;
		cout << "\t\t *new_rev": " << *new_rev << endl;
		cout << "\t\t item_master_form: " << item_master_form << endl;
		cout << "\t\t item_rev_master_form: " << item_rev_master_form << endl;	
	}
    catch(IFail & ex)
    {
        cout << "error " << ex.ifail() << endl;
        cout << ex.getMessage() << endl;
	}
    return ifail;
 }
 
