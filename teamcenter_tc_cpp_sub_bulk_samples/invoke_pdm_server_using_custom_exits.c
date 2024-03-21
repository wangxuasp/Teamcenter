/************* HEAD INVOKE_PDM_SERVER_USING_CUSTOM_EXITS CCC ITK *************/
/*
Instructions:
    Build the Open API code as usual.

    Save the ITK code as GTAC_invoke_pdm_server.c

    Entry point for a custom exit is the concatenation of the shared
    library name and "_register_callbacks.

    In this example the shared library name is GTAC.
    Therefore the entry point name is GTAC_register_callbacks.

    Compile
        %TC_ROOT%\sample\compile -DIPLIB=libuser_exits GTAC_invoke_pdm_server.c
    Link
        %TC_ROOT%\sample\link_custom_exits GTAC

    Notice: With user exits the object files must be extracted
            or un-archived.  With custom exits this is not necessary.

    Move File
        copy GTAC.dll %TC_BIN%

    Using the Preference Manager in later versions to the shared
    library.

        TC_customization_libraries=
        GTAC

    After logging the syslog should have the following lines to indicate
    that the customization was loaded.

    "Successfully loaded dynamic module C:\TCEG913\bin\GTAC.dll

    Successfully loaded the Custom Library <GTAC>
    Successfully loaded the Entry Point Function Ptr <GTAC_register_callbacks> for Custom Library <GTAC>"

/*****************************************************************************/
/*                         This is the Open API code                         */
/*****************************************************************************
Class or Business Object "Person" is unique in that real properties or atribute names are very cryptic (e.g. "PA1", "PA2", "PA3", etc).
The easiest method for matching real names to display names in to look at %TC_MSG_ROOT%\%TC_LOCALIZATION_DIR%\system_property_names_locale.xml,

In this file the display names are set as follows:
    "PA1" = "Address"
    "PA2" = "City" 
    "PA3" = "State" 
    "PA4" = "Zip Code" 
    "PA5" = "Country" 
    "PA6" = "Organization" 
    "PA7" = "Employee Number" 
    "PA8" = "Internal Mail Code" 
    "PA9" = "E-Mail Address" 
    "PA10" = "Phone Number"



#define GET_PERSONS_EMAIL 203
static void do_it(void)
{
    int
        output_code;

    char 
    attribute_name[32] = "PA8", 
    *output_string = NULL;
    msg[MAX_LINE_SIZE+1] = "", 
            
    UF_CALL(UF_UGMGR_invoke_pdm_server(GET_PERSONS_EMAIL, input_string, &output_code, &output_string));
    sprintf(msg,"person name: %s\n", output_string);
    UF_CALL(UF_UI_write_listing_window(msg));
}


/*****************************************************************************/
/*                         This is the ITK code                              */
/*****************************************************************************/
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
#include <epm/epm.h>
#include <tccore/tctype.h>
#include <tccore/item.h>
#include <ae/dataset.h>
#include <bom/bom.h>
#include <cfm/cfm.h>

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
static int get_latest_released_rev(char *item_id, char *rev_id)
{
    int
        n_revisions = 0,
        ii = 0,
        is_released = 0,
        n_released_revs = 0,
        answer = 0;
    tag_t
        item = NULLTAG,
        *revisions = NULL,
        latest_released_revision = NULLTAG;
    date_t
        latest_date = POM_null_date(),
        compare_date = POM_null_date();


    ERROR_CHECK(ITEM_find_item(item_id, &item));
    if (item == NULLTAG) printf("item not found!\n");
    else
    {
        ERROR_CHECK( ITEM_list_all_revs(item, &n_revisions, &revisions) );
        for (ii = 0; ii < n_revisions; ii++)
        {
            ERROR_CHECK( CR_ask_if_released(revisions[ii], &is_released ));
            if (is_released)
            {
                n_released_revs++;
                if (n_released_revs == 1)
                {
                    latest_released_revision = revisions[ii];
                }
                else
                {
                    ERROR_CHECK( AOM_ask_value_date(latest_released_revision,
                        "date_released", &latest_date) );
                    ERROR_CHECK( AOM_ask_value_date(revisions[ii], "date_released",
                        &compare_date) );
                    ERROR_CHECK( POM_compare_dates(latest_date, compare_date,
                        &answer) );
                    if (answer == -1) latest_released_revision = revisions[ii];
                }
            }
        }
    }
    if (n_released_revs)
    {
        ERROR_CHECK( ITEM_ask_id(item, item_id) );
        ERROR_CHECK( ITEM_ask_rev_id(latest_released_revision, rev_id) );
    }
    if (revisions) MEM_free(revisions);
    return n_released_revs;
}

#define GET_LATEST_RELEASED_REV 206

extern int GTAC_invoke_pdm_server(int *decision, va_list args)
{
    /***********  va_list for USER_invoke_pdm_server ***********/
    int  input_code = va_arg(args, int);          /* args 1 */
    char *input_string = va_arg(args, char *);    /* args 2 */
    int  *output_code = va_arg(args, int *);      /* args 3 */
    char **output_string = va_arg(args, char **); /* args 4 */
    /***********************************************************/

    int
        n_released_revs = 0;
    char
        rev_id[ITEM_id_size_c + 1] = "";

    printf("\t GTAC_invoke_pdm_server \n\n");

    switch (input_code)
    {
        case GET_LATEST_RELEASED_REV:

            *decision = ONLY_CURRENT_CUSTOMIZATION;  //aiw

            n_released_revs = get_latest_released_rev(input_string, rev_id);
            *output_code = n_released_revs;
            *output_string = malloc(sizeof(char) * (strlen(rev_id) + 1));
            strcpy(*output_string, rev_id);
            break;

        default:
            *decision = ALL_CUSTOMIZATIONS;  //aiw
            break;
    }

    return ITK_ok;
}

/* This function name must be <library_name>_register_callbacks */
extern DLLAPI int GTAC_register_callbacks()
{
    printf("\n GTAC_register_callbacks \n\n");

    ERROR_CHECK(CUSTOM_register_exit( "GTAC", "USER_invoke_pdm_server",
        (CUSTOM_EXIT_ftn_t) GTAC_invoke_pdm_server));
    return ITK_ok;
}


