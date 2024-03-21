/*HEAD EXPORT_ITEM_REVISIONS_UGMASTER_PART_FILE CCC ITK */
#include <stdlib.h>
#include <tc/tc.h>
#include <tccore/tctype.h>
#include <sa/tcfile.h>
#include <tccore/workspaceobject.h>
#include <ae/ae.h>
#include <user_exits/user_exits.h>
#include <property/prop.h>
#include <tccore/aom.h>
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
        n_specs = 0,
        ii = 0,
        n_refs = 0,
        jj = 0;
    tag_t
        rev = NULLTAG,
        relation_type = NULLTAG,
        *specs = NULL,
        type = NULLTAG,
        *refs = NULL;
    char  
        type_name[TCTYPE_name_size_c+1] = ""; 

    IFERR_REPORT(ITEM_find_rev("6150380", "A", &rev));
    IFERR_REPORT(GRM_find_relation_type("IMAN_specification", &relation_type));
    IFERR_REPORT(GRM_list_secondary_objects_only(rev, relation_type, 
        &n_specs, &specs));
    for (ii = 0; ii < n_specs; ii++)
    {
        IFERR_REPORT(TCTYPE_ask_object_type (specs[ii], &type));
        IFERR_REPORT(TCTYPE_ask_name(type, type_name));
        if (strcmp(type_name, "UGMASTER") == 0)
        {
            IFERR_REPORT(AE_ask_all_dataset_named_refs(specs[ii], "UGPART", 
				&n_refs, &refs));
            IFERR_REPORT(AE_export_named_ref(specs[ii], "UGPART", "W:\\6150380_A.prt"));        
        }
        SAFE_MEM_FREE(refs);
    }
    SAFE_MEM_FREE(specs);
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
