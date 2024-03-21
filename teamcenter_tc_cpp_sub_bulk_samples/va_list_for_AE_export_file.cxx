

int AE_export_file_fn(METHOD_message_t * msg, va_list args)
{
	int ifail = ITK_ok;
	int ifail = ITK_ok;
	ResultStatus stat;
	try
	{	
		/**** va_list for AE_export_file ***/
		va_list largs;
		va_copy( largs, args );	
		tag_t  dataset_tag = va_arg(largs, tag_t);
		char *reference_name = va_arg(largs, char *);
		char *destination_name = va_arg(largs, char *); 
		va_end( largs );
		
		cout << "\t\t dataset_tag: " << dataset_tag <<endl;
		cout << "\t\t *reference_name: " << *reference_name <<endl;
		cout << "\t\t *destination_name: " << *destination_name <<endl;
	}
    catch(IFail & ex)
    {
        cout << "error " << ex.ifail() << endl;
        cout << ex.getMessage() << endl;
	}
    return ifail;
 }
 
 
 