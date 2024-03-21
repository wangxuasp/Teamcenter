
#include <tc/tc.h>
#include <pom/enq/enq.h>
#include <tccore/aom_prop.h>

#include <iostream>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;
using Teamcenter::Main::logger;

int find_preferences_by_name()
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
        char* pEnq = "QueryFnd0PreferenceDefinition";
        stat = POM_enquiry_create(pEnq);

        const char *select_attrs[] = { "puid" };
        char szClassName[] = "Fnd0PreferenceDefinition";
        char szPrefName[] = "NX*";
        stat = POM_enquiry_add_select_attrs(pEnq, szClassName, 1, select_attrs);
        stat = POM_enquiry_set_string_expr(pEnq, "exp1", szClassName, "fnd0Name", POM_enquiry_like, szPrefName);
        stat = POM_enquiry_set_where_expr(pEnq, "exp1");

        int nRows = 0;
        int nColumns = 0;
        scoped_smptr<void **> spvReport;
        stat = POM_enquiry_execute(pEnq, &nRows, &nColumns, &spvReport);
        cout << "    Number Found: " << nRows << endl;  
        for (int ii = 0; ii < nRows; ii++)
        {
            tag_t tObject = *(tag_t *) spvReport[ii][0];
            stat = POM_load_instances_any_class(1, &tObject, POM_read_lock);

            tag_t tAttrID = NULLTAG;
            stat = POM_attr_id_of_attr("fnd0Name", szClassName, &tAttrID);
            
            scoped_smptr<char> spcPrefName;
            logical lNull = false, lEmpty = false;
            stat = POM_ask_attr_string(tObject, tAttrID, &spcPrefName, &lNull, &lEmpty);

            stat = POM_attr_id_of_attr("fnd0Category", szClassName, &tAttrID);
        
            tag_t tCategory = NULLTAG;
            stat = POM_ask_attr_tag(tObject, tAttrID, &tCategory, &lNull, &lEmpty);
            stat = POM_load_instances_any_class(1, &tCategory, POM_read_lock);
        
            stat = POM_attr_id_of_attr("fnd0Name", "Fnd0PreferenceCategory", &tAttrID);
            
            scoped_smptr<char> spcCategoryName;
            stat = POM_ask_attr_string(tCategory, tAttrID, &spcCategoryName, &lNull, &lEmpty);
            stat = POM_unload_instances(1, &tObject);

            cout << "\tCategory: " << spcCategoryName.get() << "    Preference: " << spcPrefName.get() << endl;
        }
        stat = POM_enquiry_delete(pEnq);
    }
    catch (const IFail &ex)
    {
        logger()->error(ex.ifail(), ex.getMessage());
        cout << ex.getMessage() << endl;
        cout << "error " << ex.ifail() << endl;
    }
    return ifail;
}
