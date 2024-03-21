/*HEAD REGISTER_TAG_ARRAY_RUNTIME_PROPERTY CCC ITK */
#include <stdarg.h>
#include <tccore/custom.h>
#include <tccore/grm.h>
#include <ict/ict_userservice.h>
#include <tc/tc.h>
#include <tccore/tctype.h>
#include <sa/tcfile.h>
#include <tccore/method.h>
#include <property/prop.h>
#include <property/prop_msg.h>

extern int ask_primary_objects_list( METHOD_message_t *m, va_list  args )
{
    /* va_list for PROP_ask_value_tags_msg */
    tag_t prop_tag = va_arg(args, tag_t);
    int     *count = va_arg(args, int*);
    tag_t  **tags = va_arg(args, tag_t**);

    int 
        ifail = ITK_ok, 
        n_primaries = 0, 
        ii = 0;
    tag_t  
        item_revision = NULLTAG, 
        *primaries = NULL; 

    printf( "\t\t\t ask_primary_objects_list \n ");

    *count = 0;
    *tags = 0;

    METHOD_PROP_MESSAGE_OBJECT(m, item_revision);

    ifail = GRM_list_primary_objects_only(item_revision, NULLTAG, &n_primaries,
        &primaries);
    if (ifail != ITK_ok) return(ifail);

    *count = n_primaries;
    *tags = MEM_alloc(n_primaries * sizeof(tag_t));

    for (ii = 0; ii < n_primaries; ii++)
        (*tags)[ii] = primaries[ii];

    return ifail;
}

extern int GTAC_init_runtime_property(METHOD_message_t* m, va_list args)
{
    int 
        ifail = ITK_ok;
    tag_t
        new_property = NULLTAG,
        type_tag = va_arg(args, tag_t);
    char  
        typename[TCTYPE_name_size_c +1 ] = "";
    METHOD_id_t 
        method;

    printf( "\t\t GTAC_init_runtime_property \n"); 
    
    ifail = TCTYPE_ask_name(type_tag, typename);
    if (ifail != ITK_ok) return(ifail);

    ifail = METHOD_register_prop_method(typename, "primary_objects", 
        PROP_ask_value_tags_msg, ask_primary_objects_list, 0, &method );
    if (ifail != ITK_ok) return(ifail);

    return ITK_ok;
}
                                        
extern int GTAC_runtime_properties(int *decision )
{
    int ifail = ITK_ok;
        
    USER_prop_init_entry_t user_types_methods[] = 
    {
        { "ItemRevision" , GTAC_init_runtime_property , NULL },
        { ""                , 0                   , NULL }
    };
    int n_types = sizeof(user_types_methods)/sizeof(USER_prop_init_entry_t);

    *decision = ALL_CUSTOMIZATIONS;
    ifail = TCTYPE_register_properties(user_types_methods, n_types);
    return ifail;
}


extern DLLAPI int GTAC_register_callbacks ()
{ 
    CUSTOM_register_exit ( "GTAC", "USER_register_properties", 
       (CUSTOM_EXIT_ftn_t) GTAC_runtime_properties );     
  return ( ITK_ok );
}

