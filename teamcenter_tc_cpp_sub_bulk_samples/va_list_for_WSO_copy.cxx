
int WSO_copy_fn(METHOD_message_t * msg, va_list args)
{
	int ifail = ITK_ok;
	ResultStatus stat;
	try
	{	
		/**** va_list for WSO_copy_msg ***/ 
		va_list largs;
		va_copy( largs, args );
		tag_t  wso_tag = va_arg(largs, tag_t);
		char *new_name = va_arg(largs, char *);
		tag_t  *new_wso_tag = va_arg(largs, tag_t *);	
		va_end( largs );

		cout << "wso_tag: " << wso_tag << endl;
		cout << "new_name: " << new_name << endl;
		cout << "*new_wso_tag: " << *new_wso_tag << endl;
	}
    catch(IFail & ex)
    {
        cout << "error " << ex.ifail() << endl;
        cout << ex.getMessage() << endl;
	}
    return ifail;
 }
 