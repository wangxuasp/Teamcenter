/*HEAD DELETE_ALL_BOMVIEW_REVISIONS CCC ITK */
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

static void do_it(void)
{
    logical
        *is_it_null = NULL,
        *is_it_empty = NULL;
    int
        rc = ITK_ok,
        n_bvrs = 0, 
        ii = 0, 
        n_values = 0;
    tag_t   
        rev = NULLTAG, 
        *bvrs = NULL, 
        attr_id = NULLTAG, 
        *attr_tags = NULL;
    char    
        *name = NULL;   
    
    ERROR_CHECK( ITEM_find_rev("5397460", "P", &rev) );
    EXIT_IF_NULL( rev );
    ERROR_CHECK( AOM_lock(rev) );

    ERROR_CHECK( POM_attr_id_of_attr( "structure_revisions", 
        "ItemRevision", &attr_id) );
    EXIT_IF_NULL( attr_id );

    ERROR_CHECK( POM_length_of_attr( rev, attr_id, &n_values) );
    EXIT_IF_NULL(n_values);

    ERROR_CHECK(POM_ask_attr_tags(rev, attr_id, 0, n_values, &attr_tags, 
        &is_it_null, &is_it_empty ));
    EXIT_IF_NULL(n_values);

    for (ii =  0; ii < n_values; ii++)
        ERROR_CHECK( POM_remove_from_attr( 1, &rev, attr_id, ii, 1 ) );

    if(n_values < 0) MEM_free(attr_tags);
    if(n_values < 0) MEM_free(is_it_null);
    if(n_values < 0) MEM_free(is_it_empty);

    ERROR_CHECK( AOM_save( rev ));
    ERROR_CHECK( AOM_refresh( rev, FALSE ));

    ERROR_CHECK( ITEM_rev_list_bom_view_revs(rev, &n_bvrs, &bvrs) );
    EXIT_IF_NULL( n_bvrs );
    
    for (ii = 0; ii < n_bvrs; ii++)
    {
        ERROR_CHECK( AOM_ask_name(bvrs[ii], &name) );
        ERROR_CHECK( AOM_delete( bvrs[ii]) );
        printf("- Deleting %s\n", name);
        MEM_free(name);
    }
    if (n_bvrs < 0) MEM_free(bvrs);
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
