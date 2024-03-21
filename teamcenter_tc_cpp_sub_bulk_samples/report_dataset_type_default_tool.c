#include <stdlib.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/workspaceobject.h>
#include <tccore/item.h>
#include <ae/datasettype.h>

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
    tag_t tool_tag = NULLTAG;
    char *tool_name = NULL;
    char symbol_name[AE_symbol_size_c + 1] = "";

    ITK_CALL(AE_ask_dataset_tool(dataset_tag, &tool_tag));
    ITK_CALL(AOM_ask_value_string(tool_tag, "object_name", &tool_name));
    ITK_CALL(AE_ask_tool_symbol_name(tool_tag, symbol_name));
    printf("\n  Default Tool: %s - %s \n", tool_name, symbol_name); 
}

int ITK_user_main(int argc, char* argv[])
{
    int
        status = 0; 
   
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
