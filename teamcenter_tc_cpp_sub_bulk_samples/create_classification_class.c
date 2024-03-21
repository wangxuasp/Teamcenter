/*HEAD CREATE_CLASSIFICATION_CLASS CCC ITK */
#include <stdlib.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/workspaceobject.h>
#include <tccore/item.h>
#include <ics/ics.h>
#include <ics/ics2.h>

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

#define SAFE_MEM_FREE( a )   \
do                          \
{                           \
    if ( (a) != NULL )      \
    {                       \
        MEM_free( (a) );    \
        (a) = NULL;         \
    }                       \
}                           \
while ( 0 )

static void do_it(void)
{
    int 
        ids[2] = {0,0};
    char 
        *class_id = NULL;

    ERROR_CHECK(ICS_attribute_create_unique("gtac_attr_1", 32, &ids[0]));
    ERROR_CHECK(ICS_attribute_create_unique("gtac_attr_2", 32, &ids[1]));

    ERROR_CHECK(ICS_class_create_unique("ICM", "GtacClass", 
        CLASS_flag_isStorrageClass, 2, ids, &class_id));    
    ERROR_CHECK(ICS_class_set_property(class_id, "UNITSYSTEM" , "false"));
    ERROR_CHECK(ICS_class_set_property(class_id, "ABSTRACT" , "false"));
    ERROR_CHECK(ICS_class_set_property(class_id, "ISASSEMBLY" , "false"));
    ERROR_CHECK(ICS_class_set_property(class_id, "MULTIINST" , "0"));
    ERROR_CHECK(ICS_view_create(class_id, "defaultView",
        VIEW_type_defaultView, "defaultView", 2, ids));
    SAFE_MEM_FREE(class_id);
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
