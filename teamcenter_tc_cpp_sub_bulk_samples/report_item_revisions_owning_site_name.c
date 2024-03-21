/*HEAD REPORT_ITEM_REVISIONS_OWNING_SITE_NAME CCC ITK */
#include <stdlib.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/workspaceobject.h>
#include <ae/ae.h>
#include <user_exits/user_exits.h>
#include <ss/ss_const.h>
#include <tccore/item.h>
#include <pom/pom/pom.h>
#include <ae/dataset.h>
#include <epm/epm.h>
#include <property/prop.h>
#include <tccore/aom_prop.h>
#include <tccore/aom.h>
#include <tccore/workspaceobject.h>
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
        local_site = 0,
        local_site_id = 0;
    tag_t
        item_rev = NULLTAG,
        site = NULLTAG,
        imc = NULLTAG;
    char 
        local_sitename[SA_site_size_c+1] = "",
        *remote_name = NULL;
                          
    ERROR_CHECK(ITEM_find_rev("5402580", "A", &item_rev));
    EXIT_IF_NULL( item_rev );

    ERROR_CHECK(AOM_ask_value_tag(item_rev, "owning_site", &site));
    if (site == NULLTAG)
    {    
        ERROR_CHECK( POM_site_id(&local_site) );
        ERROR_CHECK( POM_lookup_imc(local_site, &imc) );
        ERROR_CHECK( SA_ask_site_info(imc, local_sitename, &local_site_id) ); 
        printf("\nItem Revision is local - Site Name: %s\n", local_sitename);  
    }
    else
    {
        ERROR_CHECK( AOM_ask_name(site, &remote_name) );
        printf("\nItem Revision is Remote - Site Name: %s\n\n\n", remote_name);
        MEM_free(remote_name);  
     }
}

int ITK_user_main(int argc, char* argv[])
{
    int
        status = 0; 
    char
        *message;
    
    ITK_initialize_text_services( 0 );
    status = ITK_auto_login();
   
    if (  (status != ITK_ok)) printf("iMAN login NOT successful.\n");
    else
    {
        printf("iMAN login successful.\n");
        ITK_set_journalling(TRUE);
        do_it();
    }
    ITK_exit_module(TRUE);
    return status;
}
