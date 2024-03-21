/*
*    Teamcenter Ouery Builder Definition
*        Name: Find Object by UID
*        Search Type: WorkspaceObject
*        Modifiable Query Types: Local Query
*        
*        Search Critrea
*           Attribute: object_name
*           User Entry L10N Key: Name
*           User Entry Name: Name
*           operator =
*        Create
*
*    Execute utility to convert "Local Query" to "User Query"
*
*       %TC_BIN%\tc_set_query_where_run -query="Find Object by Name" -run=query
*/
#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sa/am.h>
#include <tc/emh.h>
#include <ict/ict_userservice.h>
#include <pom/pom/pom.h>
#include <property/prop_errors.h>
#include <qry/qry.h>
#include <tc/emh.h>
#include <tc/tc.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/custom.h>
#include <tccore/workspaceobject.h>
#include <user_exits/user_exits.h>
#include <ug_va_copy.h>

extern DLLAPI int libgtac_register_callbacks();
extern DLLAPI int user_exit_query_results_displayed_in_ui(int *decision, va_list args);
static void get_object_list(char *id, int *n_objects, tag_t** objects );

static void ECHO(char *format, ...)
{
    char msg[1000];
    va_list args;
    va_start(args, format);
    vsprintf(msg, format, args);
    va_end(args);
    printf(msg);
    TC_write_syslog(msg);
}

#define IFERR_ABORT(X)  (report_error( __FILE__, __LINE__, #X, X, TRUE))
#define IFERR_REPORT(X) (report_error( __FILE__, __LINE__, #X, X, FALSE))

static int report_error(char *file, int line, char *call, int status,
    logical exit_on_error)
{
    if (status != ITK_ok)
    {
        int
            n_errors = 0;
        const int
            *severities = NULL,
            *statuses = NULL;
        const char
            **messages;

        EMH_ask_errors(&n_errors, &severities, &statuses, &messages);
        if (n_errors > 0)
        {
            ECHO("\n%s\n", messages[n_errors-1]);
            EMH_clear_errors();
        }
        else
        {
            char *error_message_string;
            EMH_get_error_string (NULLTAG, status, &error_message_string);
            ECHO("\n%s\n", error_message_string);
        }

        ECHO("error %d at line %d in %s\n", status, line, file);
        ECHO("%s\n", call);
        if (exit_on_error)
        {
            ECHO("%s", "Exiting program!\n");
            exit (status);
        }
    }
    return status;
}

extern DLLAPI int libgtac_register_callbacks()
{
    ECHO("\n\n libgtac_register_callbacks \n\n");

	/* tc_set_query_where_run -query=<query name> -run=query */
	IFERR_REPORT(CUSTOM_register_exit("libgtac", "USER_execute_saved_query",
		results_displayed_in_tc_ui));

    return ITK_ok;
}

extern DLLAPI int user_exit_query_results_displayed_in_ui
	(int *decision, va_list args)
{
    ECHO("\n\n user_exit_query_results_displayed_in_ui \n\n");

	*decision = ALL_CUSTOMIZATIONS;
	int ifail = ITK_ok;

    va_list largs;
    va_copy( largs, args );

	char  *qName = va_arg(largs, char*);
	int   num_args = va_arg (largs, int);
	char  **names = va_arg (largs, char **);
	char  **values = va_arg (largs, char **);
	int   *num_found = va_arg (largs, int*); 
	tag_t **found = va_arg (largs, tag_t **);

   va_end( largs );

   if (strcmp(qName, "gtac_user_execute_saved_query") == 0)
   {
		int n_objects = 0; 
		tag_t *objects = NULL;
		get_object_list(values[0], &n_objects, &objects);

		*num_found = n_objects;
		*found = (tag_t *) MEM_alloc(n_objects * sizeof(tag_t));
		for (int ii = 0; ii < n_objects; ii++)
		{
			(*found)[ii] = objects[ii];	 
		}
		MEM_free(objects);
	}
    return ifail;
}

static void get_object_list(char *user_input, int *n_objects, tag_t** objects)
{
	/* add logic to find the desired objects */

	tag_t query = NULLTAG;
	IFERR_REPORT(QRY_find("General...", &query));

	char *entries[] = {"Name"};
	char **values = NULL;
	
    values  = (char **) MEM_alloc(sizeof(char *) * 1);
    values[0] = (char *)MEM_alloc( strlen( user_input ) + 1);
    strcpy(values[0], user_input);

	IFERR_REPORT(QRY_execute(query, 1, entries, values, n_objects, objects));
	MEM_free(values[0]);
	MEM_free(values);
}

#ifdef __cplusplus
}
#endif
