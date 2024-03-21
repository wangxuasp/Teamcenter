/*HEAD CLASSIFY_NEW_ITEM CCC ITK */
#include <stdlib.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/workspaceobject.h>
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
    logical
        verdict;
    tag_t
        item,
        itemRev,
        class,
        view,
        classificationObject;
    
    printf("\n\nCreating item 1305236...\n\n");   
    ITK_CALL(ITEM_create_item("1305236", "1305236", "Item", "A",
        &item, &itemRev));    
    ITK_CALL(ITEM_save_item(item));
    
    printf("Classify the new item just to prove it IS classifiable...\n\n");
    ITK_CALL(ICS_find_class("ugfc010101", &class));
    ITK_CALL(ICS_find_view(class, "defaultView", &view));
    ITK_CALL(ICS_create_classification_object(item, "1305236", view,
        &classificationObject));
    ITK_CALL(ICS_classify_wsobject(item, classificationObject));
    
    printf("Output from ICS_is_wsobject_classified - ");
    ITK_CALL(ICS_is_wsobject_classified(item, &verdict));
    if (verdict == TRUE) printf("Item is classified\n\n");
    if (verdict == FALSE) printf("Item is NOT classified\n\n");
    
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
