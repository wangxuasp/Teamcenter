
#include <iostream>
#include <stdio.h>
#include <base_utils/Mem.h>
#include <tc/emh.h>
#include <pom/pom/pom.h>
#include <tccore/project.h>

#include <Error_Exception.hxx>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;

static int remove_only_folder_from_project(char *project_id, tag_t folder)
{
	int ifail = ITK_ok;
	try
	{	
		ResultStatus stat;

		tag_t attr_tag = NULLTAG;
		stat = POM_attr_id_of_attr("project_list", "Folder", &attr_tag);

		stat = POM_refresh_instances_any_class(1, &folder, POM_modify_lock);

		int n_values = 0;
		stat = POM_length_of_attr(folder, attr_tag, &n_values);
		printf("\n n_values: %d \n", n_values);
		
		tag_t*	 values = NULL;
		logical* is_null = false;
		logical* is_empty = false;		
		stat = POM_ask_attr_tags(folder, attr_tag, 0, n_values, &values, &is_null, &is_empty);		
		for (int ii =  0; ii < n_values; ii++)
		{
			char* name = NULL;
			stat = PROJ_ask_name2(values[ii], &name);
			if (strcmp(name, project_id) == 0)
			{
				stat = POM_remove_from_attr(1, &folder, attr_tag, ii, 1 );
			}
		}
		stat = POM_save_instances(1, &folder, true);
		stat = POM_refresh_instances_any_class(1, &folder, POM_modify_lock);	
	}
	catch(IFail & ex)
	{
		return ex.ifail();
	}
	return ifail;
}
