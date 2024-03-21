#include <tc/emh.h>
#include <tc/tc.h>
#include <tccore/aom_prop.h>
#include <bom/bom.h>
#include <cfm/cfm.h>
#include <base_utils/TcResultStatus.hxx>
#include <base_utils/IFail.hxx>
#include <base_utils/ScopedSmPtr.hxx>

using namespace std;
using namespace Teamcenter;

static void compare_bom_views(tag_t item)
{
    int ifail = ITK_ok;
    try
    {
        ResultStatus stat;

        tag_t window1 = NULLTAG;
        stat = BOM_create_window(&window1);
        
        tag_t window2 = NULLTAG;
        stat = BOM_create_window(&window2);

        tag_t rule = NULLTAG;
        stat = CFM_find("Latest Working", &rule);
        
        stat = BOM_set_window_config_rule(window1, rule);
        stat = BOM_set_window_config_rule(window2, rule);
        
        tag_t topLine1 = NULLTAG;
        stat = BOM_set_window_top_line(window1, NULLTAG, rev1, NULLTAG, &topLine1);
        
        tag_t topLine2 = NULLTAG;
        stat = BOM_set_window_top_line(window2, NULLTAG, rev2, NULLTAG, &topLine2);

        stat = BOM_set_window_pack_all(window1, TRUE);
        stat = BOM_set_window_pack_all(window2, TRUE);
       
        stat = BOM_compare(topLine1, topLine2, BOM_compare_multilevel, BOM_compare_output_report);
  
        int length = 0;
        scoped_smptr<char*> spLines;
        scoped_smptr<tag_t> sptItems;
        ifail = BOM_compare_report(topLine1, &length, &spLines, &sptItems);
  
        for(int ii = 0; ii < length; ii++)
            printf("\n %s \n", spLines.get()[ii]);

        stat = BOM_close_window(window1);
        stat = BOM_close_window(window2);
    }
    catch( const IFail &e )
    {
            ifail = e.ifail();
    }
}