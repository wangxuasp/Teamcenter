
int ReviseInput_fn(METHOD_message_t * msg, va_list args)
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {   
        /**** va_list for ReviseInput ***/
        va_list largs;
        va_copy( largs, args );
        Teamcenter::ReviseInput *boReviseInput = va_arg(largs, Teamcenter::ReviseInput*);
        std::vector<Teamcenter::DeepCopyData*> *vecDeepCopyData = va_arg(largs, std::vector<Teamcenter::DeepCopyData*>*);
        va_end( largs );

        /* Demo setting ReviseInput Business Object property via C++ */
        boReviseInput->setString("object_name", "New Name", false);
        
        /* Demo getting ReviseInput tag and setting property via  ITK */
        tag_t tReviseInput = boReviseInput->getTag();
        stat = AOM_set_value_string(tReviseInput, "object_desc", "Some value");
        
        /* Demo getting the WorkspaceObject tag */
        tag_t tWSO = msg->object_tag;
    }
    catch(IFail & ex)
    {
        cout << "error " << ex.ifail() << endl;
        cout << ex.getMessage() << endl;
    }
    return ifail;
 }
     
