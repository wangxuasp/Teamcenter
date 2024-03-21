/*
Instructions:

In Teamcenter-> Query builder
 - Create a saved query as follows:
    Name: User's Tasks to Perform
    Modifiable Query Type: Local Query
    Search Class: User
    Display Settings: All Attributes
    Search Criteria:  Name
  
    Log out of Teamcenter
	
In a text editor 
- Add this line to %TC_MSG_ROOT%\en_US\ue_errors.xml
  <error id="2">User %1$ has no tasks to perform.</error>
  
In a Teamcenter Command Prompt  
 - Copy gtac_custom_exits.cxx to W:\

 - Build custom shared library
   cd /d W:\
   set MSDEV_HOME=C:\apps\MVS10\VC
   %TC_ROOT%\sample\compile  -64bit -DIPLIB=none gtac_custom_exits.cxx
   %TC_ROOT%\sample\link_custom_exits -64bit libgtac
   
 - Set TC_customization_libraries to load the custom shared library
   %TC_BIN%\preferences_manager -mode=import -action=merge -preference=TC_customization_libraries -scope=SITE -values=libgtac
   
   
 - Convert Query frpom Local Query to User Exit Query
   %TC_BIN%\tc_set_query_where_run -query="User's Tasks to Perform" -run=user
   Note: To modify or delete the query execute the utility above with –run=iman argument.
   
 - Set TC_USER_LIB environment variable and start Teamcenter.
   set TC_USER_LIB=W:\
   %TC_ROOT%\portal\portal.bat

In Teamcenter-> Query Builder   
 - Notice the Query Type is now User Exit Query.
 
In Teamcenter-> My Teamcenter
- Set up text table in the user interface
  Execute the Query
  In the Search results view select User's Tasks to Peform (top line).
  Select the "Details" tab.
  In the upper right corner select View Menu-> Columns...
  Remove all existing columns.
  Find scroll to find the columns where Type is the query name.
  Add these columns in the desired order
  Apply-> Close.
  
- Execute the query again and you will see the text table.
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sa/am.h>
#include <tc/emh.h>
#include <ict/ict_userservice.h>
#include <pom/pom/pom.h>
#include <property/prop_errors.h>
#include <qry/qry.h>
#include <tc/emh.h>
#include <tc/tc.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/custom.h>
#include <tccore/workspaceobject.h>
#include <user_exits/user_exits.h>
#include <sa/person.h>
#include <sa/user.h>
#include <epm/epm.h>
#include <epm/epm_task_template_itk.h>
#include <ug_va_copy.h>

extern DLLAPI int libgtac_register_callbacks();
extern DLLAPI int results_displayed_in_tc_ui(int *decision, va_list args);
static int get_users_task_to_perform_folder(char *user_name, tag_t *task_to_perform_folder);
extern DLLAPI int user_exit_query_results_to_text_table(int *decision, va_list args);
extern void gtac_make_query_row(int n_columns, QRY_user_query_row_t** row);
static int get_task_information(tag_t task_tag, char task[WSO_name_size_c+1], 
                char state[WSO_name_size_c+1], char parent[WSO_name_size_c+1], 
                char job[WSO_name_size_c+1], char process[WSO_name_size_c+1]);

extern DLLAPI int libgtac_register_callbacks()
{
    printf("\n\n libgtac_register_callbacks \n\n");

    int ifail = ITK_ok;

    /*
        Execute this coomand after query is created
        %TC_BIN%\tc_set_query_where_run -query="User's Tasks to Perform" -run=user
    */
    ifail = CUSTOM_register_exit("libgtac", "USER_query_execute", user_exit_query_results_to_text_table);
    if (ifail != ITK_ok)
    {
        return ifail;
    }

    return ITK_ok;
}

