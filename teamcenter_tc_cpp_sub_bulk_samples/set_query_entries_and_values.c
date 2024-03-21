/*HEAD SET_QUERIES_ENTRIES_AND_VALUES CCC ITK */
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
        n_entries = 3, 
        ii = 0, 
        n_found = 0;
    tag_t 
        query = NULLTAG, 
        *results = NULL;
    char
        **entries = NULL, 
        **values = NULL, 
        critrea_entry  [3][16] = {"Release Status", 
                                  "Released After", 
                                  "Released Before"},
        critrea_value [3][18] =  {"oneStepWithStatus", 
                                "07-Mar-2006 00:00", 
                                "09-Mar-2006 00:00"};


    ERROR_CHECK( QRY_find("Item Revision...", &query) );
    EXIT_IF_NULL( query );
    
    entries = (char **) MEM_alloc(n_entries * sizeof(char *));
    values  = (char **) MEM_alloc(n_entries * sizeof(char *));

    for (ii = 0; ii < n_entries; ii++)
    {
        entries[ii] = (char *)MEM_alloc(strlen( critrea_entry[ii] ) + 1);
        strcpy(entries[ii], critrea_entry[ii] );

        values[ii] = (char *)MEM_alloc( strlen( critrea_value[ii] ) + 1);
        strcpy(values[ii], critrea_value[ii] );
    }

    printf(" Print entries and values...\n");
    for (ii = 0; ii < n_entries; ii++)
        printf("\tentries[%d] = %s\tvalues[%d] = %s\n", 
            ii,entries[ii], ii, values[ii]);

    printf("\n Execute Query...\n");
    QRY_execute(query, n_entries, entries, values, &n_found, &results);
    printf("\tn_found = %d\n", n_found);
    MEM_free(results);

    if (n_entries > 0) 
    {
        printf("\n Free entries and values....\n");
        for (ii = 0; ii < n_entries; ii++)
        {
            printf("\tMEM_free(entries[%d])\n", ii);
            MEM_free(entries[ii]);

            printf("\tMEM_free(values[%d])\n", ii);
            MEM_free(values[ii]);
        }
        printf("\tMEM_free(entries)\n", ii);
        MEM_free(entries);

        printf("\tMEM_free(values)\n", ii);
        MEM_free(values);
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
