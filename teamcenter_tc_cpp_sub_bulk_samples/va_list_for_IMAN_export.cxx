

int IMAN_export_fn(METHOD_message_t * msg, va_list args)
{
	int ifail = ITK_ok;
	ResultStatus stat;
	try
	{	 
		/**** va_list for IMAN_export_msg ***/
		va_list largs;
		va_copy( largs, args );
		tag_t  tManager  = va_arg(largs, tag_t);
		tag_t  tObject  = va_arg(largs, tag_t);
		va_end( largs );

		cout << "\t\t tManager: " << tManager << endl;
		cout << "\t\t tObject: " << tObject << endl;
	}
    catch(IFail & ex)
    {
        cout << "error " << ex.ifail() << endl;
        cout << ex.getMessage() << endl;
	}
    return ifail;
 }
 
 