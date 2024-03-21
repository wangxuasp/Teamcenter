
#include <iostream>
#include <epm/epm.h>
#include <tccore/aom.h>
#include <tc/tc.h>
#include <tc/tc_startup.h>

#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;
using Teamcenter::Main::logger;

#pragma warning(disable: 4100)
#pragma warning(disable: 4189)

int custom_action_handler(EPM_action_message_t msg)
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
        tag_t tRootTask = NULLTAG;
        stat = EPM_ask_root_task(msg.task, &tRootTask);

        cout << endl << "    EPM_ask_attachments" << endl;
        int nAttachs = 0;
        scoped_smptr<tag_t> sptAttachs;
        stat = EPM_ask_attachments(tRootTask, EPM_target_attachment, &nAttachs, &sptAttachs);

        scoped_smptr<char> spcName;
        scoped_smptr<char> spcType;
        for (int ii = 0; ii < nAttachs; ii++)
        {
            stat = AOM_ask_name(sptAttachs[ii], &spcName);
            stat = WSOM_ask_object_type2(sptAttachs[ii], &spcType);
            cout << "\t" << spcName.get() << " - " << spcType.get() << endl;
        }

        cout << endl << "    EPM_ask_typed_attachments" << endl;
        stat = EPM_ask_typed_attachments(tRootTask, "UGMASTER", &nAttachs, &sptAttachs);
        for (int ii = 0; ii < nAttachs; ii++)
        {
            stat = AOM_ask_name(sptAttachs[ii], &spcName);
            stat = WSOM_ask_object_type2(sptAttachs[ii], &spcType);
            cout << "\t" << spcName.get() << " - " << spcType.get() << endl;
        }
    }
    catch( const IFail &ex )
    {
        logger()->error( ex.ifail(), ex.getMessage());
        cout << ex.getMessage()  << endl;
        cout << "error " << ex.ifail() << endl;
    }
    return ITK_ok;
}

