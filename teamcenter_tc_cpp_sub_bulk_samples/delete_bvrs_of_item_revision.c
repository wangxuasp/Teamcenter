/*HEAD DELETE_BVRS_OF_ITEM_REVISION CCC ITK */
#include <stdlib.h>
#include <tccore/aom.h>
#include <tc/emh.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/item.h>
#include <itk/mem.h>
#include <pom/pom/pom.h>

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

#define SAFE_MEM_FREE( a )   \
do                          \
{                           \
    if ( (a) != NULL )      \
    {                       \
        MEM_free( (a) );    \
        (a) = NULL;         \
    }                       \
}                           \
while ( 0 )

static void do_it(void)
{
    int
        n_bvrs = 0, 
        ii = 0;
    tag_t   
        rev = NULLTAG, 
        *bvrs = NULL;
    char
        *name = NULL;

    ERROR_CHECK(ITEM_find_rev("1644530_1", "A", &rev));
    EXIT_IF_NULL(rev);
    ERROR_CHECK(AOM_lock(rev));

    ERROR_CHECK(ITEM_rev_list_bom_view_revs(rev, &n_bvrs, &bvrs));
    for (ii = 0; ii < n_bvrs; ii++)
    {
        
        ERROR_CHECK(AOM_ask_value_string(bvrs[ii], "object_name", &name));
        printf("Deleting %s\n", name);
        ERROR_CHECK(ITEM_rev_delete_bvr(rev, bvrs[ii]));
        SAFE_MEM_FREE(name);
    }
    ERROR_CHECK(AOM_unlock(rev));   
    SAFE_MEM_FREE(bvrs);
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
