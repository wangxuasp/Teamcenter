#include <stdlib.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/workspaceobject.h>
#include <tccore/item.h>

#define ITK_CALL(x) {           \
    int stat;                     \
    char *err_string;             \
    if( (stat = (x)) != ITK_ok)   \
    {                             \
	EMH_get_error_string (NULLTAG, stat, &err_string);                 \
	printf ("ERROR: %d ERROR MSG: %s.\n", stat, err_string);           \
	printf ("FUNCTION: %s\nFILE: %s LINE: %d\n",#x, __FILE__, __LINE__); \
	if(err_string) MEM_free(err_string);                                \
    exit (EXIT_FAILURE);                                                   \
    }                                                                    \
}
#define EXIT_FAILURE 1 

extern void STRNG_MegaBytesToText(long int megaBytes, char text[50])
{
        /* returns a text version of given int in a form like 4.2M, 3K 
            or 10G as appropriate */
        if (megaBytes < 10000)
        {
            sprintf (text, "%4ld", megaBytes);
        }
        else if (megaBytes < 10*1024*1024)
        {
            sprintf (text, "%4dK", megaBytes / 1024);
        }
        else
        {
            int kb = megaBytes / 1024;
            if (kb < 1024*1024)
            {
                sprintf (text, "%3d.%dM", kb / 1024, (10 * kb) / 1024 % 10);
            }
            else
            {
                sprintf (text, "%3d.%dG", kb / (1024 * 1024), (10 * kb)/(1024*1024) % 10);
            }

        }
}

static void do_it(void)
{
    int count;
    char **names;
    int *values;
    int ifail;
    
    ifail = TC_ask_memory_statistics (&count, &names, &values);
    if (ifail == ITK_ok)
    {
        printf ("\nMemory Statistics\n=================\n");
        for (int i = 0; i < count; i++)
        {
            char text[50];
            STRNG_MegaBytesToText(values[i], text);
            printf ("  %-30s %s\n", names[i], text);
        }
         if(names) MEM_free (names);
        if(values) MEM_free (values);
        printf ("\n=================\n\n");
    }
        
}

int ITK_user_main(int argc, char* argv[])
{
    int
        status = 0; 
    char
        *message;
    
    ITK_initialize_text_services( 0 );
    status = ITK_auto_login();
   
    if (  (status != ITK_ok)) printf("iMAN login NOT successful.\n");
    else
    {
        printf("iMAN login successful.\n");
        ITK_set_journalling(TRUE);
        do_it();
    }
    ITK_exit_module(TRUE);
    return status;
}
