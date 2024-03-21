/*HEAD	EXPORT_SPECIFIC_FORM_USING_PLMXML_EXPORT_UTILITY CCC ITK */
#include <stdlib.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/workspaceobject.h>
#include <form/form.h>
#include <tccore/aom.h>
#include <tc/emh.h>

#include <itk/mem.h>


#define ERROR_CHECK(x) {           \
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

    int
        hits = 0,
		ii = 0;
    char
        *form_name = ITK_ask_cli_argument("-f="),
        *form_type = ITK_ask_cli_argument("-t="),
        *uid = NULL,
		util[] = "%TC_BIN%\\plmxml_export",
		login[] = "-u=infodba -p=infodba -g=dba",
		xml_file[] = "-xml_file=form.xml",
		cmd[256] = " ";
    tag_t
        *forms;
	WSO_search_criteria_t
        criteria;
  
    ERROR_CHECK(WSOM_clear_search_criteria (&criteria));
         
    strcpy(criteria.name, form_name);
    strcpy(criteria.class_name, form_type);
   
    ERROR_CHECK(WSOM_search(criteria, &hits, &forms));
    printf("Number of Forms found: %d\n", hits);

	sprintf(cmd, "%s %s %s -uid=", util, login, xml_file);
    for(ii = 0; ii < hits; ii++)
    {
		ERROR_CHECK(AOM_refresh(forms[ii], FALSE));
		ITK__convert_tag_to_uid(forms[ii], &uid);
		strcat(cmd, uid);
		system(cmd);
    }
	if (uid) MEM_free(uid);
    if(forms) MEM_free(forms);
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
