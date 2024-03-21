/*==================================================================================================

File description:                                                                                                              

    A unified set of CLOCK Date and time routines which are designed for ease of 
    porting.

    Date strings may be held in a couple of text forms:

 "1986/03/23 22:27:34" 

or

 "23-Mar-1986 22:27:34"

The former is the "normal" form which can be used for direct comparison.
This file contains conversion routines. Two macros are available 
from the CLOCK include file for number to string conversion:

   CLOCK_make_numeric_date (date, year, month, day, hour, minute )
                               gives first form
and

   CLOCK_make_textual_date (date, year, month, day, hour, minute )
                               gives second form

==================================================================================================*/

#include <unidefs.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <clock.h>
#include <itk/mem.h>

#if defined(UNX)
#include <unistd.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/param.h>
#else
#include <sys/timeb.h>
#include <winsock.h>
    struct tms {
       double tms_utime;
       double tms_stime;
       int tms_cutime;
       int tms_cstime;
    };

    struct timezone 
    {
     int    tz_minuteswest;
     int    tz_dsttime;
    };
#endif

static byte month_lengths[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31} ;

static byte leap_month_lengths[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31} ;

static char * CLOCK__numeric_date_format = "%04d/%02d/%02d %02d:%02d:%02d" ;

static char * CLOCK__textual_date_format = "%2d-%.3s-%04d %02d:%02d:%02d" ;

static char * CLOCK_month_names [] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                     "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" } ;

typedef enum 
  { timer_stopped,
    timer_running,
    timer_unused
  } timer_state_t;

typedef struct timer_value_s
  { struct timeval real_time;
    struct tms cpu_time;
  } timer_value_t, *timer_value_p_t;

typedef struct clock_timer_s
  { timer_value_t start;
    timer_value_t total;
    timer_state_t state;
  } clock_timer_t, *clock_timer_p_t;

static clock_timer_p_t timers = NULL;
static unsigned int n_timers = 0;
static unsigned int n_live_timers = 0;

static int convert_twodigit(int use_year);

#define ARRAY_CHUNK_SIZE 10

#define CLOCK_make_numeric_date( __date, __year, __month, __day, __hour, __minute, __second ) { \
   sprintf ((__date), CLOCK__numeric_date_format, \
      (__year), (__month), (__day), (__hour), (__minute), (__second) \
   ); \
}

#define CLOCK_make_textual_date( __date, __year, __month, __day, __hour, __minute, __second ) { \
   sprintf ((__date), CLOCK__textual_date_format, \
      (__day), CLOCK_month_names [(__month) - 1], (__year), (__hour), (__minute), (__second) \
   ); \
}

#define SECS_PER_MINUTE     60
#define SECS_PER_HOUR       (60 * SECS_PER_MINUTE)
#define SECS_PER_DAY        (24 * SECS_PER_HOUR)
#define SECS_PER_YEAR       (365 * SECS_PER_DAY)
#define SECS_PER_LEAP_YEAR  (SECS_PER_YEAR + SECS_PER_DAY)
#define SECS_PER_FOUR_YEARS (3 * SECS_PER_YEAR + SECS_PER_LEAP_YEAR)

#define MILLION 1000000

/*-----------------------------------------------------------------------------*/
static int no_case_compare(const char *a, const char *b)

  {

    while (*b != '\0')
      { char ca = toupper(*a);
        char cb = toupper(*b);

        if (ca != cb)
            return ca - cb;

        a++;
        b++;
      }

    return *a - *b;
  }

#if defined(WNT)
/*-----------------------------------------------------------------------------*/
#include <windows.h>

static void convertTime(FILETIME *input, double *output)
{
SYSTEMTIME systime;

	FileTimeToSystemTime(input, &systime);
	*output = (double)(systime.wHour)*3600. +
		  (double)(systime.wMinute)*60. +
		  (double)(systime.wSecond) +
		  (double)(systime.wMilliseconds)/1000.;
}

void CLOCK_cpu_times(double* user_time, double* system_time)
{ 
HANDLE hProc;
FILETIME Creation, Exit, Kernel, User;

	hProc = GetCurrentProcess();

	if (GetProcessTimes(hProc, &Creation, &Exit, &Kernel, &User)) {
		convertTime(&Kernel, system_time);
		convertTime(&User, user_time);
	} else {
		*system_time = 0.0;
		*user_time = (double) clock() / (double)CLOCKS_PER_SEC;
	}
}
#endif

