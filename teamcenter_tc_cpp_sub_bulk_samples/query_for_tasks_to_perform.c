/*HEAD QUERY_FOR_TASKS_TO_PERFORM CCC ITK */
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
        ii = 0, 
        n_found = 0,
        n_contents = 0,
        kk = 0;
    tag_t 
        query = NULLTAG, 
        *folders = NULL,
        *contents = NULL,
        user = NULLTAG;
    char
        **entries = NULL, 
        **values = NULL,
        personname[SA_name_size_c+1] = "",
        *type = NULL,
        *name = NULL;

    ERROR_CHECK( QRY_find("General...", &query) );
    EXIT_IF_NULL( query );
    
    entries = (char **) MEM_alloc(2 * sizeof(char *));
    entries[0] = (char *)MEM_alloc(strlen("Name") + 1);
    strcpy(entries[0], "Name");
    entries[1] = (char *)MEM_alloc(strlen("Type") + 1);
    strcpy(entries[1], "Type");

    values  = (char **) MEM_alloc(2 * sizeof(char *));
    values[0] = (char *)MEM_alloc( strlen("Tasks to Perform") + 1);
    strcpy(values[0], "Tasks to Perform");
    values[1] = (char *)MEM_alloc( strlen("TasksToPerform") + 1);
    strcpy(values[1], "TasksToPerform");

    QRY_execute(query, n_entries, entries, values, &n_found, &folders);
    for (ii = 0; ii < n_found; ii++)
    {
        ERROR_CHECK(AOM_ask_value_tags(folders[ii], "contents", &n_contents, 
            &contents));
        if (n_contents != 0)
        {
            ERROR_CHECK(AOM_ask_value_tag(folders[ii], "last_mod_user", &user));
            ERROR_CHECK(AOM_ask_name(user, &name));
            printf("\n %s had the following tasks to perform:\n", name);
            SAFE_MEM_FREE(name);
            for (kk = 0; kk < n_contents; kk++)
            {
                ERROR_CHECK(AOM_ask_value_string(contents[kk], "object_type", 
                    &type));
                ERROR_CHECK(AOM_ask_name(contents[kk], &name));
                printf("\t%s - %s \n", name, type);
                SAFE_MEM_FREE(name);
                SAFE_MEM_FREE(type);
                
            }
            SAFE_MEM_FREE(contents);
        }
    }
    SAFE_MEM_FREE(folders);

    if (n_entries > 0) 
    {
        for (ii = 0; ii < n_entries; ii++)
        {
           SAFE_MEM_FREE(entries[ii]);
           SAFE_MEM_FREE(values[ii]);
        }
        SAFE_MEM_FREE(entries);
        SAFE_MEM_FREE(values);
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
