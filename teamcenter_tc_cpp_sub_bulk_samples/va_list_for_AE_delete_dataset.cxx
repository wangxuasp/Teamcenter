

int AE_delete_dataset_fn(METHOD_message_t * msg, va_list args)
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
        /* va_list for AE_delete_dataset_msg */
        va_list largs;
        va_copy( largs, args );
        tag_t tDataset = va_arg(largs, tag_t);
        va_end( largs );

		cout << "\t\t tDataset: " << tDataset <<endl;
    }
    catch( const IFail &e )
    {
        cout << "error " << e.ifail() << endl;
        cout << e.getMessage() << endl;
    }
    return ifail;
 }
 