/*-----------------------------------------------------------------------------*/
extern void CLOCK_local_decoded_time(date_p_t local_time)
{                                                                          
    time_t timer ;
    struct tm *t ;

    time(&timer ) ;
    t = localtime(&timer ) ;
    local_time->day = t->tm_mday ;
    local_time->month = t->tm_mon ;
    local_time->year = 1900 + t->tm_year ;
    local_time->hour = t->tm_hour ;
    local_time->minute = t->tm_min ;
    local_time->second = t->tm_sec ;
}

/*-----------------------------------------------------------------------------*/
extern CLOCK_time_t CLOCK_gmt(void)
{
    time_t timer ;

    time(&timer ) ;

    return (CLOCK_time_t) timer;
}

/*-----------------------------------------------------------------------------*/
extern int CLOCK_local_offset(void)
  { date_t local_date;
    CLOCK_time_t local_time1, local_time2, gmt_time;

    do
      {
        CLOCK_local_decoded_time(&local_date);
        local_time1 = CLOCK_date_to_time(&local_date);
        gmt_time = CLOCK_gmt();
        CLOCK_local_decoded_time(&local_date);
        local_time2 = CLOCK_date_to_time(&local_date);
      }
    while(local_time1 != local_time2);

    return (local_time1 - gmt_time) / 60;
  }

/*-----------------------------------------------------------------------------*/
extern CLOCK_time_t CLOCK_date_to_time(date_p_t date)
{
    if (CLOCK_valid_date(date))
    {
        int days;
        byte i;

        days = (date->year - 1970 ) * 365;

        if ( date->year >= 1970 )
            days = days + (date->year - 1969) / 4;
        else
            days = days - (1972 - date->year) / 4;

        if( date->year > 2100 ) days = days-1;

        for (i = 0; i < date->month; i++ )
        {
            logical leap_year = date->year % 400 == 0 || (date->year % 4 == 0 && date->year % 100 != 0 ) ;

            days = days + (leap_year ? leap_month_lengths[i] : month_lengths[i] ) ;
        }

        return (CLOCK_time_t) (((days + date->day - 1 ) * 24 + date->hour ) * 60 + date->minute ) * 60 + date->second ;
    }

    else return (CLOCK_time_t) 0 ;
}

/*-----------------------------------------------------------------------------*/
extern date_t CLOCK_time_to_date(CLOCK_time_t time)
{
    date_t date;
    unsigned int left;
    int years, month;
    logical leap_year = false ; 

    left = time;
    years = (left / SECS_PER_FOUR_YEARS);
    left = left - years * SECS_PER_FOUR_YEARS;
    years *= 4;

    if (left >= 2 * SECS_PER_YEAR + SECS_PER_LEAP_YEAR)
      { years += 3;
        left -= 2 * SECS_PER_YEAR + SECS_PER_LEAP_YEAR;
      }
    else if (left >= 2 * SECS_PER_YEAR)
      { years += 2;
        left -= 2 * SECS_PER_YEAR;
        leap_year = true;
      }
    else if (left >= SECS_PER_YEAR)
      { years += 1;
        left -= SECS_PER_YEAR;
      }
       
    date.year = 1970 + years;

    for (month = 0; ; month++ )
      { int secs_per_month = (leap_year ? leap_month_lengths[month] : month_lengths[month]) * SECS_PER_DAY;

        if (left < secs_per_month)
            break;

        left -= secs_per_month;
      }

    date.month = month;
    date.day = left / SECS_PER_DAY;
    left = left - date.day * SECS_PER_DAY;
    date.day = date.day + 1 ;
    date.hour = left / SECS_PER_HOUR;
    left = left - date.hour * SECS_PER_HOUR;
    date.minute = left / SECS_PER_MINUTE;
    left = left - date.minute * SECS_PER_MINUTE;
    date.second = left;

    return date;
}

