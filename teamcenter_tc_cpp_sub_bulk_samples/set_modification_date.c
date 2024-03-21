/*HEAD SET_MODIFICATION_DATE CCC ITK */
#include <stdlib.h>
#include <tccore/aom.h>
#include <tc/emh.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <itk/mem.h>

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
    char   
        *item_name = ITK_ask_cli_argument("-i="), 
        *date_string = NULL;
    tag_t  
        item = NULLTAG, 
        attr_id = NULLTAG;
    date_t 
        ask_date, 
        set_date;

    
    ERROR_CHECK( ITEM_find_item(item_name, &item) );
    EXIT_IF_NULL( item );   
    ERROR_CHECK( AOM_refresh(item, TRUE) );
    
    ERROR_CHECK( AOM_ask_modified_date(item, &ask_date) );
    ERROR_CHECK( ITK_date_to_string(ask_date, &date_string) );
    printf("Before POM_set_modification_date: %s\n", date_string);
    if (date_string) MEM_free(date_string);
    
    ERROR_CHECK( AOM_refresh(item, TRUE) );
    ERROR_CHECK(ITK_string_to_date( "14-Feb-2006 00:00", &set_date) );
    ERROR_CHECK( POM_set_env_info(POM_bypass_attr_update, FALSE, 0, 0, NULLTAG, NULL) );
    ERROR_CHECK( POM_set_modification_date(item, set_date) );

    
    ERROR_CHECK( AOM_save(item ) );

    ERROR_CHECK( AOM_refresh(item, TRUE) );
    ERROR_CHECK( AOM_ask_modified_date(item, &ask_date) );
    ERROR_CHECK( ITK_date_to_string(ask_date, &date_string) );
    printf("After  POM_set_modification_date: %s\n", date_string);
    if (date_string) MEM_free(date_string);

    ERROR_CHECK( AOM_refresh(item, FALSE) );
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
