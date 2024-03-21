/*HEAD CREATE_A_FOLDER ITK */
#include <tc/tc.h>
#include <tccore/aom.h>
#include <tc/folder.h>
#include <tccore/workspaceobject.h>

#define EXIT_FAILURE 1

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

int ITK_user_main(int argc, char* argv[])
{
    int
        status;
    char
        folder_name[WSO_name_size_c+1],
        fldr_desc[WSO_desc_size_c + 1],
        *message;
    tag_t
        new_folder;

    ITK_initialize_text_services( 0 );
    status = ITK_auto_login();
    if(status!= ITK_ok)
    {
        report_error_stack();
    }
    else
    {
        printf("iMAN login successful.\n");
    }

    ITK_set_journalling(TRUE);

    if (argc > 1) strcpy(folder_name, argv[1]);
    else
    {
    	printf(" Specify the Input in Following format : \n");
    	printf("program_name folder_name  folder_description\n");
    	printf("E.g. create_folder phase_3 working_folder_for_phase_3_items\n");
    	return ITK_ok; 
    }	
    
    if (argc > 2) strcpy( fldr_desc, argv[2]);
    else strcpy( fldr_desc,"");
            
    status = FL_create( folder_name, fldr_desc, &new_folder);
    if(status!= ITK_ok) report_error_stack(); 

    status = AOM_save(new_folder);
    if(status!= ITK_ok)  report_error_stack();
    else  printf("Folder '%s' Saved; ", folder_name);
 
    status = AOM_unlock(new_folder);
    if(status!= ITK_ok)  report_error_stack();
    else  printf("Unlocked; ");
                                        
    status = FL_user_update_newstuff_folder(new_folder);
    if(status!= ITK_ok) report_error_stack();
    else  printf("Put in Newstuff Folder.\n");

    ITK_exit_module(TRUE);
    return status;
}
