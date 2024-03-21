#include <iostream>
#include <tc/tc.h>
#include <tc/tc_startup.h>

#include <epm/epm.h>
#include <tccore/item.h>

#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;
using Teamcenter::Main::logger;

#define RESP_PARTY  "ProposedResponsibleParty"  
#define REVIEWER    "ProposedReviewer"  

int create_participants(tag_t rev_tag, tag_t group, tag_t role)
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
        int ifail = ITK_ok;
        ResultStatus stat;
        try
        {
            tag_t current_member = NULLTAG;
            stat = SA_ask_current_groupmember(&current_member);
            
            tag_t current_user = NULLTAG;
            stat = SA_ask_groupmember_user(current_member, &current_user);
            
            tag_t group_pool = NULLTAG;
            stat = EPM_get_resource_pool(group, NULLTAG, FALSE, &group_pool);

            tag_t role_pool = NULLTAG;
            stat = EPM_get_resource_pool(NULLTAG, role, FALSE, &role_pool);
                
            tag_t resp_party;
            stat = EPM_get_participanttype(RESP_PARTY, &resp_party);
     
            tag_t reviewer;
            stat = EPM_get_participanttype(REVIEWER, &reviewer);        

            int n_assignees = 3;
            tag_t assignees[3] = {NULLTAG, NULLTAG, NULLTAG};
            assignees[0] = current_member;
            assignees[1] = group_pool;
            assignees[2] = role_pool;
            
            int n_participant_types = 3;
            tag_t participant_types[3] = {NULLTAG, NULLTAG, NULLTAG};
            participant_types[0] = resp_party;
            participant_types[1] = reviewer; 
            participant_types[2] = reviewer;
        
            int n_participants = 0;
            tag_t *participants = NULL;     
            stat = EPM_create_participants(n_assignees, assignees, 
                participant_types, &n_participants, &participants);

            stat = ITEM_rev_set_participants(rev, n_participants, participants);
        }
        catch (const IFail &ex)
        {
            logger()->error(ex.ifail(), ex.getMessage());
            cout << ex.getMessage() << endl;
            cout << "error " << ex.ifail() << endl;
        }
        return ifail;
    }
    catch (const IFail &ex)
    {
        logger()->error(ex.ifail(), ex.getMessage());
        cout << ex.getMessage() << endl;
        cout << "error " << ex.ifail() << endl;
    }

    return ifail;
}
