
#include <iostream>
#include <ics/ics.h>
#include <ics/ics2.h>
#include <tc/tc.h>
#include <tc/emh.h>

#include <Error_Exception.hxx>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <metaframework/BusinessObjectRef.hxx>

using namespace std;
using namespace Teamcenter;

static int decribe_classification_class(tag_t tClass)
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {       
        char* cpId = NULL;
        char* cpName = NULL; 
        stat = ICS_ask_id_name(tClass, &cpId, &cpName);
       
        int type   = 0;
        int nInsts = 0;
        int nChild = 0;
        int nView  = 0;
        int opt = 0;

        scoped_smptr<char> name(0);
        scoped_smptr<char> desc(0);
        scoped_smptr<char> udata1(0);
        scoped_smptr<char> udata2(0);
        scoped_smptr<char> sname(0);
        scoped_smptr<char> parent(0);

        stat = ICS_class_describe( cpId, &parent, &type, &name, &sname, &desc, 
                &udata1, &udata2, &opt, &nInsts, &nChild, &nView );

        cout << "\t Class ID: " << cpId << endl;
        cout << "\t Class Name: " << name.getString() << endl;
        cout << "\t Short Name: " << sname.getString() << endl;
        cout << "\t Parent: " << parent.getString() << endl;
        cout << "\t Device Type: " << type << endl;
        cout << "\t Description: " << desc.getString() << endl;     
        cout << "\t User Data 1: " << udata1.getString() << endl;
        cout << "\t User Data 2: " << udata2.getString() << endl;
        cout << "\t Option: " << opt << endl;
        cout << "\t Instance Count: " << nInsts << endl;
        cout << "\t Child Count: " << nChild << endl;
        cout << "\t View Count: " << nView << endl;
    }
    catch( const IFail &e )
    {
        cout << "error " << e.ifail() << endl;
        cout << e.getMessage() << endl;
    }
    return ifail;
}
