/*HEAD SAVE_DATASET_MODIFICATIONS CCC ITK */
#include <stdlib.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/workspaceobject.h>
#include <ae/ae.h>
#include <user_exits/user_exits.h>
#include <property/prop.h>
#include <tccore/aom.h>
#include <sa/sa.h>
#include <ae/dataset.h>
#include <ss/ss_const.h>
 
#define ECHO(X)  printf X; TC_write_syslog X
 
#define IFERR_ABORT(X)  (report_error( __FILE__, __LINE__, #X, X, TRUE))
#define IFERR_REPORT(X) (report_error( __FILE__, __LINE__, #X, X, FALSE))
 
static int report_error(char *file, int line, char *call, int status,
    logical exit_on_error)
{
    if (status != ITK_ok)
    {
        int
            n_errors = 0,
            *severities = NULL,
            *statuses = NULL;
        char
            **messages;
 
        EMH_ask_errors( &n_errors, &severities, &statuses, &messages );
        ECHO(("\n%s\n", messages[n_errors-1]));
        EMH_clear_errors();
 
        ECHO(("error %d at line %d in %s\n", status, line, file));
        ECHO(("%s\n", call));
 
        if (exit_on_error)
        {
            ECHO(("\nExiting program!\n"));
            exit (status);
        }
    }
 
    return status;
}
 
static void do_it(void)
{
    tag_t 
        dset_tag = NULLTAG, 
        user_tag = NULLTAG;

    IFERR_REPORT(ITK_set_bypass(true)) ;
    IFERR_REPORT(AE_find_dataset("xx1234567/A", &dset_tag));
    
    if(dset_tag)
    {
        printf("Found dataset tag...\n");

		/* use AOM_lock instead of AOM_refresh */

		IFERR_REPORT(AOM_lock(dset_tag));
         
        IFERR_REPORT(SA_find_user("valid_user_name_here", &user_tag));
        if(user_tag == NULLTAG )
        {
            printf("USER NOT FOUND!!!!!!!!\n\n"); 
            return;  
        }
        IFERR_REPORT(POM_set_owning_user(dset_tag, user_tag));
        IFERR_REPORT(POM_set_modification_user(dset_tag, user_tag));
         
        IFERR_REPORT(AE_save_myself(dset_tag));
 
    }
    else
    {
        printf("\n\ndset_tag was not found...\n\n");   
    }
}
 
int ITK_user_main(int argc, char* argv[])
{
    IFERR_REPORT(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
    IFERR_ABORT(ITK_auto_login());
    IFERR_REPORT(ITK_set_journalling(TRUE));
    do_it();
    IFERR_REPORT(ITK_exit_module(FALSE));
    return ITK_ok;
}
