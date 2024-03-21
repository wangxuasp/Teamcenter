/*HEAD ADD_UGPART_NAMED_REFERENCE_TO_DATASET ITK */
#include <stdlib.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/workspaceobject.h>
#include <ae/ae.h>
#include <user_exits/user_exits.h>
#include <ss/ss_const.h>

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

static void import_file(char *os_path_name, char dataset_name[WSO_name_size_c + 1],
    char reference_name[AE_reference_size_c + 1], tag_t *file_tag)
{
    int
        file_type = SS_BINARY, 
        machine_type = SS_WNT_MACHINE;
    char 
        *file_name = USER_new_file_name(dataset_name, reference_name, "prt", 0),
        pathname[SS_MAXPATHLEN];
        
    IMF_file_t
        file_descriptor;
        
    
    ITK_CALL(IMF_import_file(os_path_name, 
        file_name, file_type, file_tag, &file_descriptor)); 
    ITK_CALL(IMF_ask_file_pathname(*file_tag, machine_type, pathname));
    printf("pathname of file on disk = %s\n", pathname);

    ITK_CALL(AOM_save(*file_tag)); 
    if (file_name) MEM_free(file_name);   
}
    
static void do_it(void)
{
    char
        dataset_name[WSO_name_size_c +1],
        native_file_spec[WSO_name_size_c +1],
        reference_name[] = "UGPART";
    tag_t
        dataset,
        imported_file;
              
    printf("Enter Dataset Name: ");
    gets(dataset_name);
    
    /* AE_find_dataset finds latest revision of dataset */
    ITK_CALL(AE_find_dataset(dataset_name, &dataset));
       
    printf("Enter name of file to import: ");
    gets(native_file_spec);
    
    import_file(native_file_spec, dataset_name, reference_name, &imported_file);.
    
    ITK_CALL(AOM_refresh(dataset, TRUE));
    
    ITK_CALL(AE_add_dataset_named_ref(dataset, reference_name, 
                 AE_PART_OF, imported_file));
    ITK_CALL(AOM_save(dataset));
    ITK_CALL(AOM_unload(imported_file)); 
    ITK_CALL(AOM_unload(dataset));
}

int ITK_user_main(int argc, char* argv[])
{
    int
        status = 0; 
    
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
