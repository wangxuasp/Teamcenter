#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tccore/tctype.h>
#include <tc/tc.h>
#include <tccore/tctype.h>

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


static void do_it(void)
{
    int number_of_types = 0;
    tag_t *type_tags = NULL;
    IFERR_REPORT(TCTYPE_extent(&number_of_types, &type_tags));
        
    for(int ii = 0; ii < number_of_types; ii++)
    {
        char type_name[TCTYPE_name_size_c+1] = "";
        IFERR_REPORT(TCTYPE_ask_name(type_tags[ii], type_name));

		char  class_name[TCTYPE_class_name_size_c+1] = "";	
		IFERR_REPORT(TCTYPE_ask_class_name(type_tags[ii], class_name));  
	    printf(" Class: %s - Type: %s\n", class_name, type_name); 
    }
    MEM_free(type_tags);
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
