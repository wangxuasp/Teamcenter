/*HEAD SET_USER_ID CCC ITK */
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

static void do_it(void)
{
    char
        user_id[SA_name_size_c + 1],
        new_user_id[SA_name_size_c + 1];
    tag_t
        user;
        
    printf("\n\nEnter User ID: "); 
    gets(user_id);
    printf("\n\nEnter New User ID: ");
    gets(new_user_id);
    
    if (strlen(user_id))
    {
        ITK_CALL(SA_init_module());
        ITK_CALL(SA_find_user(user_id, &user));
        if (!user)
        {
            printf("User ID\"%s\" does not exist!\n", user_id);
        }
        else
        {    
            ITK_CALL(AOM_load(user));
            ITK_CALL(AOM_refresh(user, 1));
            ITK_CALL(POM_set_user_id(user, new_user_id));
            ITK_CALL(AOM_save(user));
            ITK_CALL(AOM_refresh(user,0));
            printf("Before POM_set_user_id - %s\n",user_id);
            ITK_CALL(SA_ask_user_identifier(user, user_id));            
            printf("After POM_set_user_id - %s\n",user_id); 
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
