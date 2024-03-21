
#include <iostream>
#include <tc/tc.h>
#include <ics/ics2.h>

#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;
using Teamcenter::Main::logger;

static int report_display_values_of_ico(tag_t ico)
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {       
        int n_attrs;  
        int *attr_ids; 
        char **attr_names;  
        int *n_values;  
        char ***display_values;  
        char ***display_units;
        stat = ICS_ico_ask_attributes_optimized( ico, &n_attrs, &attr_ids, 
                &attr_names, &n_values, &display_values, &display_units);               
        cout << "\n    n_attrs: " << n_attrs << endl;
        for (int ii = 0; ii < n_attrs; ii++)
        {
            for (int kk = 0;  kk < n_values[ii]; kk++)
            {
                cout << attr_ids[ii] << "\t" << attr_names[ii] <<  "\t" 
                    << display_values[ii][kk] << endl;
            }
        }      
        stat = ICS_ico_free_attributes( n_attrs, attr_ids, n_values, display_values);
    }
    catch( const IFail &ex )
    {
        logger()->error( ex.ifail(), ex.getMessage());
        cout << ex.getMessage()  << endl;
        cout << "error " << ex.ifail() << endl;        
    }   
    return ifail;
}

