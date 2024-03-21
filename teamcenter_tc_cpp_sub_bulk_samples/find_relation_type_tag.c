/*HEAD FIND_RELATION_TYPE_TAG  CCC ITK */
#include<stdlib.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <sa/am.h> 
#include <tccore/item.h>
#include <tccore/workspaceobject.h>
#include <sa/sa.h> 
#include <tccore/tctype.h>

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

static logical do_it(void)
{
    int
        ii,
        n_relation_types,
        found = FALSE;
    tag_t
        *relation_type_list,
        relation_type;
    char
        find_name[TCTYPE_name_size_c+1],
        type_name[TCTYPE_name_size_c+1];
        
    while (printf("\nEnter Relation type: ") && gets(find_name) &&
        strlen(find_name))
    {
        found = FALSE;           
        ITK_CALL(GRM_list_relation_types(&n_relation_types, &relation_type_list));
        for(ii = 0; ii < n_relation_types; ii++)
        {
            ITK_CALL(TCTYPE_ask_name(relation_type_list[ii], type_name));
            if (!strcmp(find_name, type_name))
            {            
                ITK_CALL(GRM_find_relation_type(find_name, &relation_type));
                printf("%s = %d\n", find_name, relation_type);
                found = TRUE;
            }
        }
        
        if (!found)
        {
            printf("Relation Type: %s not found!\n", find_name);
            printf("Valid Relations Types: \n");
            for(ii = 0; ii < n_relation_types; ii++)
            {
                ITK_CALL(TCTYPE_ask_name(relation_type_list[ii], type_name));
                printf("%s ", type_name);
            }
            printf("\n");
        }
        MEM_free(relation_type_list);
    }   
}

int ITK_user_main(int argc, char* argv[])
{
    int
        status = 0; 
    char
        *message;
    
    ITK_CALL(ITK_initialize_text_services( 0 ));
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
