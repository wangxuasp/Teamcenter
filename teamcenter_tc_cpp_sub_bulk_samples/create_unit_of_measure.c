/*HEAD CREATE_UNIT_OF_MEASURE CCC ITK */
#include <stdlib.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/workspaceobject.h>
#include <ae/ae.h>
#include <tccore/item.h>

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
    int
        n_uom,
        ii;
    char
        *name;
    tag_t
        *uoms,
        new_uom;
    
    printf("Before UOM was created...\n");    
    ITK_CALL(UOM_extent(&n_uom, &uoms));
    for (ii = 0; ii < n_uom; ii++)
    {
        ITK_CALL(AOM_ask_name(uoms[ii], &name));
        printf("\t%s\n", name);
        if (name) MEM_free(name);
    }
    if (uoms) MEM_free(uoms);
    
    ITK_CALL(UOM_create("inches", "in", &new_uom));
    ITK_CALL(AOM_save(new_uom));
    
    printf("\nAfter UOM was created...\n");   
    ITK_CALL(UOM_extent(&n_uom, &uoms));
    for (ii = 0; ii < n_uom; ii++)
    {
        ITK_CALL(AOM_ask_name(uoms[ii], &name))
        printf("\t%s\n", name);
        if (name) MEM_free(name);
    }
    if (uoms) MEM_free(uoms);
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
