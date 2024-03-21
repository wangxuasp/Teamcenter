/*HEAD ME_PROCESS_FROM_TEMPLATE_METHOD.CXX CCC ITK */
/*
To build custom shared library on Windows:
%TC_ROOT%\sample\compile -DIPLIB=none me_process_from_template_method.cxx
%TC_ROOT%\sample\link_custom_exits gtac_custom_exits
Copy shared library to TC_BIN or TC_USER_LIB

To build custom shared library on Unix:
$TC_ROOT/sample/compile me_process_from_template_method.cxx
$TC_ROOT/sample/link_custom_exits gtac_custom_exits
Copy shared library to TC_LIB or TC_USER_LIB

Copy shared library to TC_BIN or TC_USER_LIB

Create or set TC_customization_libraries preference to include the shared library without file extension.
e.g. %TC_BIN%\preferences_manager -u=infodba -p=infodba -mode=import -action=merge -preference=TC_customization_libraries -scope=SITE -values=gtac_custom_exits
    
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdarg.h>
#include <ug_va_copy.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/grm.h>
#include <ict/ict_userservice.h>
#include <tc/tc.h>
#include <tccore/item.h>
#include <tccore/item_msg.h>
#include <tc/emh.h>
#include <user_exits/user_exits.h>
#include <tccore/custom.h>
#include <pie/pie.h>
#include <tccore/item_msg.h>
#include <tccore/method.h>
#include <me/me_msg.h>
#include <me/me.h>

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

static void GTAC_free(void *what)
{
    if (what != NULL)
    {
        MEM_free(what);
        what = NULL;
    }
}

extern DLLAPI int process_from_template(METHOD_message_t *msg, va_list args)
{
    ECHO( "\n me_from_template_post_action \n\n");
    ECHO( "   Object Type: MEProcessRevision \n");
    ECHO( "   Operation: From Template \n");
    ECHO( "   Message name: ME_clone_template_action_msg \n\n");

    int ifail = ITK_ok;

    va_list largs;
    va_copy( largs, args );
    
    tag_t parent_tag = va_arg(largs, tag_t); 
    tag_t parent_line_tag = va_arg(largs, tag_t);
    tag_t new_object_tag = va_arg(largs, tag_t);
    const char *name = va_arg(largs, const char *);
    const char *desc = va_arg(largs, const char *);
    const char *id = va_arg(largs, const char *);
    const char *revid = va_arg(largs, const char *);

    va_end( largs );

    ECHO("   parent_tag: %u \n", parent_tag);
    ECHO("   parent_line_tag: %u \n", parent_line_tag);
    ECHO("   new_object_tag: %u \n", new_object_tag);
    ECHO("   name: %s \n", name);
    ECHO("   desc: %s \n", desc);
    ECHO("   id: %s \n", id);
    ECHO("   revid: %s \n", revid);

    return ifail;
}

extern DLLAPI int gtac_user_init_module(int *decision)
{
    ECHO("\n gtac_user_init_module \n");
    *decision = ALL_CUSTOMIZATIONS;

    int ifail = ITK_ok;
    char type[] = "MEProcessRevision";
    char message[] = "ME_clone_template_action";
    METHOD_id_t method;

    ifail = METHOD_register_method(type, message, NULL, 0, &method);
    if (ifail) ECHO("\n Error %d - METHOD_register_method\n", ifail);
    if (method.id != NULLTAG)
    { 
        ifail = METHOD_add_action(method, METHOD_post_action_type, 
            (METHOD_function_t) process_from_template, NULL);

        if 
            (ifail != ITK_ok) ECHO("\n METHOD_add_action error: %d!\n", ifail);
        else 
            ECHO("\ n%s - %s - added post_action!\n", type, message);
    }
    else 
        ECHO("\n\n method not found!\n");

    return ITK_ok;
}

extern DLLAPI int gtac_custom_exits_register_callbacks()
{
    ECHO("\n gtac_custom_exits_register_callbacks \n");
    
    CUSTOM_register_exit( "gtac_custom_exits" /* shared lib name */,
        "USER_init_module", /* user_exit name*/
        (CUSTOM_EXIT_ftn_t) gtac_user_init_module /* custom function name */);

    return ITK_ok;
}

#ifdef __cplusplus
}
#endif
