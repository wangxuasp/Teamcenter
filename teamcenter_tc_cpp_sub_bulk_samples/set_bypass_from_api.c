/* SET_BYPASS_FROM_API CCC ITK */
/*  See sample API program call_itk_to_set_bypass */
#include <stdlib.h>
#include <tc/tc.h>
#include <tccore/workspaceobject.h>
#include <sa/sa.h> 
#include <sa/tcfile.h>
#include <user_exits/user_exits.h>

#define SET_BYPASS_ON 1
#define SET_BYPASS_OFF 2

extern void USER_invoke_pdm_server(int input_code, char* input_string,
                                  int* output_code, char** output_string)                                 
{  
    int
        ifail;
                   
    switch (input_code)
    {
        case SET_BYPASS_ON:        
            ifail = POM_set_env_info( POM_bypass_access_check, 
                TRUE, 0, 0.0, NULLTAG, "" );
            if ( ifail != ITK_ok )
            {
                printf( "%d failed to turn bypass on\n", ifail );
            }
            else printf("bypass is on!\n");
            
        case SET_BYPASS_OFF:
            ifail = POM_set_env_info( POM_bypass_access_check, 
                FALSE, 0, 0.0, NULLTAG, "" );
            if ( ifail != ITK_ok )
            {
                printf( "%d failed to turn bypass off\n", ifail );
            }
            else printf("bypass is off!\n");
        default:
        break;
    }
}

