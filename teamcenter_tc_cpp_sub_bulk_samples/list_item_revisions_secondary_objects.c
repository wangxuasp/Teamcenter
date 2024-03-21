/*HEAD LIST_ITEM_REVISIONS_SECONDARY_OBJECTS CCC ITK */
#include <stdlib.h>
#include <tccore/aom.h>
#include <tc/emh.h>
#include <tccore/grm.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/item.h>
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
        n_list = 0,
        ii = 0;
    tag_t
        rev = NULLTAG,
        class = NULLTAG;
    char
        *name = NULL, 
        *class_name = NULL,
        type[WSO_name_size_c+1] = "";
    GRM_relation_t 
        *list; 
    
    ERROR_CHECK( ITEM_find_rev("5403783", "A", &rev) );
    EXIT_IF_NULL( rev );

    ERROR_CHECK( GRM_list_secondary_objects(rev, NULLTAG, &n_list, 
        &list) );
    EXIT_IF_NULL( n_list );

    for (ii = 0; ii < n_list; ii++)
    {
        ERROR_CHECK( AOM_ask_name(list[ii].secondary ,   &name) );
        printf("\nName: %s\n", name);

        ERROR_CHECK( AOM_ask_name(list[ii].relation_type,&name));
        printf("\trelation: %s\n", name);

        ERROR_CHECK( POM_class_of_instance(list[ii].secondary, &class) );
        ERROR_CHECK( POM_name_of_class(class, &class_name) );  
        printf("\tClass: %s\n", class_name);

        if (strcmp(class_name, "Dataset") == 0)
        {
            WSOM_ask_object_type(list[ii].secondary, type);
            printf("\tType: %s\n", type);
        }
        if (class_name) MEM_free(class_name);
        if (name) MEM_free(name);
    }
    if (n_list > 0 ) MEM_free(list);
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
