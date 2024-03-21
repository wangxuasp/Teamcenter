/*HEAD CREATE_LEGACY_VARIANT_OPTION CCC ITK */
#include <stdlib.h>
#include <tccore/aom.h>
#include <bom/bom.h>
#include <tc/emh.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/tctype.h>
#include <itk/mem.h>

#define EXIT_FAILURE 1 
#define ERROR_CHECK(X) (report_error( __FILE__, __LINE__, #X, (X)))
static void report_error( char *file, int line, char *function, int return_code)
{
    if (return_code != ITK_ok)
    {
        char *error_message_string;
        EMH_get_error_string (NULLTAG, return_code, &error_message_string);
        printf ("ERROR: %d ERROR MSG: %s.\n", return_code, error_message_string);
        printf ("FUNCTION: %s\nFILE: %s LINE: %d\n", function, file, line);
        if(error_message_string) MEM_free(error_message_string);
        printf("\n Exiting program! \n");
        exit (EXIT_FAILURE);    
    }
}

static void do_it(void)
{
    int
        index = 0;
    tag_t 
        rev = NULLTAG, 
        new_opt = NULLTAG, 
        new_opt_rev = NULLTAG,  
        ve = NULLTAG, 
        veb = NULLTAG; 

    ERROR_CHECK(ITEM_find_rev("5935137", "000", &rev));
    if (rev != NULLTAG)
    {
        ERROR_CHECK(AOM_load( rev ));
        ERROR_CHECK(AOM_lock( rev ));
        ERROR_CHECK(BOM_new_option(rev, "Style", NULL, BOM_option_mode_enum,
            &new_opt, &new_opt_rev, &ve, &veb));
        ERROR_CHECK(BOM_add_option_rev_value(new_opt_rev, "A", &index));
        ERROR_CHECK(BOM_add_option_rev_value(new_opt_rev, "B", &index));

        ERROR_CHECK(AOM_save(new_opt_rev));
        ERROR_CHECK(AOM_save(new_opt));
        ERROR_CHECK(AOM_save(ve));
        ERROR_CHECK(AOM_save(veb));
        ERROR_CHECK(AOM_save(rev));
        ERROR_CHECK(AOM_unlock(rev));
        ERROR_CHECK(AOM_unload(rev));
    }
    else printf("Item Revision not found!\n");
}

int ITK_user_main(int argc, char* argv[])
{
    int
        status = 0; 
   
    ITK_initialize_text_services( ITK_BATCH_TEXT_MODE );
    status = ITK_auto_login();
    if (  (status != ITK_ok)) printf("\n Login Failed! \n\n");
    else
    {
        printf("\n Login successful! \n\n");
        ITK_set_journalling(TRUE);
        do_it();
    }
    ITK_exit_module(TRUE);
    return status;
}
