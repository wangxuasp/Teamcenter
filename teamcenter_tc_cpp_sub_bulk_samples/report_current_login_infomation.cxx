
#include <iostream>
#include <sa/group.h>
#include <sa/user.h>
#include <tc/LoggedInUser.hxx>

using namespace Teamcenter;
using namespace Core;
using namespace std;

static void reportCurrentLoginInfomation()
{
    int ifail = ITK_ok;
    LoggedInUser* loggedInUser = LoggedInUser::getInstance();
    
    tag_t currentUserTag = loggedInUser->getCurrentUserTag();
    cout << "    currentUserTag: " << currentUserTag << endl;
    
    char *personName = NULL;
    IFERR_ABORT(SA_ask_user_person_name2(currentUserTag, &personName));
    cout << "    personName: " << personName << endl;
    if(personName) MEM_free(personName);
    
    const char* userID = loggedInUser->getCurrentUserID();
    cout << "    userID: " << userID << endl;

    tag_t currentGroupTag = loggedInUser->getCurrentGroupTag();
    
    char *groupName = NULL;
    IFERR_ABORT(SA_ask_group_name2(currentGroupTag, &groupName));
    cout << "    groupName: " << groupName << endl;
    if(groupName) MEM_free(groupName);

    tag_t currentRoleTag = loggedInUser->getCurrentRoleTag();
    char *roleName = NULL;
    IFERR_REPORT(SA_ask_role_name2(currentRoleTag, &roleName));
    cout << "    roleName: " << roleName << endl;
    if(roleName) MEM_free(roleName);
    
    logical isDba = loggedInUser->isUserSystemAdministrator();
    if(isDba == TRUE) cout << "    User is dba member "<< endl;
    if(isDba == FALSE) cout << "    User is not dba member "<< endl;       
}
