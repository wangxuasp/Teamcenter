/*HEAD DELETE_ALL_ITEN_REVISIONS_OBJECT_ACLS CCC ITK */
#include <stdlib.h>
#include <time.h>
#include <ae/ae.h>
#include <sa/am.h>
#include <tccore/aom.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <ss/ss_const.h>
#include <user_exits/user_exits.h>

#define ECHO(X)  printf X; TC_write_syslog X

#define IFERR_ABORT(X)  (report_error( __FILE__, __LINE__, #X, X, TRUE))
#define IFERR_REPORT(X) (report_error( __FILE__, __LINE__, #X, X, FALSE))

static int report_error(char *file, int line, char *call, int status,
    logical exit_on_error)
{
    if (status != ITK_ok)
    {
        int
            n_errors,
            *severities,
            *statuses;
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


#define SAFE_MEM_FREE( a )   \
do                          \
{                           \
    if ( (a) != NULL )      \
    {                       \
        MEM_free( (a) );    \
        (a) = NULL;         \
    }                       \
}                           \
while ( 0 )

static void do_it(void)
{
    int
        length = 0, 
        ii = 0, 
        n_granted = 0, 
        n_revoked = 0;
    tag_t
        item_rev = NULLTAG, 
        eff_acl = NULLTAG, 
        source_acl = NULLTAG, 
        accessor = NULLTAG,
        *granted = NULL, 
        *revoked = NULL;
    char
        *type = NULL, 
        *name = NULL, 
        *acl_name = NULL;

    IFERR_ABORT(ITEM_find_rev("6030694", "A", &item_rev));
    if (item_rev == NULLTAG)
    {
        printf("Item NOT found!\n");
        exit(0);
    }
    
    IFERR_ABORT(AM_get_effective_acl(item_rev, &eff_acl, &length));
    for (ii = 0; ii < length; ii++)
    {
        IFERR_ABORT(AM_effective_acl_line(eff_acl, ii, &source_acl,
            &accessor, &n_granted, &granted, &n_revoked, &revoked)); 
        IFERR_ABORT(AM_accessor_info(accessor, &type, &name));
        IFERR_ABORT(AM_ask_acl_name(source_acl, &acl_name));                    
        if (strcmp(acl_name,"OBJECT") == 0)
        {
            printf("Deleteing OBJECT acl: %s - %s\n", type, name);
            IFERR_REPORT(AM_delete_acl(source_acl));
        }
        SAFE_MEM_FREE(granted);
        SAFE_MEM_FREE(revoked);
        SAFE_MEM_FREE(name);
        SAFE_MEM_FREE(type);
        SAFE_MEM_FREE(acl_name);
    }
    IFERR_REPORT(AM_free_effective_acl(eff_acl));
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
