
int USER_query_execute_fn(METHOD_message_t * msg, va_list args)
{

    int ifail = ITK_ok;
	ResultStatus stat;
	try
	{	
		/**** va_list for USER_query_execute_msg ***/
		va_list largs;
		va_copy( largs, args );
		char  *qName = va_arg(largs, char*); 
		int   num_args = va_arg (largs, int);
		char  **fields = va_arg (largs, char **); 
		char  **values = va_arg (largs, char **);
		int   *n_columns = va_arg (largs, int*); 
		char  ***col_names = va_arg (largs, char ***); 
		QRY_user_query_row_t **rows = va_arg( largs, QRY_user_query_row_t**);
		va_end( largs );

		cout << "\n\t Property values of the va_list: \n" << endl;
		cout << "\t\t qName: " << qName << endl;
		cout << "\t\t num_args: " <<  num_args << endl;
		for (int ii = 0; ii < num_args; ii++)
		{
			cout << "\t\t *fields: " << *fields << endl;
			cout << "\t\t *values: " << *values << endl;
		}
	}
    catch(IFail & ex)
    {
        cout << "error " << ex.ifail() << endl;
        cout << ex.getMessage() << endl;
	}
    return ifail;
 }
 