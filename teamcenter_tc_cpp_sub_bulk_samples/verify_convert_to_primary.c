/*HEAD VERIFY_CONVERT_TO_PRIMARY CCC ITK */
/* 
	This sample verifies that the business_model_updater utility sucuessfully
	converted secondary business objects to primary business objects
*/
#include <stdlib.h>
#include <tccore/aom.h>
#include <tc/emh.h>
#include <ict/ict_userservice.h>
#include <tc/tc.h>
#include <tccore/tctype.h>
#include <sa/tcfile.h>
#include <ss/ss_errors.h>
#include <qry/qry.h>
#include <pom/pom/pom.h>
#include <tccore/workspaceobject.h>

#define ECHO(X)  printf X; TC_write_syslog X

#define IFERR_ABORT(X)  (report_error( __FILE__, __LINE__, #X, X, TRUE))
#define IFERR_REPORT(X) (report_error( __FILE__, __LINE__, #X, X, FALSE))
#define IFERR_RETURN(X) if (IFERR_REPORT(X)) return

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
        if (n_errors > 0)
        {
            ECHO(("\n%s\n", messages[n_errors-1]));
            EMH_clear_errors();
        }
        else
        {
            char *error_message_string;
            EMH_get_error_string (NULLTAG, status, &error_message_string);
            ECHO(("\n%s\n", error_message_string));
        }

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
    logical
        verdict = FALSE;
    int   
        n_found = 0, 
        ii = 0,
        n_not_converted = 0;
    tag_t 
        query = NULLTAG, 
        *items = NULL,
        class = NULLTAG,
        wso_class = NULLTAG;
    char
        business_object_name[TCTYPE_name_size_c+1] = "",
        *entries[1] = {"Type"},
        **values = NULL,
        *class_name = NULL,
        *object_id = NULL;

    printf("\n\nEnter Business Object Name: ") ;

    gets(business_object_name);

    values  = (char **) MEM_alloc(sizeof(char *));
    values[0] = (char *)MEM_alloc( strlen( business_object_name ) + 1);
    strcpy(values[0], business_object_name );

    IFERR_REPORT(QRY_find("General...", &query));

    IFERR_REPORT(QRY_execute(query, 1, entries, values, &n_found, &items));
    for (ii = 0; ii < n_found; ii++)
    {
        IFERR_REPORT(POM_class_of_instance(items[ii], &class));
        IFERR_REPORT(POM_name_of_class(class, &class_name)); 
        if (strcmp(business_object_name, class_name) != 0)
        {
            n_not_converted++;
            if (n_not_converted == 1) ECHO(("\n Object(s) that did NOT convert:\n"));
            IFERR_REPORT(POM_class_id_of_class("WorkspaceObject", &wso_class)); 
            IFERR_REPORT(POM_is_descendant(wso_class, class, &verdict));
            if (verdict == TRUE)
            {
                IFERR_REPORT(WSOM_ask_object_id_string(items[ii], &object_id)); 
                ECHO(("    %s - classname: %s\n", object_id, class_name));
                SAFE_MEM_FREE(object_id);
            }
        }
    }
    SAFE_MEM_FREE(class_name);
    SAFE_MEM_FREE(items);
    SAFE_MEM_FREE(values);

    ECHO(("\n\n\n Summary Report: \n"));
    ECHO(("    Number found: %d\n", n_found));
    ECHO(("    Number that did NOT convert: %d\n\n", n_not_converted));
}

int ITK_user_main(int argc, char* argv[])
{
    IFERR_REPORT(ITK_initialize_text_services(ITK_BATCH_TEXT_MODE));
    IFERR_ABORT(ITK_auto_login());
    IFERR_REPORT(ITK_set_journalling(TRUE));

    do_it();

    IFERR_REPORT(ITK_exit_module(FALSE));

    return ITK_ok;
}
