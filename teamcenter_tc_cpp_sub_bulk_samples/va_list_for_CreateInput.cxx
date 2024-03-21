
int CreateInput_fn(METHOD_message_t * msg, va_list args)
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {   
        /**** va_list for CreateInput ***/
        va_list largs;
        va_copy( largs, args );
        Teamcenter::CreateInput *boCreateInput = va_arg(largs, Teamcenter::CreateInput*);
        std::vector<Teamcenter::DeepCopyData*> *vecDeepCopyData = va_arg(largs, std::vector<Teamcenter::DeepCopyData*>*);
        va_end( largs );

        /* Demo setting CreateInput Business Object property via C++ */
        boCreateInput->setString("object_name", "New Name", false);
        
        /* Demo getting CreateInput tag and setting property via  ITK */
        tag_t tCreateInput = boCreateInput->getTag();
        stat = AOM_set_value_string(tCreateInput, "object_desc", "Some value");
        
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
     
