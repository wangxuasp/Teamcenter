/*
     %TC_ROOT%\sample\compile -DIPLIB=none test.c
     %TC_ROOT%\sample\linkitk -o test test.obj
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tc/emh.h>
#include <tc/tc.h>
#include <tccore/method.h>
#include <res/res_itk.h>
#include <tccore/tc_msg.h>

#define IFERR_ABORT(X)  (report_error( __FILE__, __LINE__, #X, X, TRUE))
#define IFERR_REPORT(X) (report_error( __FILE__, __LINE__, #X, X, FALSE))

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

static int report_error(char *file, int line, char *call, int status,
    logical exit_on_error)
{
    if (status != ITK_ok)
    {
        int
            n_errors = 0;
        const int
            *severities = NULL,
            *statuses = NULL;
        const char
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

void usage ()
{
    printf ("\n\n USAGE:  \n");
    printf ("\n\n   check_for_valid_method -t=<type name> -m=<method or operation name> \n");
    return;
}

static void do_it(void)
{
	if( (ITK_ask_cli_argument ("-h") != NULL) ||
		(ITK_ask_cli_argument ("-t=") == NULL) ||
		(ITK_ask_cli_argument ("-m=") == NULL) )
    {
        usage();
        exit(0);
    }

	RES_init_module();  // there is not a RES_exit_module

	char *type = ITK_ask_cli_argument("-t=");
	char *message = ITK_ask_cli_argument("-m=");
    METHOD_id_t method;
    IFERR_REPORT(METHOD_find_method(type, message, &method));
    if (method.id != NULLTAG) ECHO("\n Method Found! \n");
    else ECHO("\n Method NOT found! \n");
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
