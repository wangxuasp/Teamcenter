/*HEAD FIND_POM_USERS_PUID_AND_STATUS CCC ITK */
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
    void 
        ***report;
    int 
        row = 0,
        n_rows = 0, 
        n_cols = 0,
        status_int;
    tag_t
        puid_tag = NULLTAG;
    const char  
        *select_attr_list[] = { "status", "puid", "user_id"};
    char 
        *puid_string = NULL,
        status_string[10] = "";

    /*create a query*/
    IFERR_ABORT(POM_enquiry_create("enquiry"));

    /*add the list to the select clause of the query*/
    IFERR_REPORT(POM_enquiry_add_select_attrs("enquiry", "POM_user", 3, 
        select_attr_list));

    IFERR_ABORT(POM_enquiry_execute("enquiry", &n_rows, &n_cols, &report));

    for (row = 0; row < n_rows; row++)
    {
        /*    type cast to int     */
        status_int = *((int*)(report[row][0]));
        if (status_int == 0) strcpy(status_string, "Active");
        if (status_int == 1) strcpy(status_string, "Inactive");

        /*    type cast to tag_t     */
        puid_tag = *((tag_t *)report[row][1]); 
        IFERR_REPORT(POM_tag_to_uid(puid_tag, & puid_string));

        ECHO(("%-10s \t %s \t %s\n", 
            status_string, puid_string, report[row][2]));
		MEM_free(puid_string);
    }
	GTAC_free(report);
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
