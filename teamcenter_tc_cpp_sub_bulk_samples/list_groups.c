#include <stdlib.h>
#include <tc/emh.h>
#include <tc/tc.h>
#include <tccore/tctype.h>
#include <itk/mem.h>
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
        ii, jj,
        num_groups = 0,
        num_members = 0;
    tag_t
        *groups = NULL,
        *members = NULL,
        user;
    char
        **group_names = NULL,
        os_name[SA_name_size_c+1],
        person_name[SA_name_size_c+1];

    IFERR_ABORT(SA_list_groups(&num_groups, &groups, &group_names));
    ECHO("num_groups = %d\n", num_groups);
    for (ii = 0; ii < num_groups; ii++)
    {
        ECHO("group_names[%d] = %s\n", ii, group_names[ii]);
        IFERR_REPORT(SA_find_groupmembers_by_group(groups[ii],
            &num_members, &members));
        ECHO("  num_members = %d\n", num_members);
        for (jj = 0; jj < num_members; jj++)
        {
            IFERR_REPORT(SA_ask_groupmember_user(members[jj], &user));
            IFERR_REPORT(SA_ask_os_user_name(user, os_name));
            IFERR_REPORT(SA_ask_user_person_name(user, person_name));
            ECHO("    %s (%s)\n", os_name, person_name);
        }
        if (num_members > 0) MEM_free(members);
    }
    if (num_groups > 0)
    {
        MEM_free(group_names);
        MEM_free(groups);
    }
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
