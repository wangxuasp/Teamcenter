#include <iostream>
#include <lov/lov.h>
#include <lov/lov_msg.h>
#include <sa/group.h>
#include <tc/emh.h>
#include <tc/tc.h>
#include <tc/tc_errors.h>
#include <tc/tc_startup.h>
#include <base_utils/IFail.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <base_utils/TcResultStatus.hxx>

using namespace std;
using namespace Teamcenter;

static int ask_LOV_details(char *lov_name)
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
        int nLOVs = 0;
        scoped_smptr<tag_t> sptLOVs;
        stat = LOV_find(lov_name, &nLOVs, &sptLOVs);
        tag_t tLOV = sptLOVs[0];
        
        scoped_smptr<char> spcName;
        stat = LOV_ask_name2(tLOV, &spcName);
        cout << "\t  LOV Name: " << spcName.get() << endl;
        
        scoped_smptr<char> spcType;
        stat = LOV_ask_type2(tLOV, &spcType);
        cout << "\t  LOV Type: " << spcType.get() << endl;
        
        LOV_usage_t usage;  
        stat = LOV_ask_usage(tLOV, &usage);
        string sUsage = "";
        switch (usage) 
        {
            case LOV_Exhaustive_list:  sUsage = "Exhaustive";
                break;
            case LOV_Suggestions:  sUsage = "Suggestive";
                break;
            case LOV_Ranges:  sUsage = "Range"; 
                break;
            default:  sUsage = "Unknown"; 
        }
        cout << "\t LOV Usage: " << sUsage << endl;

        PROP_value_type_t valueType;
        stat =  LOV_ask_value_type(tLOV, &valueType );
        string sValueType = "";
        switch (valueType) 
        {
            case PROP_string:  sValueType = "String";
                break;
            case PROP_typed_reference:  sValueType = "Typed Reference";
                break;
            case PROP_untyped_reference:  sValueType = "Untyped Reference"; 
                break;
        }
        cout << "\tValue Type: " << sValueType << endl;     
        cout << "\tValues: " << endl;
        if (valueType == PROP_string)
        {
            int nValues = 0;        
            scoped_smptr<char*> spcValues;
            stat = LOV_ask_values_string(tLOV, &nValues, &spcValues);
            for (int ii = 0; ii < nValues; ii++ ) 
                cout << "\t\t " << spcValues.get()[ii] << endl;
        }
        else if (valueType == PROP_typed_reference || PROP_untyped_reference)
        {
            int nValues = 0;        
            scoped_smptr<tag_t> sptValues;
            stat = LOV_ask_values_tag(tLOV, &nValues, &sptValues);
            for (int ii = 0; ii < nValues; ii++ ) 
            {
                scoped_smptr<char> spcUID;
				ITK__convert_tag_to_uid(sptValues[ii], &spcUID);             
                cout << "\t\t " << spcUID.get() << endl;
            }   
        }
    }
    catch( const IFail &e )
    {
        ifail = e.ifail();
    }                                       
    return ifail;
}

