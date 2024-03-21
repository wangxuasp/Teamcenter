/*HEAD TEST_ITK_INIT_MODULE CCC ITK */
#include <stdlib.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/workspaceobject.h>
#include <ae/ae.h>
#include <user_exits/user_exits.h>
#include <ss/ss_const.h>
#include <tccore/item.h>
#include <pom/pom/pom.h>
#include <ae/dataset.h>
#include <sa/sa.h>
#include <stdarg.h>

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
            n_errors = 0,
            *severities = NULL,
            *statuses = NULL;
        char
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

static void do_it(void)
{
    char
        *user_name_string = NULL,
        groupname[SA_name_size_c + 1] = "";
    tag_t
        user_tag = NULLTAG,
        group_tag = NULLTAG;

    IFERR_ABORT(SA_init_module());
    IFERR_REPORT(POM_get_user(&user_name_string, &user_tag));
    IFERR_REPORT(SA_ask_user_login_group(user_tag, &group_tag));
    IFERR_REPORT(SA_ask_group_name(group_tag, groupname));
    ECHO("Successfully logged in as %s - %s\n", user_name_string, groupname);
    if (user_name_string != NULL) MEM_free(user_name_string);
    IFERR_REPORT(SA_exit_module());
}

int ITK_user_main(int argc, char* argv[])
{
    char
        *users_id, *users_password, *users_group;

    IFERR_REPORT(ITK_initialize_text_services(ITK_BATCH_TEXT_MODE));

/*  Begin ITK_auto_login the long way */

    users_id = ITK_ask_cli_argument("-u=");
    users_password = ITK_ask_cli_argument("-p=");
    users_group = ITK_ask_cli_argument("-g=");

    ECHO("Calling ITK_init_module(%s, %s, %s)...\n", users_id, users_password,
        users_group);
    IFERR_REPORT(ITK_init_module(users_id, users_password, users_group));

/*  End ITK_auto_login the long way */

    ECHO("Done\n");

    IFERR_REPORT(ITK_set_journalling(TRUE));

    do_it();

    IFERR_REPORT(ITK_exit_module(FALSE));

    return ITK_ok;
}
