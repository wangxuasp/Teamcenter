/*HEAD REMOVE_CONTENTS_OF_A_ECM_FOLDER CCC ITK */
#include <stdlib.h>
#include <tccore/aom.h>
#include <tc/emh.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <itk/mem.h>

#define EXIT_FAILURE 1 
#define ERROR_CHECK(X) (report_error( __FILE__, __LINE__, #X, (X)))
static void report_error( char *file, int line, char *function, int return_code)
{
    if (return_code != ITK_ok)
    {
        char *error_message_string;
        EMH_get_error_string (NULLTAG, return_code, &error_message_string);
        printf ("ERROR: %d ERROR MSG: %s.\n", return_code, error_message_string);
        printf ("FUNCTION: %s\nFILE: %s LINE: %d\n", function, file, line);
        if(error_message_string) MEM_free(error_message_string);
        printf("\nExiting program!\n");
        exit (EXIT_FAILURE);    
    }
}

#define EXIT_IF_NULL(X) (check_value(#X, (X)))
static void check_value( char *function, int value )
{
    if ((value == 0) || (value == 0))
    {
        printf ("\t%s is NULL\n", function);
        printf("\nExiting program!\n");
        exit (EXIT_FAILURE);    
    }
}


static void do_it(void)
{
    int
        n_objects = 0,
        ii = 0;
    tag_t
        *objects = NULL,
        ec_rev = NULLTAG;

    ERROR_CHECK( ECM_find_ec_revision("CR0007", "B", &ec_rev) );
    EXIT_IF_NULL( ec_rev );

    ERROR_CHECK( ECM_get_contents(ec_rev, "Affected Items", &n_objects, 
        &objects)  );
    EXIT_IF_NULL( n_objects );

    ERROR_CHECK( ECM_remove_contents(ec_rev, "Affected Items", n_objects, 
        objects) );

    if (objects) MEM_free(objects);
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
