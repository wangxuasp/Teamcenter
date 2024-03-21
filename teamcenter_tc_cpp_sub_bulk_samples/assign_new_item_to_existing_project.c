/*HEAD ASSIGN_NEW_ITEM_TO_EXISTING_PROJECT CCC ITK */
#include <sa/sa.h>
#include <tccore/project.h>
#include <stdlib.h>
#include <tccore/aom.h>
#include <tc/emh.h>
#include <ict/ict_userservice.h>
#include <tc/tc.h>
#include <tccore/tctype.h>
#include <sa/tcfile.h>
#include <tccore/item.h>
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
    tag_t
        item = NULLTAG,
        rev = NULLTAG,
        project = NULLTAG;
    char
        *id = ITK_ask_cli_argument("-project_name="),
        item_id[ITEM_id_size_c + 1] = "";

 
    IFERR_REPORT(PROJ_find(id, &project));
    if (project != NULLTAG)
    {
        IFERR_REPORT(PROJ_is_project_active(project, &active));
        if (active)
        {
            IFERR_REPORT(ITEM_create_item("", "IR-1742315", "Item", "A", &item, &rev));
            ITEM_ask_id(item, item_id); 
            ECHO(("\t\t Created item %s\n", item_id));
            IFERR_REPORT(AOM_save(item));
            IFERR_ABORT(PROJ_assign_objects(1, &project, 1, &item));
            ECHO(("\t\t Added item to project\n"));

        }
        else { ECHO(("Project is inactive!\n")); }
    }
    else { ECHO(("Project NOT found!\n")); }
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
