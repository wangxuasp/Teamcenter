/*HEAD ITEM_CREATE_REV_POST_ACTION CCC ITK */
/*
    Setting up External Extension Rules in Business Modeler

    To build:
    %TC_ROOT%\sample\compile -DIPLIB=none item_create_rev_post_action.c
    %TC_ROOT%\sample\link_custom_exits item_create_rev_post_action

    Set up in Portal:
    Admin-> Business Modeler. (Login)
        Edit-> Options... -> Index-> New.
            Name: BMF_CUSTOM_IMPLEMENTOR_PATH
            Scope:  Site
            Values: C:\path\to\my\executable
        Create.
        Cancel.

    Extension Rules
        Define Extension
            Extension Details  
                Name: item_create_rev_post_action
                External
                Language Type: C
                Library: item_create_rev_post_action
                Availablity: TYPE:ItemRevision:Create Revision:Post-Action
        Assign Extensions
            Extension Point Selection
                ItemRevision
                Type
                Operation: Create Revision
                Extension Point: Post-Action
                +
                Extension Name: item_create_rev_post_action
            Apply
*/
#include <stdlib.h>
#include <itk/bmf.h>
#include <tc/emh.h>
#include <tccore/item.h>
#include <itk/mem.h>
#include <tccore/workspaceobject.h>
#include <bmf/libuserext_exports.h>

extern USER_EXT_DLL_API int item_create_rev_post_action(METHOD_message_t* msg, va_list args)
{
    int    
        ec = ITK_ok;
    char   
        message[133] = "",
        *name = NULL,
        object_type[WSO_name_size_c+1] = "";

    /* ITEM_create_rev_msg - remember va_list is order specific */
    tag_t  item = va_arg(args, tag_t); 
    char   *rev_id = va_arg(args, char*);
    tag_t  *new_rev = va_arg(args, tag_t*);
    tag_t  item_rev_master_form = va_arg(args, tag_t);

    /* demonstrate using the new rev tag */ 
    ec = WSOM_ask_object_id_string (*new_rev, &name);
    if (ec) TC_write_syslog("\n WSOM_ask_object_id_string error! \n\n");

    ec = WSOM_ask_object_type(*new_rev, object_type);
    if (ec) TC_write_syslog("\n WSOM_ask_object_type error! \n\n");

    sprintf(message, "\n%s - %s\n\n", name, object_type);
    TC_write_syslog(message);

    if (name) MEM_free(name);

    return ITK_ok;
}
