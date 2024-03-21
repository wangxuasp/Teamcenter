/*HEAD REPORT_PUBLICATION_INFORMATION_OF_REMOTE_ITEM CCC ITK */
#include <stdlib.h>
#include <tccore/aom.h>
#include <tc/emh.h>
#include <tc/tc.h>
#include <tccore/tctype.h>
#include <sa/tcfile.h>
#include <itk/mem.h>
#include <sa/site.h>

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

void list_displayable_properties(char *indention, tag_t object)
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

    ERROR_CHECK(AOM_ask_prop_names(object, &n_props, &prop_names));
    for( ii = 0; ii < n_props; ii++)
    {
        ERROR_CHECK(AOM_UIF_is_displayable(object, prop_names[ii], 
            &is_displayable));
        if (is_displayable == TRUE)
        {
            ERROR_CHECK(AOM_UIF_ask_name(object, prop_names[ii], &disp_name));
            ERROR_CHECK(AOM_UIF_ask_value(object, prop_names[ii], &value));
            fprintf(stdout, "%s %s: %s\n",  indention, disp_name, value);
        }
        
    }
    if (prop_names != NULL) MEM_free(prop_names);
    if (disp_name != NULL) MEM_free(disp_name);
    if (value != NULL) MEM_free(value);
}

static void do_it(void)
{
    int 
        ii = 0,
        n_found = 0, 
        site_id = 0;
    tag_t 
        query = NULLTAG, 
        *results = NULL,
        description_obj = NULLTAG;
    char  
        **entries = NULL, 
        **values = NULL;

    ERROR_CHECK(SITE_ask_default_ods(&site_id)); 
    ERROR_CHECK(QRY_find("Item...", &query));
    if (query != NULLTAG)
    {    
        entries = (char **) MEM_alloc(1 * sizeof(char *));
        values  = (char **) MEM_alloc(1 * sizeof(char *));

        entries[0] = (char *)MEM_alloc(strlen( "Item ID" ) + 1);
        strcpy(entries[0], "Item ID" );

        values[0] = (char *)MEM_alloc( strlen( "5870737" ) + 1);
        strcpy(values[0], "5870737");

        QRY_execute_at_site(0, query, 1, entries, values, 
            &n_found, &results);
        printf("n_found = %d\n", n_found);
        for (ii = 0; ii < n_found; ii++)
        {
            ERROR_CHECK(ODS_describe_object(site_id, results[ii], 
                &description_obj)); 
            list_displayable_properties("    ", description_obj);
        }       
        SAFE_MEM_FREE(results);
        SAFE_MEM_FREE(entries[0]);
        SAFE_MEM_FREE(entries);
        SAFE_MEM_FREE(values[0]);
        SAFE_MEM_FREE(values);
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
