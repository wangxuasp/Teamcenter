#include<stdlib.h>
#include <tc/tc.h>
#include <sa/sa.h>
#include <tccore/workspaceobject.h>
#include <sa/user.h>

#define EXIT_FAILURE 1
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

static int ask_user_groups(tag_t user, tag_t **groups)
{
    int
        ii,
        n_members;
     char 
        group_name[SA_name_size_c + 1];
    tag_t
       *members;
       
    ITK_CALL(SA_find_groupmember_by_user(user, &n_members, &members));
    (*groups) = (tag_t *) MEM_alloc( n_members * sizeof(tag_t));
    for (ii = 0; ii < n_members; ii++)
    {
        ITK_CALL(SA_ask_groupmember_group(members[ii], *groups + ii));
    }
    if (members) MEM_free(members);
    return(n_members); 
} 

static void do_it(void)
{
    int
        ii,
        n_groups;
    char
        user_id[SA_name_size_c + 1],
        group_name[SA_name_size_c + 1],
        person_name[SA_name_size_c + 1];
    tag_t
        *groups,
        user;
        
     while (printf("\n\nEnter User ID: ") && gets(user_id) &&
        strlen(user_id))
    {
        ITK_CALL(SA_init_module());
        ITK_CALL(SA_find_user(user_id, &user));
        if (!user)
        {
            printf("User ID\"%s\" does not exist!\n", user_id);
        }
        else
        {
            ITK_CALL(SA_ask_user_person_name(user, person_name));        
            n_groups = ask_user_groups(user, &groups);
            printf("%s is a member of %d group(s):\n", person_name, n_groups); 
            for(ii = 0; ii < n_groups; ii++)
            {
                ITK_CALL(SA_ask_group_name(groups[ii], group_name));
                printf("    %s\n", group_name); 
            }
        }
    }
    ITK_CALL(SA_exit_module());    
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
