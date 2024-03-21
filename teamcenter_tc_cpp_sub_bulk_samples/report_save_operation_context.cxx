
#include <extensionframework/OperationDispatcher.hxx>
#include <tccore/tctype.h>

int report_object_type_and_operation_name_from_method_message( METHOD_message_t * msg)
{
    int ifail = ITK_ok; 
    
    Teamcenter::OperationDispatcher *id = 
        (Teamcenter::OperationDispatcher *)((msg->method).id );
    if ( id != NULL )
    {
        std::string boName = id->getBOName();
        const char* opName = id->askBMOperationName();

        
        TCTYPE_save_operation_context_t save_context;
        IFERR_REPORT(TCTYPE_ask_save_operation_context(&save_context));
        
        switch (save_context)
        {
            case TCTYPE_save_on_create: 
                ECHO("     %s - %s - TCTYPE_save_on_create \n", 
                    boName.c_str(), opName);
                break;            
            case (TCTYPE_save_on_saveas): 
                ECHO("     %s - %s - TCTYPE_save_on_saveas \n", 
                    boName.c_str(), opName);
                break;
            case (TCTYPE_save_on_revise): 
                ECHO("     %s - %s - TCTYPE_save_on_revise\n", 
                    boName.c_str(), opName);
                break;
            case (TCTYPE_save_on_update): 
                ECHO("     %s - %s - TCTYPE_save_on_update \n", 
                    boName.c_str(), opName);
                break;
            case (TCTYPE_unknown_operation_context): 
                ECHO("     %s - %s - TCTYPE_unknown_operation_context \n", 
                    boName.c_str(), opName);
                break;            
        }
    }   
    return ifail;
}