/*-----------------------------------------------------------------------------*/
extern logical CLOCK_valid_date(const date_t *date)
{                                                                          
    logical leap_year ;

    if (date->year == 0 )
        return false ;

    if (date->month > 11 )
        return false ;

    leap_year = date->year % 400 == 0 || (date->year % 4 == 0 && date->year % 100 != 0 ) ;

    if (date->day < 1 ||
        date->day > (byte)(leap_year ? leap_month_lengths[date->month] : month_lengths[date->month] ))
        return false ;

    if (date->hour > 23 )
        return false ;

    if (date->minute > 59 )
        return false ;

    if (date->second > 59 )
        return false ;

    return true ;
}

/*-----------------------------------------------------------------------------*/
extern int CLOCK_compare_dates(const date_t *date1, const date_t *date2)
  { if (date1->year != date2->year)
        return date1->year > date2->year ? 1 : -1;

    if (date1->month != date2->month)
        return date1->month > date2->month ? 1 : -1;

    if (date1->day != date2->day)
        return date1->day > date2->day ? 1 : -1;

    if (date1->hour != date2->hour)
        return date1->hour > date2->hour ? 1 : -1;

    if (date1->minute != date2->minute)
        return date1->minute > date2->minute ? 1 : -1;

    if (date1->second != date2->second)
        return date1->second > date2->second ? 1 : -1;

    return 0;
  }

/*-----------------------------------------------------------------------------*/
extern int CLOCK_get_date_string(const date_t *date, logical normal_form, char *date_string)
{
    if (CLOCK_valid_date(date))
    {
        if (normal_form)
        {
            CLOCK_make_numeric_date(date_string, 
                                    date->year, 
                                    date->month + 1,
                                    date->day,
                                    date->hour,
                                    date->minute,
                                    date->second)
        }
        
        else
        {
            CLOCK_make_textual_date(date_string, 
                                    date->year, 
                                    date->month + 1,
                                    date->day,
                                    date->hour,
                                    date->minute,
                                    date->second)
        }

        return 0 ;
    }
    
    else return 1 ;
}

/*-----------------------------------------------------------------------------*/
extern int CLOCK_string_to_date (const char *date_string, date_p_t date)
{
    int number_found ;
    char rubbish ;
    int i1, i2, i3, i4, i5, i6 ;
    int length = strlen(date_string);

    number_found = sscanf(date_string, "%d/%d/%d %d:%d:%d%c",
                          &i1, &i2, &i3, &i4, &i5, &i6, &rubbish ) ;

    if (number_found  <= 1 )
      {
        char month_string [4] ;

        number_found = sscanf(date_string, "%d-%3s-%d %d:%d:%d%c",
                              &i1, month_string, &i3, &i4, &i5, &i6, &rubbish ) ;

        if (number_found <= 1)
          {
            number_found = sscanf(date_string, "%d %d %d %d %d %d",
                              &i1, &i2, &i3, &i4, &i5, &i6 ) ;
            if (number_found <= 1)
              {
                number_found = sscanf(date_string, "%d-%d-%d %d %d %d",
                                  &i1, &i2, &i3, &i4, &i5, &i6 ) ;
                if (number_found <= 1)
                  {
                    number_found = sscanf(date_string, "%d-%d-%d %d:%d:%d",
                                      &i1, &i2, &i3, &i4, &i5, &i6 ) ;
                    if (number_found <= 1)
                      {
                        number_found = sscanf(date_string, "%d %d %d %d:%d:%d",
                                          &i1, &i2, &i3, &i4, &i5, &i6 ) ;
                          if (number_found <=1)
                              return 2;
                      }
                  }
              }
            date->year = (short ) i1 ;
            date->month = (byte ) i2 - 1 ;
            date->day = (byte ) i3 ;
            date->hour = (byte ) i4 ;
            date->minute = (byte ) i5 ;
            date->second = (byte ) i6 ;
            if (number_found >= 3 && number_found <= 6 && length <20)
              { if (number_found == 3)
                  {
                    date->hour = date->minute = date->second = 0 ;
                  }
                else if (number_found == 4)
                  { date->minute = 0;
                    date->second = 0;
                  }
                else if (number_found == 5)
                  date->second = 0;
              }
            else
              return 2;
          }
        else
          {
            int i;

            date->year = (short ) i3 ;
            date->day = (byte ) i1 ;
            date->hour = (byte ) i4 ;
            date->minute = (byte ) i5 ;
            date->second = (byte) i6 ;
    
            if (number_found == 3 && length <= 12 )
              {
                date->hour = date->minute = date->second = 0;
              }
            else if (number_found == 5)
              date->second = 0;
            else if (number_found < 5 )
              {
              return 2 ;
              }
            date->month = 0;
    
            for (i = 0 ; i < 12 ; i++ )
              {
                if (no_case_compare(CLOCK_month_names [i], month_string) == 0)
                  {
                    date->month = i;
                    break ;
                  }
              }
    
            if (date->month == 0 ) 
              {
                 return 2 ;
              }
          }
      }
    else
      {

        date->year = (short ) i1 ;
        date->month = (byte ) i2 - 1 ;
        date->day = (byte ) i3 ;
        date->hour = (byte ) i4 ;
        date->minute = (byte ) i5 ;
        date->second = (byte ) i6 ;

        if (number_found == 3 && length <= 11 )
        {
            date->hour = date->minute = date->second = 0 ;
        }
        else if (number_found == 5)
        {
            date->second = 0;
        }
        else if (number_found != 6 )
        {
            return 2 ;
        }
    }
    if (date->year < 100 && date->year >= 0)
      date->year = convert_twodigit(date->year);

    if (CLOCK_valid_date(date)) return 0 ;

    return 1 ;
}

