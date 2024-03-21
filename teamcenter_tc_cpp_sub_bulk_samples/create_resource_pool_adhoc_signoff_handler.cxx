#include <iostream>
#include <epm/epm.h>
#include <epm/epm_task_template_itk.h>
#include <epm/signoff.h>
#include <tc/emh.h>
#include <tcinit/tcinit.h>
#include <tc/tc_startup.h>
#include <sa/group.h>
#include <sa/role.h>

int create_resource_pool_adhoc_signoff(EPM_action_message_t msg)
{
    int ifail = ITK_ok;
    ResultStatus stat;

    try
    {
        tag_t tRootTask = NULLTAG;
        stat = EPM_ask_root_task(msg.task, &tRootTask);

        tag_t tTask = msg.task;

        if (msg.action == EPM_start_action  && msg.proposed_state == EPM_started)
        {
            tag_t tGroup = NULLTAG;
            stat = SA_find_group("dba", &tGroup);

            tag_t tRole = NULLTAG;
            stat = SA_find_role2("DBA", &tRole);

            tag_t tPool = NULLTAG;
            stat = EPM_get_resource_pool(tGroup, tRole, FALSE, &tPool);

            int n = 0;
            scoped_smptr<tag_t> tSignoffs;
            stat = EPM_create_adhoc_signoff(tTask, tPool, &n, &tSignoffs);

            stat = EPM_set_adhoc_signoff_selection_done(tTask, TRUE);
        }
    }
    catch(IFail & ex)
    {
        return ex.ifail();
    }
    return ifail;
}

