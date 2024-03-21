/*HEAD ADD_USER_TO_GROUP_AND_ROLE CCC ITK */

#include <stdlib.h>
#include <tccore/aom.h>
#include <tc/emh.h>
#include <ict/ict_userservice.h>
#include <tc/tc.h>
#include <tccore/tctype.h>
#include <sa/tcfile.h>
#include <ss/ss_errors.h>
#include <sa/person.h>
#include <sa/user.h>
#include <sa/group.h>
#include <sa/role.h>
#include <sa/groupmember.h>

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


static logical is_role_in_group(tag_t group, tag_t role)
{
    logical
        verdict = FALSE;
    int
        n_roles = 0,
        ii = 0;
    tag_t
        *roles = NULL;

    IFERR_REPORT(SA_ask_roles_from_group(group, &n_roles, &roles));
    for (ii = 0; ii < n_roles; ii++)
    {
        if (roles[ii] == role) 
        {
            verdict = TRUE;
            break;
        }
    }
    return verdict;
}

static logical does_groupmember_exist(tag_t user, tag_t group, tag_t role)
{
    logical 
        exists = FALSE;
    int 
        n_groupmembers = 0, 
        ii = 0;
    tag_t 
        *groupmembers = NULL, 
        gm_group = NULLTAG, 
        gm_role = NULLTAG;

    IFERR_REPORT(SA_find_groupmember_by_user(user, &n_groupmembers, &groupmembers));
    for (ii = 0; ii < n_groupmembers; ii++)
    {
        IFERR_REPORT(SA_ask_groupmember_group(groupmembers[ii], &gm_group)); 
        if (gm_group == group)
        {
            IFERR_REPORT(SA_ask_groupmember_role(groupmembers[ii], &gm_role));
            if (gm_role == role)
            {
                exists = TRUE;
                break;
            }
        }
    }
    GTAC_free(groupmembers);
    return exists;
}

static void do_it(void)
{
    logical
        role_in_group = FALSE,
        groupmember_exists = FALSE;
    tag_t
        user = NULLTAG,
        group = NULLTAG,
        role = NULLTAG,
        groupmember = NULLTAG;
    char
        user_id[] = "user1",
        group_name[] = "TheGroup",
        role_name[] = "TheRole";

    
    IFERR_ABORT(SA_find_user(user_id, &user));
    IFERR_REPORT(SA_find_group(group_name, &group));
    IFERR_REPORT(SA_find_role(role_name, &role));
    if ( (user == NULLTAG) || (group == NULLTAG)  || (role == NULLTAG) )
    {
        ECHO("\n Don't have what I need to run!\n Exiting!\n");
        exit (0);
    }

    role_in_group = is_role_in_group(group, role);
    if (role_in_group == FALSE)
    {
        ECHO("\n Cannot create groupmember if role is not in group.");
        ECHO("\n Exiting!\n");
        exit (0);
    }

    groupmember_exists = does_groupmember_exist(user, group, role);
    if (groupmember_exists == TRUE)
    {
        ECHO("\n\n Groupmember ALREADY exists \n No need to add!\n");
        exit (0);
    }
    
    IFERR_REPORT(SA_create_groupmember(group, user, FALSE, &groupmember));
    IFERR_REPORT(AOM_refresh(groupmember, TRUE));
    IFERR_REPORT(SA_set_groupmember_role(groupmember, role));
    IFERR_REPORT(AOM_save(groupmember));
    IFERR_REPORT(AOM_refresh(groupmember, FALSE));
    IFERR_REPORT(AOM_unload(groupmember));
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
