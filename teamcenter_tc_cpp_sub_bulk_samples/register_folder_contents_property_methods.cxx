/*HEAD REGISTER_FOLDER_CONTENTS_PROPERTY_METHODS CCC ITK */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ug_va_copy.h>
#include <ict/ict_userservice.h>
#include <tc/emh.h>
#include <user_exits/user_exits.h>
#include <tccore/custom.h>
#include <tccore/method.h>
#include <itk/mem.h>
#include <tccore/aom_prop.h>
#include <tccore/workspaceobject.h>
#include <tc/folder.h>
#include <tccore/tctype.h>

#define IFERR_ABORT(X)  (report_error( __FILE__, __LINE__, #X, X, TRUE))
#define IFERR_REPORT(X) (report_error( __FILE__, __LINE__, #X, X, FALSE))

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

void report_wso_object_string_and_owning_user(tag_t object)
{
    char *object_string = NULL;
    IFERR_REPORT(WSOM_ask_object_id_string(object, &object_string));

    char *owning_user = NULL;
    IFERR_REPORT(AOM_UIF_ask_value(object, "owning_user", &owning_user));

    ECHO("       %s - %s \n", object_string, owning_user);
    MEM_free(object_string);
    MEM_free(owning_user);
}

extern DLLAPI int A2_PROP_set_value_tags(METHOD_message_t *m, va_list args)
{
    ECHO("\n\n A2_PROP_set_value_tags \n\n");

    int ifail = ITK_ok;
    tag_t folder_tag  = m->object_tag;

    va_list largs;
    va_copy( largs, args );
    tag_t prop_tag = va_arg( largs, tag_t);
    int num_tags = va_arg( largs, int);
    tag_t  *tags = va_arg( largs, tag_t*);
    va_end( largs );

    char  name[WSO_name_size_c + 1] = "";  
    IFERR_REPORT(WSOM_ask_name(folder_tag, name));

    char *owning_user = NULL;
    IFERR_REPORT(AOM_UIF_ask_value(folder_tag, "owning_user", &owning_user));
    ECHO("    %s - %s - num_tags: %d \n", name, owning_user, num_tags);
    MEM_free(owning_user);

    for (int ii = 0; ii < num_tags; ii++)
    {
		report_wso_object_string_and_owning_user(tags[ii]);
    }
    return ifail;
}

static void get_tag_array(tag_t object_tag, int *num_tags, tag_t **tags)
{	
    IFERR_REPORT(FL_ask_references(object_tag, FL_fsc_by_name, num_tags, tags));
}

extern DLLAPI int A2_PROP_ask_value_tags(METHOD_message_t *m, va_list args)
{
    ECHO("\n\n A2_PROP_ask_value_tags \n\n");

    int ifail = ITK_ok;
    tag_t folder_tag  = m->object_tag;

    va_list largs;
    va_copy( largs, args );
    tag_t prop_tag = va_arg( largs, tag_t);
    int num_tags = va_arg( largs, int);
    tag_t  *tags = va_arg( largs, tag_t*);

	/* must populate property value */
    get_tag_array(folder_tag, &num_tags, &tags);

    va_end( largs );

    char  name[WSO_name_size_c + 1] = "";  
    IFERR_REPORT(WSOM_ask_name(folder_tag, name));

    char *owning_user = NULL;
    IFERR_REPORT(AOM_UIF_ask_value(folder_tag, "owning_user", &owning_user));
    ECHO("    %s - %s - num_tags: %d \n", name, owning_user, num_tags);
    MEM_free(owning_user);

    for (int ii = 0; ii < num_tags; ii++)
    {
        report_wso_object_string_and_owning_user(tags[ii]);
    }
    return ifail;
}

extern DLLAPI int gtac_user_init_module(int *decision, va_list args)
{
    ECHO("\n\n gtac_user_init_module \n\n");
    *decision = ALL_CUSTOMIZATIONS;

    int ifail = ITK_ok;
    METHOD_id_t  method;

    int num_custom_methods = 2;
    char *type_name[] = {"Folder", "Folder"};
    char *property_name[] = {"contents", "contents"};
    char *message[] = {"PROP_ask_value_tags", "PROP_set_value_tags"};

    METHOD_function_t custom_method[] = {A2_PROP_ask_value_tags, A2_PROP_set_value_tags};

    for(int ii = 0; ii <num_custom_methods; ii++)
    {
        IFERR_REPORT(METHOD_find_prop_method(type_name[ii], property_name[ii],message[ii], &method));
        if (method.id != NULLTAG)
        {
            IFERR_REPORT( METHOD_add_action(method, METHOD_post_action_type, custom_method[ii], NULL));
            ECHO("\t Registered %s - %s custom method\n", type_name[ii], message[ii]);
        }
        else  ECHO("\t Method not found for %s - %s \n", type_name[ii], message[ii]);
    }
    return ifail;
}

extern DLLAPI int gtac_custom_exits_register_callbacks()
{
    ECHO("\n gtac_custom_exits_register_callback \n");

    IFERR_REPORT(CUSTOM_register_exit("gtac_custom_exits", "USER_init_module", (CUSTOM_EXIT_ftn_t) gtac_user_init_module));
    return ITK_ok;
}

#ifdef __cplusplus
}
#endif
