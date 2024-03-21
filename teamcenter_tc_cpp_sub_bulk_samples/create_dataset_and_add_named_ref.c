/*HEAD CREATE_DATASET_AND_ADD_NAMED_REF CCC ITK */
#include <ae/ae.h>
#include <stdlib.h>
#include <tccore/aom.h>
#include <tc/emh.h>
#include <ict/ict_userservice.h>
#include <tc/tc.h>
#include <tccore/tctype.h>
#include <sa/tcfile.h>
#include <ss/ss_errors.h>

#define ECHO(X)  printf X; TC_write_syslog X

#define IFERR_ABORT(X)  (report_error( __FILE__, __LINE__, #X, X, TRUE))
#define IFERR_REPORT(X) (report_error( __FILE__, __LINE__, #X, X, FALSE))
#define IFERR_RETURN(X) if (IFERR_REPORT(X)) return

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

static void do_it(void)
{
    tag_t 
		home_folder = NULLTAG, 
		user = NULLTAG, 
		item = NULLTAG, 
		rev = NULLTAG, 
		dataset = NULLTAG, 
		ds_type = NULLTAG, 
		tool = NULLTAG, 
		relation_type = NULLTAG, 
		relation = NULLTAG, 
		file = NULLTAG;
    char 
		*user_name = NULL;
    IMF_file_t  
		descriptor; 

    /* create new item and item revison */
    IFERR_ABORT(ITEM_create_item("", "My Test", "Item", "A", &item, &rev));
    IFERR_ABORT(ITEM_save_item(item));

    /* add new item to the top of users home folder */
    IFERR_ABORT(POM_get_user(&user_name, &user));
    IFERR_ABORT(SA_ask_user_home_folder(user, &home_folder));
    MEM_free(user_name);
    IFERR_REPORT(FL_insert(home_folder, item, -1));
    IFERR_REPORT(AOM_save(home_folder));
    IFERR_REPORT(AOM_unload(item));

    /* create dataset not using default tool */
    IFERR_ABORT(AE_find_datasettype("Text", &ds_type));
    IFERR_REPORT(AE_find_tool("Notepad", &tool));
    if (tool == NULLTAG)
    {
        ECHO(( "Tool not found...Exiting Program!\n"));
        exit (0);
    }
    IFERR_ABORT(AE_create_dataset_with_id(ds_type, "ds name", "ds description", "ds_id", "A", &dataset));
    IFERR_ABORT(AE_set_dataset_tool( dataset, tool));
    IFERR_ABORT(AE_set_dataset_format(dataset, "TEXT_REF"));
    IFERR_ABORT(AOM_save(dataset)); 

    /* attach dataset to the item revision */
    IFERR_REPORT(GRM_find_relation_type("IMAN_specification", &relation_type));
    IFERR_REPORT(GRM_create_relation(rev, dataset, relation_type, NULL, &relation));
    IFERR_REPORT(GRM_save_relation(relation)); 
    IFERR_REPORT(AOM_unload(rev));

    /* import file */
    IFERR_REPORT(IMF_fmsfile_import("W:\\my_file.txt", "my_file.txt", SS_TEXT , &file, &descriptor));

    /* add ImanFile to dataset */
    IFERR_REPORT(AOM_refresh(dataset, TRUE));
    IFERR_REPORT(AE_add_dataset_named_ref(dataset, "Text", AE_PART_OF, file));
    IFERR_REPORT(AOM_save(dataset));
    IFERR_REPORT(AOM_unload(file)); 
    IFERR_REPORT(AOM_unload(dataset));
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
