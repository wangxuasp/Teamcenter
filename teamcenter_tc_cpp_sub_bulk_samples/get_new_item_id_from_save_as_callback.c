/*HEAD get_new_item_id_from_save_as_callback ITK CCC*/
/* modified $TC_ROOT/sample/properties/user_init.c */
#include <tc/tc.h>
#include <user_exits/user_exits.h>
#include <tccore/tctype.h>
#include <ict/ict_userservice.h>
#include <tccore/method.h>
#include <tccore/item_msg.h>
#include <tccore/item_errors.h>

extern int GTAC_create_from_rev_msg(METHOD_message_t *m, va_list args)
{
    char   
		*new_item_id,
		*new_rev_id;
	tag_t 
	   *old_item,
	   *old_rev,
	   *new_item,
	   *new_rev;
   
/* From item_msg.h  concerning Item_create_from_rev_msg */ 

/*  Create a new item based on an existing item revision.
**  Varargs parameters:
**      tag_t *  old_item, tag_t *  old_rev,
**      const char *  new_item_id,
**      const char *  new_rev_id,
**      tag_t *  new_item,  tag_t *  new_rev
*/

	old_item = va_arg (args, tag_t*);
	old_rev = va_arg (args, tag_t*);

	new_item_id = va_arg (args, char*);
	new_rev_id = va_arg (args, char*);
	
	new_item = va_arg (args, tag_t*);   
	new_rev = va_arg (args, tag_t*);
    
	printf("%d - %s/%s\n", *new_rev, new_item_id, new_rev_id);
	return ITK_ok;
}

extern int GTAC_init_module(void)
{
    int
        status;
    METHOD_id_t
        method_id;
        
    status = METHOD_find_method("Item", ITEM_create_from_rev_msg, 
        &method_id);
        printf("METHOD_find_method  status = %d\n", status);
    
    status = METHOD_add_action(method_id, METHOD_post_action_type,
        GTAC_create_from_rev_msg, NULL); 
    printf("METHOD_add_action  status = %d\n", status); 

return ITK_ok;
}
/****************************************************************************
 *  USER_init_module
 *
 *  Here, you may register functions which modify the create, save etc.
 *  behaviour of Types.
 *
 *  PLEASE DO NOT WRITE EXTENSIVE CODE IN HERE, IT IS DANGEROUS BECAUSE THE
 *  SYSTEM IS STILL IN THE PROCESS OF BEING INITIALIZED
 ****************************************************************************/
extern int USER_init_module ( void )
{
    GTAC_init_module();
    return ITK_ok;
}
                       
/****************************************************************************/

extern int USER_exit_module ( void )
{
    return ITK_ok;
}

/****************************************************************************
 *  USER_preinit_module
 * 
 *  Here, you may register functions for pre-login activities such as functions
 *  for external user authentication.
 *
 *  PLEASE DO NOT WRITE EXTENSIVE CODE IN HERE, IT IS DANGEROUS BECAUSE THE
 *  SYSTEM IS STILL IN THE PROCESS OF BEING INITIALIZED
 ****************************************************************************/
extern int USER_preinit_module ( void )
{
    return ITK_ok;
}

/****************************************************************************/


/****************************************************************************
 *  USER_register_properties
 *
 *  In this function, you may register Methods to initialize properties for
 *  Types.
 *
 *  NOTE:  There is a Type with the same name as each WorkspaceObject Class
 *         in your database at the time of installation.  If you wish to use
 *         properties to customize any other Classes, you must create a Type
 *         based on that Class.  A Type with the same name as the Class will
 *         be considered a SuperType of a Type with a different name than the
 *         Class.
 *
 *  ONLY EDIT WHERE INDICATED. ANY OTHER CODE IN HERE CAN HAVE SERIOUS
 *  NEGATIVE EFFECTS
 ****************************************************************************/
extern int USER_register_properties( void )
{
    int        ifail = ITK_ok;

    /*
     * You may declare your function prototypes here or in another file
     * #included by this one. Example:
     *
     * extern int       SMPTYPE_prop_init_module();
     */

    /*
     * This array is used for registering Methods which initialize properties
     * on types. The message name is TCTYPE_init_user_props_msg. No coding
     * is necessary in this function. Just add an entry in this array to
     * indicate the object Type, the function which implements the property
     * additions/modifications and the list of arguments.
     *
     * The array should be terminated with a NULL entry.
     */
    USER_prop_init_entry_t user_types_methods[] =
    {
        /*
        { "SampleType", SMPTYPE_prop_init_module        , NULL },
        */
        { ""          , 0                               , NULL }
    };

    int n_types = sizeof(user_types_methods)/sizeof(USER_prop_init_entry_t);

    /*
     * Do not edit this call. This call is not to be used elsewhere.
     */
    ifail = TCTYPE_register_properties(user_types_methods, n_types);

    return ifail;
}

/****************************************************************************
 *  USER_add_canned_methods
 *
 *  In this function, you may add canned methods so as to make them available
 *  for configuration. 
 *  
 *  The methods added here are not registered unless they are configured. 
 *  Once configured, next sessions on initialization will register the 
 *  configured methods and hence make them active for the session.
 *
 ****************************************************************************/
extern int USER_add_canned_methods( void )
{
    return ITK_ok;
}

