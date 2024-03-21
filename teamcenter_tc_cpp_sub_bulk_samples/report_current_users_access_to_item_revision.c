/*HEAD REPORT_CURRENT_USERS_ACCESS_TO_ITEM_REVISION CCC ITK */
#include <stdlib.h>
#include <sa/am.h>
#include <tccore/aom.h>
#include <tc/emh.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <itk/mem.h>
#include <sa/sa.h>
#include <sa/user.h>



#define EXIT_FAILURE 1 
#define ERROR_CHECK(X) (report_error( __FILE__, __LINE__, #X, (X)))
static void report_error( char *file, int line, char *function, int return_code)
{
    if (return_code != ITK_ok)
    {
        char *error_message_string;
        EMH_get_error_string (NULLTAG, return_code, &error_message_string);
        printf ("ERROR: %d ERROR MSG: %s.\n", return_code, error_message_string);
        printf ("FUNCTION: %s\nFILE: %s LINE: %d\n", function, file, line);
        if(error_message_string) MEM_free(error_message_string);
        printf("\nExiting program!\n");
        exit (EXIT_FAILURE);    
    }
}

#define EXIT_IF_NULL(X) (check_value(#X, (X)))
static void check_value( char *function, int value )
{
    if ((value == 0) || (value == 0))
    {
        printf ("\t%s is NULL\n", function);
        printf("\nExiting program!\n");
        exit (EXIT_FAILURE);    
    }
}

static void do_it(void)
{
    logical
         verdict = FALSE;
    int
        ii = 0;
    tag_t
        user = NULLTAG, 
        itemRev = NULLTAG;
    char
        *user_name_string = NULL,
        groupname[SA_name_size_c + 1],
        privilege[11][13] = {"READ", "WRITE", "DELETE", "CHANGE", "PROMOTE", 
           "DEMOTE", "COPY", "IMPORT", "EXPORT", "TRANSFER_IN", "TRANSFER_OUT"};

    ERROR_CHECK( POM_get_user(&user_name_string, &user) );

    ERROR_CHECK( ITEM_find_rev("21000", "A", &itemRev) );
    EXIT_IF_NULL( itemRev );
    
    for (ii = 0; ii < 12; ii++)
    {
        ERROR_CHECK( AM_check_users_privilege(user, itemRev, 
            privilege[ii], &verdict) );
        if (verdict == TRUE) 
            printf("%s may %s\n", user_name_string, privilege[ii]);
        else  printf("%s may NOT %s\n", user_name_string, privilege[ii]);
    }
    MEM_free(user_name_string);
}

int ITK_user_main(int argc, char* argv[])
{
    int
        status = 0; 
   
    ITK_initialize_text_services( ITK_BATCH_TEXT_MODE );
    status = ITK_auto_login();
   
    if (  (status != ITK_ok)) printf("\nLogin Failed!\n\n");
    else
    {
        printf("\nLogin successful!\n\n");
        ITK_set_journalling(TRUE);
        do_it();
    }
    ITK_exit_module(TRUE);
    return status;
}
