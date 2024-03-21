/*HEAD SET_PROPERTY_ON_ITEMS_IMAN_REFERENCE_FORM CCC ITK */
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
    int
        n_iman_references = 0, ii =0;
    tag_t
        item = NULLTAG, 
        relation_type = NULLTAG,
        *iman_references = NULL,
        type = NULLTAG,
        the_form = NULLTAG;
    char
        *item_id = ITK_ask_cli_argument("-i="),
        *field_name = ITK_ask_cli_argument("-f="),
        type_name[TCTYPE_name_size_c+1] = ""; 


    IFERR_REPORT(ITEM_find_item(item_id, &item)); 
    if (item == NULLTAG) { ECHO(("Item NOT found!\n")); }

    IFERR_REPORT(GRM_find_relation_type("IMAN_reference", &relation_type));
    IFERR_REPORT(GRM_list_secondary_objects_only(item, relation_type, 
        &n_iman_references, &iman_references));
    for (ii = 0; ii < n_iman_references; ii++)
    {
        IFERR_REPORT(TCTYPE_ask_object_type (iman_references[ii], &type));
        IFERR_REPORT(TCTYPE_ask_name(type, type_name));
        if  (strcmp(type_name, "Employee Record") == 0)
        {
            the_form = iman_references[ii];
            IFERR_REPORT(AOM_refresh(the_form, TRUE));
            IFERR_REPORT(AOM_set_value_string(the_form, field_name, "infodba"));
            IFERR_REPORT(AOM_save(the_form));
            IFERR_REPORT(AOM_unload(the_form));
        }
    }
    SAFE_MEM_FREE( iman_references );
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
