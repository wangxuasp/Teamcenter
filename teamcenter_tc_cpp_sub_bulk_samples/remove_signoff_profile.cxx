
#include <epm/signoff.h>
#include <sa/groupmember.h>
#include <tccore/aom_prop.h>

#include <iostream>
#include <Error_Exception.hxx>
#include <base_utils/IFail.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;
using Teamcenter::Main::logger;

int remove_signoff_profile(tag_t review_task)
{
    int ifail = ITK_ok;
    ResultStatus rstat;
    try
    {
        int n_tasks = 0;
        scoped_smptr<tag_t> tasks;
        rstat = AOM_ask_value_tags(review_task, "child_tasks", &n_tasks,&tasks);
        for (int ii = 0; ii < n_tasks; ii++)
        {
            scoped_smptr<char> task_type;
            rstat = AOM_UIF_ask_value(tasks[ii], "object_type", &task_type);
            if (strcmp(task_type.getString(), "Select Signoff Team Task") == 0)
            {
                tag_t sst_task = tasks[ii];

                EPM_state_t state;
                rstat = EPM_ask_state(sst_task, &state);
                if (state == EPM_completed)
                {
                    ifail = EPM_already_completed;
                    cout << "Cannot remove signoff profile because the"; 
                    cout << " select-signoff-team task is complete." << endl;
                }
                else
                {
                    int n_signoff = 0;
                    scoped_smptr<tag_t> signoffs;
                    rstat = EPM_ask_attachments(sst_task, EPM_signoff_attachment, 
                        &n_signoff, &signoffs);                                     
                    if (n_signoff == 0)
                    {
                        cout << "No signoff profiles exist!" << endl;
                    }
                    else
                    {
                        tag_t member = NULLTAG;
                        SIGNOFF_TYPE_t type; 
                        rstat = EPM_ask_signoff_member(signoffs[0], &member, &type);
                                                
                        tag_t groupmembers[1] = {member};
                        rstat = EPM_remove_signoffs(review_task, 1, &groupmembers[0]);
                    }
                }
            }
        }
    }
    catch( const IFail &e )
    {
        ifail = e.ifail();
        /*
            EPM_signoff_profile_not_staffed is returned like an error 
            but is really only information.
        */
        if (ifail == EPM_signoff_profile_not_staffed)
        {
            cout << "INFORMATION: The required staffing for the signoff";
            cout << " profiles are not complete." << endl;
        }
        else
        {
            cout << "error " << e.ifail() << endl;
            cout << e.getMessage() << endl;;    
            logger()->error(e.ifail(),e.getMessage().c_str());
        }
    }
    return ifail;
}
