/*HEAD REMOVE_ALL_BOMLINE_SUBSTITUTES CCC ITK */
#include <stdlib.h>
#include <tccore/aom.h>
#include <pom/pom/pom.h>
#include <tc/emh.h>
#include <ict/ict_userservice.h>
#include <tc/tc.h>
#include <tccore/tctype.h>
#include <imanf
#include <tccore/item.h>
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
    logical
        verdict = FALSE;
    int 
        ii = 0,
        n_children;
    tag_t 
        rev_tag = NULLTAG,
        window = NULLTAG,
        top_line = NULLTAG,
        *children = NULL;
    char
        *title = NULL;
        
        IFERR_REPORT(ITEM_find_rev ("EndItem", "00", &rev_tag)); 
        ECHO(("rev_tag: %u\n", rev_tag));

        IFERR_REPORT(BOM_create_window (&window));
        IFERR_REPORT(BOM_set_window_top_line(window, NULLTAG, rev_tag, NULLTAG, &top_line)); 
        IFERR_REPORT(BOM_line_ask_child_lines(top_line, &n_children, &children));
        for (ii = 0; ii < n_children; ii++)
        {
            IFERR_REPORT(AOM_ask_value_string(children[ii], "bl_formatted_title", &title));
            ECHO(("bl_formatted_title: %s \n", title));

            IFERR_REPORT(BOM_line_ask_is_substitute(children[ii], &verdict));
            if (verdict) IFERR_REPORT(BOM_line_cut(children[ii]));
            
            GTAC_free(title);
        }
        GTAC_free(children);
        IFERR_REPORT(BOM_save_window(window));
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
