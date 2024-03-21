/*HEAD REPLACE_FORM_STORAGE_CLASS CCC ITK */
#include <stdlib.h>
#include <time.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/tctype.h>
#include <user_exits/user_exits.h>
#include <ae/ae.h>
#include <tccore/aom.h>
#include <ss/ss_const.h>
#include <tc/folder.h>

#define ECHO(X)  printf X; TC_write_syslog X

#define IFERR_ABORT(X)  (report_error( __FILE__, __LINE__, #X, X, TRUE))
#define IFERR_REPORT(X) (report_error( __FILE__, __LINE__, #X, X, FALSE))

static int report_error(char *file, int line, char *call, int status,
    logical exit_on_error)
{
    if (status != ITK_ok)
    {
        int
            n_errors,
            *severities,
            *statuses;
        char
            **messages;

        EMH_ask_errors( &n_errors, &severities, &statuses, &messages );
        ECHO(("\n%s\n", messages[n_errors-1]));
        EMH_clear_errors();

        ECHO(("error %d at line %d in %s\n", status, line, file));
        ECHO(("%s\n", call));

        if (exit_on_error)
        {
            ECHO(("\nExiting program!\n"));
            exit (status);
        }
    }

    return status;
}

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


static tag_t find_folder(void)
{
    int   
        n_found = 0;
    tag_t 
        query = NULLTAG, 
        *folders = NULL,
        the_folder = NULLTAG;
    char
        *entries[2] = {"Name", "Type"},
        *values[2] =  {"ReplaceFormStorageClass", "Folder"};

    IFERR_REPORT(QRY_find("General...", &query));

    IFERR_REPORT(QRY_execute(query, 2, entries, values, &n_found, &folders));
    if (n_found != 1)
    {
        printf("\tFound %d ReplaceFormStorageClass Folder(s)!\n", n_found);
        printf("\tExpecting there to be only one!\n");
        printf("\tDont know what to do...Exiting Program!\n");
        exit (0);
    }
    the_folder = folders[0];
    SAFE_MEM_FREE(folders);
    return the_folder;
}

void ask_object_class_name(tag_t object, char **class_name)
{
    tag_t
        class = NULLTAG;

    IFERR_REPORT(POM_class_of_instance(object, &class));
    IFERR_REPORT(POM_name_of_class(class, class_name)); 
}

static void do_it(void)
{
    int
        n_contents = 0, 
        ii = 0;
    tag_t
        folder = NULLTAG,
        *contents = NULL;
    char 
        *class_name = NULL,
        *id_string = NULL;;

    printf("Finding folder ReplaceFormStorageClass...\n");
    folder = find_folder();
    IFERR_REPORT(FL_ask_references(folder, FL_fsc_by_name, &n_contents, &contents));

    for (ii = 0; ii < n_contents; ii++)
    {
        ask_object_class_name(contents[ii], &class_name);
        if (strcmp("Form", class_name) == 0)
        {
            IFERR_REPORT(WSOM_ask_object_id_string(contents[ii], &id_string));
            printf("\t%s\n", id_string);
            IFERR_REPORT(AOM_refresh(contents[ii], TRUE));
            IFERR_REPORT(FORM_null_pom_instance(contents[ii]));
            IFERR_REPORT(AOM_save(contents[ii]));
            IFERR_REPORT(AOM_refresh(contents[ii], FALSE));
            IFERR_REPORT(AOM_unload(contents[ii]));
            SAFE_MEM_FREE(id_string);
        }
        else printf("Object in folder is not a Form...skipping\n");
        SAFE_MEM_FREE(class_name);
    }
    SAFE_MEM_FREE(contents);
}

int ITK_user_main(int argc, char* argv[])
{
    IFERR_REPORT(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
    IFERR_ABORT(ITK_auto_login());
    IFERR_REPORT(ITK_set_journalling(TRUE));
    do_it();
    IFERR_REPORT(ITK_exit_module(FALSE));
    return ITK_ok;
}
