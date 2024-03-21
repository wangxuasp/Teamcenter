#include <tc/emh.h>
#include <tc/tc.h>
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
    int
        n = 0;
    char
        group_name[SA_name_size_c + 1] = { "" },
        *my_user_name = NULL,
        role_name[SA_name_size_c+1] = { "" },
        user_name[SA_name_size_c+1] = { "" };
    tag_t
        group = NULLTAG,
        role = NULLTAG,
        *members = NULL,
        user = NULLTAG;

    IFERR_ABORT(SA_init_module());

    do
    {
        printf("Enter user id: ");
        gets(user_name);
        if (!strcmp(user_name, "")) return;

        IFERR_ABORT(SA_find_user(user_name, &user));
        ECHO("User: %s (%d)\n", user_name, user);

        IFERR_ABORT(SA_ask_user_login_group(user, &group));
        IFERR_REPORT(SA_ask_group_name(group, group_name));
        ECHO("Login Group: %s (%d)\n", group_name, group);

        IFERR_ABORT(SA_ask_user_default_role_in_group(user, group, &role));
        IFERR_REPORT(SA_ask_role_name(role, role_name));
        ECHO("Default Role: %s (%d)\n", role_name, role);

        IFERR_REPORT(SA_find_groupmember_by_role(role, group, &n, &members));
        ECHO("  (1 of %d members of %s with Role %s)\n\n",
            n, group_name, role_name);
        if (members) MEM_free(members);
    } while (TRUE);

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
