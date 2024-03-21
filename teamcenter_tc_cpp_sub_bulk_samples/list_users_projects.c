/*HEAD LIST_USERS_PROJECTS CCC ITK */

#include <sa/sa.h>
#include <tccore/project.h>
#include <stdlib.h>
#include <tccore/aom.h>
#include <tc/emh.h>
#include <ict/ict_userservice.h>
#include <tc/tc.h>
#include <tccore/tctype.h>
#include <sa/tcfile.h>
#include <ss/ss_errors.h>

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
        active = TRUE;
    int
        n_projects = 0,
        ii = 0;
    tag_t
        user = NULLTAG,
        *projects = NULL;
    char
        *user_name_string = NULL,
        groupname[SA_name_size_c + 1] = "",
        id[PROJ_id_size_c+1] = "",
        name[PROJ_name_size_c+1] = "";
        
    IFERR_REPORT(SA_init_module());
    IFERR_REPORT(POM_get_user(&user_name_string, &user));
    IFERR_REPORT(PROJ_find_user_project(user, TRUE, &n_projects, &projects)); 
    ECHO(("\n %s in assigned to %d projects.\n", user_name_string, n_projects));
    SAFE_MEM_FREE(user_name_string);

    for (ii = 0; ii < n_projects; ii++)
    {
        IFERR_REPORT(PROJ_ask_id(projects[ii], id));
        IFERR_REPORT(PROJ_ask_name(projects[ii], name));
        IFERR_REPORT(PROJ_is_project_active(projects[ii], &active));
        if (active == TRUE) 
        {
            ECHO(("\t id: %s - name: %s - active. \n", id, name, projects[ii]));
        }

        if (active == FALSE) 
        {
            ECHO(("\t id: %s - name: %s - inactive. \n", id, name, projects[ii]));
        }
    }
    SAFE_MEM_FREE(projects);
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
