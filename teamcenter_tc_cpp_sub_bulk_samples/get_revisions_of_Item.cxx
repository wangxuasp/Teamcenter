#include <iostream>

#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <metaframework/BusinessObjectRef.hxx>
#include <tccore/Item.hxx>
#include <tccore/ItemRevision.hxx>

using namespace std;
using namespace Teamcenter;

int getRevision_list(BusinessObjectRef< Item > item)
{
    int ifail = ITK_ok;
    ResultStatus rstat;
    try
    {
        vector<tag_t> revisions;
        vector<int> isValueNulls;
        item->getTagArray("revision_list", revisions, isValueNulls);
        for( int ii = 0; ii < revisions.size(); ii++)
        {
            /* Convert tags to BusinessObjects */
            BusinessObjectRef< ItemRevision > rev(revisions[ii]);

            string objectString;
            bool isNull = false;
            rev->getObject_string(objectString, isNull);
            cout << endl << "   " << objectString << endl;
        }
    }
    catch( const IFail &e )
    {
        cout << "error " << e.ifail() << endl;
        cout << e.getMessage() << endl; 
    }
    return ifail;
}
