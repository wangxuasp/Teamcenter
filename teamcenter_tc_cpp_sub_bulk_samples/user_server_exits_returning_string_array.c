/* HEAD USER_SERVER_EXIT_RETURNING_STRING_ARRAY CCC ITK */
/* client side
        IMANUserService us = (IMANUserService)session.getUserService();
        String q = new String("What car do you like?");
        Object [] objs = new Object[1];
        objs[0] = q;
        String[] ans = (String[])us.call("functionPtr",objs);
        nt jt = 1; //just to stop here and look at ans
*/

#include <server_exits/user_server_exits.h>
extern int functionName (void * returnValueType)
{
    int
        arraySize = 3,
        status = ITK_ok;
    char 
        *arg,   
        *string1 = NULL,
        *string2 = NULL,
        *string3 = NULL,
        **stringarray = NULL;
    USERSERVICE_array_t 
        arrayStruct;

    printf("functionName in\n");

    status = USERARG_get_string_argument(&arg);
    stringarray = (char **) MEM_alloc (arraySize * sizeof(char*));
    string1 = (char *) MEM_alloc ( 5 * sizeof(char));
    string2 = (char *) MEM_alloc ( 2 * sizeof(char));
    string3 = (char *) MEM_alloc ( 2 * sizeof(char));

    strcpy(string1,"Saab");
    strcpy(string2,"9");
    strcpy(string3,"3");

    stringarray[0] = string1;
    stringarray[1] = string2;
    stringarray[2] = string3;

    USERSERVICE_return_string_array((const char**)stringarray, arraySize,
        &arrayStruct);

    if (arrayStruct.length != 0)
     *((USERSERVICE_array_t*) returnValueType) = arrayStruct;

    printf("functionName out\n");
    return status;
}

extern DLLAPI int USERSERVICE_register_methods()
{
    int
        numberOfArguments = 1,
        argumentList[1],
        returnValueType,  
        status = ITK_ok;
    USER_function_t 
        functionPtr = functionName;

    argumentList[0] = USERARG_STRING_TYPE;
    returnValueType = USERARG_STRING_TYPE + USERARG_ARRAY_TYPE;

    USERSERVICE_register_method("functionName", functionPtr, numberOfArguments,
        argumentList,  returnValueType);

    return(status);
}
