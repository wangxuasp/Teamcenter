/*HEAD PERFORM_CASE_SENSITIVE_QUERY CCC ITK */
#include <stdlib.h>
#include <tccore/aom.h>
#include <tc/emh.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <itk/mem.h>
#include <sa/person.h>

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
        n_entries = 2, 
        n_found = 0;
    tag_t 
        query = NULLTAG, 
        *objects = NULL;
    char
        *entries[2] = {"Name", "Type"},
        *values[2] =  {"MyFolder", "Folder"};

    ERROR_CHECK( QRY_find("General...", &query) );
    EXIT_IF_NULL( query );

    printf("Results using default search options:\n");
    QRY_execute(query, n_entries, entries, values, &n_found, &objects);
    printf("\t n_found = %d\n\n", n_found);
    SAFE_MEM_FREE(objects);

    printf("Results using QRY_ignore_case_on_search(FALSE):\n");
    QRY_ignore_case_on_search(FALSE);
    QRY_execute(query, n_entries, entries, values, &n_found, &objects);
    printf("\t n_found = %d\n", n_found);
    SAFE_MEM_FREE(objects);


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
