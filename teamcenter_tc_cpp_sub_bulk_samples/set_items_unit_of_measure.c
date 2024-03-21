/*HEAD SET_ITEMS_UNIT_OF_MEASURE CCC ITK */
#include <stdlib.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/workspaceobject.h>
#include <ae/ae.h>
#include <ss/ss_const.h>
#include <tccore/item.h>

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
        n_uom,
        ii;
    char
        item_name[WSO_name_size_c + 1],
        uom_name[WSO_name_size_c + 1],
        *name;
    tag_t
        item,
        uom,
        *uom_list = NULL;
        
    ITK_CALL(UOM_extent(&n_uom, &uom_list));
    if (!uom_list)
    { 
        printf("There are no Units of Measure defined\n");
        return;
    }
    else
    {
        printf("\nAvailable Unit of Measures are...\n");
        for (ii = 0; ii < n_uom; ii++)
        {
            ITK_CALL(AOM_ask_name(uom_list[ii], &name));
            printf("\t%s\n", name);
            if (name) MEM_free(name);
        }
    } 
    if (uom_list) MEM_free(uom_list); 
    while (printf("\n\nEnter the Item name : ") 
        && gets(item_name) && strlen(item_name))   
    {
        ITK_CALL(ITEM_find_item(item_name, &item));    
        ITK_CALL(ITEM_ask_unit_of_measure(item, &uom));
        ITK_CALL(AOM_ask_name(uom, &name));
        printf("Before ITEM_set_unit_of_measure - %s\n\n", name);
        if (name) MEM_free(name);
        
        if (printf("Enter the new UOM symbol: ") 
            && gets(uom_name) && strlen(uom_name))
        {    
            ITK_CALL(UOM_find_by_symbol(uom_name, &uom));    
            ITK_CALL(ITEM_set_unit_of_measure(item, uom));
            ITK_CALL(AOM_save(item));
    
            ITK_CALL(ITEM_ask_unit_of_measure(item, &uom));
            ITK_CALL(AOM_ask_name(uom, &name));
            printf("\nAfter ITEM_set_unit_of_measure - %s\n", name);
            if (name) MEM_free(name); 
        }
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
