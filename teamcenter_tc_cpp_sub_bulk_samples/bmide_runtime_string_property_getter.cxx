
#include <iostream>
#include <time.h> 

#include <base_utils/IFail.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <base_utils/TcResultStatus.hxx>

using namespace std;
using namespace Teamcenter;

int  A2ItemImpl::getA2_Date_and_Time_StringBase( std::string & value, 
        bool & /*isNull*/ ) const
{
    int ifail = ITK_ok;
    date_t current_date;
    struct tm *  now;

    time_t  time_now;
    time( &time_now );
    now = localtime( &time_now );

    current_date.year = static_cast<short>(now->tm_year + 1900);
    current_date.month = static_cast<byte>(now->tm_mon);
    current_date.day = static_cast<byte>(now->tm_mday);
    current_date.hour = static_cast<byte>(now->tm_hour);
    current_date.minute = static_cast<byte>(now->tm_min);
    current_date.second = static_cast<byte>(now->tm_sec);

    ResultStatus rstat;
    try
    {
        scoped_smptr<char> date_str;
        rstat = ITK_date_to_string(current_date, &date_str);
        value = date_str.getString();
    }
    catch( const IFail &e )
    {
        cout << "error " << e.ifail() << endl;
        cout << e.getMessage() << endl;
    }
    return ifail;
}
