greps /* HEAD INITIATE_A_JOB CCC ITK */
#include <stdlib.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/item.h>
#include <tc/folder.h>
#include <tccore/workspaceobject.h>
#include <ae/ae.h>

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
    tag_t
        new_item,
        new_rev,
        new_job;
          
   ITK_CALL( ITEM_create_item("5012332", "5012332", "Item", "A",  &new_item, &new_rev));
   printf("Saving Item\n");
   ITK_CALL(ITEM_save_item(new_item));
   
   ITK_CALL(CR_create_job("my_job","","QuickRelease",&new_job));
   ITK_CALL(CR_add_target_objects(new_job, 1, &new_rev));
   ITK_CALL(CR_initiate(new_job));
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
