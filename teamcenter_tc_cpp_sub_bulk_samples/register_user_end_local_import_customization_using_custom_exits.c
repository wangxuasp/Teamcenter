/*HEAD REGISTER_USER_END_LOCAL_IMPORT_CUSTOMIZATION_USING_CUSTOM_EXITS CCC ITK */
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
            %TC_ROOT%\link_custom_exits GTAC
        Move File
            copy GTAC.dll %TC_BIN%

        Notice: The object files did not have extracted with custom exits.

        Login to My Navigator
            Edit-> Options-> Index-> New
                Name: TC_customization_libraries
                Scopes: Site
                Type: String
                Multiple Values: True
                Values: GTAC
                Create-> Cancel

                File-> Exit
         Close TAO ImR window.

        Login to My Navigator

Note: After logging in the syslog should have the following lines to indicate that the customization was loaded. 
"Successfully loaded the Custom Library <GTAC>
 Successfully loaded the Entry Point Function Ptr 
              <GTAC_register_callbacks> for Custom Library <GTAC>"

*/
#include <ict/ict_userservice.h>
#include <tccore/tcaehelper.h>
#include <user_exits/aiws_ext.h>
#include <tc/preferences.h>
#include <ss/ss_errors.h>
#include <tccore/custom.h>
#include <itk/mem.h>
#include <tc/tc.h>
#include <user_exits/user_exits.h>
#include <tccore/method.h>
#include <epm/epm.h>
#include <tccore/tctype.h>
#include <tccore/item.h>
#include <bom/bom.h>
#include <cfm/cfm.h>
#include <tccore/tctype.h>
#include <tccore/method.h>

extern DLLAPI int GTAC_end_local_import(int *decision, va_list args)
{
	int ifail = ITK_ok;

	/******** va_list for USER_end_local_import **************/
    tag_t   main_object_tag = va_arg(args, tag_t);          
    logical is_transferring_ownership = va_arg(args, logical);          
    char *export_directory = va_arg(args, char *);          

    *decision = ALL_CUSTOMIZATIONS;

    printf("\n GTAC_end_local_import \n");

    printf("\t export_directory: %s\n", export_directory);

    if (is_transferring_ownership) printf("\t Transfering Ownership!\n");

    return ifail;
}

extern DLLAPI int GTAC_register_callbacks ()
{ 
    printf("GTAC_register_callbacks\n");
    CUSTOM_register_exit ( "GTAC", "USER_end_local_import", 
       (CUSTOM_EXIT_ftn_t) GTAC_end_local_import );
       
  return ( ITK_ok );
}
