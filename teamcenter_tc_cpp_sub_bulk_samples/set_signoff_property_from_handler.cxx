#include <iostream>
#include <epm\epm.h>
#include <tccore\aom.h>
#include <tccore\aom_prop.h>

#include <base_utils/IFail.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <base_utils/TcResultStatus.hxx>

using namespace std;
using namespace Teamcenter;

extern "C" int set_signoff_property_from_handler(EPM_action_message_t msg)
{
    int ifail = ITK_ok;
    ResultStatus stat;

    try
    {
        tag_t task = msg.task;
        int n_signoffs = 0;
        scoped_smptr<tag_t> signoffs;
        stat = EPM_ask_attachments(task, EPM_signoff_attachment, &n_signoffs, &signoffs);

        /* assumming only one signoff */
        tag_t signoff = signoffs[0];
        stat = AOM_lock(signoff);
        stat = AOM_set_value_string(signoff, "a4_Additional_Info", "Set from Handler");
        stat = AOM_save(signoff);

        stat = AOM_refresh(signoff, TRUE);

        scoped_smptr<char> value;
        stat = AOM_ask_value_string(signoff, "a4_Additional_Info", &value);
        printf("\n\t value: %s \n", value.get());
        TC_write_syslog("\n\t value: %s \n", value.get());
        stat = AOM_unload(signoff);

    }
    catch( const IFail &e )
    {
        cout << "error " << e.ifail() << endl;
        cout << e.getMessage() << endl;
    }
   return ifail;
}
