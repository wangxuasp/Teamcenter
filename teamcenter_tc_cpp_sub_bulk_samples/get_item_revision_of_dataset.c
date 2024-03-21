/*HEAD GET_ITEM_REVISION_OF_DATASET CCC ITK */
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
        printf("\n Exiting program! \n");
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
        n_objects = 0,
        ii = 0;
    tag_t
        dataset = NULLTAG, 
        relation_type = NULLTAG,
        *objects = NULL,
        class = NULLTAG; 
    char
        *id_string = NULL, 
        *class_name = NULL;

    ERROR_CHECK(AE_find_dataset("000001/A", &dataset)); 
    if (dataset == NULLTAG)
        printf("Dataset not found!\n");
    else
    {
        ERROR_CHECK(GRM_list_primary_objects_only(dataset, relation_type,
            &n_objects, &objects));
        for (ii = 0; ii < n_objects; ii++)
        {
            ERROR_CHECK( POM_class_of_instance(objects[ii], &class));
            ERROR_CHECK( POM_name_of_class(class, &class_name));
            if (strcmp(class_name, "ItemRevision") == 0)
            {
                ERROR_CHECK( WSOM_ask_object_id_string (objects[ii], 
                    &id_string));
                printf("%s\n", id_string);
                SAFE_MEM_FREE(id_string);
            }                   
            SAFE_MEM_FREE(class_name);  
        }
        SAFE_MEM_FREE(objects);
    }
}

int ITK_user_main(int argc, char* argv[])
{
    int
        status = 0; 
   
    ITK_initialize_text_services( ITK_BATCH_TEXT_MODE );
    status = ITK_auto_login();
    if (  (status != ITK_ok)) printf("\n Login Failed! \n\n");
    else
    {
        printf("\n Login successful! \n\n");
        ITK_set_journalling(TRUE);
        do_it();
    }
    ITK_exit_module(TRUE);
    return status;
}
