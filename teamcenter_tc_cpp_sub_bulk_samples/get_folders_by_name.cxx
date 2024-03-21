#include <iostream>
#include <stdio.h>
#include <qry/qry.h>
#include <tc/emh.h>

#include <Error_Exception.hxx>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <mld/logging/TcMainLogger.hxx>
8
using namespace std;
using namespace Teamcenter;

int get_folders_by_name(char *name, int* n_projects, tag_t** projects)
{
	int ifail = ITK_ok;
	try
	{	
		ResultStatus stat;

		tag_t query_tag = NULLTAG;
		stat = QRY_find2("General...", &query_tag);

		int n_entries = 2;
		char *entries[2] = {"Name", "Type"};
		char **values = NULL;
	
		values	= (char **) MEM_alloc(sizeof(char *) * n_entries);
		
		values[0] = (char *)MEM_alloc(strlen(name) + 1);
		strcpy(values[0], name);
		
		values[1] = (char *)MEM_alloc(strlen("Folder") + 1);
		strcpy(values[1], "Folder");		
		
		stat = QRY_execute(query_tag, 1, entries, values, n_projects, projects);
	}
	catch(IFail & ex)
	{
		return ex.ifail();
	}									   
	return ifail;
}
