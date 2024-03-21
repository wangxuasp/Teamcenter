/*HEAD GET_LATEST_DATASET_VERSION_BY_DATASET_NAME CCC ITK */
/*
     %TC_ROOT%\sample\compile -DIPLIB=none get_latest_dataset_version_by_dataset_name.c
     %TC_ROOT%\sample\linkitk -o get_latest_dataset_version_by_dataset_name get_latest_dataset_version_by_dataset_name.obj
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <tccore/aom.h>
#include <tc/emh.h>
#include <ict/ict_userservice.h>
#include <tc/tc.h>
#include <tccore/tctype.h>
#include <sa/tcfile.h>
#include <ss/ss_errors.h>
#include <pom/enq/enq.h>
#include <tccore/workspaceobject.h>

#define ECHO(X)  printf X; TC_write_syslog X

#define IFERR_ABORT(X)  (report_error( __FILE__, __LINE__, #X, X, TRUE))
#define IFERR_REPORT(X) (report_error( __FILE__, __LINE__, #X, X, FALSE))
#define IFERR_RETURN(X) if (IFERR_REPORT(X)) return

static int report_error(char *file, int line, char *call, int status,
    logical exit_on_error)
{
    if (status != ITK_ok)
    {
        int
            n_errors = 0,
            *severities = NULL,
            *statuses = NULL;
        char
            **messages;

        EMH_ask_errors( &n_errors, &severities, &statuses, &messages );
        if (n_errors > 0)
        {
            ECHO(("\n%s\n", messages[n_errors-1]));
            EMH_clear_errors();
        }
        else
        {
            char *error_message_string;
            EMH_get_error_string (NULLTAG, status, &error_message_string);
            ECHO(("\n%s\n", error_message_string));
        }

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

static void do_it(void)
{
    IFERR_REPORT(POM_enquiry_create("enqid"));

    // First find all dataset all dataset 
    const char * enq_select_attrs[] = {"puid"};
    IFERR_REPORT(POM_enquiry_add_select_attrs("enqid", "Dataset", 1, enq_select_attrs));

    char name[WSO_name_size_c + 1] = "000202/00";
    IFERR_REPORT(POM_enquiry_set_string_expr("enqid", "exp1", "Dataset", "object_name", POM_enquiry_equal, name));
    IFERR_REPORT(POM_enquiry_set_where_expr ("enqid", "exp1"));

    // create a sub-enquiry to filter just latest datasets
    IFERR_REPORT(POM_enquiry_set_sub_enquiry("enqid", "sub_enqid"));
    IFERR_REPORT(POM_enquiry_set_pseudo_calias("sub_enqid", "RevisionAnchor", "revisions", "class_revisions"));

    const char *subenq_select_attrs_1[] = {"pval"}; 
    IFERR_REPORT(POM_enquiry_add_select_attrs("sub_enqid", "class_revisions", 1, subenq_select_attrs_1));

    int int_val = 0;
    IFERR_REPORT(POM_enquiry_set_int_value("sub_enqid", "sub_value_1", 1, &int_val, POM_enquiry_const_value));
    IFERR_REPORT(POM_enquiry_set_attr_expr("sub_enqid","sub_exp_1", "class_revisions", "pseq", POM_enquiry_equal, "sub_value_1"));
    IFERR_REPORT(POM_enquiry_set_where_expr( "sub_enqid","sub_exp_1"));
    IFERR_REPORT(POM_enquiry_set_setexpr ("enqid", "sub_exp_1", POM_enquiry_intersection, "sub_enqid"));

    void ***report = NULL;
    int rows = 0, columns = 0;
    IFERR_REPORT(POM_enquiry_execute("enqid", &rows, &columns, &report));
    if (rows != 0)
    {
        tag_t *ds_list = NULL;
        ds_list = (tag_t *) MEM_alloc ( rows * sizeof(tag_t));
        for (int ii = 0; ii < rows; ii++) 
        {
            ds_list[ii] = *((tag_t *)report[ii][0]);

            char *id_string = NULL;
            IFERR_REPORT(WSOM_ask_object_id_string( ds_list[ii], &id_string));

            char *uid = NULL;
            ITK__convert_tag_to_uid(ds_list[ii], &uid);

            printf("    %s (%s)\n", id_string, uid );

            MEM_free(uid);
            MEM_free(id_string);
        }
         GTAC_free(ds_list);
    }
    GTAC_free(report);
    IFERR_REPORT(POM_enquiry_delete ("enqid"));
}

int ITK_user_main(int argc, char* argv[])
{
    IFERR_REPORT(ITK_initialize_text_services(ITK_BATCH_TEXT_MODE));
    IFERR_ABORT(ITK_auto_login());
    IFERR_REPORT(ITK_set_journalling(TRUE));

    do_it();

    IFERR_REPORT(ITK_exit_module(FALSE));

    return ITK_ok;
}

#ifdef __cplusplus
}
#endif
