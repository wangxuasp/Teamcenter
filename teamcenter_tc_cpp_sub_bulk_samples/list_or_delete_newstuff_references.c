/* HEAD REPORT_CURRENT_USERS_NEWSTUFF_FOLDER_REFERENCES CCC ITK */
#include<stdlib.h>
#include <tc/tc.h>
#include <tccore/workspaceobject.h>
#include <sa/sa.h> 
#include <sa/tcfile.h>
#include <tc/folder.h>
#include <tccore/item.h>
#include <tccore/aom.h>

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

void delete_all_folder_references(tag_t folder)
{
    int
        num_of_references,
        ii;
    char
        *object_id = NULL;
    tag_t
        *list_of_references;

    ITK_CALL(FL_ask_references(folder, FL_fsc_by_name, &num_of_references, 
        &list_of_references));
    ITK_CALL(AOM_refresh(folder, TRUE));
    for (ii = 0; ii < num_of_references; ii++)
    {
        ITK_CALL(FL_remove(folder, list_of_references[ii]));
    }
    ITK_CALL(AOM_save(folder));
    ITK_CALL(AOM_unlock(folder));
    ITK_CALL(AOM_refresh(folder, TRUE));    
    for (ii = 0; ii < num_of_references; ii++)
    {
        ITK_CALL(WSOM_ask_object_id_string(list_of_references[ii], &object_id));
        printf("Delete %s\n", object_id); 
        ITK_CALL(AOM_delete(list_of_references[ii]));
        if (object_id) MEM_free(object_id);
    }
   
    if(num_of_references) MEM_free(list_of_references);
}

void report_folder_references(tag_t folder)
{
    int
        num_of_references,
        ii;
    char
        *object_id,
        name[WSO_name_size_c + 1],
        type[WSO_name_size_c + 1];
    tag_t
        *list_of_references;
    
    ITK_CALL(FL_ask_references(folder, FL_fsc_by_name, &num_of_references, 
        &list_of_references));
    for (ii = 0; ii < num_of_references; ii++)
    {
        ITK_CALL(WSOM_ask_object_id_string(list_of_references[ii], &object_id));
        ITK_CALL(WSOM_ask_name(list_of_references[ii], name));
        ITK_CALL(WSOM_ask_object_type(list_of_references[ii], type));
        printf("%s\t%s\t%s\n", object_id, name, type);
        if (object_id) MEM_free(object_id);
    } 
    if(num_of_references) MEM_free(list_of_references);     
}   
 
static void do_it(void)
{
    int
        n_found;
    char
        user_id[SA_name_size_c + 1],
        action[2];
    tag_t
        user,
        folder;
    
    ITK_CALL(SA_init_module());
    ITK_CALL(FL_init_module());
    
    printf("\n\nEnter User ID: ");
    gets(user_id);
        
    if (strlen(user_id))
    {
        ITK_CALL(SA_find_user(user_id, &user));
        if (!user)
        {
            printf("User ID \"%s\" does not exist!\n", user_id);
        }
        else
        { 
            while (printf("\n\nSelect Option [ 1 - List All, 2 - Delete All, 3 - Exit]: ") 
                && gets(action) && strlen(action) && (atoi(action) < 3))
            {
                ITK_CALL(SA_ask_user_newstuff_folder(user, &folder));  
                if ((atoi(action)) == 1) report_folder_references(folder);           
                if ((atoi(action)) == 2) delete_all_folder_references(folder);
            }
        }
    }
    SA_exit_module();
    FL_init_module();
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
        printf("iMAN login successful.\n\n");
        ITK_CALL(ITK_set_journalling(TRUE));
        do_it();
    }
    ITK_exit_module(TRUE);
    return status;
}
