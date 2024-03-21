/* HEAD  REPORT CURRENT SITE NAME CCC ITK */
#include <stdlib.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/workspaceobject.h>
#include <tccore/item.h>
#include <sa/site.h>
#include <pom/pom/pom.h>

#define ITK_CALL(x) {           \
    int stat;                     \
    char *err_string;             \
    if( (stat = (x)) != ITK_ok)   \
    {                             \
	EMH_get_error_string (NULLTAG, stat, &err_string);                 \
	printf ("ERROR: %d ERROR MSG: %s.\n", stat, err_string);           \
	printf ("FUNCTION: %s\nFILE: %s LINE: %d\n",#x, __FILE__, __LINE__); \
	if(err_string) MEM_free(err_string);                                \
    exit (EXIT_FAILURE);                                                   \
    }                                                                    \
}
#define EXIT_FAILURE 1 

static void do_it(void)
{
    int site;
    tag_t   imc;
    char sitename[SA_site_size_c+1];
    int siteid;
    
    ITK_CALL(POM_site_id(&site));
    ITK_CALL(POM_lookup_imc(site, &imc));
    ITK_CALL(SA_ask_site_info(imc, sitename, &siteid)); 
    printf("sitename - %s\n", sitename);  
}

int ITK_user_main(int argc, char* argv[])
{
    int
        status = 0; 
   
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
