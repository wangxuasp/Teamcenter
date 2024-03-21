/*HEAD LIST_PROPERTIES_OF_OBJECT_BY_UID CCC ITK */
/*
     %TC_ROOT%\sample\compile -DIPLIB=none list_properties_by_uid.c
     %TC_ROOT%\sample\linkitk -o list_properties_by_uid list_properties_by_uid.obj
*/

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <tc/emh.h>
#include <tc/tc.h>
#include <tccore/tctype.h>
#include <tccore/aom_prop.h>

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

static logical convert_uid_to_tag(char *uid_string, tag_t *object_tag)
{
    logical  is_valid = TRUE;

    /* this function does not return an error code */
    ITK__convert_uid_to_tag(uid_string, object_tag);
    if (object_tag == NULLTAG)
    {
        is_valid = FALSE;
    }
    IFERR_REPORT(POM_is_tag_valid(*object_tag, &is_valid));
    return is_valid;
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
    printf ("\n\n   list_props_by_uid -uid=<14 characters> \n");
    return;
}

void list_displayable_properties_with_value(tag_t object)
{
    logical
        is_displayable = TRUE;
    int
        n_props = 0,
        ii = 0;
    char
        **prop_names = NULL,
        *disp_name = NULL,
        *value = NULL;

    ECHO("\n\n%-31s %-37s %s \n", "Real Name", "Display Name", "Value");
    ECHO("%-31s %-37s %s \n", "==============", "=============", "=============");

    IFERR_REPORT( AOM_ask_prop_names(object, &n_props, &prop_names) );
    for( ii = 0; ii < n_props; ii++)
    {
        IFERR_REPORT( AOM_UIF_is_displayable(object, prop_names[ii],
            &is_displayable));
        if (is_displayable == TRUE)
        {
            value = NULL;
            IFERR_REPORT( AOM_UIF_ask_name(object, prop_names[ii], &disp_name) );
            IFERR_REPORT( AOM_UIF_ask_value(object, prop_names[ii], &value) );
            if ( (value != NULL) && (strlen(value) > 0 ) )
            {
                if (strlen(value) == 1 )
                {
                    if ( strcmp(value, " ") != 0 )
                        fprintf(stdout, "%-31s %-37s %s \n", 
                        prop_names[ii], disp_name, value );
                }
                else
                fprintf(stdout, "%-31s %-37s %s \n", 
                        prop_names[ii], disp_name, value );
            }
        }
    }
    if (prop_names != NULL) MEM_free(prop_names);
    if (disp_name != NULL) MEM_free(disp_name);
    if (value != NULL) MEM_free(value);
}
static void get_object_tag(char *input, tag_t *object_tag)
{
	char uid_string[15] = "";

	if (strlen(input) < 14)
	{
		printf("\n  UID must be 14 characters, \n");
		printf("  You entered %d characters, \n", strlen(input));
		printf("  Exiting program. \n");
	    exit(0);
	}
	 
	if (strlen(input) > 14)
	{
		char *pch = NULL; 
		pch = strstr(input,"-o="); 
		if (pch == NULL)
		{
			printf("\n  UID must be 14 characters, \n");
			printf("  You entered %d characters, \n", strlen(input));
			printf("  Exiting program. \n");
			exit(0);
		}
		else 
		{
		  for(int inx = 0;inx < 14; inx++)
          {
			uid_string[inx] = pch[inx+3];
          }
          uid_string[14] = '\0';
		}		 	
	}

	if(strlen(input) == 14) strcpy(uid_string, input);  
			 
	ITK__convert_uid_to_tag(uid_string, object_tag); 
}

static void do_it(void)
{
    if( (ITK_ask_cli_argument ("-h") != NULL) ||
		(ITK_ask_cli_argument("-uid=") == NULL) )
    {
        usage();
        exit(0);
    } 

	char *uid = ITK_ask_cli_argument("-uid=");

	tag_t  object_tag = NULLTAG;
	get_object_tag(uid, &object_tag);

	logical is_valid = FALSE;
    IFERR_REPORT(POM_is_tag_valid(object_tag, &is_valid));
    if ( (object_tag == NULLTAG) || (is_valid == FALSE))
    {
        printf("\n\n No object found with matching UID! \n\n");
        exit (0);
    }
	else
	{
		tag_t class_tag = NULLTAG;
		IFERR_REPORT(POM_class_of_instance(object_tag, &class_tag));

		char *class_name = NULL;
		IFERR_REPORT(POM_name_of_class(class_tag, &class_name));

		char *object_type = NULL;
		IFERR_REPORT(AOM_UIF_ask_value(object_tag, "object_type", &object_type));
		printf("\n\n Class: %s - Type: %s\n", class_name, object_type); 
		list_displayable_properties_with_value(object_tag);
		MEM_free(class_name);
		MEM_free(object_type);
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
