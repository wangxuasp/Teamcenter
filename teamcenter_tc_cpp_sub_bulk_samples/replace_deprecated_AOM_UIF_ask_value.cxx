
#include <iostream>
#include <tccore/aom_prop.h>

#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;

int PLMS_AOM_UIF_ask_value(tag_t object, const char* prop_name, char** disp_value) 
{
    int ifail = ITK_ok;

    try
    {
        ResultStatus stat;
        
        int nValues = 0;
        char** scpValues;
        stat = AOM_ask_displayable_values(object, prop_name, &nValues, &scpValues);

        *disp_value = (char *) MEM_alloc(sizeof(char) * strlen(scpValues[0]));
        strcpy(*disp_value, scpValues[0]);

        MEM_free(scpValues);
    }
    catch( const IFail& ex)
    {
        ifail = ex.ifail();
    }  
    return ifail;
}   
  