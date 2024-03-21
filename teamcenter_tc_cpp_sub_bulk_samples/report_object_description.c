/* HEAD REPORT_OBJECT_DESCRIPTION ITK */
#include<stdlib.h>
#include <tc/tc.h>
#include <tccore/workspaceobject.h>
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
        printf("\nExiting program!\n");
        exit (EXIT_FAILURE);    
    }
}

static void do_it(void)
{
    int
        hits = 0,
        ii = 0;
    char
        name[WSO_name_size_c + 1] = "";
    tag_t   
        *list = NULL; 
    WSO_search_criteria_t
        criteria;
    WSO_description_t
        desc; 
        
    ERROR_CHECK( WSOM_clear_search_criteria (&criteria) );
         
    printf("Enter Name string : ");  
    gets(name);
    strcpy(criteria.name, name);
       
    ERROR_CHECK( WSOM_search(criteria, &hits, &list) );
    printf("Number of Items found: %d\n", hits);
    for(ii = 0; ii < hits; ii++)
    {
        ERROR_CHECK(WSOM_describe(list[ii], &desc));
        printf("\n Object Name:     %s", desc.object_name);
        printf("\n Object Type:     %s", desc.object_type);
        printf("\n Owner's Name:    %s", desc.owners_name);
        printf("\n Owning Group:    %s", desc.owning_group_name);
        printf("\n Description:     %s", desc.description);
        printf("\n Date Created:    %s", desc.date_created);
        printf("\n Date Modified:   %s", desc.date_modified);
        printf("\n Application:     %s", desc.application);
        printf("\n Date Released:   %s", desc.date_released);
        printf("\n Released For:    %s", desc.released_for);
        printf("\n ID String:       %s", desc.id_string);
        printf("\n Revision Number: %d", desc.revision_number);
        printf("\n Revision Limit:  %d", desc.revision_limit);
        printf("\n Last Modifier    %s", desc.last_modifying_user_name);
        printf("\n Archive Date:    %s", desc.archive_date);
        printf("\n Backup Date:     %s", desc.backup_date);
        printf("\n Is Frozen:       %d", desc.is_frozen);
        printf("\n Is Reserved:     %d", desc.is_reserved);
        printf("\n Revision ID:     %s\n", desc.revision_id);   
        printf("\n Owning Site:     %s\n", desc.owning_site_name);
    }
    if (list) MEM_free(list);
}

int ITK_user_main(int argc, char* argv[])
{
    int
        status = 0; 
    char
        *message;
    
    ITK_initialize_text_services( 0 );
    status = ITK_auto_login();
   
    if (  (status != ITK_ok)) printf("|n\niMAN login NOT successful.\n\n");
    else
    {
        printf("\n\niMAN login successful.\n\n");
        ITK_set_journalling(TRUE);
        do_it();
    }
    ITK_exit_module(TRUE);
    return status;
}
