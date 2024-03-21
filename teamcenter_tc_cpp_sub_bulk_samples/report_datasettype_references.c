/*HEAD REPORT_DATASETTYPE_REFERENCES CCC ITK */
#include <stdlib.h>
#include <tccore/aom.h>
#include <ae/datasettype.h>
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
        ref_count = 0, 
        ii = 0, 
        file_refs_count = 0, 
        kk = 0;
    tag_t
        datasettype = NULLTAG;
    char**       
        ref_list = NULL,
        **ref_templates = NULL, 
        **ref_formats = NULL;

    ERROR_CHECK( AE_find_datasettype("UGMASTER", &datasettype) );
    EXIT_IF_NULL( datasettype );

    ERROR_CHECK( AE_ask_datasettype_refs(datasettype, &ref_count, &ref_list) );
    EXIT_IF_NULL( ref_count );
    
    for (ii = 0; ii < ref_count; ii++)
    {
        printf("%s\n", ref_list[ii]);

        ERROR_CHECK( AE_ask_datasettype_file_refs(datasettype, ref_list[ii], 
            &file_refs_count, &ref_templates, &ref_formats) );
        EXIT_IF_NULL( file_refs_count );

        for (kk = 0; kk < file_refs_count; kk++)
        {
            printf("\t%s\t\t%s\n", ref_templates[kk], ref_formats[kk]);
            if (ref_templates[kk]) MEM_free(ref_templates[kk]);
            if (ref_formats[kk]) MEM_free(ref_formats[kk]);
        }
        if (ref_templates) MEM_free(ref_templates);
        if (ref_formats) MEM_free(ref_formats);

        if (ref_list[ii]) MEM_free(ref_list[ii]);
    }
    if (ref_list) MEM_free(ref_list);

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