static int convert_twodigit(int use_year)
{
       time_t today;
       struct tm *date;
       int this_year, this_century;
       int real_year, test_year;

       today = time(NULL);
       date = localtime(&today);

       if( use_year < 0 )
           return( date->tm_year + 1900 );

       this_year = (date->tm_year + 1900) % 100;
       this_century = (date->tm_year + 1900) / 100;

       test_year = this_year + 50;
       
       if( test_year < 100 )
       {
           if( use_year <= test_year )
              real_year = (this_century)*100 + use_year;
           else
              real_year = (this_century-1)*100 + use_year;
       }
       else
       {
           test_year -= 100;
           if( use_year <= test_year )
              real_year = (this_century+1)*100 + use_year;
           else
              real_year = (this_century)*100 + use_year;
       }
   
       return( real_year );
}

static void zero_timer_value(timer_value_p_t value);
static void set_timer_value_to_current(timer_value_p_t value);
static void add_timer_values(timer_value_p_t value, timer_value_p_t addend);
static void subtract_timer_values(timer_value_p_t value, timer_value_p_t subtrahend);

#ifdef WNT
static int gettimeofday( struct timeval *tp, struct timezone *tzp)
{
    struct _timeb time_struct;
    _ftime( &time_struct );

    tp->tv_sec = (unsigned long) time_struct.time;
    tp->tv_usec = ((long) time_struct.millitm) * 1000;

    if (tzp != NULL)
    {
        tzp->tz_minuteswest = (int) time_struct.timezone;
        tzp->tz_dsttime = (int) time_struct.dstflag;
    }
    return 0;

}
#endif

/*----------------------------------------------------------------------*/
static void zero_timer_value(timer_value_p_t value)
  { value->real_time.tv_sec = 0;
    value->real_time.tv_usec = 0;
    value->cpu_time.tms_utime = 0;
    value->cpu_time.tms_stime = 0;
    value->cpu_time.tms_cutime = 0;
    value->cpu_time.tms_cstime = 0;
  }

/*----------------------------------------------------------------------*/
static void set_timer_value_to_current(timer_value_p_t value)
  {

#if UNX
    times(&value->cpu_time);
#elif defined(WNT)
	{ double user, system;
		CLOCK_cpu_times(&user, &system);
    		value->cpu_time.tms_utime  = user;
    		value->cpu_time.tms_stime  = system;
    		value->cpu_time.tms_cutime = 0;
    		value->cpu_time.tms_cstime = 0;
	}
#else
#error Porting attention required!
#endif

  gettimeofday(&value->real_time, NULL);
  }

/*----------------------------------------------------------------------*/
static void add_timer_values(timer_value_p_t value, timer_value_p_t addend)
  { value->real_time.tv_sec += addend->real_time.tv_sec;
    value->real_time.tv_usec += addend->real_time.tv_usec;

    if (value->real_time.tv_usec >= MILLION)
      { value->real_time.tv_usec -= MILLION;
        value->real_time.tv_sec += 1;
      }

    value->cpu_time.tms_utime += addend->cpu_time.tms_utime;
    value->cpu_time.tms_stime += addend->cpu_time.tms_stime;
    value->cpu_time.tms_cutime += addend->cpu_time.tms_cutime;
    value->cpu_time.tms_cstime += addend->cpu_time.tms_cstime;
  }

