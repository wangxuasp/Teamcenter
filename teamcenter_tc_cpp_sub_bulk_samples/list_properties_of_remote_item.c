/*HEAD LIST_PROPERTIES_OF_REMOTE_ITEM CCC ITK */
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
            n_errors = 0,
            *severities = NULL,
            *statuses = NULL;
        char
            **messages = NULL;

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

void list_displayable_properties(char *indention, tag_t object)
{
    logical 
        is_displayable = TRUE;
    int 
        n_props = 0, 
        ii = 0;
    char 
        **prop_names = NULL, 
        *disp_name = NULL, 
        *value = NULL; 

    IFERR_REPORT( AOM_ask_prop_names(object, &n_props, &prop_names) );
    for( ii = 0; ii < n_props; ii++)
    {
        IFERR_REPORT( AOM_UIF_is_displayable(object, prop_names[ii], &is_displayable));
        if (is_displayable == TRUE)
        {
            IFERR_REPORT( AOM_UIF_ask_name(object, prop_names[ii], &disp_name) );
            IFERR_REPORT( AOM_UIF_ask_value(object, prop_names[ii], &value) );
            fprintf(stdout, "%s %s: %s\n",  indention, disp_name, value);
        }   
    }
    if (prop_names != NULL) MEM_free(prop_names);
    if (disp_name != NULL) MEM_free(disp_name);
    if (value != NULL) MEM_free(value);
}

static void do_it(void)
{
    int   
        site_id = 0,
        n_entries = 1, 
        n_found = 0,
        ii = 0;
    tag_t 
        site = NULLTAG,
        query = NULLTAG, 
        *objects = NULL,
        desc_obj = NULLTAG;
    char
        remote_site[] = "tce7ods",
        *entries[1] = {"Item ID"},
        *values[1] =  {"5968867"};

    IFERR_REPORT( SA_find_site(remote_site, &site));
    IFERR_REPORT( SA_ask_site_info(site, remote_site, &site_id));
    IFERR_REPORT( QRY_find("Item...", &query) );
    IFERR_REPORT( QRY_execute_at_site(site_id, query, n_entries, entries, 
        values, &n_found, &objects));
    printf("\t n_found = %d\n\n", n_found);
    for (ii = 0; ii < n_found; ii++)
    {
        IFERR_REPORT(ODS_describe_object(site_id, objects[ii], &desc_obj)); 
        list_displayable_properties("    ", desc_obj);
    } 
    
    if (n_found) SAFE_MEM_FREE(objects);
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

