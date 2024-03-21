/*HEAD FIND_WORKSPACE_OBJECTS_CREATED_AFTER_A_GIVEN_DATE CCC UFUN */
#include <stdlib.h>
#include <tccore/aom.h>
#include <tc/emh.h>
#include <tc/tc.h>
#include <pom/pom/pom.h>
#include <sa/tcfile.h>

#define ECHO(X)  { printf X; TC_write_syslog X; }

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
    void    ***report;
    int     n_rows, n_cols, row, column;
    const char  *select_attr_list[] = { "object_name", "object_type",
        "creation_date" };
    char *date_string = NULL;
    date_t aDate, cDate;

    /*create a query*/
    IFERR_ABORT(POM_enquiry_create("aEnqId"));

    /*add the list to the select clause of the query*/
    IFERR_REPORT(POM_enquiry_add_select_attrs("aEnqId", "WorkspaceObject",
        3, select_attr_list));

    /* a date value object using local time zone time */
    IFERR_REPORT(ITK_string_to_date("19-May-2009 10:00", &aDate));

    IFERR_REPORT(POM_enquiry_set_date_value("aEnqId", "aValId", 1, &aDate,
        POM_enquiry_bind_value));

    /* create expression for creation_date after this Date */
    IFERR_REPORT(POM_enquiry_set_attr_expr("aEnqId", "aExpId",
        "WorkspaceObject", "creation_date", POM_enquiry_greater_than_or_eq,
        "aValId"));

    /*set the where clause search condition.*/
    IFERR_ABORT(POM_enquiry_set_where_expr( "aEnqId", "aExpId"));

    /*execute the query*/
    IFERR_ABORT(POM_enquiry_execute("aEnqId", &n_rows, &n_cols, &report));

    for (row = 0; row < n_rows; row++)
    {
        for (column = 0; column < 2; column++)
            ECHO(("%s\t", report[row][column]));
        IFERR_REPORT(POM_convert_date(POM_gmt_to_local,
            (*(date_t *)report[row][2]), &cDate));
        IFERR_REPORT(ITK_date_to_string(cDate, &date_string));
        ECHO(("%s\n", date_string));
        GTAC_free(date_string);
    }
    ECHO(("n_rows = %d\n", n_rows));
	GTAC_free(report);
}

int ITK_user_main(int argc, char* argv[])
{
    IFERR_REPORT(ITK_initialize_text_services(ITK_BATCH_TEXT_MODE));
    IFERR_ABORT(ITK_auto_login());
    IFERR_REPORT(ITK_set_journalling(TRUE));

    do_it();

    IFERR_REPORT(ITK_exit_module(TRUE));

    return ITK_ok;
}
