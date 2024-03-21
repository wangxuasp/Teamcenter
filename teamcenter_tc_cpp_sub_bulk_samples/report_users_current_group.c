/*HEAD REPORT_USERS_CURRENT_GROUP CCC ITK */
#include <stdlib.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/workspaceobject.h>
#include <ae/ae.h>
#include <user_exits/user_exits.h>
#include <ss/ss_const.h>
#include <tccore/item.h>
#include <pom/pom/pom.h>
#include <ae/dataset.h>
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
    char
        *user_name_string,
        groupname[SA_name_size_c + 1];
    tag_t
        user_tag,
        group_tag;
              
    ITK_CALL(SA_init_module());
    ITK_CALL(POM_get_user(&user_name_string, &user_tag));
    ITK_CALL(SA_ask_user_login_group(user_tag, &group_tag));
    ITK_CALL(SA_ask_group_name(group_tag, groupname));
    printf("%s - %s\n", user_name_string, groupname);
    MEM_free(user_name_string);
    ITK_CALL(SA_exit_module());
}

int ITK_user_main(int argc, char* argv[])
{
    int
        status = 0; 
    char
        *message;
    
    ITK_initialize_text_services( 0 );
    ITK_CALL(ITK_init_module("my_username", "my_password", "my_groupname"));
   
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
