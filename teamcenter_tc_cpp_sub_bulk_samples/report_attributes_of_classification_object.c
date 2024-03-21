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
    int
        ii,
        nAttributes;
    tag_t
        class,
        view,
        item,
        rev,
        classificationObject;
    char
        **attributeNames,
        **attributeValues:
                 
    ITK_CALL(ICS_find_class("MY_CLASS", &class));
    ITK_CALL(ICS_find_view(class, "MY_USER_VIEW", &view));
    ITK_CALL(ITEM_create_item("MY_ITEM", "item_name", "Item", "A", &item, &rev));
    
    ITK_CALL(ICS_create_classification_object(item, "MY_ITEM", 
        view, &classificationObject));
                
    ITK_CALL(ICS_ask_attributes_of_classification_obj(classificationObject,
        &nAttributes, &attributeNames, &attributeValues));
        
    printf("ICS_ask_attributes_of_classification_obj nAttributes = %d\n", nAttributes);
    for(ii = 0; ii < nAttributes; ii++)
    {
        printf("\t%s - %s\n", attributeNames[ii], attributeValues[ii]);
        MEM_free(attributeNames[ii]);
        MEM_free(attributeValues[ii]); 
    }
    MEM_free(attributeNames);
    MEM_free(attributeValues);     
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
