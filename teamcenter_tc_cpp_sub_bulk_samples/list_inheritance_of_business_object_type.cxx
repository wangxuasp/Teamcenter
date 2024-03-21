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

static void get_hierachy_tags(tag_t type_tag, int *n_types, tag_t **hierachy_tags)
{
    tag_t root_type = NULLTAG; 
    TCTYPE_find_type("BusinessObject", "",  &root_type);  

    int count = 0; 
    tag_t  child_type_tag = type_tag; 
    tag_t parent_type_tag = NULLTAG; 

    char type_name[TCTYPE_name_size_c+1] = ""; 
    IFERR_REPORT(TCTYPE_ask_name(type_tag, type_name)); 
    do 
    {
        count++; 
        if (count == 1)
        {
            (*hierachy_tags) = (tag_t *) MEM_alloc( sizeof(tag_t)); 
        }
        else
        {
            (*hierachy_tags) = (tag_t *) MEM_realloc( (*hierachy_tags), count * sizeof(tag_t)); 
        }
      
        (*hierachy_tags)[count - 1] = child_type_tag;  
        *n_types = count; 

        IFERR_REPORT(TCTYPE_ask_parent_type(child_type_tag,  &parent_type_tag)); 
        child_type_tag = parent_type_tag; 
    } 
    while (parent_type_tag != root_type); 
}

void usage ()
{
    printf ("\n\n USAGE:  \n");
    printf ("\n\n   list_inheritance_of_business_object_type -t=<business object type name> \n");
	printf ("\n Examples: \n");
    printf ("\n\n     list_inheritance_of_business_object_type -t=dataset \n");
    printf ("\n\n     list_inheritance_of_business_object_type -t=RequirementSpec \n");
    printf ("\n\n     list_inheritance_of_business_object_type -t=\"ArcWeld Master\" \n");
   return;
}
static void do_it(void)
{
   /* print useage -t is null or -h was entered */
    if( (ITK_ask_cli_argument ("-h") != NULL) || (ITK_ask_cli_argument ("-t") == NULL) )
    {
        usage();
        exit (0);
    } 

    char  *input_type = ITK_ask_cli_argument("-t=");

    tag_t type_tag = NULLTAG;
    IFERR_REPORT(TCTYPE_find_type(input_type, "", &type_tag));
    if(type_tag == NULLTAG) 
    {
		int number_of_types = 0;
		tag_t *type_tags = NULL;
		IFERR_REPORT(TCTYPE_extent(&number_of_types, &type_tags));

		char type_name[TCTYPE_name_size_c+1] = "";
		for(int ii = 0; ii < number_of_types; ii++)
		{
			IFERR_REPORT(TCTYPE_ask_name(type_tags[ii], type_name)); 
			if(tc_strcasecmp(type_name, input_type) == 0)
			{
				IFERR_REPORT(TCTYPE_find_type(type_name, "", &type_tag));
				break;
			}
		}
		MEM_free(type_tags);
	}

    if(type_tag == NULLTAG) 
    {
        printf("\n\n Busineess Object type \"%s\" NOT found! \n\n", input_type);
        exit (0);
    }      
    int n_types = 0;  
    tag_t *hierachy_tags = NULL; 

    get_hierachy_tags(type_tag, &n_types, &hierachy_tags);

    int str_size = (TCTYPE_name_size_c + 3) * n_types;
    char *str = NULL;
    str = (char *) MEM_alloc(str_size + 1);
    for (int ii = (n_types - 1); ii >= 0; ii--)
    {
        char type_name[TCTYPE_name_size_c+1] = "";
        IFERR_REPORT(TCTYPE_ask_name(hierachy_tags[ii], type_name));
        if (ii == (n_types - 1) ) strcpy(str, type_name);
        else sprintf(str, "%s-> %s", str, type_name);
    }
    ECHO("\n  %s \n", str);
    MEM_free(str);

    MEM_free(hierachy_tags);
	
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
