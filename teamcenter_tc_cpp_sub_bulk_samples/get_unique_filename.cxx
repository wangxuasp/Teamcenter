#include <stdio.h>
#include <string.h>
#include <time.h>

static const char* get_unique_filename() 
{   
    time_t the_time;
    struct tm *time_ptr;
    char *time_format = "%Y-%m-%d_%H:%M";

    the_time = time((time_t *)0);
    time_ptr = localtime (&the_time);

    char date_time[128 + 1] = {"\0"};
    strftime(date_time, 128, time_format, time_ptr); 

    char *temp_dir = getenv("TC_TMP_DIR"); 
    int len = strlen(temp_dir) + strlen(date_time) + 1;

    char *filename = (char *)MEM_alloc(sizeof(char) * (len + 1));
    sprintf(filename, "%s\\%s", temp_dir, date_time);

    return filename;                                                            
}