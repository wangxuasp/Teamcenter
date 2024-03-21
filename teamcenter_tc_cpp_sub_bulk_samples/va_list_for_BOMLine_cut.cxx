

int BOMLine_cut_fn(METHOD_message_t * msg, va_list args)
{
	int ifail = ITK_ok;
	ResultStatus stat;
	try
	{	 
		/**** va_list for BOMLine_cut_msg ***/
		va_list largs;
		va_copy( largs, args );          
		tag_t bomline_tag = va_arg(largs, tag_t);
		va_end( largs );

		cout << "\t\t bomline_tag: " << bomline_tag << endl;
	}
    catch(IFail & ex)
    {
        cout << "error " << ex.ifail() << endl;
        cout << ex.getMessage() << endl;
	}
    return ifail;
 }
 
 