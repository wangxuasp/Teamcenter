#include <iostream>
#include <epm/epm.h>
#include <tc/emh.h>
#include <tc/tc.h>
#include <tc/tc_startup.h>
#include <sa/user.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>

#include <metaframework/BusinessObjectRef.hxx>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;
using Teamcenter::Main::logger;

int remove_all_reviewers(tag_t selectSignoffTaskTag)
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
        int nReviewersTags = 0;
        scoped_smptr<tag_t> spReviewersTags;    
        stat = EPM_ask_attachments(selectSignoffTaskTag, EPM_signoff_attachment, &nReviewersTags, &spReviewersTags);
        cout << endl << "Before Number of Reviewers: " << nReviewersTags << endl;
        for (int ii = 0; ii < nReviewersTags; ii++)
        {       
            int nValues = 0;
            scoped_smptr<char*> spValues;
            stat = AOM_ask_displayable_values(spReviewersTags[ii], "group_member", &nValues, &spValues);
            for( int jj = 0 ; jj < nValues; jj++ )
                cout << "    " <<  spValues.get()[jj] << endl;
        }
        
        stat = EPM_remove_attachments(selectSignoffTaskTag, nReviewersTags, spReviewersTags.get() );
        
        stat = EPM_ask_attachments(selectSignoffTaskTag, EPM_signoff_attachment, &nReviewersTags, &spReviewersTags);
        cout << endl << "After Number of Reviewers: " << nReviewersTags << endl;
        for (int ii = 0; ii < nReviewersTags; ii++)
        {
            int nValues = 0;
            scoped_smptr<char*> spValues;
            stat = AOM_ask_displayable_values(spReviewersTags[ii], "group_member", &nValues, &spValues);
            for( int jj = 0 ; jj < nValues; jj++ )
                cout << "    " <<  spValues.get()[jj] << endl;
        }       
    }
    catch( const IFail &e )
    {
        ifail = e.ifail();
        cout << "\n    error " << ifail << endl;
        cout << "    "<< e.getMessage() <<endl;
    }
    return ifail;
}
