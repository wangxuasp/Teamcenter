
#include <sa/user.h>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>

using namespace std;

int remove_all_mailbox_contents(char *user_id)
{
    int ifail = ITK_ok;
    ResultStatus rstat;
    try
    {
        tag_t user = NULLTAG;
        rstat = SA_find_user(user_id, &user);
        if (user == NULLTAG)
        {
            printf("\n   User %s NOT found! \n   Exiting program! \n", user_id);
            exit(0);
        }
        
        tag_t mailbox = NULLTAG;
        rstat = SA_ask_user_mailbox(user, &mailbox);

        if (mailbox != NULLTAG)
        {
            tag_t attr = NULLTAG;
            rstat = POM_attr_id_of_attr( "contents", "Folder", &attr);
            rstat = POM_refresh_instances_any_class(1, &mailbox, POM_modify_lock);
            rstat = POM_clear_attr(1, &mailbox, attr);
            
            logical unload = true; 
            rstat = POM_save_instances(1, &mailbox, unload);            
        }   
    }
    catch( const IFail &e )
    {
        cout << "error " << e.ifail() << endl;
        cout << e.getMessage() << endl; 
    }
    return ifail;
}
 