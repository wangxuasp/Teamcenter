#include <tc/emh.h>
#include <tc/tc.h>
#include <tccore/item.h>
#include <tccore/aom.h>
#include <epm/epm.h>
#include <tccore/tctype.h>
#include <stdarg.h>

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

/*
*************************************************
* WARNING - ITEM_find_rev is deprecated in
* Teamcenter version Teamcenter 8.1
* and no longer supported in Teamcenter 10
* Replace this call to ITEM_find_rev with ITEM_find_item_revs_by_key_attributes
*************************************************

    So for everyone's convenience - here is a wrapper replacement
*/

#define ITEM_find_rev GTAC_find_rev

static int GTAC_find_rev(char *item_id, char *rev_id, tag_t *rev)
{
    int
        n = 0;
    tag_t
        *items;
    const char
        *names[1] = { "item_id" },
        *values[1] = { item_id };

    IFERR_REPORT(ITEM_find_item_revs_by_key_attributes(1, names, values, rev_id,
        &n, &items));
    if (n > 0) *rev = items[0];
    if (items) MEM_free(items);

    return 0;
}

static tag_t prompt_for_existing_item_revision(void)
{
    tag_t
        rev = NULLTAG;
    char
        item_id[ITEM_id_size_c +1],
        rev_id[ITEM_id_size_c + 1];

    do
    {
        printf("Enter Item ID: ");
        gets(item_id);
        if (!strcmp(item_id, "")) return NULLTAG;

        printf("Enter %s Rev: ", item_id);
        gets(rev_id);
        if (!strcmp(rev_id, "")) return NULLTAG;

        ECHO("Searching for Item ID %s with Rev ID %s...\n", item_id, rev_id);
        IFERR_REPORT(ITEM_find_rev(item_id, rev_id, &rev));
        if (rev == NULL_TAG)
            ECHO("ItemRev %s/%s was NOT found!\n", item_id, rev_id);
        else
            return rev;

    } while (TRUE);

}

static tag_t ask_item_revisions_master_form(tag_t item_revision)
{
    int
        n_secondary_objects = 0;
    tag_t
        relation = NULLTAG,
        *secondary_objects = NULL,
        item_revision_master_form = NULLTAG;

    IFERR_REPORT(GRM_find_relation_type("IMAN_master_form", &relation));
    IFERR_REPORT(GRM_list_secondary_objects_only(item_revision, relation,
        &n_secondary_objects, &secondary_objects));

    /* should always be just one */
    item_revision_master_form = secondary_objects[0];

    if (secondary_objects) MEM_free(secondary_objects);

    return item_revision_master_form;
}

void report_object_class_name_and_type_name(tag_t object)
{
    tag_t
        class = NULLTAG,
        type = NULLTAG;
    char
        *class_name = NULL,
        type_name[TCTYPE_name_size_c+1] = "";

    IFERR_REPORT(POM_class_of_instance(object, &class));
    IFERR_REPORT(POM_name_of_class(class, &class_name));
    ECHO("  class_name: %s\n", class_name);
    if (class_name) MEM_free(class_name);

    IFERR_REPORT(TCTYPE_ask_object_type(object, &type));
    IFERR_REPORT(TCTYPE_ask_name(type, type_name));
    ECHO("  type_name: %s\n", type_name);
}

static void do_it(void)
{
    tag_t
        itemRev = prompt_for_existing_item_revision(),
        itemRevForm = NULLTAG;

    while (itemRev != NULLTAG)
    {
        IFERR_ABORT(AOM_refresh(itemRev, FALSE));

        itemRevForm = ask_item_revisions_master_form(itemRev);
        ECHO("itemRevForm = %d\n", itemRev);
        if (itemRevForm != NULLTAG)
            report_object_class_name_and_type_name(itemRevForm);

        itemRev = prompt_for_existing_item_revision();
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
