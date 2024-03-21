/*
    set TC_customization_libraries -scope=SITE -values=GTAC

    %TC_ROOT%\sample\compile -DIPLIB=none test.c
    %TC_ROOT%\sample\link_custom_exits GTAC
    copy GTAC.dll %TC_BIN%
*/

#include <tc/tc.h>
#include <tccore/tc_msg.h>
#include <ict/ict_userservice.h>
#include <tccore/custom.h>
#include <tccore/method.h>
#include <ae/ae.h>
#include <tccore/item_msg.h>
#include <tccore/item.h>
#include <tccore/aom.h>
#include <tc/emh.h>
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

static logical ReviseInProgress = FALSE;
static char DescToClear[ITEM_desc_size_c+1] = { "" };

extern DLLAPI int GTAC_del_Desc_step2(METHOD_message_t *m, va_list args)
{
    /* va_list for TC_save_msg */
    tag_t save_rev = va_arg(args, tag_t);
    logical isNew = va_arg(args, logical);

    char rev_desc[ITEM_desc_size_c+1];

    if (!ReviseInProgress) return ITK_ok;

    ECHO("\nGTAC_del_Desc_step2\n");
    ECHO("\tsave_rev = %d\n", save_rev);
    ECHO("\tisNew = %d\n", isNew);

    IFERR_REPORT(ITEM_ask_rev_description(save_rev, rev_desc));
    ECHO("\told rev_desc = %s\n", rev_desc);

/*
    If the Description field from the Revise wizard that is being
    applied here still shows the description from the previous revision
    clear it.  If the user entered something different leave it alone.
*/
    if (!strcmp(rev_desc, DescToClear))
    {
        ECHO("\tClearing duplicate description:\n");
        IFERR_REPORT(ITEM_set_rev_description(save_rev, ""));
        IFERR_REPORT(ITEM_ask_rev_description(save_rev, rev_desc));
        ECHO("\tnew rev_desc = %s\n", rev_desc);

    }

    ReviseInProgress = FALSE;

    return ITK_ok;
}

extern DLLAPI int GTAC_del_Desc_step1(METHOD_message_t *m, va_list args)
{
    /* Object Type: ItemRevision - Operation: Revise */

    /* va_list for ITEM_copy_rev_msg */
    tag_t  source_rev = va_arg(args, tag_t);
    char   *rev_id = va_arg(args, char *);
    tag_t  *new_rev = va_arg(args, tag_t  *);
    tag_t  item_rev_master_form = va_arg(args, tag_t);

    char rev_desc[ITEM_desc_size_c+1];

    ECHO("\nGTAC_del_Desc_step1\n");
    ECHO("\tsource_rev = %d\n", source_rev);
    ECHO("\trev_id = %s\n", rev_id);
    ECHO("\tnew_rev = %d\n", *new_rev);
    ECHO("\titem_rev_master_form = %d\n", item_rev_master_form);

    IFERR_REPORT(ITEM_ask_rev_description(source_rev, DescToClear));
    ECHO("\told DescToClear = %s\n", DescToClear);

/*
    A call to ITEM_set_rev_description on the new_rev here will not
    stick because the Description entered in the Revise wizard dialog
    is applied to the ItemRevision AFTER the copy and this post action
    have completed.

    Set the flag and wait to set the description during the Save that
    happens when applying the new values.
*/

    ReviseInProgress = TRUE;

    return ITK_ok;
}

extern int GTAC_register_methods(int *decision, va_list args)
{
    METHOD_id_t  method;

    *decision = ALL_CUSTOMIZATIONS;

    ECHO("\nGTAC_register_methods\n");

    IFERR_REPORT(METHOD_find_method("ItemRevision", "ITEM_copy_rev", &method));
    if (method.id != NULLTAG)
    {
        IFERR_REPORT( METHOD_add_action(method, METHOD_post_action_type,
            (METHOD_function_t) GTAC_del_Desc_step1, NULL));
        ECHO("\tRegistered GTAC_del_Desc_step1 PostAction for ITEM_copy_rev\n");
    }
    else
        ECHO("\tItemRevision ITEM_copy_rev Method not found!\n");

    IFERR_REPORT(METHOD_find_method("ItemRevision", "IMAN_save", &method));
    if (method.id != NULLTAG)
    {
        IFERR_REPORT( METHOD_add_action(method, METHOD_pre_action_type,
            (METHOD_function_t) GTAC_del_Desc_step2, NULL));
        ECHO("\tRegistered GTAC_del_Desc_step2 Pre Action for IMAN_save\n");
    }
    else
        ECHO("\tItemRevision IMAN_save Method not found!\n");
    return ITK_ok;
}

extern DLLAPI int GTAC_register_callbacks ()
{
    ECHO("\n\nGTAC_register_callbacks\n\n");
    IFERR_ABORT(CUSTOM_register_exit ( "GTAC", "USER_init_module",
        (CUSTOM_EXIT_ftn_t)  GTAC_register_methods));

  return ( ITK_ok );
}
