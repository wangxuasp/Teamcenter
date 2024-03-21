#include <iostream>
#include <time.h>
#include <dispatcher/dispatcher_itk.h>
#include <tc/emh.h>
#include <tc/tc.h>
#include <tc/tc_startup.h>

#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using Teamcenter::Main::logger;

int create_dispatcher_request_Later(tag_t dataset_tag, tag_t rev_tag)
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
		time_t start_time = time(NULL) + 3600;  // start in one hour
        tag_t request = NULLTAG;
        stat = DISPATCHER_create_request(
                    "SIEMENS",       // provider_name
                    "nxtocgmdirect", //service_name
                    1,               // priority (low = 1, med = 2, high = 3)
                    start_time,      // start time
                    time(NULL),      // end time
                    0,               // repeating interval
                    1,               // number of primary/secondary objects
                    &dataset_tag,    // primary object
                    &rev_tag,        // secondary objects
                    0,               // number of request arguments 
                    NULL,            // request arguments 
                    "ONDEMAND",      // request type string 
                    0,               // number of data files to attach 
                    NULL,            // keys for data files to attach 
                    NULL,            // data files to attach absolute path 
                    &request );      // created request 
                                                
        int interval = 10;           // Check status after every 10 seconds
        int wait = 0;                // 0 means wait until complete
        logical is_complete = false;
        stat =  DISPATCHER_wait_for_request(request, interval, wait, &is_complete);
		if(is_complete) cout << endl << "   Translation is completed!" << endl;
    }
    catch( const IFail &ex )
    {
        logger()->error( ex.ifail(), ex.getMessage());
        cout << ex.getMessage()  << endl;
        cout << "error " << ex.ifail() << endl;
    }
    return ifail;
}
