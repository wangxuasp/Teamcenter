/*HEAD GET_RELEASE_STATUS_LIST CCC ITK */
#include <stdlib.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/item.h>
#include <tccore/workspaceobject.h>

#define ERROR_CHECK(x) {           \
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
        n_statuses = 0,
        ii = 0;
    tag_t
        item_rev = NULLTAG,
        *statuses = NULL;
    char 
        *name = NULL;
                               
    ERROR_CHECK(ITEM_find_rev("123456", "A", &item_rev));
    if (item_rev == NULLTAG)
    {
        printf("Item Revision NOT found!\n");
    }
    else
    {
        ERROR_CHECK(WSOM_ask_release_status_list(item_rev, &n_statuses, 
            &statuses)); 
        printf("number of statuses: %d \n",  n_statuses);
        for (ii = 0; ii < n_statuses; ii++)
        {
            ERROR_CHECK(AOM_ask_name(statuses[ii], &name));
            printf("\t %s\n", name);
        }
        if (name) MEM_free(name);
        if (n_statuses) MEM_free(statuses);                                
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
