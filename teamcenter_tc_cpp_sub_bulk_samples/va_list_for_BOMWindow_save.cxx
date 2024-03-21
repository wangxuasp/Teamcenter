

int BOMWindow_save_fn(METHOD_message_t * msg, va_list args)
{
	int ifail = ITK_ok;
	ResultStatus stat;
	try
	{	 
		/**** va_list for BOMWindow_save_msg ***/
		va_list largs;
		va_copy( largs, args );
		tag_t window_tag = va_arg( largs, tag_t );
		va_end( largs );

		cout << "\t\t window_tag: " << window_tag << endl;
	}
    catch(IFail & ex)
    {
        cout << "error " << ex.ifail() << endl;
        cout << ex.getMessage() << endl;
	}
    return ifail;
 }
 
 