/*----------------------------------------------------------------------*/
static void subtract_timer_values(timer_value_p_t value, timer_value_p_t subtrahend)
  { value->real_time.tv_sec -= subtrahend->real_time.tv_sec;
    value->real_time.tv_usec -= subtrahend->real_time.tv_usec;

    if (value->real_time.tv_usec < 0)
      { value->real_time.tv_usec += MILLION;
        value->real_time.tv_sec -= 1;
      }

    value->cpu_time.tms_utime -= subtrahend->cpu_time.tms_utime;
    value->cpu_time.tms_stime -= subtrahend->cpu_time.tms_stime;
    value->cpu_time.tms_cutime -= subtrahend->cpu_time.tms_cutime;
    value->cpu_time.tms_cstime -= subtrahend->cpu_time.tms_cstime;
  }

/*----------------------------------------------------------------------*/
extern CLOCK_timer_t CLOCK_create_timer(logical start_now)
  { unsigned int i;

    for (i = 0; i < n_timers; i++)
        if (timers[i].state == timer_unused)
            break;

    if (i == n_timers)
      { unsigned int j;

        timers = (clock_timer_p_t) MEM_realloc(timers, (n_timers + ARRAY_CHUNK_SIZE) * sizeof(clock_timer_t));

        for (j = n_timers; j < (n_timers + ARRAY_CHUNK_SIZE); j++)
            timers[j].state = timer_unused;

        n_timers += ARRAY_CHUNK_SIZE;
      }

    zero_timer_value(&timers[i].total);

    if (start_now)
      { set_timer_value_to_current(&timers[i].start);
        timers[i].state = timer_running;
      }
    else
        timers[i].state = timer_stopped;

    n_live_timers++;

    return i;
  }

/*----------------------------------------------------------------------*/
extern void CLOCK_start_timer(CLOCK_timer_t timer)
  { set_timer_value_to_current(&timers[timer].start);
    timers[timer].state = timer_running;
  }

/*----------------------------------------------------------------------*/
extern void CLOCK_read_timer(CLOCK_timer_t timer, CLOCK_read_timer_opt_t option, CLOCK_timer_values_t *values)
  { timer_value_t now;
    timer_value_p_t resultp;
#if defined(UNX)
    static logical got_clk_tck = false;
    static double clk_tck;
#endif

    if (timers[timer].state == timer_stopped)
        zero_timer_value(&now);
    else
      { set_timer_value_to_current(&now);
        subtract_timer_values(&now, &timers[timer].start);
      }

    if (option == CLOCK_read_and_stop || option == CLOCK_read_and_delete)
      { add_timer_values(&timers[timer].total, &now);
        resultp = &timers[timer].total;
        timers[timer].state = timer_stopped;
      }
    else
      { add_timer_values(&now, &timers[timer].total);
        resultp = &now;
      }

#if defined(UNX)

    if (!got_clk_tck)
      { clk_tck = (double) sysconf(_SC_CLK_TCK);
        got_clk_tck = true;
      }

    values->cpu_time = (double) (resultp->cpu_time.tms_utime + resultp->cpu_time.tms_stime) / clk_tck;
    values->child_cpu_time = (double) (resultp->cpu_time.tms_cutime + resultp->cpu_time.tms_cstime) / clk_tck;
#elif defined(WNT)
    values->cpu_time = resultp->cpu_time.tms_utime + resultp->cpu_time.tms_stime;
    values->child_cpu_time = 0.0;
#else
#error Porting attention required!
#endif
    values->real_time = (double) resultp->real_time.tv_sec + (double) resultp->real_time.tv_usec / (double) MILLION;

    if (option == CLOCK_read_and_delete)
        CLOCK_delete_timer(timer);
  }

/*----------------------------------------------------------------------*/
extern void CLOCK_delete_timer(CLOCK_timer_t timer)
  { timers[timer].state = timer_unused;
    n_live_timers--;

    if (n_live_timers == 0)
      { n_timers = 0;
        MEM_free(timers);
        timers = NULL;
      }
  }

