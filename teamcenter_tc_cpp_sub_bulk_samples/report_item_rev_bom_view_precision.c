#include<stdlib.h>
#include <tc/tc.h>
#include <tccore/workspaceobject.h>
#include <sa/sa.h> 
#include <sa/tcfile.h>
#include <tccore/item.h>
 #include <ps/ps_errors.h>

#define EXIT_FAILURE 1
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

static void do_it(void)
{
    
    int
         ii,
         count;
    logical
        is_precise; 
    char
        item_name[ITEM_name_size_c +1],
        name[WSO_name_size_c + 1];
    tag_t   
        item, 
        rev,
        *bvrs;

     while (printf("\nEnter the item name : ") && gets(item_name) &&
         strlen(item_name))
     {
         ITK_CALL(ITEM_find_item(item_name, &item));
         ITK_CALL(ITEM_ask_latest_rev(item, &rev));
         ITK_CALL(ITEM_rev_list_bom_view_revs(rev, &count, &bvrs));
         for (ii = 0; ii < count; ii++)
         {
            ITK_CALL(WSOM_ask_name(bvrs[ii], name));
            ITK_CALL(PS_ask_is_bvr_precise(bvrs[ii], &is_precise));
            if (is_precise) printf("Bomview: %s is precise\n", name);
            if (!is_precise) printf("Bomview: %s is imprecise\n", name);
         }         
     }  
}

int ITK_user_main(int argc, char* argv[])
{
    int
        status = 0; 
    char
        *message;
    
    ITK_CALL(ITK_initialize_text_services( 0 ));
    status = ITK_auto_login();
   
    if (  (status != ITK_ok)) printf("iMAN login NOT successful.\n");
    else
    {
        printf("iMAN login successful.\n\n");
        ITK_CALL(ITK_set_journalling(TRUE));
        do_it();
    }
    ITK_exit_module(TRUE);
    return status;
}
