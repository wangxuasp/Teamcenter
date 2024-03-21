#include <bom/bom.h>
#include <cfm/cfm.h>
#include <tc/emh.h>
#include <tc/tc.h>
#include <tccore/aom_prop.h>

#include <base_utils/IFail.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <base_utils/TcResultStatus.hxx>

using namespace std;
using namespace Teamcenter;

static void report_bom_quantity_differences(tag_t tRevA, tag_t tRevB)
{
    int ifail = ITK_ok;
    try
    {
        ResultStatus stat;      
        tag_t tRule = NULLTAG;
        stat = CFM_find("Any status; No Working", &tRule);  
        
        tag_t tWindowA = NULLTAG;
        stat = BOM_create_window(&tWindowA);
        
        tag_t tWindowB = NULLTAG;
        stat = BOM_create_window(&tWindowB);
        
        tag_t tTopLineA = NULLTAG;
        stat = BOM_set_window_top_line(tWindowA, NULLTAG, tRevA, NULLTAG, &tTopLineA);
        
        tag_t tTopLineB = NULLTAG;
        stat = BOM_set_window_top_line(tWindowB, NULLTAG, tRevB, NULLTAG, &tTopLineB);

        stat = BOM_set_window_config_rule(tWindowA, tRule);
        stat = BOM_set_window_config_rule(tWindowB, tRule);

        stat = BOM_set_window_pack_all(tWindowA, TRUE);
        stat = BOM_set_window_pack_all(tWindowB, TRUE);
        
        stat = BOM_compare( tTopLineA, tTopLineB, BOM_compare_singlelevel, BOM_compare_output_bomline ) ;

        int iCountA = 0;
        scoped_smptr<tag_t> tLinesA;
        scoped_smptr <int> iFlagsA;
        stat = BOM_compare_list_diffs(tTopLineA, &iCountA, &tLinesA, &iFlagsA);

        int iCountB = 0;
        scoped_smptr<tag_t> tLinesB;
        scoped_smptr <int> iFlagsB;
        stat = BOM_compare_list_diffs(tTopLineB, &iCountB, &tLinesB, &iFlagsB);

        for (int ii = 0; ii < iCountA; ii++) 
        {
            if (BOM_cmp_chg_qty(iFlagsA[ii]) == 1)
            {
                char *name = NULL;
                stat = AOM_ask_value_string(tLinesA[ii], "bl_line_name", &name);

                char *qty = NULL;
                stat = AOM_ask_value_string(tLinesA[ii], "bl_quantity", &qty);
                printf("\n    A. %s  %s \n", qty, name);

                stat = AOM_ask_value_string(tLinesB[ii], "bl_line_name", &name);

                stat = AOM_ask_value_string(tLinesB[ii], "bl_quantity", &qty);
                printf("    B. %s  %s \n", qty, name);
            }
        }

        stat = BOM_close_window(tWindowA);
        stat = BOM_close_window(tWindowB);
    }
    catch( const IFail &e )
    {
        ifail = e.ifail();
    }
}
