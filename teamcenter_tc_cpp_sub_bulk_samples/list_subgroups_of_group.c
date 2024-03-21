/*HEAD LIST_SUBGROUPS_OF_GROUP CCC ITK */
#include <stdlib.h>
#include <tccore/aom.h>
#include <tc/emh.h>
#include <sa/group.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/tctype.h>
#include <itk/mem.h>


#define EXIT_FAILURE 1 
#define ERROR_CHECK(X) (report_error( __FILE__, __LINE__, #X, (X)))
static void report_error( char *file, int line, char *function, int return_code)
{
    if (return_code != ITK_ok)
    {
        char *error_message_string;
        EMH_get_error_string (NULLTAG, return_code, &error_message_string);
        printf ("ERROR: %d ERROR MSG: %s.\n", return_code, error_message_string);
        printf ("FUNCTION: %s\nFILE: %s LINE: %d\n", function, file, line);
        if(error_message_string) MEM_free(error_message_string);
        printf("\n Exiting program! \n");
        exit (EXIT_FAILURE);    
    }
}

#define SAFE_MEM_FREE( a )   \
do                          \
{                           \
    if ( (a) != NULL )      \
    {                       \
        MEM_free( (a) );    \
        (a) = NULL;         \
    }                       \
}                           \
while ( 0 )

static void do_it(void)
{
    int
        n_groups = 0,
        ii = 0,
        n_subgroups = 0;
    tag_t 
        *groups = NULL,
        parent = NULLTAG;
    char
        group_name[SA_name_size_c+1] = "Engineering",
        parent_group_name[SA_name_size_c+1] = "",
        sub_group_name[SA_name_size_c+1] = "";   

    ERROR_CHECK(SA_extent_group(&n_groups, &groups));
    for (ii = 0; ii < n_groups; ii++)
    {
        ERROR_CHECK(SA_ask_group_parent(groups[ii], &parent));
        if ( parent != NULLTAG)
        {
            ERROR_CHECK(SA_ask_group_name(parent, parent_group_name));
            if (strcmp(parent_group_name, group_name) == 0)
            {
                n_subgroups++;
                if (n_subgroups = 1) printf("Subgroups of %s\n", group_name);
                
                ERROR_CHECK(SA_ask_group_name(groups[ii], sub_group_name));
                printf("    %s\n", sub_group_name);
            }
        }
    }
    if (n_subgroups == 0) printf("%s has no subgroups\n", group_name);
    SAFE_MEM_FREE(groups);
}

int ITK_user_main(int argc, char* argv[])
{
    int
        status = 0; 
   
    ITK_initialize_text_services( ITK_BATCH_TEXT_MODE );
    status = ITK_auto_login();
    if (  (status != ITK_ok)) printf("\n Login Failed! \n\n");
    else
    {
        printf("\n Login successful! \n\n");
        ITK_set_journalling(TRUE);
        do_it();
    }
    ITK_exit_module(TRUE);
    return status;
}
