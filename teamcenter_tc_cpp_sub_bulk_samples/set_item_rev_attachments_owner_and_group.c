/*HEAD SET_ITEM_REV_ATTACHMENTS_OWNER_AND_GROUP ITK */
#include<stdlib.h>
#include <tc/tc.h>
#include <tccore/aom.h>
#include <tccore/item.h>
#include <sa/sa.h>
#include <tccore/workspaceobject.h>

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

static void report_owner_and_group(tag_t object)
{
    WSO_description_t
        desc;
    ITK_CALL(WSOM_describe(object, &desc));
    printf("\n\tName: %s\tType: %s\n", desc.object_name, desc.object_type);
    printf("\t\tOwner: %s\tGroup: %s\n", desc.owners_name, desc.owning_group_name);
}

static void set_ownership(tag_t object, tag_t user, tag_t group)
{
    ITK_CALL(AOM_refresh(object, 1));
    
    printf("\nBefore calling AOM_set_ownership...");
    report_owner_and_group(object);
    
    ITK_CALL(AOM_set_ownership(object, user, group));
    ITK_CALL(AOM_save(object));
    
    printf("After calling AOM_set_ownership...");
    report_owner_and_group(object);
    
    ITK_CALL(AOM_unload(object));   
}
 
static void do_it(void)
{
    int
        ii,
        count;
    char
        owner_name[SA_user_size_c + 1],
        group_name[SA_name_size_c + 1],
        item_name[ITEM_name_size_c +1];
    tag_t   
        item, 
        group,
        rev,
        user;

    ITEM_attached_object_t
        *objects;
            
    printf("\nEnter New Owner Name : ");
    gets(owner_name);
    ITK_CALL(SA_find_user(owner_name, &user));
    
    printf("\nEnter New Group Name : ");
    gets(group_name);
    ITK_CALL(SA_find_group(group_name, &group));    
    
    while (printf("\nEnter the item name : ") && gets(item_name) &&
        strlen(item_name))
    {
        ITK_CALL(ITEM_find_item(item_name, &item));
        set_ownership(item, user, group);
    
        ITK_CALL(ITEM_ask_latest_rev(item, &rev));
        set_ownership(rev, user, group);
    
        ITK_CALL(ITEM_list_all_rev_attachments(rev, &count, &objects));
        for (ii = 0; ii < count; ii++)
        {
            set_ownership(objects[ii].attachment, user, group);
        }
        if (objects) MEM_free(objects); 
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
