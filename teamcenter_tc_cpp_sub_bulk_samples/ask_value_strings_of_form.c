/*HEAD ASK_VALUE_STRINGS_OF_FORM CCC ITK */
#include <stdlib.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/workspaceobject.h>
#include <tccore/item.h>
#include <tccore/workspaceobject.h>
#include <property/prop.h>
#include <form/form.h>

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

static void do_it(void)
{
    WSO_search_criteria_t
        criteria;
    int
        hits = 0,
        ii = 0,
        kk = 0,
        num;
    char
        name[WSO_name_size_c + 1] = "5033271",
        **values = NULL;
    tag_t
        *forms = NULL,
        pom_instance = NULLTAG;

    ERROR_CHECK(WSOM_clear_search_criteria (&criteria));
    strcpy(criteria.name, name);

    ERROR_CHECK(WSOM_search(criteria, &hits, &forms));
    printf("Number of Forms found: %d\n", hits);
    for(ii = 0; ii < hits; ii++)
    {
        ERROR_CHECK(FORM_ask_pom_instance(forms[ii], &pom_instance));
        ERROR_CHECK(AOM_ask_value_strings(pom_instance, "NameList" ,
            &num, &values));
        for (kk = 0; kk < num; kk++)
        {
             printf("\t%s\n",values[kk]);
             MEM_free(values[kk]);  /* validated as necessary aiw 11/8/07 */
        }
        if(values) MEM_free(values);
    }
    if(forms) MEM_free(forms);
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
