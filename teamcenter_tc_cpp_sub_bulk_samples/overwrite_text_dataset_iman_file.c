/*HEAD  OVERWRITE_TEXT_DATASET_IMAN_FILE ITK */
#include<stdlib.h>
#include <tc/tc.h>
#include <tccore/workspaceobject.h>
#include <sa/sa.h> 
#include <sa/tcfile.h>
#include <tccore/item.h>
#include <ae/dataset.h>

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

static void import_file(char *os_path_name, tag_t *file_tag)
{
    int
        file_type = SS_TEXT, 
        machine_type = SS_WNT_MACHINE; 
    char
        *file_name = NULL, 
        pathname[SS_MAXPATHLEN];
    IMF_file_t
        file_descriptor;
        
    
    ITK_CALL(IMF_import_file(os_path_name, file_name, file_type, 
                 file_tag, &file_descriptor));
    ITK_CALL(IMF_ask_file_pathname(*file_tag, machine_type, pathname));
    printf("pathname of file on disk = %s\n", pathname);

    ITK_CALL(AOM_save(*file_tag));    
}

static void do_it(void)
{
    int
        ii,
        n_attachs;
    char
        item_name[ITEM_name_size_c +1],
        attach_type[WSO_name_size_c + 1],
        native_file_spec[IMF_filename_size_c + 1];
    tag_t
        rev,
        item,
        imported_file,
        named_ref; 
    ITEM_attached_object_t
        *rev_attachments;
    AE_reference_type_t
        reference_type;
        
    while (printf("\nEnter the Item ID: ") && gets(item_name) &&
        strlen(item_name))
    {
        ITK_CALL(ITEM_find_item(item_name, &item));  
        ITK_CALL(ITEM_ask_latest_rev(item, &rev));
        ITK_CALL(ITEM_list_rev_attachs_of_type(rev, ITEM_specification_atth, 
            &n_attachs, &rev_attachments));
        for (ii = 0; ii < n_attachs; ii++)
        {
            ITK_CALL(WSOM_ask_object_type(rev_attachments[ii].attachment, attach_type));
            if(!strcmp (attach_type, "Text" ))
            {
                ITK_CALL(AOM_refresh(rev_attachments[ii].attachment, TRUE));
                ITK_CALL(AE_ask_dataset_named_ref(rev_attachments[ii].attachment, "Text",
                    &reference_type, &named_ref));
                if( named_ref != NULLTAG )
                {
					ITK_CALL(AE_purge_dataset_revs(dataset));
                    ITK_CALL(AE_remove_dataset_named_ref_by_tag(
                        rev_attachments[ii].attachment, "Text", named_ref));
                    printf("Enter name of file to import: ");
                    gets(native_file_spec);
                    import_file(native_file_spec, &imported_file);
                    ITK_CALL(AE_add_dataset_named_ref(rev_attachments[ii].attachment,
                         "Text", AE_PART_OF, imported_file));
                    ITK_CALL(AOM_save(rev_attachments[ii].attachment));
                    ITK_CALL(AOM_unload(imported_file));
                    ITK_CALL(AOM_unload(rev_attachments[ii].attachment));
               }
            }
        }
        if (rev_attachments) MEM_free(rev_attachments);   
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
