
#include <tccore/aom_prop.h>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <base_utils/ScopedSmPtr.hxx>

using namespace std;
using namespace Teamcenter;

static logical is_object_currently_in_process(tag_t object_tag)
{
    int ifail = ITK_ok;
    ResultStatus rstat;
    logical is_in_process = false;
    try
    {
        int n_tasks = 0;
        scoped_smptr<tag_t> tasks;
        rstat = AOM_ask_value_tags(object_tag, "process_stage_list", &n_tasks, &tasks);
        if (n_tasks > 0) is_in_process = true;
    }
    catch( const IFail &e )
    {
        cout << "error " << e.ifail() << endl;
        cout << e.getMessage() << endl;
    }
    return is_in_process;
}
