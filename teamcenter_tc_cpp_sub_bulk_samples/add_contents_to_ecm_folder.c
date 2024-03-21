/*HEAD ADD_CONTENTS_TO_ECM_FOLDER CCC ITK */
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
    int
        n_revisions = 0,
        ii = 0;
    tag_t
        item = NULLTAG,
        *item_revisions = NULL,
        ec_rev = NULLTAG;

    ERROR_CHECK( ITEM_find_item("5395884", &item) );
    EXIT_IF_NULL( item );

    ERROR_CHECK( ITEM_find_revisions(item, "*", &n_revisions, &item_revisions) ); 

    ERROR_CHECK( ECM_find_ec_revision("CR0003", "A", &ec_rev) );
    EXIT_IF_NULL( ec_rev );

    ERROR_CHECK( ECM_add_contents(ec_rev, "Reference Items", n_revisions, 
        item_revisions) );

    if (item_revisions) MEM_free(item_revisions);
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
