/*HEAD EXPORT_DATASET_NAMED_REFERENCE ITK */
#include <stdlib.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/workspaceobject.h>
#include <ae/ae.h>
#include <user_exits/user_exits.h>
#include <ss/ss_const.h>
#include <ae/dataset.h>

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
    char
        dataset_name[WSO_name_size_c +1],
        native_file_spec[SS_MAXPATHLEN],
        reference_type[AE_reference_size_c + 1];   
    tag_t
        dataset,
        named_ref;
    AE_reference_type_t
        ref_type;

   printf("Enter Dataset Name: ");
    gets(dataset_name);
    
    /* AE_find_dataset finds latest revision of dataset */
    ITK_CALL(AE_find_dataset(dataset_name, &dataset));
    
    /* See ugmanager_export.dat for list of acceptable
       reference names for dataset type and file extension */
    printf("Enter the Named Reference Type(e.g. TEXT, UGPART, CLS) : ");
    gets(reference_type);
    
    printf("Enter Native Os Filename: ");
    gets(native_file_spec);

    ITK_CALL(AOM_refresh(dataset, TRUE));
        
    ITK_CALL(AE_ask_dataset_named_ref(dataset, reference_type, 
                 &ref_type, &named_ref));

    ITK_CALL(IMF_export_file(named_ref, native_file_spec));

    ITK_CALL(AOM_unload(dataset));
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
