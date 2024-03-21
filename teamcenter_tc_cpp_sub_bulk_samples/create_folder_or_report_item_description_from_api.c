/*HEAD CREATE_FOLDER_OR_REPORT_ITEM_DESCRIPTION_FROM_API ITK */
#include <string.h>
#include <tccore/aom.h>
#include <tc/folder.h>
#include <tc/tc.h>
#include <tccore/item.h>
#include <user_exits/user_exits.h>
#include <tccore/workspaceobject.h>

#define EXIT_FAILURE 1
#define CREATE_FOLDER 1
#define PRINT_DESCRIPTION 2


static void report_error_stack( void )
 {
        int
            n_errors = 0,
            i = 0;
        const int
            *severities = NULL,
            *statuses = NULL;
        const char
            **messages;
         
     EMH_ask_errors( &n_errors, &severities, &statuses, &messages );
     printf( "Error(s): \n");
     for (i = 0; i < n_errors; i++) 
     {
         printf( "    %6d: %s\n", statuses[i], messages[i] );
     }
     exit(EXIT_FAILURE);
 }

extern void USER_invoke_pdm_server(int input_code, char* input_string,
                                  int* output_code, char** output_string)
{  
    int
        status,
        i,
        hits;
     char    
        fldr_desc[WSO_desc_size_c + 1],
        class_name[WSO_name_size_c + 1],
        *message;
    POM_tag_t   
        *tags;
    tag_t
        new_folder;
    WSO_description_t
        desc;
    WSO_search_criteria_t	criteria;
    
   switch (input_code)
    {
        case CREATE_FOLDER:
            strcpy( fldr_desc, "folder description");
            
            status = FL_create( input_string, fldr_desc, &new_folder);
            if(status!= ITK_ok) report_error_stack(); 

            status = AOM_save(new_folder);
            if(status!= ITK_ok) report_error_stack();
            else
                printf("Folder '%s' Saved; ", input_string);
 
            status = AOM_unlock(new_folder);
            if(status!= ITK_ok) report_error_stack();
            else
                printf("Unlocked; ");
                                        
            status = FL_user_update_newstuff_folder(new_folder);
            if(status!= ITK_ok) report_error_stack();
            else
                printf("Put in Newstuff Folder.\n");
                
            break;
            
        case PRINT_DESCRIPTION:        
            status = WSOM_clear_search_criteria (&criteria);
            if ( status != ITK_ok ) report_error_stack();
            
            printf ("\nEnter the WSOM_search class name (object type)[End]: ");
            gets(class_name);
            strcpy(criteria.class_name,class_name);
            strcpy(criteria.name,input_string);

            status = WSOM_search(criteria, &hits, &tags);
            if(status!= ITK_ok) report_error_stack();
            if (hits == 0)
            printf("Class: \"%s\", Name: \"%s\" not found.\n",class_name, input_string);

            if (hits == 1)
            printf("\nUnique Item Found!\n");

            if (hits > 1)
            printf("\nNumber of Items Found: %d\n", hits);

            for(i = 0; i < hits; i++)
            {
                status = WSOM_describe(tags[i], &desc);
                if ( status != ITK_ok ) report_error_stack();

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
            }
            if (tags) MEM_free(tags);
            break;
        default:
            break;
    }
}


