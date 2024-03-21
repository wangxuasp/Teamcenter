/*HEAD WRITE_ITEM_NAMES_MATCHING_SEARCH_STRING_TO_TEXT_FILE CCC ITK */
#include <stdlib.h>
#include <string.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/item.h>
#include <pom/pom/pom.h>
#include <sa/sa.h>
#include <tccore/workspaceobject.h>

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

void ensure_user_is_infodba(void)
{
    char
        *user_name_string = NULL,
        groupname[SA_name_size_c + 1] = "";
    tag_t
        user_tag = NULLTAG,
        group_tag = NULLTAG;
        
    ITK_CALL(SA_init_module());
    ITK_CALL(POM_get_user(&user_name_string, &user_tag));
    ITK_CALL(SA_ask_user_login_group(user_tag, &group_tag));
    ITK_CALL(SA_ask_group_name(group_tag, groupname));
    if (strcmp(user_name_string, "infodba") && strcmp(groupname, "dba"))
    {
        printf("Must be DBA to execute this program\n");
        printf("Exiting program...\n");        
        if (user_name_string) MEM_free(user_name_string);
        ITK_exit_module(TRUE);
        exit(EXIT_FAILURE);
    }
    MEM_free(user_name_string);
    ITK_CALL(SA_exit_module());
}

static void do_it(void)
{
    int
        number_of_items = 0,
        ii = 0,
        status = 0;
    char
        text_filespec[IMF_filename_size_c + 1] = "",
        search_string[WSO_name_size_c + 1] = "",
        item_name[ITEM_name_size_c + 1] = "";
    FILE
        *text_file;
    tag_t
        *item_tags = NULL;
        
    /* ITK_set_bypass RESTRICTION must be an TCEng */
    ensure_user_is_infodba();
    ITK_set_bypass(TRUE);
        
    printf("Enter text file name: ");
    gets(text_filespec);
    text_file = fopen(text_filespec, "w");
    
    printf("\nEnter search string (e.g. *, A*, 123*): ");
    gets(search_string);
                
    ITK_CALL(ITEM_find(search_string, &number_of_items, &item_tags));
    printf("%d Item names will be printed to text file %s\n", 
        number_of_items, text_filespec);
    
    for(ii = 0; ii < number_of_items; ii++)
    {
        ITK_CALL(ITEM_ask_name(item_tags[ii], item_name));              
        fprintf(text_file, "%s\n", item_name);
    }
    fclose(text_file);
}

int ITK_user_main(int argc, char* argv[])
{
    int
        status = 0; 
   
    ITK_initialize_text_services( ITK_BATCH_TEXT_MODE );
    status = ITK_auto_login();
   
    if (  (status != ITK_ok)) printf("\nLogin Failed!\n\n");
    else
    {
        printf("\nLogin successful!\n\n");
        ITK_set_journalling(TRUE);
        do_it();
    }
    ITK_exit_module(TRUE);
    return status;
}
