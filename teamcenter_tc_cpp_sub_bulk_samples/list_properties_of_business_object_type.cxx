/*HEAD LIST_PROPERTIES_OF_BUSINESS_OBJECT_TYPE CCC ITK */
/*
     %TC_ROOT%\sample\compile -DIPLIB=none list_properties_of_business_object_type.c
     %TC_ROOT%\sample\linkitk -o list_properties_of_business_object_type list_properties_of_business_object_type.obj
*/
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <tc/emh.h>
#include <tc/tc.h>
#include <tccore/aom.h>
#include <property/prop.h>
#include <property/propdesc.h>
#include <fclasses/tc_string.h>
#include <tccore/tctype.h>

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



void usage ()
{
    printf ("\n\n USAGE: list_props_of_bo_type -t=<type name> -pn=<property name> \n");

    printf ("\n Examples: \n");
    printf ("\n     To list details of all properties on business object \n");
    printf ("\n         list_properties_of_business_object_type -t=Item \n");

    printf ("\n\n   To list details one property on business object using either real or display name\n");
    printf ("\n     list_properties_of_business_object_type -t=Item -pn=uom_tag  \n");
    printf ("\n     list_properties_of_business_object_type -t=Item -pn=\"Unit of Measure\" \n");

    printf ("\n\n   Notice if the display name has spaces you need to include double quotes. \n");

    return;
}

static void list_property_details(tag_t prop_tag)
{
    int 
        n_elements = 0, 
        max_strlen = 0;
    char    
        *prop_name = NULL,
        *disp_name = NULL,
        *prop_type = NULL,
        *value_type = NULL,
        *prop_type_name = NULL,
        disp_name_string[40] = "",
        prop_type_string[30] = "",
        value_type_string[30] = "",
        prop_desc[7][40] = {"Unknown", "Attribute", "Reference", "ImanRelation",
                            "Compound",  "Runtime", "Operation"},
        prop_protection [3][20] = {"Write",  "Read", "Write Only If Null"};
    PROP_value_type_t 
        valtype;         
    PROP_type_t
        proptype;  
    PROP_protection_t
        protection ;  

    IFERR_REPORT(PROPDESC_ask_name(prop_tag, &prop_name));
    IFERR_REPORT(PROPDESC_ask_display_name(prop_tag, &disp_name));
    if (strlen(disp_name) == 0) 
    {
        strcpy(disp_name_string, "no display name");
    }
    else
    {
        strcpy(disp_name_string, disp_name);
    }

    IFERR_REPORT(PROPDESC_ask_value_type(prop_tag, &valtype, &value_type));
    if (valtype == PROP_string)
    {
        IFERR_REPORT(PROPDESC_ask_max_string_length(prop_tag, &max_strlen));
        sprintf(value_type_string, "%s(%d)", value_type, max_strlen);
    }
    else
    {
        sprintf(value_type_string, "%s", value_type);
    }
    IFERR_REPORT(PROPDESC_ask_max_num_elements(prop_tag, &n_elements));
    if ( (n_elements == -1) || (n_elements > 1) ) 
    {
        sprintf(value_type_string, "%s Array", value_type);
    }   
    IFERR_REPORT(PROPDESC_ask_property_type(prop_tag, &proptype, 
         &prop_type_name));
    IFERR_REPORT(PROPDESC_ask_protection(prop_tag, &protection));
    ECHO("%-31s %-31s %-17s %-12s %s\n", prop_name, value_type_string,
        prop_desc[proptype], prop_protection[protection], disp_name_string);

    MEM_free(prop_name);
    MEM_free(disp_name);
    MEM_free(prop_type);
    MEM_free(value_type);
    MEM_free(prop_type_name);
}

static void do_it(void)
{
    logical
        found = FALSE;
    int
        n_props = 0,
        ii = 0;
    tag_t
        type_tag = NULLTAG,
        *props_tag = NULLTAG;
    char    
        *input_type = ITK_ask_cli_argument("-t="),
        *find_prop = ITK_ask_cli_argument("-pn="),
        *prop_name = NULL,
        *disp_name = NULL; 
    
    if( (ITK_ask_cli_argument ("-h") != NULL) || 
		(ITK_ask_cli_argument ("-t") == NULL) )
    {
        usage();
        exit (0);
    } 
    IFERR_ABORT(TCTYPE_find_type(input_type, "", &type_tag));
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

    IFERR_REPORT(TCTYPE_list_properties (type_tag, &n_props, &props_tag));
    if (find_prop != NULL)  
    {
        for (ii = 0; ii < n_props; ii++)
        {
            IFERR_REPORT(PROPDESC_ask_name(props_tag[ii], &prop_name));
            IFERR_REPORT(PROPDESC_ask_display_name(props_tag[ii], &disp_name));
            if ( (tc_strcasecmp(find_prop, prop_name) == 0) || 
                 ( tc_strcasecmp(find_prop, disp_name) == 0 ) )
            {
                found = TRUE;
                ECHO("\n\n%-31s %-31s %-17s %-12s %s\n", "Real Name", 
                     "Storage Type", "Type", "Access", "Display Name");
                ECHO("%-31s %-31s %-17s %-12s %s\n", 
                      "==============", "==============", "==============", 
                      "========", "==============");
                list_property_details(props_tag[ii]);
            }
        }
        if (found == FALSE) 
            printf("\n\n Property %s NOT found!\n Exiting...\n", find_prop);
    }

    if (find_prop == NULL)  
    {
         ECHO("\n\n%-31s %-31s %-17s %-12s %s\n", "Real Name", 
              "Storage Type", "Type", "Access", "Display Name");
         ECHO("%-31s %-31s %-17s %-12s %s\n", 
              "==============", "==============", "==============", 
              "========", "==============");

        for (ii = 0; ii < n_props; ii++)
        {
            list_property_details(props_tag[ii]);
        }
    }
    ECHO("\n\n");
    MEM_free(prop_name);
    MEM_free(disp_name);
    MEM_free(props_tag);
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
