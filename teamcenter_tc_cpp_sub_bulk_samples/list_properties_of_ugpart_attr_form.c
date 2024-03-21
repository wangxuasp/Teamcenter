/*HEAD LIST_PROPERTIES_OF_UGPART_ATTR_FORM CCC ITK */
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
void list_displayable_properties_with_value(char *indention, tag_t object)
{
    logical 
        is_displayable = TRUE;
    int 
        n_props = 0, 
        ii = 0;
    char 
        **prop_names = NULL, 
        *disp_name = NULL, 
        *value = NULL; 

    ERROR_CHECK( AOM_ask_prop_names(object, &n_props, &prop_names) );
    for( ii = 0; ii < n_props; ii++)
    {
        ERROR_CHECK( AOM_UIF_is_displayable(object, prop_names[ii], &is_displayable));
        if (is_displayable == TRUE)
        {
            value = NULL;
            ERROR_CHECK( AOM_UIF_ask_name(object, prop_names[ii], &disp_name) );
            ERROR_CHECK( AOM_UIF_ask_value(object, prop_names[ii], &value) );
            if ( (value != NULL) && (strlen(value) > 0 ) )
            {
                if (strlen(value) == 1 )
                {
                    if ( strcmp(value, " ") != 0 )
                        fprintf(stdout, "%s %s: %s \n",indention, disp_name, value ); 
                }
                else
                fprintf(stdout, "%s %s: %s\n",  indention, disp_name, value);
            }
        }
    }
    if (prop_names != NULL) MEM_free(prop_names);
    if (disp_name != NULL) MEM_free(disp_name);
    if (value != NULL) MEM_free(value);
}

static void do_it(void)
{
    int
        n_named_refs = 0,
        ii = 0;
    tag_t
        ugpart = NULLTAG,
        *named_refs = NULL;

    ERROR_CHECK( AE_find_dataset("5916377/A", &ugpart) );
    if (ugpart == NULLTAG)
    {
        printf("Dataset NOT found!\n");
    }
    else
    {
        ERROR_CHECK( AE_ask_all_dataset_named_refs(ugpart, 
            "UGPART-ATTR", &n_named_refs, &named_refs));
        if (n_named_refs == 0 )
        {
            printf("UGPART-ATTR named reference NOT found!\n");
        }
        else
        {
            for (ii = 0; ii < n_named_refs; ii++)
            {
                list_displayable_properties_with_value("   ", named_refs[ii]);                  
            }                   
        }
        if (named_refs) MEM_free(named_refs);
    }
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
