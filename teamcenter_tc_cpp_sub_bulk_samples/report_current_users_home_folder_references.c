#include <tc/emh.h>
#include <tc/tc.h>
#include <tccore/workspaceobject.h>
#include <tc/folder.h>
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

void report_folder_references(tag_t folder)
{
    int
        num_of_references,
        ii;
    char
        *object_id = NULL,
        name[WSO_name_size_c + 1],
        type[WSO_name_size_c + 1];
    tag_t
        *list_of_references = NULL;

    IFERR_ABORT(FL_ask_references(folder, FL_fsc_by_name, &num_of_references,
        &list_of_references));
    ECHO("num_of_references = %d\n", num_of_references);

    for (ii = 0; ii < num_of_references; ii++)
    {
        IFERR_REPORT(WSOM_ask_object_id_string(list_of_references[ii],
            &object_id));
        IFERR_REPORT(WSOM_ask_name(list_of_references[ii], name));
        IFERR_REPORT(WSOM_ask_object_type(list_of_references[ii], type));
        ECHO(" %d.  Id=\"%s\"\tName=\"%s\"\tType=\"%s\"\n",
            ii + 1, object_id, name, type);
        MEM_free(object_id);
    }

    if (num_of_references > 0) MEM_free(list_of_references);
}

tag_t ask_current_users_home_folder(void)
{
    char
        *user_name_string = NULL;
    tag_t
        home_folder = NULLTAG,
        user = NULLTAG;

    IFERR_REPORT(POM_get_user(&user_name_string, &user));
    IFERR_REPORT(SA_ask_user_home_folder(user, &home_folder));

    return home_folder;
}

static void do_it(void)
{
    tag_t
        home_folder = ask_current_users_home_folder();

    if (home_folder != NULLTAG) report_folder_references(home_folder);
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
