/*HEAD SET_GROUP_NAME CCC ITK */
#include <stdlib.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/workspaceobject.h>
#include <ae/ae.h>
#include <user_exits/user_exits.h>
#include <ss/ss_const.h>
#include <tccore/item.h>
#include <pom/pom/pom.h>
#include <ae/dataset.h>
#include <sa/sa.h>

#define ITK_CALL(X) (report_error( __FILE__, __LINE__, #X, (X)))

static void report_error( char *file, int line, char *function, int return_code)
{
    if (return_code != ITK_ok)
    {
        int
            ii,
            n_errors,
            *severities,
            *statuses;
        char
            **messages;

        printf("\n");

        EMH_ask_errors( &n_errors, &severities, &statuses, &messages );
        for (ii = 0; ii < n_errors; ii++) printf( "%s\n", messages[ii] );
        EMH_clear_errors();

        printf( "error %d at line %d in %s\n", return_code, line, file);
        printf( "%s\n", function);
        printf( "\nExiting program!\n");

        exit (!ITK_ok);
    }
}

static void do_it(void)
{
    char
        group_name[SA_name_size_c + 1],
        new_group_name[SA_name_size_c + 1];
    tag_t
        group_tag;

    printf("\n\nEnter Group Name: ");
    gets(group_name);

    printf("\n\nEnter New Group Name: ");
    gets(new_group_name);

    ITK_CALL(SA_init_module());
    ITK_CALL(SA_find_group(group_name, &group_tag));

    ITK_CALL(AOM_load(group_tag));
    ITK_CALL(AOM_refresh(group_tag, 1));

    ITK_CALL(SA_ask_group_name(group_tag, group_name));
    printf("Before - %s\n", group_name);

    ITK_CALL(SA_set_group_name(group_tag, new_group_name));
    ITK_CALL(AOM_save(group_tag));
    ITK_CALL(AOM_refresh(group_tag,0));

    ITK_CALL(SA_ask_group_name(group_tag, group_name));
    printf("After - %s\n", group_name);
    ITK_CALL(SA_exit_module());
}

int ITK_user_main(int argc, char* argv[])
{
    ITK_CALL(ITK_initialize_text_services( ITK_BATCH_TEXT_MODE ));
    ITK_CALL(ITK_auto_login());
    ITK_CALL(ITK_set_journalling(TRUE));
    do_it();
    ITK_exit_module(TRUE);
    return ITK_ok;
}
