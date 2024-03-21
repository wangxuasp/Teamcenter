
int PS_bvr_import_new_occ_fn(METHOD_message_t * msg, va_list args)
{
	int ifail = ITK_ok;
	ResultStatus stat;
	try
	{	
		/**** va_list for PS_bvr_import_new_occ***/
        va_list largs;
        va_copy( largs, args );
		tag_t parentBvrTag = va_arg(largs, tag_t);
		tag_t childItemTag = va_arg(largs, tag_t);
		tag_t childBomViewTag = va_arg(largs, tag_t);
		tag_t existingOccThreadTag = va_arg(largs, tag_t);
		tag_t* occurrenceTag = va_arg( largs, tag_t*);
		va_end( largs );

		cout << "\t\t parentBvrTag: " <<  parentBvrTag << endl;
		cout << "\t\t childItemTag: " <<  childItemTag << endl;
		cout << "\t\t childBomViewTag: " <<  childBomViewTag << endl;
		cout << "\t\t existingOccThreadTag: " <<  existingOccThreadTag << endl;
		cout << "\t\t *occurrenceTag: " <<  *occurrenceTag << endl;
	}
    catch(IFail & ex)
    {
        cout << "error " << ex.ifail() << endl;
        cout << ex.getMessage() << endl;
	}
    return ifail;
 }
     
