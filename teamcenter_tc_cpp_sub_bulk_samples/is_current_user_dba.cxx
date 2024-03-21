
#include <tc/LoggedInUser.hxx>

using namespace std;
using namespace Teamcenter::Core;

static logical isCurrentUserDba()
{
    LoggedInUser* loggedInUser = LoggedInUser::getInstance();

    logical isDba = loggedInUser->isUserSystemAdministrator();
    if(isDba == TRUE) cout << "    User is dba member "<< endl;
    if(isDba == FALSE) cout << "    User is not dba member "<< endl;
	
	return isDba;
}

