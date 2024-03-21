/*HEAD SET_BYPASS_FROM_OPEN_C CCC ITK */
/* 
    Instructions:
        Save this file as GTAC_register_callbacks.c
        The naming format is very important.
            GTAC_register_callbacks.c
            GTAC_register_callbacks() - function name
            GTAC - the name of the shared library

        Compile 
 %TC_ROOT%\sample\compile -DIPLIB=libuser_exits  GTAC_register_callbacks.c
 
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
#include <ae/dataset.h>
#include <bom/bom.h>
#include <cfm/cfm.h>

#define SET_BYPASS_ON    1
#define SET_BYPASS_OFF   2
#define REPORT_BYPASS    3

#define FAIL 1
#define EXIT_FAILURE 1 

#define ERROR_CHECK(X) (report_error( __FILE__, __LINE__, #X, (X)))
static void report_error( char *file, int line, char *function, int return_code)
{
    if (return_code != ITK_ok)
    {
        char *error_msg_string;

        EMH_get_error_string (NULLTAG, return_code, &error_msg_string);
        printf("ERROR: %d ERROR MSG: %s.\n", return_code, error_msg_string);
        TC_write_syslog("ERROR: %d ERROR MSG: %s.\n", return_code, 
            error_msg_string);
        printf ("FUNCTION: %s\nFILE: %s LINE: %d\n", function, file, line);
        TC_write_syslog("FUNCTION: %s\nFILE: %s LINE: %d\n", function, file, line);
        if(error_msg_string) MEM_free(error_msg_string);
    }
}

extern int GTAC_invoke_pdm_server(int *decision, va_list args)
{
    /*********** va_list for USER_invoke_pdm_server *********/ 
    int  input_code = va_arg(args, int);          /* args 1 */  
    char *input_string = va_arg(args, char *);    /* args 2 */
    int  *output_code = va_arg(args, int *);      /* args 3 */
    char **output_string = va_arg(args, char **); /* args 4 */

    int
        ifail;
    logical
        bypass_status = FALSE;

    *decision = ALL_CUSTOMIZATIONS;        


    switch (input_code)
    {
        case SET_BYPASS_ON:        
            ifail = ITK_set_bypass( TRUE );
            if ( ifail != ITK_ok )
            {
                printf( "%d failed to turn bypass on\n", ifail );
            }
            else 
            {
                printf("BYPASS IS ON!\n");
            }
            break;

        case SET_BYPASS_OFF:
            ifail = ITK_set_bypass( FALSE );
            if ( ifail != ITK_ok )
            {
                printf( "%d failed to turn bypass off\n", ifail );
            }
            else printf("bypass is off!\n");
            break;
        case REPORT_BYPASS:
            ifail = ITK_ask_bypass( &bypass_status );

            if( bypass_status == TRUE )
            {
                printf("BYPASS IS ON...BE CAREFUL!\n");
            }
            if( bypass_status == FALSE )
            {
                printf("BYPASS IS OFF\n");
            }

        default:
        break;
    }

    return ITK_ok;
}

/* This function name must be <library_name>_register_callbacks */
extern DLLAPI int GTAC_register_callbacks()
{
    printf("\n GTAC_register_callbacks \n\n");

    ERROR_CHECK(CUSTOM_register_exit( "GTAC", "USER_invoke_pdm_server", 
        (CUSTOM_EXIT_ftn_t) GTAC_invoke_pdm_server));       
    return ITK_ok;
}
