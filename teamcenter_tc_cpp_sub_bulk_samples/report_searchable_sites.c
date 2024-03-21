/*HEAD REPORT_SEARCHABLE_SITES CCC ITK */
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

static void do_it(void)
{
    int 
        nsites_included = 0,  
        nsites_excluded = 0,  
        user_nsites_included = 0,  
        user_nsites_excluded = 0,
        ii = 0,
        site = 0;   
    tag_t 
        *sites_included = NULL,
        *sites_excluded = NULL,
        *user_sites_included = NULL, 
        *user_sites_excluded = NULL;
    char  
        sitename[SA_site_size_c+1] = "";  
    
    ERROR_CHECK(SITE_ask_searchable_sites( &nsites_included, &sites_included, 
		&nsites_excluded, &sites_excluded, &user_nsites_included, 
		&user_sites_included, &user_nsites_excluded, &user_sites_excluded));

    printf("\nnsites_included = %d\n", nsites_included);
    for (ii = 0; ii < nsites_included; ii++)
    {
        ERROR_CHECK(SA_ask_site_info(sites_included[ii], sitename, &site));
        printf("\tsite name: %s\n", sitename);
    }

    printf("\nnsites_excluded = %d\n", nsites_excluded);
    for (ii = 0; ii < nsites_excluded; ii++)
    {
        ERROR_CHECK(SA_ask_site_info(sites_excluded[ii], sitename, &site)); 
        printf("\tsite name: %s\n", sitename);
    }

    printf("\nuser_nsites_included = %d\n", user_nsites_included);
    for (ii = 0; ii < user_nsites_included; ii++)
    {
        ERROR_CHECK(SA_ask_site_info(user_sites_included[ii], sitename, 
			&site));
        printf("\tsite name: %s\n", sitename);
    }
      
    printf("\nuser_nsites_excluded = %d\n", user_nsites_excluded);
    for (ii = 0; ii < user_nsites_excluded; ii++)   
    {
        ERROR_CHECK(SA_ask_site_info(user_sites_excluded[ii], sitename, 
			&site)); 
        printf("\tsite name: %s\n", sitename);
    }
    SAFE_MEM_FREE(sites_included);
    SAFE_MEM_FREE(sites_excluded);
    SAFE_MEM_FREE(user_sites_included); 
    SAFE_MEM_FREE(user_sites_excluded);
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
