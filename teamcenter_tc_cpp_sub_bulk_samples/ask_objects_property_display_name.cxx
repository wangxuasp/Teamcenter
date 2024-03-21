
#include <iostream>
#include <base_utils/IFail.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <base_utils/TcResultStatus.hxx>

#include <property/prop.h>
#include <property/propdesc.h>

using namespace std;
using namespace Teamcenter;

static void ask_property_display_name_by_object(tag_t object_tag, char *prop_real_name)
{
    int ifail = ITK_ok;
    ResultStatus rstat;
    try
    {   tag_t prop_tag = NULLTAG;
        rstat = PROP_ask_property_by_name(object_tag, prop_real_name, &prop_tag);
        
        tag_t prop_desc_tag = NULLTAG;
        rstat = PROP_ask_descriptor(prop_tag, &prop_desc_tag) ;
        printf("\n\n");

        scoped_smptr<char> display_name;
        rstat = PROPDESC_ask_display_name(prop_desc_tag, &display_name);
        cout << display_name.get() << endl;  
    }
    catch( const IFail &e )
    {
        cout << "error " << e.ifail() << endl;
        cout << e.getMessage() << endl; 
    }
}
