/* HEAD USER_SERVER_EXIT_RETURNING_STRING CCC ITK */
/*  client side
        IMANUserService us = (IMANUserService)session.getUserService();
        String q = new String("zzz,yy,x");
        Object [] objs = new Object[1];
        objs[0] = q;
        String ans = (String)us.call("functionPtr",objs);
        int jt = 1; 
*/    
#include <stdlib.h>
#include <tccore/aom.h>
#include <tc/emh.h>
#include <ict/ict_userservice.h>
#include <tc/tc.h>
#include <tccore/tctype.h>
#include <sa/tcfile.h>
#include <ss/ss_errors.h>
#include <server_exits/user_server_exits.h>


#define ECHO(X)  printf X; TC_write_syslog X

#define IFERR_ABORT(X)  (report_error( __FILE__, __LINE__, #X, X, TRUE))
#define IFERR_REPORT(X) (report_error( __FILE__, __LINE__, #X, X, FALSE))
#define IFERR_RETURN(X) if (IFERR_REPORT(X)) return

static int report_error(char *file, int line, char *call, int status,
    logical exit_on_error)
{
    if (status != ITK_ok)
    {
        int
            n_errors = 0,
            *severities = NULL,
            *statuses = NULL;
        char
            **messages;

        EMH_ask_errors( &n_errors, &severities, &statuses, &messages );
        if (n_errors > 0)
        {
            ECHO(("\n%s\n", messages[n_errors-1]));
            EMH_clear_errors();
        }
        else
        {
            char *error_message_string;
            EMH_get_error_string (NULLTAG, status, &error_message_string);
            ECHO(("\n%s\n", error_message_string));
        }

        ECHO(("error %d at line %d in %s\n", status, line, file));
        ECHO(("%s\n", call));

        if (exit_on_error)
        {
            ECHO(("\nExiting program!\n"));
            exit (status);
        }
    }

    return status;
}
extern int functionName (void * returnValueType)
{
    int  status = ITK_ok;
    char *arg = NULL;
 
    ECHO(("\n\n entering pass_string "));

    // The following line retrieves the argument(s) from Java.
    IFERR_REPORT(USERARG_get_string_argument(&arg));
    ECHO(("\n\t %s\n", arg));

    *((char**) return_string) = (char*) MEM_alloc ( strlen (("String from ITK" ) + 1 ));
    strcpy( *((char**) return_string), ("String from ITK" ));

    ECHO(("\n exiting pass_string \n"));
    return status;
}

extern DLLAPI int USERSERVICE_register_methods()
{
    int   
        status = ITK_ok,
        numberOfArguments = 1,
        returnValueType = USERARG_STRING_TYPE,
        *argumentList = NULL;

    USER_function_t 
        functionPtr = functionName;

    argumentList = (int *) MEM_alloc (numberOfArguments * sizeof(int));
    argumentList[0] = USERARG_STRING_TYPE;

    USERSERVICE_register_method("functionPtr", functionPtr, numberOfArguments,
        argumentList, returnValueType);
    MEM_free(argumentList);

    return(status);
}
