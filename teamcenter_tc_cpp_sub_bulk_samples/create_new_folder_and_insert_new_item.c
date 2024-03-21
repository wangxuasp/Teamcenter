/*HEAD  CREATE_NEW_FOLDER_AND_INSERT_NEW_ITEM ITK CCC*/
#include<stdlib.h>
#include <tc/tc.h>
#include <tccore/workspaceobject.h>
#include <sa/sa.h> 
#include <sa/tcfile.h>
#include <tccore/item.h>
#include <tc/folder.h>
#include <ae/ae.h>

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

void report_folder_references(tag_t folder)
{
    int
        num_of_references,
        ii;
    char
        *object_id,
        name[WSO_name_size_c + 1],
        type[WSO_name_size_c + 1];
    tag_t
        *list_of_references;
        
    ITK_CALL(FL_ask_references(folder, FL_fsc_by_name, &num_of_references, 
        &list_of_references));
    for (ii = 0; ii < num_of_references; ii++)
    {
        ITK_CALL(WSOM_ask_object_id_string(list_of_references[ii], &object_id));
        ITK_CALL(WSOM_ask_name(list_of_references[ii], name));
        ITK_CALL(WSOM_ask_object_type(list_of_references[ii], type));
        printf("%s\t%s\t%s\n", object_id, name, type);
        MEM_free(object_id);
    } 
    if(num_of_references) MEM_free(list_of_references);     
}   

static void create_item (char *item_id, tag_t *new_item, tag_t *new_rev)
{
    char
        item_name[ITEM_name_size_c + 1],
        type_name[ITEM_type_size_c + 1] = {""},
        rev_id[ITEM_id_size_c + 1] = "A";
        
    strcpy(item_name, item_id);
    printf("Creating Item ID: %s\n", item_id);
    ITK_CALL( ITEM_create_item( item_id, item_name, type_name, rev_id, new_item, new_rev));

    printf("Saving Item ID: %s\n", item_id);
    ITK_CALL(ITEM_save_item(*new_item));

    printf("Attaching Item ID: %s to Newstuff Folder\n", item_id);
    ITK_CALL( FL_user_update_newstuff_folder(*new_item)); 
}

static void create_dataset(char *type_name, char *name, tag_t item, tag_t rev, tag_t *dataset)
{
    char
        format_name[AE_io_format_size_c + 1] = "BINARY_REF";
    tag_t
        datasettype,
        tool;
    
    ITK_CALL(AE_find_datasettype(type_name, &datasettype));
    if (datasettype == NULLTAG)
    {
        printf("Dataset Type %s not found!\n", type_name);
        exit (EXIT_FAILURE);
    }
    
    ITK_CALL(AE_ask_datasettype_def_tool(datasettype, &tool));
    
    printf("Creating Dataset: %s\n", name);
    ITK_CALL(AE_create_dataset(datasettype, name, "", dataset));
    
    ITK_CALL(AE_set_dataset_tool(*dataset, tool));
    if (strcmp(type_name, "Text")) strcpy(format_name, "TEXT_REF");
    
    ITK_CALL(AE_set_dataset_format(*dataset, format_name));
    printf("Saving Dataset: %s\n", name);
    ITK_CALL(AOM_save(*dataset));
    
    
    /*attach dataset to item revision */
    ITK_CALL(ITEM_attach_rev_object(rev, *dataset, ITEM_specification_atth));
    ITK_CALL(ITEM_save_item(item));

}


void ask_current_users_home_folder(tag_t *home_folder)
{
    char
        *user_name_string;
    tag_t
        user;

    ITK_CALL(SA_init_module());
    ITK_CALL(POM_get_user(&user_name_string, &user));
    ITK_CALL(SA_ask_user_home_folder(user, home_folder));
    ITK_CALL(SA_exit_module());
}
 
static void do_it(void)
{
    tag_t
        home_folder,
        folder,
        new_item,
        new_rev,
        dataset;
    char
        item_id[] = "1335685B",
        dataset_name[] = "1335685B/A" ,
        datasettype_name[] = "UGPART";
           
    ask_current_users_home_folder(&home_folder);
    create_item(item_id, &new_item, &new_rev);
    create_dataset(datasettype_name, dataset_name, new_item, new_rev, &dataset);
    ITK_CALL(FL_create("1335685B_folder", "1335685B_folder_desc", &folder));
    ITK_CALL(AOM_save(folder));    
    ITK_CALL(FL_insert(home_folder, folder, -1));
    ITK_CALL(FL_insert(folder, new_item, -1));
    ITK_CALL(AOM_save(folder));    
    ITK_CALL(AOM_save(home_folder));    
    report_folder_references(folder);
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
        printf("iMAN login successful.\n\n");
        ITK_CALL(ITK_set_journalling(TRUE));
        do_it();
    }
    ITK_exit_module(TRUE);
    return status;
}
