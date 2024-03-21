
#include <epm/epm_task_template_itk.h>

#include <base_utils/ScopedSmPtr.hxx>
#include <base_utils/TcResultStatus.hxx>

using namespace std;
using namespace Teamcenter;

int add_handler_to_task_template()
{
    int ifail = ITK_ok;
    ResultStatus rstat;
    try
    {
        tag_t temp_tag = NULLTAG;
        rstat = EPM_find_process_template("TCM Release Process", &temp_tag);
        
        int n_subtasks = 0;
        scoped_smptr<tag_t> subtasks;
        rstat = EPM_ask_subtask_templates(temp_tag, &n_sub_tasks, &subtasks);
        
        tag_t subtask = subtasks[0];  // Only one subtask 
        
        EPM_action_t action = EPM_complete_action;
        const char* name = "EPM-notify";
        const char *args[3] = {"-subject=Status Added", 
                               "-recipient=$OWNER", 
                               "-comment=The project is complete"};
        tag_t action_handler = NULLTAG;
        rstat = EPM_create_action_handler2(subtask, action, name, 3, args, 
                    &action_handler);
    }
    catch( const IFail &e )
    {
        cout << "error " << e.ifail() << endl;
        cout << e.getMessage() << endl;     
    }
    return ifail;   
}
