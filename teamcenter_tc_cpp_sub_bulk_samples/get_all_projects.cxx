#include <iostream>
#include <stdio.h>
#include <qry/qry.h>
#include <tc/emh.h>

#include <Error_Exception.hxx>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;

int get_all_projects(int* n_projects, tag_t** projects)
{
    int ifail = ITK_ok;
    try
    {   
        ResultStatus stat;

        tag_t query_tag = NULLTAG;
        stat = QRY_find2("Projects...", &query_tag);

        char *entries[1] = {"Project ID"};
        char *values[1] = {"*"};
        stat = QRY_execute(query_tag, 1, entries, values, n_projects, projects);
    }
    catch(IFail & ex)
    {
        return ex.ifail();
    }                                      
    return ifail;
}
