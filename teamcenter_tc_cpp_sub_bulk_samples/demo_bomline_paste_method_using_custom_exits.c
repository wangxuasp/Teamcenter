/*HEAD DEMO_BOMLINE_PASTE_METHOD_USING_CUSTOM_EXITS CCC ITK */
/*
    edit the option TC_customization_libraries to add GTAC
    %TC_ROOT%\sample\compile -DIPLIB=none demo_bomline_paste_method_using_custom_exits.c
    %TC_ROOT%\sample\link_custom_exits GTAC
    copy GTAC.dll %TC_BIN%
*/

#include <tc/tc.h>
#include <ict/ict_userservice.h>
#include <tccore/custom.h>
#include <tccore/method.h>
#include <tccore/workspaceobject.h>
#include <bom/bom.h>
#include <bom/bom_msg.h>
#include <tccore/tctype.h>
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
            ECHO("%s", "Exiting program!\n");
            exit (status);
        }
    }

    return status;
}

void report_object_type_name(tag_t object)
{
    tag_t
        type = NULLTAG;
    char
        type_name[TCTYPE_name_size_c+1] = "";

    if (object == NULLTAG) return;

    IFERR_REPORT(TCTYPE_ask_object_type(object, &type));
    IFERR_REPORT(TCTYPE_ask_name(type, type_name));
    ECHO("    type_name: %s\n", type_name);
}

extern DLLAPI int GTAC_BOMLine_method(METHOD_message_t *m, va_list args)
{

/*  Add BOMLine message arguments.  */
    tag_t parent_tag = va_arg(args, tag_t);
    tag_t item_tag = va_arg(args, tag_t);
    tag_t itemRev_tag = va_arg(args, tag_t);
    tag_t bv_tag = va_arg(args, tag_t);
    char *occType = va_arg(args, char *);
    tag_t *newBomline_tag = va_arg(args, tag_t *);

    TC_init_argument_list(m->user_args);
    switch (TC_next_int_argument(m->user_args))
    {
        case METHOD_pre_action_type:
            ECHO("\nGTAC_BOMLine_method running as pre-action:\n");
            break;
        case METHOD_post_action_type:
            ECHO("\nGTAC_BOMLine_method running as post-action:\n");
            break;
        default:
            ECHO("\nGTAC_BOMLine_method didn't get user_args\n");
    }

    ECHO("  parent_tag = %d\n", parent_tag);
    report_object_type_name(parent_tag);
    ECHO("  item_tag = %d\n", item_tag);
    report_object_type_name(item_tag);
    ECHO("  itemRev_tag = %d\n", itemRev_tag);
    report_object_type_name(itemRev_tag);
    ECHO("  bv_tag = %d\n", bv_tag);
    report_object_type_name(bv_tag);
    ECHO("  occType = %s\n", occType);
    ECHO(" *newBomline_tag = %d\n", *newBomline_tag);
    report_object_type_name(*newBomline_tag);

    return ITK_ok;
}

extern int GTAC_register_methods(int *decision, va_list args)
{
    METHOD_id_t
        method;
    TC_argument_list_t
        *myArgs;

    ECHO("GTAC_register_methods\n");

    *decision = ALL_CUSTOMIZATIONS;

    IFERR_REPORT(METHOD_find_method("BOMLine", BOMLine_add_msg, &method));
    if (method.id != NULLTAG)
    {
        myArgs = (TC_argument_list_t *) MEM_alloc( sizeof(TC_argument_list_t));
        myArgs->number_of_arguments = 1;
        myArgs->arguments = (TC_argument_t *) MEM_alloc( sizeof(TC_argument_t));
        myArgs->arguments[0].type = POM_int;
        myArgs->arguments[0].array_size = 1;
        myArgs->arguments[0].val_union.int_value = METHOD_pre_action_type;
        IFERR_REPORT( METHOD_add_action(method, METHOD_pre_action_type,
            (METHOD_function_t) GTAC_BOMLine_method, myArgs));
        ECHO("Registered GTAC_BOMLine_method as Pre-Action for BOMLine paste\n");

        myArgs = (TC_argument_list_t *) MEM_alloc( sizeof(TC_argument_list_t));
        myArgs->number_of_arguments = 1;
        myArgs->arguments = (TC_argument_t *) MEM_alloc( sizeof(TC_argument_t));
        myArgs->arguments[0].type = POM_int;
        myArgs->arguments[0].array_size = 1;
        myArgs->arguments[0].val_union.int_value = METHOD_post_action_type;
        IFERR_REPORT( METHOD_add_action(method, METHOD_post_action_type,
            (METHOD_function_t) GTAC_BOMLine_method, myArgs));
        ECHO("Registered GTAC_BOMLine_method as Post-Action for BOMLine paste\n");
    }
    else
        ECHO("Method not found!\n");

    return ITK_ok;
}

extern DLLAPI int GTAC_register_callbacks ()
{
    ECHO("GTAC_register_callbacks\n");
    IFERR_ABORT(CUSTOM_register_exit ( "GTAC", "USER_init_module",
       (CUSTOM_EXIT_ftn_t)  GTAC_register_methods));

  return ( ITK_ok );
}
