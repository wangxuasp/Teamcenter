/* HEAD REPORT_GROUPMEMBERS_BY_USER CCC ITK */
#include <stdlib.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/workspaceobject.h>
#include <tccore/item.h>
#include <sa/sa.h>

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

static void do_it(void)
{
    logical
        is_deactivated;
    int
        ii, n_members;
	char 
        groupName[SA_name_size_c + 1] = "",
        roleName[SA_name_size_c + 1] = "",
        userName[SA_name_size_c + 1] = "user1";
    
    tag_t
        role, 
        group, 
        user, 
        *members;
        
	ITK_CALL(SA_find_user(userName, &user));
    ITK_CALL(SA_find_groupmember_by_user(user, &n_members, &members)); 
    for(ii =0; ii < n_members; ii++)
    {
        ITK_CALL(AOM_refresh(members[0], TRUE));

        ITK_CALL(SA_ask_groupmember_group(members[ii], &group));
        ITK_CALL(SA_ask_group_name(group, groupName)); 
        
        ITK_CALL(SA_ask_groupmember_role(members[ii], &role));
        ITK_CALL(SA_ask_role_name(role, roleName)); 
       
        ITK_CALL(SA_ask_groupmember_inactive(members[ii], &is_deactivated));
        if (is_deactivated)
            printf("\t%s\n\ttrole = %s\n\tis inactive\n", 
            groupName, roleName);               
        else
            printf("\tgroup = %s\n\trole = %s\n\tis active\n\n", 
            groupName, roleName);   

        ITK_CALL(AOM_unload(members[0]));    
    }
    MEM_free(members);
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
