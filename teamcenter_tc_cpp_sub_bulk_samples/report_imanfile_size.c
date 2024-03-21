/*HEAD REPORT_IMANFILE_SIZE CCC ITK */
#include <stdlib.h>

#include <ae/ae.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <ae/dataset.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <itk/mem.h>
#include <ss/ss_const.h>
#include <tccore/workspaceobject.h>

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
        n_props = 0,
        ii = 0;
    char
        dataset_name[WSO_name_size_c +1] = "brace-01",
        reference_type[AE_reference_size_c + 1] = "UGPART",   
        *name = NULL,
        *value = NULL,
        **props = NULL;
    tag_t
        dataset = NULLTAG,
        named_ref = NULLTAG;
    AE_reference_type_t
        ref_type;
 
    ITK_CALL(AE_find_dataset(dataset_name, &dataset));
    ITK_CALL(AE_ask_dataset_named_ref(dataset, reference_type, 
                 &ref_type, &named_ref));
    ITK_CALL(AOM_ask_prop_names(named_ref, &n_props, &props)); 
    for (ii = 0; ii < n_props; ii++)
    {
        if ((strcmp(props[ii], "file_size") == 0) || 
            (strcmp(props[ii], "byte_size") == 0))
        {
            ITK_CALL(AOM_UIF_ask_name(named_ref, props[ii], &name));
            ITK_CALL(AOM_UIF_ask_value(named_ref, props[ii], &value));
            printf("Display Name: %s   Property name: %s   Size: %s\n", 
                name, props[ii], value);
            if (name) MEM_free(name);
            if(value) MEM_free(value);
        }        
    }
    for (ii = 0; ii < n_props; ii++) 
        if(props[ii]) MEM_free(props[ii]);
    if(props) MEM_free(props);
    ITK_CALL(AOM_unload(dataset));
}

int ITK_user_main(int argc, char* argv[])
{
    int
        status = 0; 
    char
        *message;
    
    ITK_initialize_text_services(ITK_BATCH_TEXT_MODE);
    status = ITK_auto_login();
   
    if (  (status != ITK_ok)) printf("iMAN login NOT successful.\n\n");
    else
    {
        printf("iMAN login successful.\n\n");
        ITK_set_journalling(TRUE);
        do_it();
    }
    ITK_exit_module(TRUE);
    return status;
}
