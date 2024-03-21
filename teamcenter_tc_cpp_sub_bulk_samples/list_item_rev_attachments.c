#include<stdlib.h>
#include <tc/tc.h>
#include <tccore/tctype.h>
#include <sa/tcfile.h>
#include <sa/am.h>
#include <tccore/item.h>
#include <tccore/workspaceobject.h>
#include <sa/sa.h>
#include <tccore/grm.h>

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
            ECHO("Exiting program!\n");
            exit (status);
        }
    }

    return status;
}

static void report_attachment_information(GRM_relation_t attach)
{
    char
        *id_string,
        *class_name,
        wso_type[WSO_name_size_c +1],
        relation[TCTYPE_name_size_c+1];
    tag_t
        class;

    IFERR_REPORT(WSOM_ask_object_id_string(attach.secondary, &id_string));
    IFERR_REPORT(POM_class_of_instance(attach.secondary, &class));
    IFERR_REPORT(POM_name_of_class(class, &class_name));
    IFERR_REPORT(WSOM_ask_object_type(attach.secondary, wso_type));
    IFERR_REPORT(TCTYPE_ask_name(attach.relation_type, relation));
    ECHO("\t%s\n\t  Class name: %s\n\t  WSOM Type: %s\n\t  Relation: %s\n",
        id_string, class_name, wso_type, relation);
    if (id_string) MEM_free(id_string);
    if (class_name) MEM_free(class_name);
}

static void do_it(void)
{
    int
        n_revs,
        ii,
        n_rev_attach,
        jj;
    char
        item_name[ITEM_name_size_c +1],
        *id_string,
        *class_name;
    tag_t
        item,
        *revs,
        class;
    GRM_relation_t
        *rev_attachs;

    while (printf("\nEnter the Item ID: ") && gets(item_name) &&
        strlen(item_name))
    {
        IFERR_REPORT(ITEM_find_item(item_name, &item));
        IFERR_REPORT(ITEM_list_all_revs(item, &n_revs, &revs));
        ECHO("\nNumber of Item Revisions = %d.\n", n_revs);
        for (ii = 0; ii < n_revs; ii++)
        {

            IFERR_REPORT(WSOM_ask_object_id_string(revs[ii], &id_string));
            IFERR_REPORT(POM_class_of_instance(revs[ii], &class));
            IFERR_REPORT(POM_name_of_class(class, &class_name));
            ECHO("%d.  %s\t%s\n", ii+1, id_string, class_name);

            IFERR_REPORT(GRM_list_secondary_objects(revs[ii], NULL_TAG,
                &n_rev_attach, &rev_attachs));
            ECHO("\tNumber of Revision Attachments = %d \n", n_rev_attach);
            for (jj = 0; jj < n_rev_attach; jj++)
            {
                report_attachment_information(rev_attachs[jj]);
            }
            MEM_free(id_string);
            MEM_free(rev_attachs);
        }
        MEM_free(revs);
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
