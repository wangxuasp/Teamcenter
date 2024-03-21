/*HEAD PROPERTY_SET_VALUE_STRING_EXTENSION_RULES CCC ITK */
#include <tc/tc.h>
#include <tccore/aom.h>
#include <sa/user.h>
#include <tccore/grm.h>
#include <sa/group.h>
#include <user_exits/user_exits.h>
#include <tccore/tctype.h>
#include <ict/ict_userservice.h>
#include <tccore/method.h>
#include <tccore/item_msg.h>
#include <tccore/item_errors.h>
#include <itk/bmf.h>

#define EXIT_FAILURE 1 
#define ERROR_CHECK(X) (report_error( __FILE__, __LINE__, #X, (X)))
static void report_error( char *file, int line, char *function, int return_code)
{
    if (return_code != ITK_ok)
    {
        char *error_message_string;
        EMH_get_error_string (NULLTAG, return_code, &error_message_string);
        printf ("ERROR: %d ERROR MSG: %s.\n", return_code, error_message_string);
        printf ("FUNCTION: %s\nFILE: %s LINE: %d\n", function, file, line);
        if(error_message_string) MEM_free(error_message_string);
    }
}

extern DLLAPI int prop_set_value_string_precondition(METHOD_message_t *m, va_list args)
{
    /** va_list for PROP_set_value_string_msg **/
    tag_t  prop = va_arg(args, tag_t );
    char*  value = va_arg(args, char* );

    tag_t  object = NULLTAG;
    tag_t  object_type = NULLTAG;
    char   type_name[TCTYPE_name_size_c+1] = "";

    METHOD_PROP_MESSAGE_OBJECT(m, object); 
    ERROR_CHECK(TCTYPE_ask_object_type(object, &object_type));
    ERROR_CHECK(TCTYPE_ask_name(object_type, type_name));

    printf("\n- prop_set_value_string_precondition - object type: %s - value: %s\n", 
        type_name, value);

    return ITK_ok;
}

extern DLLAPI int prop_set_value_string_preaction(METHOD_message_t *m, va_list args)
{
    /** va_list for PROP_set_value_string_msg **/
    tag_t  prop = va_arg(args, tag_t );
    char*  value = va_arg(args, char* );

    tag_t  object = NULLTAG;
    tag_t  object_type = NULLTAG;
    char   type_name[TCTYPE_name_size_c+1] = "";

    METHOD_PROP_MESSAGE_OBJECT(m, object); 
    ERROR_CHECK(TCTYPE_ask_object_type(object, &object_type));
    ERROR_CHECK(TCTYPE_ask_name(object_type, type_name));

    printf("\n- prop_set_value_string_preaction - object type: %s - value: %s\n", 
        type_name, value);

    return ITK_ok;
}

extern DLLAPI int prop_set_value_string_postaction(METHOD_message_t *m, va_list args)
{
    /** va_list for PROP_set_value_string_msg **/
    tag_t  prop = va_arg(args, tag_t );
    char*  value = va_arg(args, char* );

    tag_t  object = NULLTAG;
    tag_t  object_type = NULLTAG;
    char   type_name[TCTYPE_name_size_c+1] = "";

    METHOD_PROP_MESSAGE_OBJECT(m, object); 
    ERROR_CHECK(TCTYPE_ask_object_type(object, &object_type));
    ERROR_CHECK(TCTYPE_ask_name(object_type, type_name));

    printf("\n- prop_set_value_string_postaction - object type: %s - value: %s\n", 
        type_name, value);

    return ITK_ok;
}
