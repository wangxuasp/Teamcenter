/*HEAD CREATE_FORM_AND_SET_STRING_ARRAY_VALUES CCC ITK */
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
        numInArray = 0,
        ii = 0;
    tag_t
        form = NULLTAG;
    char
        name[132] = "",
        **pointerToArrayOfStringPointers = NULL;

    printf("---------------- Building the string array -------------\n\n");
    
    printf("Press 'Enter' without value to indicate the list is complete.\n");
    
    while( printf("\n    Enter Name: ") && gets(name) && strlen(name) )
    {
        numInArray++;

        /* allocate memory for the first string pointer */ 
        if ( numInArray == 1)
        {
            pointerToArrayOfStringPointers = 
                (char **) MEM_alloc(sizeof(char *) * numInArray);
        }
        else
        {
            /* allocate memory for the additional string pointer */ 
            pointerToArrayOfStringPointers = (char **)MEM_realloc(
                pointerToArrayOfStringPointers, sizeof(char *) * numInArray);
        }

        /* allocate memory for array elements */        
        pointerToArrayOfStringPointers[numInArray - 1] =  
            (char *) MEM_alloc(sizeof(char) * (strlen(name) + 1));

        /* assign value to array element */
        strcpy(pointerToArrayOfStringPointers[numInArray - 1], name); 
    }
    
    /* only create form if at least one name was entered */
    if (numInArray)
    {
        printf("\n--------------- Creating the Form -------------------\n\n");

        ERROR_CHECK( FORM_create("MyFormName", "", "MyFormType", &form) ); 

        ERROR_CHECK( AOM_set_value_strings(form, "ListOfNames", numInArray, 
            pointerToArrayOfStringPointers) );
        ERROR_CHECK( AOM_save(form) );
        ERROR_CHECK( AOM_unload(form) );

        printf("\n--------------- Freeing the allocated memory ---------\n\n");
        
        for (ii = 0; ii < numInArray; ii++)
            MEM_free(pointerToArrayOfStringPointers[ii]);
        MEM_free(pointerToArrayOfStringPointers);
    }


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
