#include <tc/emh.h>
#include <tc/tc.h>
#include <tccore/item.h>
#include <tccore/tctype.h>
#include <stdio.h>
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

static void create_item(char *item_id, char *rev_id, char *type_name,
    tag_t *new_item, tag_t *new_rev)
{
    tag_t
        old_item = NULLTAG,
        type = NULLTAG;
    char
        item_name[ITEM_name_size_c + 1];

    strcpy(item_name, item_id);

    IFERR_REPORT(ITEM_find_item(item_id, &old_item));
    if (old_item != NULLTAG)
    {
        ECHO("Item %s already exists!\n", item_id);
        return;
    }

    if (strcmp(type_name, ""))
    {
        IFERR_REPORT(TCTYPE_find_type(type_name, "Item", &type));
        if (type == NULLTAG)
        {
            ECHO("Item Type %s does not exist!\n", type_name);
            return;
        }
    }

    ECHO("Creating %s with Item ID: %s - Rev ID: %s\n", type_name, item_id,
    rev_id);
    IFERR_REPORT(ITEM_create_item( item_id, item_name, type_name, rev_id,
        new_item, new_rev));

    if (*new_item != NULLTAG)
    {
        ECHO("Saving Item ID: %s\n", item_id);
        IFERR_REPORT(ITEM_save_item(*new_item));

        ECHO("Attaching Item ID: %s to Newstuff Folder\n", item_id);
        IFERR_REPORT(FL_user_update_newstuff_folder(*new_item));
    }
}

static void do_it(void)
{
    tag_t
        item = NULLTAG,
        itemRev = NULLTAG;
    char
        input_file[256],
        item_id[ITEM_id_size_c +1],
        rev_id[ITEM_id_size_c + 1],
        type_name[TCTYPE_name_size_c+1];
    FILE
        *the_file;

    printf("Enter list of Item ID<Tab>Rev ID<Tab>Type name filename: ");
    gets(input_file);
    if (!strcmp(input_file, "")) return;

    if ((the_file = fopen(input_file, "r")) != NULL)
    {
        while ((fscanf(the_file, "%s\t%s\t%s\n", item_id, rev_id, type_name))
            != EOF)
        {
            create_item(item_id, rev_id, type_name, &item, &itemRev);
            ECHO("Item tag: %d\n", item);
            ECHO("ItemRev tag: %d\n", itemRev);
        }
        fclose(the_file);
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
