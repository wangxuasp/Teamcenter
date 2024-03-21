#include <stdlib.h>
#include <tc/tc.h>
#include <ae/ae.h>
#include <user_exits/user_exits.h>
#include <property/prop.h>
#include <tccore/aom.h>
#include <ae/dataset.h>

#define ECHO(X)  printf X; TC_write_syslog X

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

static rename_dataset(char *old_name, char *new_name)
{
    tag_t
        dset_tag = NULLTAG,
        prop = NULLTAG;

    IFERR_REPORT(AE_find_dataset(old_name, &dset_tag));

    if (dset_tag)
    {
        ECHO(("Found dataset tag = %d\n", dset_tag));

        IFERR_REPORT(PROP_UIF_ask_property_by_name(dset_tag, "Name", &prop));
        if (prop != NULLTAG)
        {
            IFERR_REPORT(AOM_lock(dset_tag));
            IFERR_REPORT(PROP_UIF_set_value(prop, new_name));
            IFERR_REPORT(AE_save_myself(dset_tag));
        }
        else
        {
            ECHO(("Did not find property named Name.\n"));
        }
    }
    else
    {
        ECHO(("dset_tag was not found for name %s\n", old_name));
    }
}

int ITK_user_main(int argc, char* argv[])
{
    IFERR_REPORT(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
    IFERR_ABORT(ITK_auto_login());
    IFERR_REPORT(ITK_set_journalling(TRUE));
    rename_dataset(argv[1], argv[2]);
    IFERR_REPORT(ITK_exit_module(TRUE));
    return ITK_ok;
}
