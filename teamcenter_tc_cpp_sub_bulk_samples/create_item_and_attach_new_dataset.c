/*HEAD CREATE_ITEM_AND_ATTACH_NEW_DATASET CCC UFUN */
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

static void do_it(void)
{
    char
        item_id[ITEM_name_size_c + 1],
        dataset_name[WSO_name_size_c +1],
        datasettype_name[AE_datasettype_name_size_c +1];
    tag_t
        new_item,
        new_rev,
        dataset;
          
    printf("Enter new Item ID: ");
    gets(item_id);
    
    printf("Enter Dataset Type (e.g. UGMASTER, UGPART, Text): ");
    gets(datasettype_name);
    
    strcpy(dataset_name, item_id);
    strcat(dataset_name, "/A");    
   
    create_item(item_id, &new_item, &new_rev);
    create_dataset(datasettype_name, dataset_name, new_item, new_rev, &dataset);

    ITK_CALL(AOM_unload(dataset));
    ITK_CALL(AOM_unload(new_rev));
    ITK_CALL(AOM_unload(new_item));
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
