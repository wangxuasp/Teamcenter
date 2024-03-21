

int AE_import_file_fn(METHOD_message_t * msg, va_list args)
{
	int ifail = ITK_ok;
	ResultStatus stat;
	try
	{	
		/* va_list for AE_import_file_msg */
		va_list largs;
		va_copy( largs, args ); 
		tag_t  dataset_tag = va_arg(largs, tag_t);
		char *reference_name = va_arg(largs, char *);
		AE_reference_type_t  reference_type = va_arg(largs, AE_reference_type_t);
		char *full_os_path = va_arg(largs, char *);
		char *file_name = va_arg(largs, char *);
		int fileTypeFlag = va_arg(largs, int);
		va_end( largs );
		 
		cout << "\t\t dataset_tag: " << dataset_tag << endl;
		cout << "\t\t reference_name: " << reference_name <<endl;
		if (reference_type == AE_BAD_REF_TYPE) cout << "\t\t reference_type: AE_BAD_REF_TYPE" << endl;
		if (reference_type == AE_ASSOCIATION) cout << "\t\t reference_type: AE_ASSOCIATION" << endl;
		if (reference_type == AE_PART_OF) cout << "\t\t reference_type: AE_PART_OF" << endl;
		cout << "\t\t full_os_path: " << full_os_path <<endl;
		cout << "\t\t file_name: " << file_name << endl;
		if (fileTypeFlag == SS_TEXT) cout << "\t\t fileTypeFlag: SS_TEXT" << endl;
		if (fileTypeFlag == AE_ASSOCIATION) cout << "\t\t fileTypeFlag: SS_BINARY" << endl;
	}
    catch(IFail & ex)
    {
        cout << "error " << ex.ifail() << endl;
        cout << ex.getMessage() << endl;
	}	
    return ifail;
 }

 