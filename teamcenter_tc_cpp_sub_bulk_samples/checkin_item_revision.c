/*HEAD CHECKIN_ITEM_REVISION CCC ITK */
#include <stdlib.h>
#include <tccore/aom.h>
#include <tc/emh.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <itk/mem.h>
#include <sa/user.h>
#include <res/res_itk.h>

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
        printf("\n Exiting program! \n");
        exit (EXIT_FAILURE);    
    }
}

static void do_it(void)
{
    logical 
        is_checked_out = FALSE;
    tag_t 
        item_revision = NULLTAG, 
        reservation_object = NULLTAG, 
        user = NULLTAG;
    char  
        userid[SA_user_size_c+1] = "";

    ERROR_CHECK(ITEM_find_rev("5913011", "A", &item_revision));
    if (item_revision == NULLTAG)
    {
        printf("Item Revision NOT found!\n");
    }
    else
    {
        ERROR_CHECK(RES_is_checked_out(item_revision, &is_checked_out));
        if (is_checked_out == FALSE)
        {
            printf("Item Revision is NOT checked out!\n");
        }
        else
        {
            ERROR_CHECK(RES_ask_reservation_object(item_revision, 
                &reservation_object));
            ERROR_CHECK(RES_ask_user(reservation_object, &user));
            ERROR_CHECK(SA_ask_user_person_name(user, userid));
            printf("Item Revision is checked out by: %s\n", userid);
            ERROR_CHECK(RES_checkin(item_revision));
            printf("Item Revision has been checked in!\n");
        }
    }   
}

int ITK_user_main(int argc, char* argv[])
{
    int
        status = 0; 
   
    ITK_initialize_text_services( ITK_BATCH_TEXT_MODE );
    status = ITK_auto_login();
    if (  (status != ITK_ok)) printf("\n Login Failed! \n\n");
    else
    {
        printf("\n Login successful! \n\n");
        ITK_set_journalling(TRUE);
        do_it();
    }
    ITK_exit_module(TRUE);
    return status;
}
