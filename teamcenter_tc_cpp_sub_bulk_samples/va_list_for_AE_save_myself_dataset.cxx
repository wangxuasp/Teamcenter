

int AE_save_myself_dataset_fn(METHOD_message_t * msg, va_list args)
{
	int ifail = ITK_ok;
	ResultStatus stat;
	try
	{	 
		/**** va_list for AE_save_myself_dataset_msg ***/
		va_list largs;
		va_copy( largs, args );
		tag_t  dataset_tag = va_arg(largs, tag_t);
		logical  isNew = va_arg(largs, logical);
		va_end(largs);
		
		cout << "\t\t dataset_tag: " << dataset_tag << endl;
		cout << "\t\t isNew: " << isNew << endl;
	}
    catch(IFail & ex)
    {
        cout << "error " << ex.ifail() << endl;
        cout << ex.getMessage() << endl;
	}
    return ifail;
 }
 
 