extern DLLAPI int user_exit_query_results_to_text_table(int *decision, va_list args)
{
	*decision = ALL_CUSTOMIZATIONS;
	int ifail = ITK_ok;

    va_list largs;
    va_copy( largs, args );
    char  *qName = va_arg(largs, char*); 
    int   num_args = va_arg (largs, int);
    char  **fields = va_arg (largs, char **); 
    char  **values = va_arg (largs, char **);
    int   *n_columns = va_arg (largs, int*); 
    char  ***col_names = va_arg (largs, char ***); 
	QRY_user_query_row_t **rows = va_arg( largs, QRY_user_query_row_t**);
    va_end( largs ); 

    *n_columns = 0;  
    *rows = NULL; 

	if (strcmp (qName, "User's Tasks to Perform") == 0)
    { 
        char user_name[SA_name_size_c + 1] = ""; 
        strcpy(user_name, values[0]);

        /* Query only has one field/value pair, "User" */
        tag_t task_to_perform_folder = NULLTAG;
        get_users_task_to_perform_folder(user_name, &task_to_perform_folder);  

        if(task_to_perform_folder == NULLTAG )
        {
            ifail = 919002; // custom error in ue_errors.xml
            EMH_store_error_s1(EMH_severity_information, ifail, user_name);
            *rows = NULL; 
            return ifail;
        }
 
        int n_tasks = 0;
        tag_t *tasks = NULL;
        ifail = AOM_ask_value_tags(task_to_perform_folder, "contents", &n_tasks, &tasks);  
        if( (ifail != ITK_ok) || (n_tasks < 1) )
        {
            ifail = 919002; // custom error in ue_errors.xml
            EMH_store_error_s1(EMH_severity_information, ifail, user_name);
            *rows = NULL; 
            return ifail;
        }
 
        /* set column number and column names */
		char names[][32] = {"User", "Task Name", "Task State", "Parent Task", "Job", "Process"}; 
		*n_columns = 6;
		*col_names = (char **) MEM_alloc (*n_columns * sizeof (char *)); 
		for (int ii = 0; ii < *n_columns; ii++)
		{
			(*col_names)[ii] = (char*) MEM_alloc(strlen(names[ii]) + 1);  
			strcpy((*col_names)[ii], names[ii]);
		}
        
        char task[WSO_name_size_c+1] = "";
        char state[WSO_name_size_c+1] = "";
        char parent[WSO_name_size_c+1] = ""; 
        char job[WSO_name_size_c+1] = ""; 
        char process[WSO_name_size_c+1] = "";

		int count = 0;
		QRY_user_query_row_t *prev_row = NULL;
		QRY_user_query_row_t *current_row = NULL;
		QRY_user_query_row_t *first_row = NULL;

		for (int ii = 0; ii < n_tasks; ii++)
		{
			gtac_make_query_row (*n_columns, &current_row); 

            char *task_uid = NULL;
			ITK__convert_tag_to_uid(tasks[ii], &task_uid);
                         
			current_row->object_string = (char*) MEM_alloc(strlen(task_uid) + 1);  
			strcpy(current_row->object_string, task_uid);
            MEM_free(task_uid);

            current_row->values[0] = (char*) MEM_alloc(strlen(user_name) + 1);  
			strcpy(current_row->values[0], user_name);

            get_task_information(tasks[ii], task, state, parent, job, process);

        	current_row->values[1] = (char*) MEM_alloc(strlen(task) + 1);  
			strcpy(current_row->values[1], task);

			current_row->values[2] = (char*) MEM_alloc (strlen(state) + 1 );  
			strcpy(current_row->values[2], state);

            current_row->values[3] = (char*) MEM_alloc(strlen(parent) + 1 );  
			strcpy(current_row->values[3], parent);

			current_row->values[4] = (char*) MEM_alloc (strlen(job) + 1 );  
			strcpy(current_row->values[4], job);

			current_row->values[5] = (char*) MEM_alloc (strlen(process) + 1 );  
			strcpy(current_row->values[5], process);

            if (count == 0)
            {
                first_row = current_row;
                prev_row = current_row;
            }
            else
            {
                prev_row->next = current_row;
                prev_row = current_row;
            }
            count++;
		}
		*rows = first_row;
	
	    /* print text table */
        for(int kk = 0; kk < *n_columns; kk++)
        {
            if (kk == 0)
                printf("%s",  (*col_names)[kk]);
            else 
                printf("\t\t %s",  (*col_names)[kk]);
            
        }
        printf("\n");

		for (QRY_user_query_row_t *row = (*rows); row != 0; row = row->next)
        {
			printf("\n %s \t %s \t %s \t %s \t %s\n", 
				row->values[0], row->values[1], row->values[2], row->values[3], row->values[4]);
		}
	}
	return ifail;	
}

