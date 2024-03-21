
int AE_create_dataset_fn(METHOD_message_t * msg, va_list args)
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
        /* va_list for AE_create_dataset_msg */
        va_list largs;
        va_copy( largs, args );
        tag_t tDatasetType = va_arg (largs, tag_t);
        char* pcDatasetName = va_arg (largs, char*);
        char* pcDatasetDescription = va_arg (largs, char*);
        char* pcDatasetID = va_arg (largs, char*);
        char* pcDatasetRevision = va_arg (largs, char*);
        tag_t tNewDataset = va_arg (largs, tag_t);
        va_end( largs );

		cout << "tDatasetType: " << tDatasetType << endl;
		cout << "pcDatasetName: " << pcDatasetName << endl;
		cout << "pcDatasetDescription: " << pcDatasetDescription << endl;	
		cout << "pcDatasetID: " << pcDatasetID << endl;
		cout << "pcDatasetRevision: " << pcDatasetRevision << endl;
		cout << "tNewDataset: " << tNewDataset << endl;		
    }
    catch( const IFail &e )
    {
        cout << "error " << e.ifail() << endl;
        cout << e.getMessage() << endl;
    }
    return ifail;
 }
 