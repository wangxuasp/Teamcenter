/* HEAD SET_LOV_STRING_VALUES ITK CCC */
#include <stdlib.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <lov/lov.h>

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
    tag_t
        lov;
    int 
		number_of_values = 4;
    char 
		*values[4]  = {"Option 1", "Option 2", "Option 3", "Option 4"};
    
    ITK_CALL(LOV_create("DMB4",  "DMB4", "ListOfValuesString", PROP_string,
        LOV_Exhaustive_list, &lov));
    ITK_CALL(AOM_refresh(lov, TRUE));
    ITK_CALL(LOV_set_values_string(lov, number_of_values, values));
    ITK_CALL(LOV_save(lov));
    ITK_CALL(AOM_refresh(lov, FALSE)); 
}

int ITK_user_main(int argc, char* argv[])
{
    int
        status = 0; 
   
    ITK_initialize_text_services( ITK_BATCH_TEXT_MODE );
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
