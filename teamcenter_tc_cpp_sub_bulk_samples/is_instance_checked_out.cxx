
#include <iostream>
#include <res/res_itk.h>
#include <sa/user.h>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <base_utils/ScopedSmPtr.hxx>

using namespace std;
using namespace Teamcenter;

static logical is_instance_checked_out(tag_t object_tag)
{
    int ifail = ITK_ok;
    ResultStatus rstat;
    logical is_checked_out = false;
    try
    {
		rstat = RES_is_checked_out(object_tag, &is_checked_out);
		if (is_checked_out == true)
		{
			tag_t user_tag = NULLTAG;
			tag_t group_tag = NULLTAG;
			rstat = RES_who_checked_object_out(object_tag, &user_tag, &group_tag); 

			scoped_smptr<char> user_id; 
			rstat = SA_ask_user_identifier2(user_tag, &user_id);

            char msg[] = "   Checked out by: ";
            cout << endl << msg <<  user_id.get() << endl;
            is_checked_out = true;
		}
    }
    catch( const IFail &e )
    {
        cout << "error " << e.ifail() << endl;
        cout << e.getMessage() << endl;
    }
    return is_checked_out;
}
