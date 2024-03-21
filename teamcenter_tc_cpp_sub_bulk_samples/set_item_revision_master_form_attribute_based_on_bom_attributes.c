/*HEAD SET_ITEM_REVISION_MASTER_FORM_ATTRIBUTE_BASED_ON_BOM_ATTRIBUTES CCC ITK */
#include<stdlib.h>
#include <bom/bom.h>
#include <cfm/cfm.h>
#include <tc/tc.h>
#include <tccore/tctype.h>
#include <tccore/item.h>
#include <ps/ps_errors.h>
#include <property/prop.h>
#include <pom/pom/pom_tokens.h>

#define EXIT_FAILURE 1
#define ERROR_CHECK(x) {           \
    int stat;                     \
    char *err_string;             \
    if( (stat = (x)) != ITK_ok)   \
    {                             \
    EMH_get_error_string (NULLTAG, stat, &err_string);                 \
    printf ("ERROR: %d ERROR MSG: %s.\n", stat, err_string);           \
    printf ("FUNCTION: %s\nFILE: %s LINE: %d\n",#x, __FILE__, __LINE__); \
    if(err_string) MEM_free(err_string);                                \
    exit (EXIT_FAILURE);                                                   \
    }                                                                    \
}

static char total[256] = "";

static void set_item_revisions_master_form_property(tag_t item_revision, 
    char *property_name, char *property_value)
{
    int
        n_secondary_objects = 0;
    tag_t
        relation = NULLTAG,
        *secondary_objects = NULL,
        irm_form = NULLTAG;

    ERROR_CHECK(GRM_find_relation_type("IMAN_master_form", &relation));
    
    ERROR_CHECK(GRM_list_secondary_objects_only(item_revision, relation, 
        &n_secondary_objects, &secondary_objects) );

    /* should always be just one */
    irm_form = secondary_objects[0];
    if (secondary_objects) MEM_free(secondary_objects);

    ERROR_CHECK(AOM_refresh(irm_form, TRUE));
    ERROR_CHECK(AOM_set_value_string(irm_form, property_name, property_value));
    ERROR_CHECK(AOM_save(irm_form));
    ERROR_CHECK(AOM_unload(irm_form));
}

static tag_t ask_item_revisions_master_form(tag_t item_revision)
{
    int
        n_secondary_objects = 0;
    tag_t
        relation = NULLTAG,
        *secondary_objects = NULL,
        item_revision_master_form = NULLTAG;

    ERROR_CHECK(GRM_find_relation_type("IMAN_master_form", &relation));
    ERROR_CHECK(GRM_list_secondary_objects_only(item_revision, relation, 
        &n_secondary_objects, &secondary_objects));

    /* should always be just one */
    item_revision_master_form = secondary_objects[0];

    if (secondary_objects) MEM_free(secondary_objects);
    return item_revision_master_form;
}

static tag_t ask_item_revision_from_bom_line(tag_t bom_line)
{
    tag_t
        item_revision = NULLTAG;
    char 
        *item_id = NULL,
        *rev_id = NULL;
    
    ERROR_CHECK(AOM_ask_value_string(bom_line, "bl_item_item_id", &item_id ));
    ERROR_CHECK(AOM_ask_value_string(bom_line, "bl_rev_item_revision_id", 
        &rev_id));
    ERROR_CHECK(ITEM_find_rev(item_id, rev_id, &item_revision));
    
    if (item_id) MEM_free(item_id);
    if (rev_id) MEM_free(rev_id);
    return item_revision;
}

static void traverse_product_structure(tag_t line, char *field_name)
{
    int 
        ii, kk,
        count;
    tag_t 
        *children = NULL,
        item_revision = NULLTAG,
        irm_form = NULLTAG;
    char 
        *value = NULL;

    item_revision = ask_item_revision_from_bom_line(line);
    irm_form = ask_item_revisions_master_form(item_revision);

    ERROR_CHECK(AOM_ask_value_string(irm_form, field_name, &value));
    strcat(total, value);

    ERROR_CHECK(BOM_line_ask_child_lines(line, &count, &children));
    for (ii = 0; ii < count; ii++)
        traverse_product_structure(children[ii], field_name);

    if (value) MEM_free(value);
    if (children) MEM_free(children);    
}

static void send_item_revision_to_pse(tag_t item_revision, char *field_name)    
{
    tag_t
        rule = NULLTAG,
        window = NULLTAG,
        bvr = NULLTAG,
        top_line = NULLTAG;
        
    ERROR_CHECK(BOM_create_window (&window));
    ERROR_CHECK(CFM_find("Latest Working", &rule));
    ERROR_CHECK(BOM_set_window_config_rule(window, rule));
    ERROR_CHECK(BOM_set_window_pack_all(window, TRUE));
    ERROR_CHECK(BOM_set_window_top_line(window, NULLTAG, item_revision, bvr, 
        &top_line));
    traverse_product_structure(top_line, field_name);
    ERROR_CHECK(BOM_close_window(window));
}

static void do_it(void)
{
    tag_t
        item = NULLTAG,
        item_revision = NULLTAG;
    char
        *item_id = ITK_ask_cli_argument("-i="),
        *field_name = ITK_ask_cli_argument("-f=");

    ERROR_CHECK(ITEM_find_item(item_id, &item)); 
    if (!item) 
    {
        printf("\tItem %s not found!\n", item_id);
        exit(EXIT_FAILURE); 
    }

    ERROR_CHECK(ITEM_ask_latest_rev(item, &item_revision));        
 
    send_item_revision_to_pse(item_revision, field_name);
    set_item_revisions_master_form_property(item_revision, field_name, total);
}

int ITK_user_main(int argc, char* argv[])
{
    int
        status = 0; 
    char
        *message;
    
    ERROR_CHECK(ITK_initialize_text_services( 0 ));
    status = ITK_auto_login();
   
    if (  (status != ITK_ok)) printf("iMAN login NOT successful.\n");
    else
    {
        printf("iMAN login successful.\n\n");
        ERROR_CHECK(ITK_set_journalling(TRUE));
        do_it();
    }
    ITK_exit_module(TRUE);
    return status;
}
