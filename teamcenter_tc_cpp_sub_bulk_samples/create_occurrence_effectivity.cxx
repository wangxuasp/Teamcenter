/*HEAD CREATE_OCCURRENCE_EFFECTIVITY CCC ITK */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/item.h>
#include <bom/bom.h>
#include <itk/mem.h>
#include <tc/tc.h>
#include <tccore/item.h>
#include <bom/bom.h>
#include <cfm/cfm.h>
#include <ps/ps_errors.h>
#include <tc/emh.h>
#include <tc/tc.h>
#include <ps/ps.h>

#define ITEM_find_rev GTAC_find_rev
#define IFERR_ABORT(X)  (report_error( __FILE__, __LINE__, #X, X, TRUE))
#define IFERR_REPORT(X) (report_error( __FILE__, __LINE__, #X, X, FALSE))

static int report_error(char *file, int line, char *call, int status, 
    logical exit_on_error);
static void ECHO(char *format, ...);
static void GTAC_free(void *what);
static int GTAC_find_rev(char *item_id, char *rev_id, tag_t *rev);

static void do_it(void)
{
    tag_t end_item_rev = NULLTAG;
    IFERR_ABORT(ITEM_find_rev("EndItem", "A", &end_item_rev ));
    if (end_item_rev == NULLTAG) 
    { 
        ECHO("End Item Revision not found!\n"); 
        exit (0);
    }

    tag_t assy_rev = NULLTAG;
    IFERR_ABORT(ITEM_find_rev("Assy", "A", &assy_rev));
    if (end_item_rev == NULLTAG) 
    { 
        ECHO("Assy Revision not found!\n"); 
        exit (0);
    }

    int n_bvrs = 0;
    tag_t *bvrs = NULL;
    IFERR_ABORT(ITEM_rev_list_bom_view_revs(assy_rev, &n_bvrs, &bvrs));
    if (n_bvrs == 0) 
    { 
        ECHO("Assy Revision BVR not found!\n"); 
        exit (0);
    }
    // only be one bvr for this test case
    tag_t assy_bvr = bvrs[0];
    if (bvrs) MEM_free(bvrs);

    tag_t window = NULLTAG;
    IFERR_REPORT(BOM_create_window (&window));

    tag_t rule = NULLTAG;
    IFERR_REPORT(CFM_find("Latest Working", &rule));
    IFERR_REPORT(BOM_set_window_config_rule(window, rule));
    IFERR_REPORT(BOM_set_window_pack_all(window, TRUE));

    tag_t top_line = NULLTAG;
    IFERR_REPORT(BOM_set_window_top_line(window, NULLTAG, assy_rev, NULLTAG, 
        &top_line));

    int n_children = 0;
    tag_t *children = NULL;
    IFERR_REPORT(BOM_line_ask_child_lines(top_line, &n_children, &children));

    // being lazy, just using first child line
    tag_t bom_line = children[0];
    if (children) MEM_free(children);

    int bl_occurrence = 0;
    IFERR_REPORT(BOM_line_look_up_attribute( "bl_occurrence", &bl_occurrence));

    tag_t bl_occ = NULLTAG;
    IFERR_REPORT(BOM_line_ask_attribute_tag(bom_line, bl_occurrence, &bl_occ));

    IFERR_REPORT(AOM_refresh(assy_bvr, TRUE));
    IFERR_REPORT(AOM_refresh(bl_occ, TRUE));

    tag_t occ_eff = NULLTAG;
    IFERR_REPORT(PS_occ_eff_create (assy_bvr, bl_occ, &occ_eff));
    IFERR_REPORT(AOM_save(occ_eff));
    IFERR_REPORT(AOM_save(bl_occ));
    IFERR_REPORT(AOM_save(assy_bvr));

    IFERR_REPORT(PS_occ_eff_set_id(assy_bvr, bl_occ, occ_eff, "Effectivity1"));
    IFERR_REPORT(PS_occ_eff_set_enditemrev(assy_bvr, bl_occ, occ_eff, 
        end_item_rev));
    IFERR_REPORT(PS_occ_eff_set_unit_range(assy_bvr, bl_occ, occ_eff, 
        "1-UP", true));

    IFERR_REPORT(AOM_save(occ_eff));
    IFERR_REPORT(AOM_unlock(occ_eff));
    IFERR_REPORT(AOM_unload(occ_eff));

    IFERR_REPORT(AOM_save(bl_occ));
    IFERR_REPORT(AOM_unlock(bl_occ));
    IFERR_REPORT(AOM_unload(bl_occ));

    IFERR_REPORT(AOM_save(assy_bvr));
    IFERR_REPORT(AOM_unlock(assy_bvr));
    IFERR_REPORT(AOM_unload(assy_bvr));

    IFERR_REPORT(BOM_save_window(window));
    IFERR_REPORT(BOM_close_window(window));
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

static void GTAC_free(void *what)
{
    if (what != NULL)
    {
        MEM_free(what);
        what = NULL;
    }

}

static int GTAC_find_rev(char *item_id, char *rev_id, tag_t *rev)
{
    int
        n = 0;
    tag_t
        *revs;
    const char
        *names[1] = { "item_id" },
        *values[1] = { item_id };

    IFERR_REPORT(ITEM_find_item_revs_by_key_attributes(1, names, values,
        rev_id,
        &n, &revs));
    if (n > 0) *rev = revs[0];
    if (revs) MEM_free(revs);

    return 0;
}

#ifdef __cplusplus
}
#endif
