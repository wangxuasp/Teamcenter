/*HEAD PRINT_CUSTOM_BOM_VIEW CCC ITK */
#include<stdlib.h>
#include <bom/bom.h>
#include <cfm/cfm.h>
#include <tc/tc.h>
#include <tccore/item.h>
#include <ps/ps_errors.h>

#define EXIT_FAILURE 1
#define ITK_CALL(x) {           \
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

static int name_att, qty_att, id_att, desc_att;

static void look_up_bom_attributes(void)
{
    ITK_CALL(BOM_line_look_up_attribute(bomAttr_lineName, &name_att));
    ITK_CALL(BOM_line_look_up_attribute(bomAttr_occQty, &qty_att));
    ITK_CALL(BOM_line_look_up_attribute(bomAttr_itemId, &id_att));
    ITK_CALL(BOM_line_look_up_attribute(bomAttr_itemDesc, &desc_att));    
}

static void print_bom (tag_t line, int indention)
{
    int 
        ii, 
        count;
    char 
        *name,        
        *qty,
        *id,
        *desc;
    tag_t 
        *children;
        
    if (indention == 0) printf(" QTY    ITEM ID                  DESCRIPTION\n");
    
    indention++;
    ITK_CALL(BOM_line_ask_attribute_string(line, name_att, &name));
    ITK_CALL(BOM_line_ask_attribute_string(line, qty_att, &qty));

    if (strcmp(qty,"") == 0) strcpy(qty, "");
    ITK_CALL(BOM_line_ask_attribute_string(line, id_att, &id ));
    ITK_CALL(BOM_line_ask_attribute_string(line, desc_att, &desc));
    
    for (ii = 0; ii < indention; ii++) printf ("  ");
    printf("%s\t%-20s\t%-20s\n", qty, id, desc);
    
    ITK_CALL(BOM_line_ask_child_lines(line, &count, &children));
    for (ii = 0; ii < count; ii++) print_bom (children[ii], indention);
    if (name) MEM_free(name);
    if (qty) MEM_free(qty);
    if (id) MEM_free(id);
    if (desc) MEM_free(desc); 
    if (children) MEM_free(children);    
}

static void create_bom_window(tag_t item, char *view_name, tag_t *top_line)
{
    int
        num_of_boms,
        ii;
    char
        *type_name;
    tag_t
        rule,
        window,
        bom_view = NULLTAG,
        *bvs,
        view_type;
        
    ITK_CALL(BOM_create_window (&window));
    ITK_CALL(CFM_find("Latest Working", &rule));
    ITK_CALL(BOM_set_window_config_rule(window, rule));
    ITK_CALL(BOM_set_window_pack_all(window, TRUE));
        
    ITK_CALL(ITEM_list_bom_views( item, &num_of_boms, &bvs));
    for (ii = 0; ii < num_of_boms; ii++)
    {   
        ITK_CALL(PS_ask_bom_view_type(bvs[ii], &view_type));
        ITK_CALL(PS_ask_view_type_name(view_type, &type_name));
        if (!strcmp(type_name, view_name)) bom_view = bvs[ii];                   
    }
    if (bom_view == NULLTAG) 
        printf("BOMview: \"%s\" not found!\nUsing default BOMview.\n\n", view_name);
    ITK_CALL(BOM_set_window_top_line(window, item, NULLTAG, bom_view, top_line));     
}

static void do_it(void)
{
    int
        indention = 0;
    char
        *item_name = ITK_ask_cli_argument("-i="),
        *view_name = ITK_ask_cli_argument("-v=");
    tag_t 
        item,
        window,
        top_line;
        
        ITK_CALL(ITEM_find_item (item_name, &item));        
        if (!item) 
        {
            printf("\tItem %s does not exist!\n", item_name);
            exit(EXIT_FAILURE); 
        }
        create_bom_window(item, view_name, &top_line);
        look_up_bom_attributes(); 
        print_bom(top_line, indention);
}

int ITK_user_main(int argc, char* argv[])
{
    int
        status = 0; 
    char
        *message;
    
    ITK_CALL(ITK_initialize_text_services( 0 ));
    status = ITK_auto_login();
   
    if (  (status != ITK_ok)) printf("iMAN login NOT successful.\n");
    else
    {
        printf("iMAN login successful.\n\n");
        ITK_CALL(ITK_set_journalling(TRUE));
        do_it();
    }
    ITK_exit_module(TRUE);
    return status;
}
