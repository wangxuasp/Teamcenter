#include <stdlib.h>
#include <tc/emh.h>
#include <tc/tc.h>
#include <tccore/tctype.h>
#include <tccore/item.h>
#include <pom/pom/pom.h>
#include <tccore/aom.h>
#include <tc/folder.h>

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
    * WARNING - ITEM_find_item is deprecated in
    * Teamcenter version Teamcenter 8.1
    * and no longer supported in Teamcenter 10
    * Replace this call to ITEM_find_item with ITEM_find_items_by_key_attributes
    *************************************************

    So for everyone's convenience - here is a wrapper replacement
*/

#define ITEM_find_item GTAC_find_item

static void GTAC_find_item(char *item_id, tag_t *item)
{
    int
        n = 0;
    tag_t
        *items;
    const char
        *names[1] = { "item_id" },
        *values[1] = { item_id };

    IFERR_REPORT(ITEM_find_items_by_key_attributes(1, names, values, &n,
        &items));
    if (n > 0) *item = items[0];
    if (items) MEM_free(items);
}

static int remove_from_all_folders(tag_t object)
{
    int
        cnt = 0,
        ii,
        n_levels,
        n_instances,
        *instance_levels,
        *instance_where_found,
        n_classes,
        *class_levels,
        *class_where_found;
    tag_t
        folder_type,
        newstuff_type,
        *ref_instances,
        *ref_classes,
        type = NULLTAG;

    IFERR_REPORT(TCTYPE_find_type("Folder", NULL, &folder_type));
    if (folder_type == NULLTAG)
    {
        ECHO("Folder type not found!\n");
        return FALSE;
    }

    IFERR_REPORT(TCTYPE_find_type("Newstuff Folder", NULL, &newstuff_type));
    if (newstuff_type == NULLTAG)
    {
        ECHO("Newstuff Folder type not found!\n");
        return FALSE;
    }

    IFERR_REPORT(POM_referencers_of_instance(object, 1, POM_in_ds_and_db,
        &n_instances, &ref_instances, &instance_levels,
        &instance_where_found, &n_classes, &ref_classes, &class_levels,
        &class_where_found));

    if (n_instances > 0)
    {
        for (ii = 0; ii < n_instances; ii++)
        {
            IFERR_REPORT(TCTYPE_ask_object_type(ref_instances[ii], &type));
            if ((type == folder_type) || (type == newstuff_type))
            {
                IFERR_REPORT(AOM_refresh(ref_instances[ii], TRUE));
                IFERR_REPORT(FL_remove(ref_instances[ii], object));
                IFERR_REPORT(AOM_save(ref_instances[ii]));
                IFERR_REPORT(AOM_refresh(ref_instances[ii], FALSE));
                cnt++;
            }
        }
        MEM_free(ref_instances);
        MEM_free(instance_levels);
        MEM_free(instance_where_found);
    }

    if (n_classes > 0)
    {
        MEM_free(ref_classes);
        MEM_free(class_levels);
        MEM_free(class_where_found);
    }

    return cnt;
}

static logical is_object_referenced(tag_t object)
{
    int
        ii,
        n_levels,
        n_instances,
        *instance_levels,
        *instance_where_found,
        n_classes,
        *class_levels,
        *class_where_found;
    tag_t
        *ref_instances,
        *ref_classes,
        type = NULLTAG;
    char
        *class_name,
        *name,
        type_name[TCTYPE_name_size_c+1] = "";

    IFERR_REPORT(POM_referencers_of_instance(object, 1, POM_in_ds_and_db,
        &n_instances, &ref_instances, &instance_levels,
        &instance_where_found, &n_classes, &ref_classes, &class_levels,
        &class_where_found));

    if (n_instances > 0)
    {
        ECHO("Referencing instances:\n");
        for (ii = 0; ii < n_instances; ii++)
        {
            IFERR_REPORT(AOM_ask_name(ref_instances[ii], &name));
            IFERR_REPORT(TCTYPE_ask_object_type(ref_instances[ii], &type));
            IFERR_REPORT(TCTYPE_ask_name(type, type_name));

            ECHO("  %s (%s)\n", name, type_name);
            if (name) MEM_free(name);
        }
        MEM_free(ref_instances);
        MEM_free(instance_levels);
        MEM_free(instance_where_found);
    }

    if (n_classes > 0)
    {
        ECHO("Referencing classes:\n");
        for (ii = 0; ii < n_classes; ii++)
        {
            IFERR_REPORT(POM_name_of_class(ref_classes[ii], &class_name));
            ECHO("  %s\n", class_name);
            if (class_name) MEM_free(class_name);
        }
        MEM_free(ref_classes);
        MEM_free(class_levels);
        MEM_free(class_where_found);
    }

    if ((n_instances + n_classes) > 0) return TRUE;

    return FALSE;

}

#define WRITE_D(X) ECHO("%s = %d\n", #X, X);

#define WRITE_L(X) if (X) ECHO("%s = TRUE\n", #X); \
    else ECHO("%s = FALSE\n", #X);

static void do_it(void)
{
    tag_t
        item = NULLTAG;
    char
        item_id[ITEM_id_size_c +1];

    do
    {
        printf("Enter Item ID: ");
        gets(item_id);
        if (!strcmp(item_id, "")) return;

        item = NULLTAG;
        ITEM_find_item(item_id, &item) ;
        if (item == NULL_TAG)
        {
            ECHO("Item %s was NOT found!\n", item_id);
        }
        else
        {
            ECHO("Item %s was found\n", item_id);
        }

        WRITE_D(remove_from_all_folders(item));
        IFERR_REPORT(AOM_refresh(item, TRUE));

        WRITE_L(is_object_referenced(item));

        IFERR_REPORT(AOM_lock_for_delete(item));
        IFERR_REPORT(ITEM_delete_item(item));

    } while (TRUE);
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
