
#include <sa/groupmember.h>
#include <sa/user.h>
#include <tccore/aom_prop.h>
#include <tccore/project.h>

#include <base_utils/IFail.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <base_utils/TcResultStatus.hxx>

using namespace std;
using namespace Teamcenter;

static int describe_project_team(tag_t project)
{
    int ifail = ITK_ok;
    ResultStatus rstat;

    try
    {
        int n_members = 0;
        scoped_smptr<tag_t> members;

        int n_admins = 0;
        scoped_smptr<tag_t> admins;
                        
        int n_priv_users = 1;
        scoped_smptr<tag_t> priv_users;
        
        rstat = PROJ_ask_team(project, &n_members, &members, &n_admins, &admins, 
                    &n_priv_users, &priv_users);
        cout << endl << "  n_members: " << n_members << endl;
        scoped_smptr<char> user_name;
        for (int ii = 0; ii < n_members; ii++)
        {
            rstat = AOM_ask_value_string(members[ii], "user_name", &user_name);
            cout << "\t" << user_name.get() << endl;
        }
        
        cout << endl << "  n_admins: " << n_admins << endl;
        for (int ii = 0; ii < n_admins; ii++)
        {
            rstat = SA_ask_user_identifier2(admins[ii], &user_name);
            cout << "\t" << user_name.get() << endl;
        }
        
        cout << endl << "  n_priv_users: " << n_priv_users << endl;
        for (int ii = 0; ii < n_priv_users; ii++)
        {
            rstat = SA_ask_user_identifier2(priv_users[ii], &user_name);
            cout << "\t" << user_name.get() << endl;
        }
        
    }
    catch( const IFail &e )
    {
        cout << "error " << e.ifail() << endl;
        cout << e.getMessage() << endl; 
    }

    return ifail;   
}

