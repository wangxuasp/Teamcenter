#include <iostream>
#include <time.h>
#include <stdio.h>
#include <tc/emh.h>
#include <tc/preferences.h>
#include <tcinit/tcinit.h>
#include <Error_Exception.hxx>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;


static int create_single_value_preference(char* cpName, char* cValue)
{
    try
    {   
        ResultStatus stat;

        int nCxts = 1;
        int nVals = 1;

        char **prefValue = NULL;       
        prefValue   = (char **) MEM_alloc(sizeof(char *) * nVals);          
        prefValue[0] = (char *)MEM_alloc(strlen(cValue) + 1);
        strcpy(prefValue[0], cValue);   
        
        PREF_preference_object_t prefObj;
        prefObj.preferenceDefinition.preferenceName        = cpName;
        prefObj.preferenceDefinition.preferenceCategory    = "MyCategory";
        prefObj.preferenceDefinition.preferenceDescription = cpName;
        prefObj.preferenceDefinition.isArray               = false; 
        prefObj.preferenceDefinition.isDisabled            = false;
        
        prefObj.numberOfContexts  = nCxts;
        prefObj.contextValues = (PREF_preference_context_values_t *) MEM_alloc(nCxts * sizeof (PREF_preference_context_values_t));
        prefObj.contextValues[0].contextName = "Teamcenter";

        prefObj.contextValues[0].numberOfValues = nVals;
        prefObj.preferenceDefinition.preferenceType = PREF_preference_string;
        prefObj.preferenceDefinition.preferenceScope = TC_preference_site;
        prefObj.contextValues[0].values = prefValue;
        stat = PREF_set_definition(cpName, prefObj, false);
    } 
    catch(IFail & ex)
    {
        return ex.ifail(); 
    }       
    return ITK_ok;
}
