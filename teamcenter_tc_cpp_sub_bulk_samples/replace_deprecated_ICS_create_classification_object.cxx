/*
    Code to replace:
    #define ICS_view_not_found        ( EMH_ICS_error_base + 24 )
    ....
    stat = ICS_find_class("TcCls_UDC", &tClass);

    ifail = ICS_find_view(tClass, "defaultView", &tView);
    if (ifail == ICS_view_not_found) tView = tClass;

    stat = ICS_create_classification_object(tWso, "8668905", tView, &tIco);
    stat = ICS_classify_wsobject(tWso, tIco);
*/
#include <iostream>
#include <ics/ics.h>
#include <ics/ics2.h>
#include <sml/sml_errors.h>
#include <tc/tc.h>
#include <tc/emh.h>

#include <Error_Exception.hxx>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <metaframework/BusinessObjectRef.hxx>

using namespace std;
using namespace Teamcenter;

static int replace_deprecated_ICS_create_classification_object(tag_t tWso)
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
        tag_t tIco = NULLTAG;
        stat = ICS_ico_create(NULL, tWso, "TcCls_UDC", 0, 0, 0, &tIco );
        
        logical verdict = FALSE;
        stat = ICS_is_wsobject_classified(tWso, &verdict);
        
        if (verdict == TRUE) 
            printf("\n\t WorkspaceObject is classified\n\n");
            
        if (verdict == FALSE)
            printf("\n\t WorkspaceObject is NOT classified\n\n");
    }
    catch( const IFail &e )
    {
        cout << "error " << e.ifail() << endl;
        cout << e.getMessage() << endl;
    }
    return ifail;
}

