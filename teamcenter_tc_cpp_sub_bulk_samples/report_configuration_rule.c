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
#include <tccore/tctype.h>
#include <tccore/tc_msg.h>

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
        *rule_name;
    tag_t
        rule,
        window;
        
    ITK_CALL(BOM_create_window (&window));
    ITK_CALL(BOM_ask_window_config_rule(window, &rule));
    ITK_CALL(CFM_ask_rule_text(rule, &rule_name));
    printf("---%s\n", rule_name);
    if (rule_name) MEM_free(rule_name); 
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
