
#include <extensionframework/OperationDispatcher.hxx>

int get_object_type_and_operation_name_from_method_message( METHOD_message_t * msg, va_list args )
{
    int ifail = ITK_ok; 
    
    Teamcenter::OperationDispatcher *id = 
        (Teamcenter::OperationDispatcher *)((msg->method).id );
    if ( id != NULL )
    {
        std::string boName = id->getBOName();
        const char* opName = id->askBMOperationName();
        ECHO("     %s - %s  \n", boName.c_str(), opName);
    }
    return ifail;
}