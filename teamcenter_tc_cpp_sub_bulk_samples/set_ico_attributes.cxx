/*HEAD CREATE_PRODUCT_STRUCTURE CCC ITK */
/*
     %TC_ROOT%\sample\compile -DIPLIB=none create_product_structure.c
     %TC_ROOT%\sample\linkitk -o create_product_structure create_product_structure.obj
*/
#include <stdlib.h>
#include <aom.h>
#include <bom.h>
#include <emh.h>
#include <folder.h>
#include <iman.h>
#include <imanfile.h>
#include <item.h>
#include <mem.h>
#include <ps.h>
#include <emh.h>
#include <tc.h>
#include <stdarg.h>
#include <aom.h>
#include <prop.h>
#include <propdesc.h>
#include <sa/user.h>
#include <ics.h>
#include <ics2.h>


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
    printf ("\n\n   set_ico_attributes -u=<userid> -p=<password> -i=<item id> -t=<item type> \n");

    printf ("\n\n Example:  \n");
    printf ("\n      set_ico_attributes -u=gtac -p=gtac -i=6543210  -t=A2Item \n");

    return;
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

    printf("Creating Folder: %s\n", folder_name);
    IFERR_REPORT(FL_create(folder_name, "", folder));
    IFERR_REPORT(AOM_save(*folder));

    IFERR_REPORT(FL_insert(home_folder, *folder, -1));
    IFERR_REPORT(AOM_save(home_folder));
    IFERR_REPORT(AOM_refresh( home_folder, FALSE));
    MEM_free(user_name_string);
}

static void do_it(void)
{
    /* print useage if -h was entered or -i is null*/
    if( (ITK_ask_cli_argument ("-h") != NULL) || (ITK_ask_cli_argument ("-i=") == NULL) || (ITK_ask_cli_argument ("-t=") == NULL))
    {
        usage();
        exit (0);
    }

    char *id = ITK_ask_cli_argument("-i=");
    char *item_type = ITK_ask_cli_argument("-t=");

    if(item_type == NULL) 
    {
        item_type = (char *) MEM_alloc(strlen("Item") + 1 * sizeof(char));
        strcpy(item_type, "Item");
    }

    tag_t item = NULLTAG;
    tag_t rev = NULLTAG;
    ITEM_find_item(id, &item);
    if (item != NULLTAG)
    {
        ECHO("Item %s already exists!\n", id);     
    }
    else
    {
        tag_t folder = NULLTAG;
        create_folder_in_home_folder(id, &folder);

        printf("   Creating Item: %s\n", id);
        IFERR_REPORT(ITEM_create_item(id, id, item_type, "A",&item, &rev));
        IFERR_REPORT(AOM_save(item));

        insert_object_into_folder(folder, item, 999);

        tag_t class_tag;
        IFERR_REPORT(ICS_find_class("ugfc010101", &class_tag));

        tag_t view;
        IFERR_REPORT(ICS_find_view(class_tag, "defaultView", &view));

        tag_t ico;
        IFERR_REPORT(ICS_create_classification_object(item, id, view, &ico));
        IFERR_REPORT(ICS_classify_wsobject(item, ico));

        int n_attrs = 2;
        int att_ids[2] = {-7502, -7501};
        int n_attr_val[2] = {1, 1};

        char ***attr_vals;
        attr_vals = (char***) MEM_alloc( n_attrs * sizeof(char**) );

        attr_vals[0] = (char**) MEM_alloc( n_attr_val[0] * sizeof(char*) );
        attr_vals[1] = (char**) MEM_alloc( n_attr_val[1] * sizeof(char*) ); 

        attr_vals[0][0] = (char *) MEM_alloc(strlen( "150.00" ) + 1);
        strcpy(attr_vals[0][0], "150.00");

        attr_vals[1][0] = (char *)MEM_alloc(strlen( "1000.00" ) + 1);
        strcpy(attr_vals[1][0], "1000.00");

        IFERR_REPORT(ICS_ico_set_attributes(ico, n_attrs, att_ids, n_attr_val, 
            (const char***) attr_vals));
    }
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
        printf("\nLogin successful!\n\n");
        ITK_set_journalling(TRUE);
        do_it();
    }
    ITK_exit_module(TRUE);
    return status;
}