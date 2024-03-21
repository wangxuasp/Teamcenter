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
        n_objects = 0,
        ii = 0;
    tag_t
        rev = NULLTAG,
        class = NULLTAG,
        relation_type = NULLTAG,
        *objects;
    char
        *id_string = NULL, 
        *class_name = NULL,
        type[WSO_name_size_c+1] = "",
        *display_value = NULL; 
    
    ERROR_CHECK( ITEM_find_rev("10001", "A", &rev) );
    EXIT_IF_NULL( rev );

    ERROR_CHECK( GRM_find_relation_type("IMAN_specification", &relation_type) );
    EXIT_IF_NULL( relation_type );

    ERROR_CHECK( GRM_list_secondary_objects_only(rev, relation_type, 
        &n_objects, &objects) );
    EXIT_IF_NULL( n_objects );

    for (ii = 0; ii < n_objects; ii++)
    {
        ERROR_CHECK( POM_class_of_instance(objects[ii], &class) );
        ERROR_CHECK( POM_name_of_class(class, &class_name) );
        if (strcmp(class_name, "Dataset") == 0)
        {
            ERROR_CHECK( WSOM_ask_object_type(objects[ii], type) );
            if (strcmp(type, "UGMASTER") == 0)
            {
                ERROR_CHECK( WSOM_ask_object_id_string (objects[ii], 
                    &id_string) );
                ERROR_CHECK( AOM_UIF_ask_value(objects[ii], "last_mod_date",
                    &display_value) );
                printf("Object: %s - Type: %s - Date Modified: %s\n",
                    id_string, type, display_value);
            }
        }
        if (class_name) MEM_free(class_name);
        if (id_string) MEM_free(id_string);
        if (display_value) MEM_free(display_value);
    }
    if (objects) MEM_free(objects);
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