/*----------------------------------------------------------------------*/
extern void CLOCK_reset_timer(CLOCK_timer_t timer)
  { zero_timer_value(&timers[timer].total);

    if (timers[timer].state == timer_running)
        set_timer_value_to_current(&timers[timer].start);
  }

/*----------------------------------------------------------------------*/
extern void CLOCK_add_time(CLOCK_time_t *cd , CLOCK_time_t incr)
{ 
    *old = *old + incr;
}

/*----------------------------------------------------------------------*/
extern void CLOCK_sub_time(CLOCK_time_t *old, CLOCK_time_t decr)
{ 
    *old = *old - decr;
}

/*----------------------------------------------------------------------*/
extern char * CLOCK_format_date_time(date_p_t date, int form_min, int form_date)
 {
     char *time_format, *date_format;
     char *buf;
     char fmt[25];
     char fmt1[512];
     struct tm time;
     char *month[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
     logical  xml_datetime = false;

     time.tm_sec = date->second;
     time.tm_min = date->minute;
     time.tm_hour = date->hour;
     time.tm_mday = date->day;

     time.tm_mon = date->month;
     time.tm_year = date->year - 1900;
     time.tm_wday = 0;
     time.tm_isdst = -1; 
     time.tm_yday = 0; 

     if (form_min == CLOCK_FMT_XML_DATETIME || form_date == CLOCK_FMT_XML_DATETIME)
     {
         xml_datetime = true;
         form_min = CLOCK_FMT_XML_TIME;
         form_date = CLOCK_FMT_XML_DATE;
     }

     if( form_min == CLOCK_FMT_NONE )
         time_format="";
     else if( form_min == CLOCK_FMT_24 )
         time_format="%H:%M";
     else if( form_min == CLOCK_FMT_12 )
         time_format="%I:%M %p";
     else if( form_min == CLOCK_FMT_24_SEC || form_min == CLOCK_FMT_XML_TIME)
         time_format="%H:%M:%S";
     else
         time_format="%X";

     if( form_date == CLOCK_FMT_NONE )
         date_format="";
     else if( form_date == CLOCK_FMT_MMDDYY )
         date_format="%m/%d/%y";
     else if( form_date == CLOCK_FMT_DDMMMYY )
     {
         char percent = '%';
         sprintf(fmt1,"%cd-%s-%cy",percent,month[date->month],percent);
         date_format=fmt1;
     }
     else if( form_date == CLOCK_FMT_MMDDYYYY )
         date_format="%m/%d/%Y";
     else if( form_date == CLOCK_FMT_DDMMMYYYY )
     {
         char percent = '%';
         sprintf(fmt1,"%cd-%s-%cY",percent,month[date->month],percent);
         date_format=fmt1;
     }
     else if( form_date == CLOCK_FMT_UG )
         date_format="%m--%d--%y";
     else if( form_date == CLOCK_FMT_UG_YYYY )
         date_format="%m--%d--%Y";
     else if( form_date == CLOCK_FMT_YYYYMMDD )
         date_format="%Y/%m/%d";
     else if ( form_date == CLOCK_FMT_XML_DATE )
         date_format="%Y-%m-%d";
     else
         date_format="%x";

     if( form_date == CLOCK_FMT_NONE || form_min == CLOCK_FMT_NONE )
     {
        strcpy(fmt,date_format);
        strcat(fmt,time_format);
     }
     else if( form_date == CLOCK_FMT_USER_FRIENDLY && form_min == CLOCK_FMT_USER_FRIENDLY )
     {
        time_t new_time;
        struct tm *updated_time;
        new_time = mktime(&time);
        updated_time = localtime(&new_time);
        
        time.tm_wday = updated_time->tm_wday; 
        time.tm_yday = updated_time->tm_yday; 

        strcpy(fmt,"%c");
     }
     else
     {
        strcpy(fmt,date_format);

        if (xml_datetime)
            strcat (fmt, "T");
        else
            strcat(fmt," ");

        strcat(fmt,time_format);
     }
    
     buf = (char *) MEM_alloc(100);

     strftime(buf,100,fmt,&time);

     return(buf);
}