static int get_users_task_to_perform_folder(char *user_name, tag_t *task_to_perform_folder)
{
    int ifail = ITK_ok;
    tag_t type_tag = NULLTAG;
    ifail = TCTYPE_find_type("TasksToPerform", NULL, &type_tag);
    if (ifail != ITK_ok)
    {
        return ifail;
    }

    char *display_name = NULL;
    ifail = TCTYPE_ask_display_name(type_tag, &display_name);
    if (ifail != ITK_ok)
    {
        return ifail;
    }

    tag_t query = NULLTAG;
    ifail = QRY_find("General...", &query);
    if (ifail != ITK_ok)
    {
        return ifail;
    }

    // Find user's "Tasks to Perform" folder
    char *entries[2] = {"Owning User", "Type"};
    char *values[2] =  {user_name, display_name};
    int n_folder = 0;
    tag_t *folder = NULLTAG;
    ifail = QRY_execute(query, 2, entries, values, &n_folder, &folder);
    if (ifail != ITK_ok)
    {
        return ifail;
    }
    
    if(n_folder == 0)
    {
        *task_to_perform_folder = NULLTAG;
        return ifail;
    }

    *task_to_perform_folder = folder[0];   
    MEM_free(folder); 

    return ifail;
}

static int get_task_information(tag_t task_tag, char task[WSO_name_size_c+1], 
                char state[WSO_name_size_c+1], char parent[WSO_name_size_c+1], 
                char job[WSO_name_size_c+1], char process[WSO_name_size_c+1]) 
{
    int ifail = ITK_ok;

    ifail = EPM_ask_name(task_tag, task);
    if (ifail != ITK_ok)
    {
        return ifail;
    }

    tag_t  parent_tag =  NULLTAG;  
    ifail = EPM_ask_parent_task(task_tag, &parent_tag);
    if (ifail != ITK_ok)
    {
        return ifail;
    }

    ifail = EPM_ask_name(parent_tag, parent);
    if (ifail != ITK_ok)
    {
        return ifail;
    }

    EPM_state_t  epm_state;
    ifail = EPM_ask_state(task_tag, &epm_state);
    if (ifail != ITK_ok)
    {
        return ifail;
    }

    ifail = EPM_ask_state_string(epm_state, state);
    if (ifail != ITK_ok)
    {
        return ifail;
    }
 
    tag_t  job_tag =  NULLTAG;  
    ifail = EPM_ask_job(task_tag, &job_tag);
    if (ifail != ITK_ok)
    {
        return ifail;
    }

    ifail = WSOM_ask_name(job_tag, job);
    if (ifail != ITK_ok)
    {
        return ifail;
    }

    tag_t  root_tag =  NULLTAG;  
    ifail = EPM_ask_root_task(task_tag, &root_tag);
    if (ifail != ITK_ok)
    {
        return ifail;
    }

    ifail = EPM_ask_name(root_tag, process);
    if (ifail != ITK_ok)
    {
        return ifail;
    }

    return ifail;
 } 

extern void gtac_make_query_row(int n_columns, QRY_user_query_row_t** row)
{
    *row = (QRY_user_query_row_t *) MEM_alloc (sizeof(QRY_user_query_row_t) + n_columns * sizeof(char *));
    (*row)->next = NULL;
    (*row)->object_string = NULL;
    (*row)->might_map_to_tc = false;
    for (int i = 0; i < n_columns; i++)
    {
        (*row)->values[i] = NULL;
    }
}

#ifdef __cplusplus
}
#endif