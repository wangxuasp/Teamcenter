/*HEAD REGISTER_ITEM_REVISION_METHOD_TO_CHECKOUT_COPIED_DATASETS CCC ITK */
/* 
    Instructions:
        Save this file as GTAC_register_callbacks.c
        The naming format is very important.
            GTAC_register_callbacks.c
            GTAC_register_callbacks() - function name
            GTAC - the name of the executable

        Compile 
            %TC_ROOT%\sample\compile -DIPLIB=libuser_exits 
                 GTAC_register_callbacks.c
        Link
            %TC_ROOT%\sample\link_custom_exits GTAC
        Move File
            copy GTAC.dll %TC_BIN%

        Set the following environment variable in the .iman_env in TCEng V9
		or using Preference Manager in later versions.
		TC_customization_libraries=
        GTAC
*/
#include <ict/ict_userservice.h>
#include <tccore/tcaehelper.h>
#include <user_exits/aiws_ext.h>
#include <tc/preferences.h>
#include <ss/ss_errors.h>
#include <tccore/custom.h>
#include <itk/mem.h>
#include <tccore/grm.h>
#include <tccore/grmtype.h>
#include <tc/tc.h>
#include <user_exits/user_exits.h>
#include <tccore/method.h>
#include <property/prop.h>
#include <property/prop_msg.h>
#include <tccore/tctype.h>
#include <property/prop_errors.h>
#include <tccore/item.h>
#include <lov/lov.h>
#include <sa/sa.h>
#include <tc/emh.h>
#include <res/reservation.h>
#include <tccore/workspaceobject.h>
#include <tc/wsouif_errors.h>
#include <tccore/aom.h>
#include <form/form.h>
#include <tccore/typecannedmethod.h>
#include <tccore/item_msg.h>
#include <tccore/wso_msg.h>
#include <tccore/grm.h>

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
        printf("\nExiting program!\n");
        exit (EXIT_FAILURE);    
    }
}

int GTAC_deep_copy_post_action(METHOD_message_t *m, va_list args)
{
    int ii = 0;
    tag_t  type = NULLTAG;
    char  type_name[TCTYPE_name_size_c+1] = "";

    /*************************************************************************/
    /*                      ITEM_deep_copy_msg paramseters                   */
    /*************************************************************************/
    /* The Tag of the newly created Item Revision */
    tag_t  new_rev = va_arg(args, tag_t); 

    /* Whether the operation is Save As/Revise */
    char*  operation = va_arg(args, char*); 

    /* The IR over which Save As/Revise operation will be performed */
    tag_t  parent_rev = va_arg(args, tag_t);

    /* Number of deep copy rules set */
    int*  copyCount = va_arg(args, int*); 

    /* Deep Copy Rule Information */
    ITEM_deepcopy_info_t*  copyInfo = va_arg(args, ITEM_deepcopy_info_t*);

    /* Number of Datasets attached */
    int*  count = va_arg(args, int*); 
    
    /* The tags of the deep copied attachments */
    tag_t**  copied_objects = va_arg(args, tag_t**); 
    /*************************************************************************/

    fprintf(stdout, "\nGTAC_deep_copy_post_action...\n");

    *decision = ALL_CUSTOMIZATIONS;

    for (ii = 0; ii < *count; ii++)
    {
        ERROR_CHECK( TCTYPE_ask_object_type((*copied_objects)[ii], &type) );
        ERROR_CHECK( TCTYPE_ask_name(type, type_name) );

        if ( (strcmp(type_name, "UGMASTER") == 0) ||
             (strcmp(type_name, "UGPART") == 0))
        {
            ERROR_CHECK( RES_checkout((*copied_objects)[ii], "auto checkout",
                "001", "C:\\temp", RES_EXCLUSIVE_RESERVE) );
        }
    }
    return ITK_ok;
}

int GTAC_register_methods(int *decision)
{
    int 
        ec = ITK_ok;
    METHOD_id_t
        method;

	*decision = ALL_CUSTOMIZATIONS;

    fprintf(stdout, "\n GTAC_register_methods...\n");

    ec = METHOD_find_method("ItemRevision", "ITEM_deep_copy", &method);
    if (ec) fprintf(stdout, "Error %d - METHOD_find_method\n", ec);

    if (method.id != NULLTAG)
    {       
        ec = METHOD_add_action(method, METHOD_post_action_type, 
            (METHOD_function_t) GTAC_deep_copy_post_action, NULL);
        if (ec != ITK_ok)
            fprintf(stdout, "\tMETHOD_add_action error: %d!\n", ec);
        else fprintf(stdout, "METHOD_add_action successful!\n");
    }
    else  fprintf(stdout, "Method NOT found!\n", ec);
    return ec;
}

extern DLLAPI int GTAC_register_callbacks ()
{ 
   fprintf(stdout, "\nRegistering gtac\n"); 
   CUSTOM_register_exit ( "GTAC", "USER_init_module", 
       (CUSTOM_EXIT_ftn_t) GTAC_register_methods);

  return ( ITK_ok );
}

