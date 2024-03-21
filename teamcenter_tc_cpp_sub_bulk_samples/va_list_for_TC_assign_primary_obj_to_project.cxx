
int TC_assign_primary_obj_to_project_fn(METHOD_message_t * msg, va_list args)
{
	int ifail = ITK_ok;
	ResultStatus stat;
	try
	{	
		/**** va_list for TC_assign_primary_obj_to_project_msg ***/
		va_list largs;
		va_copy( largs, args );
		tag_t wso_tag = va_arg(largs, tag_t);
		tag_t project_tag = va_arg(largs, tag_t);
		va_end( largs );
		
		printf("\n\t Property values of the va_list: \n");
		printf("\t\t wso_tag: %u\n", wso_tag);
		printf("\t\t project_tag: %u\n", project_tag);
	
	}
    catch(IFail & ex)
    {
        cout << "error " << ex.ifail() << endl;
        cout << ex.getMessage() << endl;
	}
    return ifail;
 }
 