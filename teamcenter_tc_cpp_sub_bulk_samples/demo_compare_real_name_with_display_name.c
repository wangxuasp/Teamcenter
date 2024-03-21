#include <stdlib.h>
#include <tccore/aom.h>
#include <tc/emh.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <property/propdesc.h>

#define ECHO(X)  printf X; TC_write_syslog X

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

static int GTAC_strcmp(const char *s1, const char *s2)
{
     int ret_val = 0;
     char *s1_disp_name = NULL;
     char *s2_disp_name = NULL;

     PROPDESC_ask_display_name_by_name(s1, &s1_disp_name);
     PROPDESC_ask_display_name_by_name(s2, &s2_disp_name);

     ret_val = strcmp(s1_disp_name, s2_disp_name);

     if (s1_disp_name) MEM_free(s1_disp_name);
     if (s2_disp_name) MEM_free(s2_disp_name);

     return ret_val;
}

static void do_it(void)
{
    char
        *s1 = "IMAN_specification",
        *s2 = "Specifications",
        *s3 = "Specification";
    int
        t1 = GTAC_strcmp(s1, s2),
        t2 = GTAC_strcmp(s2, s1),
        t3 = GTAC_strcmp(s1, s3),
        t4 = GTAC_strcmp(s3, s2);

    ECHO(("GTAC_strcmp(%s, %s) = %d\n", s1, s2, t1));
    ECHO(("GTAC_strcmp(%s, %s) = %d\n", s2, s1, t2));
    ECHO(("GTAC_strcmp(%s, %s) = %d\n", s1, s3, t3));
    ECHO(("GTAC_strcmp(%s, %s) = %d\n", s3, s2, t4));

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
