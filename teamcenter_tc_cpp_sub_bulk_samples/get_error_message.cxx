/*HEAD GET_ERROR_CODE_MESSAGE CCC ITK */
/*
     %TC_ROOT%\sample\compile -DIPLIB=none get_error_code_message.c
     %TC_ROOT%\sample\linkitk -o get_error_code_message get_error_code_message.obj
*/

#include <stdlib.h>
#include <stdarg.h>
#include <tc/emh.h>
#include <tc/tc.h>

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

void usage ()
{
    printf ("\n\n USAGE: get_error_code_message -e=<error code>\n");
    printf ("\n Examples: \n");
    printf ("\n         get_error_code_message -e=7007 \n");
    printf ("\n         get_error_code_message -e=515009 \n");
    return;
}

static void do_it(void)
{
    int error_code = 0;
    char *error_code_string = ITK_ask_cli_argument("-e=");
    char *error_message_string;

    if (error_code_string == NULL) 
    {
        usage();
        exit (0);

    }
    else
    {
        error_code = atoi(error_code_string);

        EMH_get_error_string (NULLTAG,  error_code, &error_message_string);
        ECHO("\n %s\n", error_message_string);     
    }
	MEM_free(error_message_string);
}

int ITK_user_main(int argc, char* argv[])
{
    ITK_initialize_text_services(ITK_BATCH_TEXT_MODE);
    ITK_auto_login();
    ITK_set_journalling(TRUE);

    do_it();

    ITK_exit_module(FALSE);

    return ITK_ok;
}

