//@<COPYRIGHT>@
//==================================================
//Copyright $2019.
//Siemens Product Lifecycle Management Software Inc.
//All Rights Reserved.
//==================================================
//@<COPYRIGHT>@

/* 
 * @file 
 *
 *   This file contains the implementation for the Extension A5_fnd0postCloneAction_PostAction
 *
 */
#include <A5gtac/A5_fnd0postCloneAction_PostAction.hxx>
#include <iostream>
#include <ug_va_copy.h>
#include <tccore/item.h>

#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;
using Teamcenter::Main::logger;

/* MEOP - fnd0postCloneAction - PostAction */
int A5_fnd0postCloneAction_PostAction( METHOD_message_t * msg, va_list args )
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
		tag_t newOperationTag = msg->object_tag;
		scoped_smptr<char>  newOperationID;
		stat = ITEM_ask_id2(newOperationTag, & newOperationID);
		cout << endl << "  newOperationID: " <<   newOperationID.get() << endl;

		va_list largs;
		va_copy ( largs, args );
		tag_t *oldOperationTag = va_arg ( largs, tag_t* );
		va_end(largs);

		scoped_smptr<char> oldOperationID;
		stat = ITEM_ask_id2(*oldOperationTag, &oldOperationID);
		cout << endl << " oldOperationID: " <<  oldOperationID.get() << endl;
    }
    catch( const IFail &ex )
    {
		logger()->error( ex.ifail(), ex.getMessage());
		cout << ex.getMessage()  << endl;
        cout << "error " << ex.ifail() << endl;
    }
	return ifail;
}
