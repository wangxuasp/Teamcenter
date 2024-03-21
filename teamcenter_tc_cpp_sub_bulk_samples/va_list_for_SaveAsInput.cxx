
int SaveAsInput_fn(METHOD_message_t * msg, va_list args)
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {   
        /**** va_list for SaveAsInput ***/
        va_list largs;
        va_copy( largs, args );
        Teamcenter::SaveAsInput *boSaveAsInput = va_arg(largs, Teamcenter::SaveAsInput*);
        std::vector<Teamcenter::DeepCopyData*> *vecDeepCopyData = va_arg(largs, std::vector<Teamcenter::DeepCopyData*>*);
        va_end( largs );

        /* Demo setting SaveAsInput Business Object property via C++ */
        boSaveAsInput->setString("object_name", "New Name", false);
        
        /* Demo getting SaveAsInput tag and setting property via  ITK */
        tag_t tSaveAsInput = boSaveAsInput->getTag();
        stat = AOM_set_value_string(tSaveAsInput, "object_desc", "Some value");
        
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
     
