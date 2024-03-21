#include <stdlib.h>
#include <time.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <user_exits/user_exits.h>
#include <ae/ae.h>
#include <tccore/aom.h>
#include <ss/ss_const.h>
#include <ss/ss_errors.h>

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

static void do_it(void)
{
    tag_t
        dataset = NULLTAG,
        new_text_file = NULL_TAG,
        text_file = NULLTAG;
    char
        dataset_name[WSO_name_size_c +1],
        text_line[SS_MAXLLEN+1];
    IMF_file_t
        file_descriptor = NULL;
    AE_reference_type_t
        ref_type;

    do
    {
        printf("Enter Dataset Name: ");
        gets(dataset_name);
        if (!strcmp(dataset_name, "")) return;

        IFERR_ABORT(AE_find_dataset(dataset_name, &dataset));
        if (dataset == NULLTAG) continue;
        IFERR_ABORT(AOM_refresh(dataset, TRUE));

        IFERR_ABORT(AE_ask_dataset_named_ref(dataset, "Text",
            &ref_type, &text_file));
        if (text_file == NULLTAG) continue;
        IFERR_ABORT(AOM_refresh(text_file, TRUE));

        IFERR_ABORT(IMF_ask_file_descriptor(text_file, &file_descriptor));
        IFERR_ABORT(IMF_open_file(file_descriptor, SS_RDONLY));

        while (IMF_read_file_line(file_descriptor, text_line) == ITK_ok)
            ECHO("%s", text_line);

        IFERR_ABORT(IMF_close_file(file_descriptor));

        IFERR_ABORT(AOM_unload(text_file));
        IFERR_ABORT(AOM_unload(dataset));

        ECHO("\n");

    } while (TRUE);

}

int ITK_user_main(int argc, char* argv[])
{
    IFERR_REPORT(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
    IFERR_ABORT(ITK_auto_login());
    IFERR_REPORT(ITK_set_journalling(TRUE));
    do_it();
    IFERR_REPORT(ITK_exit_module(FALSE));
    return ITK_ok;
}
