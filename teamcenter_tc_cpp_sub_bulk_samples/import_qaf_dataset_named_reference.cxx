/*HEAD IMPORT_QAF_DATASET_NAMED_REFERENCE.CXX CCC ITK */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ss/ss_const.h>
#include <ae/dataset.h>
#include <tccore/aom.h>
#include <tc/emh.h>
#include <tc/tc.h>

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

static void do_it(void)
{
    /*  AE_find_dataset finds latest revision of dataset */
    tag_t dataset = NULLTAG;
    IFERR_ABORT(AE_find_dataset("6667776-A", &dataset));
    ECHO("\n dataset: %u \n", dataset);

    IFERR_ABORT(AOM_refresh(dataset, TRUE));
    
    /* the fourth argument must be a unique name in the volume */
    IFERR_ABORT(AE_import_named_ref(dataset, "UG-QuickAccess-Text", "W:\\qafmetadata.qaf", "6667776-A_text.qaf",  SS_TEXT));
    IFERR_ABORT(AE_import_named_ref(dataset, "UG-QuickAccess-Binary", "W:\\images_preview.qaf", "6667776-A_binary.qaf",  SS_BINARY));

    IFERR_ABORT(AOM_save(dataset)); 
    IFERR_ABORT(AOM_refresh(dataset, FALSE));
    IFERR_ABORT(AOM_unload(dataset));
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


#ifdef __cplusplus
}
#endif
