
#include <time.h> 

int  A2ItemImpl::getA2_Date_and_TimeBase( date_t & value, bool & /*isNull*/ ) const
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

    value = current_date;
    return ifail;
}
