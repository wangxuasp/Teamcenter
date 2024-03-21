#include <iostream>
#include <pom/pom/pom.h>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <base_utils/ScopedSmPtr.hxx>

using namespace std;
using namespace Teamcenter;

static logical instance_has_modify_lock(tag_t object_tag)
{
    int ifail = ITK_ok;
    ResultStatus rstat;
    logical has_modify_lock = false;
    try
    {
        scoped_smptr<char> user_name; 
        date_t login_date = NULLDATE;  
        scoped_smptr<char> node_name;       
        ifail = POM_who_locked_instance (object_tag, &user_name, 
            &login_date, &node_name);
        if (ifail == ITK_ok)
        {
            char msg[] = "   Locked by: ";
            cout << endl << msg <<  user_name.get()<< " - " << node_name.get() << endl;
            has_modify_lock = true;
        }
        if (ifail == POM_inst_not_in_use) printf("\n  Instance is not locked. \n");   
    }
    catch( const IFail &e )
    {
        cout << "error " << e.ifail() << endl;
        cout << e.getMessage() << endl; 
    }
    return has_modify_lock;
}
