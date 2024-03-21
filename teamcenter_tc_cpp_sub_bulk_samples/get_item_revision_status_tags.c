/* HEAD GET_ITEM_REVISIONS_STATUS_TAG  CCC ITK */
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
    int
        prop_count = 0,
        ii = 0,
        n_statuses = 0;
    tag_t
        item_rev = 0,
        *statuses = 0;
    char 
        **prop_names = NULL;
                               
    ITK_CALL(ITEM_find_rev("block", "A", &item_rev));
    ITK_CALL(AOM_ask_prop_names(item_rev, &prop_count, &prop_names));    
    for (ii = 0; ii < prop_count; ii++)
    {
        if (!strcmp(prop_names[ii],"release_statuses"))
        {
            ITK_CALL(AOM_ask_value_tags(item_rev, prop_names[ii], &n_statuses, &statuses)); 
            printf("There are %d status tags for this Item Revision\n",  n_statuses); 
            if (n_statuses) MEM_free(statuses);                          
        }        
    } 
    MEM_free(prop_names);  
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
