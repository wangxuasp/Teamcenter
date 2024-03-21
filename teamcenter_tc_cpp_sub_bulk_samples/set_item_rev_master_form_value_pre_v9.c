/*** HEAD SET_ITEM_REV_MASTER_FORM_VALUE ITK ***/
#include<stdlib.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <sa/am.h> 
#include <tccore/item.h>
#include <tccore/workspaceobject.h>
#include <sa/sa.h> 
#include <form/form.h>
#include <tccore/aom.h>

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
    logical
        verdict;
    int
        n_revs,
        ii, 
        n_rev_attach,
        jj,
        actual_chars;
    char
        item_name[ITEM_name_size_c +1],
        value[FORM_name_size_c + 1];
    tag_t
        item,
        *revs;         
	ITEM_attached_object_t
        *objects;
        
    while (printf("\nEnter the Item ID: ") && gets(item_name) &&
        strlen(item_name))
    {
        ITK_CALL(ITEM_find_item(item_name, &item));   
        ITK_CALL(ITEM_list_all_revs(item, &n_revs, &revs));
	    printf("\nNumber of Item Revisions = %d.\n", n_revs);    
        for (ii = 0; ii < n_revs; ii++)
        {
            ITK_CALL(ITEM_list_rev_attachs_of_type(revs[ii], ITEM_item_master_atth,
                &n_rev_attach, &objects));
            for (jj = 0; jj < n_rev_attach; jj++)
            {
                ITK_CALL(FORM_ask_value(objects[jj].attachment, "project_id",
                    32, value, &actual_chars));
                printf("value = %s\n", value);
              
                ITK_CALL(POM_modifiable(objects[jj].attachment, &verdict));                
                if (!verdict)
                {
                    ITK_CALL(AOM_refresh (objects[jj].attachment, TRUE ));
                    ITK_CALL(FORM_set_value(objects[jj].attachment, "project_id", "zzz" ));
                    ITK_CALL(AOM_save(objects[jj].attachment));
                    ITK_CALL(AOM_unlock(objects[jj].attachment));
                    ITK_CALL(AOM_unload(objects[jj].attachment));
                    
                }
                ITK_CALL(FORM_ask_value(objects[jj].attachment, "project_id", 
                   32, value, &actual_chars));
                   printf("value = %s\n", value);            
            }
            MEM_free(objects);
        }
        MEM_free(revs);
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
