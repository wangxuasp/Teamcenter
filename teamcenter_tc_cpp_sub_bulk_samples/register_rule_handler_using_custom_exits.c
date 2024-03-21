/*HEAD REGISTER_RULE_HANDLERS_USING_CUSTOM_EXITS CCC ITK */
/* 
    Instructions:
        Save this file as GTAC_register_callbacks.c
        The naming format is very important.
            GTAC_register_callbacks.c
            GTAC_register_callbacks() - function name
            GTAC - the name of the executable

        Compile 
            %TC_ROOT%\sample\compile -DIPLIB=libuser_exits 
                 GTAC_register_callbacks.c
        Link
            %TC_ROOT%\sample\link_custom_exits GTAC
        Move File
            copy GTAC.dll %TC_BIN%


        Set the following site environment variable in the .iman_env in TCEng V9
        or using Preference Manager in later versions.
        TC_customization_libraries=
        GTAC
*/
#include <ict/ict_userservice.h>
#include <tccore/tcaehelper.h>
#include <user_exits/aiws_ext.h>
#include <tc/preferences.h>
#include <ss/ss_errors.h>
#include <tccore/custom.h>
#include <itk/mem.h>
#include <tc/tc.h>
#include <user_exits/user_exits.h>
#include <tccore/method.h>
#include <tc/emh.h>
#include <epm/epm.h>
#include <tccore/tctype.h>
#include <tccore/item.h>
#include <epm/epm.h>
#include <tccore/workspaceobject.h>

#define EXIT_FAILURE 1 
#define ERROR_CHECK(X) (report_error( __FILE__, __LINE__, #X, (X)))
static void report_error( char *file, int line, char *function, int return_code)
{
    if (return_code != ITK_ok)
    {
        char *error_msg_string;

        EMH_get_error_string (NULLTAG, return_code, &error_msg_string);
        printf("ERROR: %d ERROR MSG: %s.\n", return_code, error_msg_string);
        TC_write_syslog("ERROR: %d ERROR MSG: %s.\n", return_code, 
            error_msg_string);
        printf ("FUNCTION: %s\nFILE: %s LINE: %d\n", function, file, line);    
        TC_write_syslog("FUNCTION: %s\nFILE: %s LINE: %d\n", 
            function, file, line);
        if(error_msg_string) MEM_free(error_msg_string);
    }
}

EPM_decision_t GTAC_rule_handler(EPM_rule_message_t message)
{
    int
        n_attachments = 0,
        ii = 0;
    tag_t
        *attachments = NULL,
        class_tag = NULLTAG;
    char  
        task_name[WSO_name_size_c+1] = "",
        *class_name = NULL,
        description[WSO_desc_size_c+1]  = "";
    EPM_decision_t
        decision = EPM_go;
        
    printf("\n GTAC_rule_handler \n");

    ERROR_CHECK(EPM_ask_name(message.task, task_name));
    printf("\n\t message.task name: %s\n", task_name);

    ERROR_CHECK(EPM_ask_attachments(message.task, EPM_target_attachment, 
        &n_attachments, &attachments));
    printf("\t %s has %d attachments\n", task_name, n_attachments);

    for (ii = 0; ii < n_attachments; ii++)
    {
        ERROR_CHECK(POM_class_of_instance(attachments[ii], &class_tag));
        ERROR_CHECK(POM_name_of_class(class_tag, &class_name)); 
        if (strcmp(class_name, "ItemRevision") == 0)
        {
            ERROR_CHECK(WSOM_ask_description(attachments[ii], description));
            if ( strlen(description) == 0 )
                decision = EPM_nogo;
            else
                decision = EPM_go;
        }
    }           
    if (class_name) MEM_free(class_name);   
    if(attachments) MEM_free(attachments);

    return decision;
}

int GTAC_register_rule_handler(int *decision, va_list args)
{
    int ec = ITK_ok; 

    *decision = ALL_CUSTOMIZATIONS;

    printf("\n GTAC_registe_rule_handler \n");

    ec = EPM_register_rule_handler("GTAC-rule-handler", "", GTAC_rule_handler);
    if (ec == ITK_ok) 
        printf("\t Registered GTAC_rule_handler!\n\n");
    else 
        printf("\t FAILED to register GTAC_rule_handler\n\n");

    return ec;
}

extern DLLAPI int GTAC_register_callbacks ()
{ 
    printf("\n GTAC_register_callbacks \n");

    CUSTOM_register_exit ( "GTAC", "USER_gs_shell_init_module", 
       (CUSTOM_EXIT_ftn_t) GTAC_register_rule_handler );
       
    return ITK_ok;
}


