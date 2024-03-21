
#include <property/propdesc.h>
#include <tccore/tctype.h>

#include <base_utils/IFail.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <base_utils/TcResultStatus.hxx>

using namespace std;
using namespace Teamcenter;

static int ask_property_display_name_by_type(const char * type_name, const char *prop_name, char **display_name)
{
    int ifail = ITK_ok;
    ResultStatus rstat;

    try
    {
        tag_t type_tag = NULLTAG;
        rstat = TCTYPE_find_type(type_name, "", &type_tag); 

        tag_t prop_desc_tag = NULLTAG;
        rstat = TCTYPE_ask_property_by_name(type_tag, prop_name, &prop_desc_tag);
        
        rstat = PROPDESC_ask_display_name(prop_desc_tag, display_name);
    }
    catch( const IFail &e )
    {
        cout << "error " << e.ifail() << endl;
        cout << e.getMessage() << endl; 
    }

    return ifail;   
}

