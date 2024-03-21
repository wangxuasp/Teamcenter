/*HEAD ITEM_CREATE_REV_POST_ACTION_TO_ATTACH_FORM CCC ITK */
/*
    Setting up External Extension Rules in Business Modeler

    To build:
    %TC_ROOT%\sample\compile -DIPLIB=none source_name.c
    %TC_ROOT%\sample\link_custom_exits libname

    Set up in Portal:
    Admin-> Business Modeler. (Login)
        Edit-> Options... -> Index-> New.
            Name: BMF_CUSTOM_IMPLEMENTOR_PATH
            Scope:  Site
            Values: C:\path\to\my\executables\folder
        Create.
        Cancel.

    Extension Rules
        Define Extension
            Extension Details  
                Name: item_create_rev_post_action_to_attach_form
                External
                Language Type: C
                Library: item_create_rev_post_action_to_attach_form
                Availablity: TYPE:ItemRevision:Create Revision:Post-Action
        Assign Extensions
            Extension Point Selection
                ItemRevision
                Type
                Operation: Create Revision
                Extension Point: Post-Action
                +
                Extension Name: item_create_rev_post_action_to_attach_form
            Apply
*/
#include <stdlib.h>
#include <itk/bmf.h>
#include <tc/emh.h>
#include <tccore/item.h>
#include <itk/mem.h>
#include <tccore/workspaceobject.h>
#include <bmf/libuserext_exports.h>

extern USER_EXT_DLL_API int item_create_rev_post_action_to_attach_form
    (METHOD_message_t* msg, va_list args)
{
    int    
        ec = ITK_ok;
    tag_t
        form = NULLTAG,
        relation_type = NULLTAG,
        relation = NULLTAG;
    char   
        *item_rev_object_string = NULL,
        form_name[WSO_name_size_c+1] = "";

    /* ITEM_create_rev_msg - remember va_list is order specific */
    tag_t  item = va_arg(args, tag_t); 
    char   *rev_id = va_arg(args, char*);
    tag_t  *new_rev = va_arg(args, tag_t*);
    tag_t  item_rev_master_form = va_arg(args, tag_t);

    ec = WSOM_ask_object_id_string (*new_rev, &item_rev_object_string);
    if (ec) TC_write_syslog("\n WSOM_ask_object_id_string error: \n\n", ec);

    sprintf(form_name, "%s_form", item_rev_object_string);
    ec = FORM_create(form_name, "", "GTAC Form Type", &form);
    if (ec) TC_write_syslog("\n FORM_create error: %d! \n\n", ec);

    ec = AOM_save(form);
    if (ec) TC_write_syslog("\n AOM_save error: %d! \n\n", ec);    

    ec = GRM_find_relation_type("IMAN_specification", &relation_type);
    if (ec) TC_write_syslog("\n GRM_find_relation_type error: %d! \n\n", ec);

    ec = GRM_create_relation(*new_rev, form, relation_type, NULLTAG, &relation); 
    if (ec) TC_write_syslog("\n GRM_create_relation error: %d! \n\n", ec);    

    ec = GRM_save_relation(relation); 
    if (ec) TC_write_syslog("\n GRM_create_relation error: %d! \n\n", ec);
    
    ec = AOM_save(form);
    if (ec) TC_write_syslog("\n AOM_save error: %d! \n\n", ec);

	ec = AOM_unload(form);
    if (ec) TC_write_syslog("\n AOM_unload error: %d! \n\n", ec);


    if (item_rev_object_string) MEM_free(item_rev_object_string);
    return ITK_ok;
}
