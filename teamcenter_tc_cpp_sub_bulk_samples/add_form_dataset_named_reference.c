/*HEAD ADD_FORM_DATASET_NAMED_REFERENCE CCC ITK */
#include <tc/emh.h>
#include <tc/tc.h>
#include <stdarg.h>
#include <ae/ae.h>
#include <form/form.h>

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

static void do_it(void)
{
    tag_t 
        dataset = NULLTAG,
        ds_type = NULLTAG,
        tool = NULLTAG,
        form = NULLTAG;

    IFERR_ABORT(AE_find_datasettype("UGMASTER", &ds_type));
    IFERR_REPORT(AE_ask_datasettype_def_tool(ds_type, &tool));
    if (tool == NULLTAG)
    {
        ECHO(( "Tool not found...Exiting Program!\n"));
        exit (0);
    }
    IFERR_ABORT(AE_create_dataset_with_id(ds_type, "ITK_UGMASTER", "ds description", "ds_id", "A", &dataset));
    IFERR_ABORT(AE_set_dataset_tool( dataset, tool));
    IFERR_ABORT(AE_set_dataset_format(dataset, "BINARY_REF"));
    IFERR_ABORT(AOM_save(dataset)); 

    /* create masspropform */
    IFERR_REPORT(FORM_create("ITK_Form", "", "UGPartMassPropsForm" , &form));
    IFERR_REPORT(AOM_save(form));

    /* add ImanFile to dataset */
    IFERR_REPORT(AOM_refresh(dataset, TRUE));
    IFERR_REPORT(AE_add_dataset_named_ref(dataset, "UGPART-MASSPR", AE_PART_OF, form));
    IFERR_REPORT(AOM_save(dataset));
    IFERR_REPORT(AOM_unload(form)); 
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
