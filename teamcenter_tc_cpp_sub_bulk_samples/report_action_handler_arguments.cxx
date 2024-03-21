#include <iostream>
#include <epm/epm.h>
#include <epm/epm_toolkit_tc_utils.h>

#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;
using Teamcenter::Main::logger;

int report_action_handler_arguments(EPM_action_message_t msg)
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
        int numOfArgs = TC_number_of_arguments(msg.arguments);
        for(int ii=0; ii < numOfArgs; ii++)
        {
            char * pszArg = NULL;
            scoped_smptr<char> argument;
            scoped_smptr<char> value;
            scoped_smptr<char> extracted_value;
            pszArg = TC_next_argument(msg.arguments);
            if(pszArg)
            {
                stat  = ITK_ask_argument_named_value((const char*)pszArg, &argument, &value);
                
                /* Translate keyword if applicable e.g. $USER extracts the logon ID */
                stat  = EPM_substitute_keyword( value.get(), &extracted_value);
                cout << "    " << argument.get() << " - " << value.get() << " - " << extracted_value.get() << endl;
            }
        }
    }
    catch( const IFail &ex )
    {
        logger()->error( ex.ifail(), ex.getMessage());
        cout << ex.getMessage()  << endl;
        cout << "error " << ex.ifail() << endl;
    }
    return ifail;
}



