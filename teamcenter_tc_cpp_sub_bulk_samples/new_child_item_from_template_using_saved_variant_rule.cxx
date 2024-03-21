#include <iostream>
#include <bom/bom.h>
#include <cfm/cfm.h>
#include <collabctx/collabctx.h>
#include <me/me.h>
#include <tc/tc.h>
#include <tc/tc_startup.h>
#include <tccore/item.h>
#include <tccore/grm.h>
#include <tccore/grmtype.h>
#include <tccore/tctype.h>
#include <user_exits/user_exits.h>

#include <base_utils/TcResultStatus.hxx>
#include <base_utils/IFail.hxx>

using namespace std;

static int new_child_item_from_template_using_saved_variant_rule(tag_t tRev, tag_t tSVR)
{  
    int ifail = ITK_ok;
	ResultStatus status;
	try
	{	
		tag_t tWindow = NULLTAG;
		status = BOM_create_window (&tWindow); 
		
		tag_t tTopLine = NULLTAG;
		status = BOM_set_window_top_line(tWindow, NULLTAG, tRev, NULLTAG, &tTopLine);
		
		tag_t tRule = NULLTAG;
		status = CFM_find("Latest Working", &tRule);		
		status = BOM_set_window_config_rule(tWindow, tRule);
		status = BOM_window_hide_variants (tWindow);
		status = BOM_window_apply_full_vrule(tWindow, tSVR);
		
		tag_t ccContext = NULLTAG;
		status = COLLABCTX_create_config_context_based_on_ref( "", "CCName", "CCDesc", &tWindow, &ccContext);		

		tag_t tItem = NULLTAG;
		status = ITEM_ask_item_of_rev(tRev, &tItem);
		
		tag_t tType =NULLTAG;
		status = TCTYPE_ask_object_type(tItem, &tType);
		
		logical isModified = FALSE;
		char* itemId = NULL;
		status = USER_new_item_id(NULLTAG, tType, &isModified, &itemId);
		cout << "\n  itemId: "<< itemId << endl;

		tag_t tNewRev;
		char ruleKey[] = "Product.Template.Cloning";
		status = ME_create_product_from_template(itemId, "A", itemId, "", tRev, ccContext, tWindow, ruleKey, &tNewRev);		
		status = BOM_close_window(tWindow);
		if(itemId) MEM_free(itemId);
	}
	catch( const IFail &e )
	{
		ifail = e.ifail();
		cout << "\n    error " << ifail << endl;
		cout << "    "<< e.getMessage() << endl;	
	}	
    return ifail;
}
