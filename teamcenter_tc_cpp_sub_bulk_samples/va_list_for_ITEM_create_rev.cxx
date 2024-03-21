

int ITEM_create_fn(METHOD_message_t * msg, va_list args)
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {    
        /* va_list for ITEM_create_rev_msg */
        va_list largs;
        va_copy( largs, args );
        tag_t  item = va_arg(largs, tag_t);
        char   *rev_id = va_arg(largs, char *);
        tag_t  *new_rev = va_arg(largs, tag_t *);
        tag_t  item_rev_master_tag = va_arg(largs, tag_t );
        va_end( largs );
        
        cout << "\t\t item: " << item << endl;
        cout << "\t\t rev_id: " << rev_id << endl;
        cout << "\t\t *new_rev: " << new_rev << endl;
        cout << "\t\t item_rev_master_tag: " << item_rev_master_tag << endl;
    }
    catch(IFail & ex)
    {
        cout << "error " << ex.ifail() << endl;
        cout << ex.getMessage() << endl;
    }
    return ifail;
 }
 
