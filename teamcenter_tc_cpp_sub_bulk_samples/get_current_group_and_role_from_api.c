/* GET_CURRENT_GROUP_FROM_API CCC ITK */
/*  See sample API program call_itk_to_report_current_group_and_role */
#include <stdlib.h>
#include <tc/tc.h>
#include <tccore/workspaceobject.h>
#include <sa/sa.h> 
#include <sa/tcfile.h>
#include <user_exits/user_exits.h>

#define GET_CURRENT_GROUP 1
#define GET_CURRENT_ROLE 2

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

extern void USER_invoke_pdm_server(int input_code, char* input_string,
                                  int* output_code, char** output_string)                                 
{  
    char
        *group_name,
        role_name[SA_name_size_c + 1];
    tag_t
        group,
        role;
           
    switch (input_code)
    {
        case GET_CURRENT_GROUP:
            ITK_CALL(POM_ask_group(&group_name, &group));
             *output_string = malloc(sizeof(char) * (strlen(group_name) + 1));
            strcpy(*output_string, group_name);
            if (group_name) MEM_free(group_name);
            break;
        case GET_CURRENT_ROLE:
            ITK_CALL(SA_ask_current_role(&role));
            ITK_CALL(SA_ask_role_name(role, role_name));
            *output_string = malloc(sizeof(char) * (strlen(role_name) + 1));
            strcpy(*output_string, role_name);
            break;        
        default:
            break;
    }
}

