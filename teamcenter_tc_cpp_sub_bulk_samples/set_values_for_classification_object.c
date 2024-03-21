/*HEAD SET_VALUES_FOR_CLASSIFICATION_OBJECT CCC ITK */
#include <tc/emh.h>
#include <tc/tc.h>
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

static void GTAC_free(void *what)
{
    if (what != NULL)
    {
        MEM_free(what);
        what = NULL;
    }
}

static void report_attributes_of_classification_object(tag_t ico_tag)
{
    int
        ii = 0,
        nAttributes = 0;
    char
        **attributeNames = NULL,
        **attributeValues = NULL;
                                 
    IFERR_REPORT(ICS_ask_attributes_of_classification_obj(ico_tag, 
        &nAttributes, &attributeNames, &attributeValues));
        
    ECHO("ICS_ask_attributes_of_classification_obj nAttributes = %d\n", nAttributes);
    for(ii = 0; ii < nAttributes; ii++)
        ECHO("\t%s - %s\n", attributeNames[ii], attributeValues[ii]);

    GTAC_free(attributeNames);
    GTAC_free(attributeValues);     
}


static void do_it(void)
{
    tag_t 
        item = NULLTAG, 
        ico = NULLTAG;
    char 
        *names[2] = {"StrArray","YesorNo"},
        *values [2] = {"{001}{002}{003}{004}{005}{006}{007}{008}{009}", "Yes"};

    IFERR_REPORT(ITEM_find_item("000033", &item));
    if (item == NULLTAG) ECHO("Item not found!\n");
    else
    {
        IFERR_REPORT(ICS_ask_classification_object(item, &ico));
        if (ico == NULLTAG) ECHO("Item not classified!\n");
        {
            IFERR_REPORT(ICS_set_values_for_classification_obj(ico, 2, names, 
                values)); 
            report_attributes_of_classification_object(ico);
        }
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
