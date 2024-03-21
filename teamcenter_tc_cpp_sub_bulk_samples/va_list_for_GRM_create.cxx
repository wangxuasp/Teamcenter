

int GRM_create_fn(METHOD_message_t * msg, va_list args)
{
	int ifail = ITK_ok;
	ResultStatus stat;
	try
	{	 
		/**** va_list for GRM_create_msg ***/
		va_list largs;
		va_copy( largs, args );
		tag_t  primary_object = va_arg(largs, tag_t);
		tag_t  secondary_object = va_arg(largs, tag_t);
		tag_t  relation_type = va_arg(largs, tag_t);
		tag_t  user_data = va_arg(largs, tag_t);
		tag_t  *new_relation = va_arg(largs, tag_t *);
		va_end( largs );

		cout << "\t\t primary_object: " << primary_object << endl;
		cout << "\t\t secondary_object: " << secondary_object << endl;
		cout << "\t\t relation_type: " << relation_type << endl;
		cout << "\t\t user_data: " << user_data << endl;		
		cout << "\t\t *new_relation: " << *new_relation << endl;
	}
    catch(IFail & ex)
    {
        cout << "error " << ex.ifail() << endl;
        cout << ex.getMessage() << endl;
	}
    return ifail;
 }
 
 