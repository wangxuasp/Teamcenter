/*HEAD REPORT_ITEMS_PROPERTIES CCC ITK */
#include <stdlib.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/workspaceobject.h>
#include <tccore/item.h>
#include <property/prop.h>

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
        n_props,
        ii;
    char
        item_name[WSO_name_size_c + 1],
        *name,
        *value;
    tag_t
        item,
        *props = NULL;
        
    while (printf("\n\nEnter the Item name : ") 
        && gets(item_name) && strlen(item_name))   
    {
        ITK_CALL(ITEM_find_item(item_name, &item));
        ITK_CALL(PROP_list_properties(item, &n_props, &props)); 
        for (ii = 0; ii < n_props; ii++)
        {
            ITK_CALL(PROP_UIF_ask_name(props[ii], &name));
            ITK_CALL(PROP_UIF_ask_value(props[ii], &value));
            printf("%s = %s\n", name, value);
            if (name) MEM_free(name);
            if(value) MEM_free(value);
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
