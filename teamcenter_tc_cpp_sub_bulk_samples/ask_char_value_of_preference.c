/*HEAD ASK_CHAR_VALUE_OF_PREFERENCE CCC ITK */
/*

    %TC_ROOT%\sample\compile -DIPLIB=none test.c
    %TC_ROOT%\sample\linkitk -o test test.obj
*/
#include <tc/emh.h>
#include <tc/tc.h>
#include <stdarg.h>
#include <tc/preferences.h>

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

static void GTAC_free(void *what)
{
    if (what != NULL)
    {
        MEM_free(what);
        what = NULL;
    }
}
static void do_it(void)
{
    TC_preference_search_scope_t  
        scope[5] = {TC_preference_all, TC_preference_user, TC_preference_role,
                    TC_preference_group, TC_preference_site};   
    int 
        ii = 0, 
        kk = 0;
    char 
        scope_string [5][20] = {"All", "User", "Role", "Group", "Site"},
        pref[5][30] = {"TC_display_real_prop_names", "ADA_enabled",
             "PIE_Extra_Error_Info_to_Xml", "PLMXML_log_file_content"},
        *value = NULL;

    PREF_initialize();
   
    for (ii = 0; ii < 5; ii++) 
    {
        ECHO("\n Scope: %s\n", scope_string[ii]);
        PREF_set_search_scope(scope[ii]);
        for (kk = 0; kk < 5; kk++)
        {
            IFERR_REPORT(PREF_ask_char_value(pref[kk],0,&value));
            if (value == NULL ) value=(char*)"NULL";
            ECHO("     %s: %s\n", pref[kk], value);
        }
    }
    if (value) MEM_free(value);
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
