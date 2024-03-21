/*HEAD CREATE_PRODUCT_STRUCTURE CCC ITK */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tccore/aom.h>
#include <bom/bom.h>
#include <tc/emh.h>
#include <tc/folder.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/item.h>
#include <itk/mem.h>
#include <ps/ps.h>
#include <sa/user.h>
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

void insert_object_into_folder(tag_t folder, tag_t object, int pos)
{
    IFERR_REPORT(AOM_refresh( folder, TRUE));
    IFERR_REPORT(FL_insert(folder, object, pos));
    IFERR_REPORT(AOM_save(folder));
    IFERR_REPORT(AOM_refresh( folder, FALSE));
}

void create_folder_in_home_folder(char *folder_name, tag_t *folder)
{
    char
        *user_name_string = NULL;
    tag_t
        user = NULLTAG, 
        home_folder = NULLTAG;

    IFERR_REPORT(POM_get_user(&user_name_string, &user));
    IFERR_REPORT(SA_ask_user_home_folder(user, &home_folder));

    printf("   Creating Folder: Home-> %s\n", folder_name);
    IFERR_REPORT(FL_create(folder_name, "", folder));
    IFERR_REPORT(AOM_save(*folder));

    IFERR_REPORT(FL_insert(home_folder, *folder, -1));
    IFERR_REPORT(AOM_save(home_folder));
    IFERR_REPORT(AOM_refresh( home_folder, FALSE));
    MEM_free(user_name_string);
}

static void case_insensitive_find_type(char *input_name, char **item_type, tag_t *type_tag)
{
	int number_of_types = 0; 
	tag_t *type_tags = NULL;
	IFERR_REPORT(TCTYPE_extent(&number_of_types, &type_tags));

	logical is_valid_type_name = FALSE; 

	char type_name[TCTYPE_name_size_c+1] = ""; 
	for(int ii = 0; ii < number_of_types; ii++)
	{
		IFERR_REPORT(TCTYPE_ask_name(type_tags[ii], type_name));  
		if(tc_strcasecmp(type_name, input_name) == 0)
		{
			IFERR_REPORT(TCTYPE_find_type(type_name, "", type_tag)); 
			is_valid_type_name = TRUE; 
			*item_type = (char *)MEM_alloc(sizeof(char) * (strlen(type_name) +1 ) );
			strcpy(*item_type, type_name);
			break;
		}
	}
	MEM_free(type_tags); 
}

void usage ()
{
    printf ("\n\n USAGE:  \n");
    printf ("\n\n   create_product_structure -i=<item id> -t=<type>  -n=<number of child lines>\n");
    return;
}

static void do_it(void)
{
    int
		n_children = 0,
        ii = 0;
    tag_t 
        folder = NULLTAG, 
        assy_item = NULLTAG, 
        assy_rev = NULLTAG, 
        window = NULLTAG, 
        top_line = NULLTAG, 
        bv = NULLTAG, 
        bvr = NULLTAG,
        item = NULLTAG, 
        rev = NULLTAG, 
        child_line = NULLTAG;
    char
        assy_name[ITEM_id_size_c + 1] = "";

	if( (ITK_ask_cli_argument ("-h") != NULL) ||
		(ITK_ask_cli_argument("-i=") == NULL) || 
		(ITK_ask_cli_argument("-t=") == NULL) || 
		(ITK_ask_cli_argument("-n=") == NULL) )
	{
		usage();
	    exit(0);
	}

	char *item_id = ITK_ask_cli_argument("-i=");
	char *input_name = ITK_ask_cli_argument("-t=");
	char *children = ITK_ask_cli_argument("-n="); 
	char *item_type = NULL; 
    tag_t type_tag = NULLTAG; 
    IFERR_REPORT(TCTYPE_find_type(input_name, "", &type_tag)); 
	if (type_tag != NULLTAG)
	{
		item_type = (char *)MEM_alloc(sizeof(char) * (strlen(input_name) +1 ) );
		strcpy(item_type, input_name);	
	}
	else
    { 
		int number_of_types = 0; 
		tag_t *type_tags = NULL;
		IFERR_REPORT(TCTYPE_extent(&number_of_types, &type_tags));
		char type_name[TCTYPE_name_size_c+1] = ""; 
		for(int ii = 0; ii < number_of_types; ii++)
		{ 
			IFERR_REPORT(TCTYPE_ask_name(type_tags[ii], type_name));  
			if(tc_strcasecmp(type_name, input_name) == 0)
			{ 
				IFERR_REPORT(TCTYPE_find_type(type_name, "", &type_tag)); 
				item_type = (char *)MEM_alloc(sizeof(char) * (strlen(type_name) +1 ) );
				strcpy(item_type, type_name);
				break;
			}
		}
		MEM_free(type_tags); 
	}
	 
	if (type_tag == NULLTAG)
    {
        printf("\n\n Busineess Object type \"%s\" NOT found! \n\n", input_name);
        exit (0);
    }  	

	n_children = atoi(children); 
	strcpy(assy_name, item_id);

    printf("   Creating Item: %s \n", item_id);
    IFERR_ABORT(ITEM_create_item(item_id, "Top Assembly", item_type, "A", &assy_item, &assy_rev));
    IFERR_REPORT(AOM_save (assy_item));
	create_folder_in_home_folder(item_id, &folder);
	insert_object_into_folder(folder, assy_item, -1);
    
    IFERR_REPORT(PS_create_bom_view (NULLTAG, "", "", assy_item, &bv));
    IFERR_REPORT(AOM_save (bv));

    IFERR_REPORT(PS_create_bvr (bv, "", "",  false, assy_rev, &bvr));
    IFERR_REPORT(AOM_save (bvr));
    IFERR_REPORT(AOM_save (assy_item));

    for (ii = 0; ii < n_children; ii++)
    {
        if (ii == 0)
        {
            IFERR_REPORT(BOM_create_window (&window));
            IFERR_REPORT(BOM_set_window_top_line(window, assy_item, assy_rev,   
                NULLTAG, &top_line));
        }

        sprintf(item_id, "%s_%d", assy_name, ii + 1);
        printf("   Creating Item: %s\n", item_id);

        IFERR_REPORT(ITEM_create_item(item_id, "Component", item_type, "A",
            &item, &rev) );
        IFERR_REPORT(AOM_save(item));

        insert_object_into_folder(folder, item, 999);

        IFERR_REPORT(BOM_line_add(top_line, item, rev, NULLTAG, &child_line));
        IFERR_REPORT(BOM_save_window(window)); 		
    }
	IFERR_REPORT(BOM_close_window(window));
}

int ITK_user_main(int argc, char* argv[])
{
    int
        status = 0; 
   
    ITK_initialize_text_services( ITK_BATCH_TEXT_MODE );
    status = ITK_auto_login();
   
    if (  (status != ITK_ok)) printf("\nLogin Failed!\n\n");
    else
    {
        ITK_set_journalling(TRUE);
        do_it();
    }
    ITK_exit_module(TRUE);
    return status;
}


#ifdef __cplusplus
}
#endif
