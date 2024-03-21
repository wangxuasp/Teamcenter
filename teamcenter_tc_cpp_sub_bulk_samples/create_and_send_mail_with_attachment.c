/*HEAD CREATE_AND_SEND_MAIL_WITH_ATTACHMENT ITK */
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
        *user_name_string,
        dataset_name[] = "4913166/A",
        reference_type[] = "Text";   
    tag_t
        user_tag,
        dataset,
        named_ref,
        envelope;
        
    ITK_CALL(POM_get_user(&user_name_string, &user_tag));
    MEM_free(user_name_string);
    
    /* AE_find_dataset finds latest revision of dataset */
    ITK_CALL(AE_find_dataset(dataset_name, &dataset));    
    ITK_CALL(AOM_refresh(dataset, TRUE));        
    ITK_CALL(MAIL_create_envelope("4913166/A", "4913166/A", &envelope));
    ITK_CALL(MAIL_initialize_envelope(envelope,"4913166/A", "4913166/A"));
    ITK_CALL(FL_insert(envelope, dataset, 1));
    ITK_CALL(MAIL_add_envelope_receiver(envelope, user_tag));
    ITK_CALL(MAIL_send_envelope(envelope));
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
