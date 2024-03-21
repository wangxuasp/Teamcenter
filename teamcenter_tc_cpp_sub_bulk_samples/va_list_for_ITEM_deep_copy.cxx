
int ITEM_deep_copy_fn(METHOD_message_t * msg, va_list args)
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
        /* va_list for ITEM_deep_copy_msg */
        va_list largs;
        va_copy(largs, args);
        tag_t  new_rev = va_arg(largs, tag_t);
        char*  operation = va_arg(largs, char*);
        tag_t  parent_rev = va_arg(largs, tag_t);
        int  copyCount = va_arg(largs, int);
        ITEM_deepcopy_info_t*  copyInfo = va_arg(largs, ITEM_deepcopy_info_t*);
        int*  count = va_arg(largs, int*);
        tag_t**  copied_objects = va_arg(largs, tag_t**);
        va_end( largs );

        cout << "\t\t new_rev: " << new_rev << endl;
        cout << "\t\t operation: " << operation << endl;
        cout << "\t\t parent_rev: " << parent_rev << endl;
        cout << "\t\t copyCount: " << copyCount << endl;

        for (int ii = 0; ii < copyCount; ii++)
        {
            cout << "\t\t\t copyInfo[ii].component: " << copyInfo[ii].component << endl;
            switch ( copyInfo[ii].action )
            {
                case CopyAsObjectType :
                    cout << "\t\t\t copyInfo[%d].action: CopyAsObject\n" << ii << endl;
                    break ;
                case CopyAsRefType :
                    cout << "\t\t\t copyInfo[%d].action: CopyAsReference\n" << ii << endl;
                    break ;
                case RelateToLatestType :
                    cout << "\t\t\t copyInfo[%d].action: RelateToLatest\n" << ii << endl;
                    break ;
                case ReviseAndRelateToLatestType :
                    cout << "\t\t\t copyInfo[%d].action: ReviseAndRelateToLatest\n" << ii << endl;
                    break ;
                case DontCopyType :
                    cout << "\t\t\t copyInfo[%d].action: DontCopy\n" << ii << endl;
                    break ;
                case Select :
                    cout << "\t\t\t copyInfo[%d].action: Select\n" << ii << endl;
                    break ;
                case SystemCopy :
                    cout << "\t\t\t copyInfo[%d].action: SystemCopy\n" << ii << endl;
                    break ;
                case NoCopyOrReferenceNewCopyType :
                    cout << "\t\t\t copyInfo[%d].action: NoCopyOrReferenceNewCopy\n" << ii << endl;
                    break ;
                default :
                    cout << "\t\t\t copyInfo[%d].action: ITEM_copy_type_not_valid\n" << ii << endl;
            }
            cout << "\t\t\t copyInfo[%d].objName: " << ii << copyInfo[ii].objName << endl;
            cout << "\t\t\t copyInfo[%d].relation: "<< ii << copyInfo[ii].relation << endl;
            cout << "\t\t\t copyInfo[%d].relation: "<< ii << copyInfo[ii].relation << endl;
            
            if(copyInfo[ii].is_target_primary == TRUE)  cout << "\t\t\t copyInfo[%d]: true\n" << ii << endl;
            if(copyInfo[ii].is_target_primary == FALSE) cout << "\t\t\t copyInfo[%d]: false\n" << ii << endl;
            
            if(copyInfo[ii].is_required == TRUE)  cout << "\t\t\t copyInfo[%d].is_required: true\n" << ii << endl;
            if(copyInfo[ii].is_required == FALSE) cout << "\t\t\t copyInfo[%d]is_required: false\n" << ii << endl;
            
            if(copyInfo[ii].copy_relations == TRUE)  cout << "\t\t\t copyInfo[%d].copy_relations: true\n" << ii << endl;
            if(copyInfo[ii].copy_relations == FALSE) cout << "\t\t\t copyInfo[%d.copy_relations: false\n" << ii << endl;
        }
        cout << "\t\t *count: " << *count << endl;

        for (int ii = 0; ii < *count; ii++)
        {
            cout << "\n" << endl;
            cout << "\t\t (*copied_objects)[ii]: " << (*copied_objects)[ii] << endl;
        }
    }
    catch(IFail & ex)
    {
        return ex.ifail();
    }
    return ifail;
}

