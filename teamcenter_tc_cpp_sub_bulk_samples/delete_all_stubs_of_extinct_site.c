/*HEAD DELETE_ALL_STUBS_OF_EXTINCT_SITE CCC ITK */
#include <stdlib.h>
#include <tc/tc.h>
#include <ss/ss_const.h>
#include <pom/pom/pom.h>
#include <sa/site.h>

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

static void GTAC_free(void *what)
{
    if (what != NULL)
    {
        MEM_free(what);
        what = NULL;
    }
}

static void do_it(void)
{
    void    
        ***report;
    tag_t   
        site = NULLTAG,
        *delete_list = NULL;
    int
        rows = 0, 
        columns = 0, 
        ii = 0;
    char
        extinct_site_name[] = "tceg91msc1_odsidsm",
        *select_attrs[1] = {"puid"};

    ERROR_CHECK( SA_find_site(extinct_site_name, &site));
    EXIT_IF_NULL(site);
    
    ERROR_CHECK(POM_enquiry_create ("enquiry"));

    ERROR_CHECK(POM_enquiry_add_select_attrs("enquiry", "POM_stub", 
        1, select_attrs));

    ERROR_CHECK(POM_enquiry_set_tag_value("enquiry", "value", 1, &site, 
        POM_enquiry_bind_value));

    ERROR_CHECK(POM_enquiry_set_attr_expr ("enquiry", "expression", "POM_stub",
        "definitive",  POM_enquiry_equal , "value"));

    ERROR_CHECK(POM_enquiry_set_where_expr ( "enquiry", "expression" ));
    ERROR_CHECK(POM_enquiry_execute ("enquiry", &rows, &columns, &report));
    printf("\nEnquiry found %d stubs\n", rows);

    if (rows != 0)
    {
        delete_list = (tag_t *) MEM_alloc ( rows * sizeof(tag_t));

        for (ii = 0; ii < rows; ii++)
            delete_list[ii] = *((tag_t *)report[ii][0]);

        printf("\nDeleting Stubs!\n");
        ERROR_CHECK(POM_delete_instances(rows, delete_list));
        
        ERROR_CHECK(POM_enquiry_execute ("enquiry", &rows, &columns, &report));
        printf("Second enquiry found %d stubs\n", rows);

    }
    GTAC_free(report);
    GTAC_free(delete_list);
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
