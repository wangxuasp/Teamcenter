

int IMAN_delete_fn(METHOD_message_t * msg, va_list args)
{
	int ifail = ITK_ok;
	ResultStatus stat;
	try
	{	 
		/**** va_list for IMAN_delete_msg ***/
		va_list largs;
		va_copy( largs, args );
		tag_t  object_tag  = va_arg(largs, tag_t);
		va_end( largs );

		cout << "\t\t object_tag: " << object_tag<< endl;
	}
    catch(IFail & ex)
    {
        cout << "error " << ex.ifail() << endl;
        cout << ex.getMessage() << endl;
	}
    return ifail;
 }
 
 