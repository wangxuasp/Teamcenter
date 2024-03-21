/*HEAD EVALUATE_CONDITION_USING_USER_SESSION_PARAMETER CCC ITK */
/*
     %TC_ROOT%\sample\compile -DIPLIB=none evaluate_condition_using_user_session_parameter.cxx
     %TC_ROOT%\sample\linkitk -o evaluate_condition_using_user_session_parameter evaluate_condition_using_user_session_parameter.obj
*/

#ifdef __cplusplus
extern "C" {
#endif


#include <ce/ce.h>
#include <stdlib.h>
#include <tccore/aom.h>
#include <tc/emh.h>
#include <ict/ict_userservice.h>
#include <tc/tc.h>
#include <tccore/tctype.h>
#include <sa/tcfile.h>
#include <ss/ss_errors.h>
#include <tc/tc.h>
#include <tccore/tctype.h>

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
    tag_t type = NULLTAG;
    IFERR_ABORT(TCTYPE_find_type ("UserSession", "UserSession", &type ));

    tag_t createInputTag = NULLTAG;
    IFERR_ABORT(TCTYPE_construct_create_input ( type , &createInputTag));

    tag_t user_session = NULLTAG;
    IFERR_ABORT(TCTYPE_create_object ( createInputTag, &user_session));

    tag_t condition = NULLTAG;
    IFERR_ABORT(CE_find_condition("isProblemReportCreatable", &condition));

    logical result = FALSE;
    IFERR_ABORT(CE_evaluate_condition(condition, 1, &user_session, &result));

    if (result == TRUE) ECHO("Condition is true!\n");
    if (result == FALSE) ECHO("Condition is false!\n");
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

#ifdef __cplusplus
}
#endif
