
#include <iostream>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <metaframework/OperationInput.hxx>
#include <metaframework/CreateInput.hxx>
#include <metaframework/BusinessObjectRegistry.hxx>
#include <tccore/Item.hxx>
#include <form/Form.hxx>

using namespace std;
using namespace Teamcenter;

int createItem()
{
    int ifail = ITK_ok;
	ResultStatus rstat;
	try
	{
		int ifail = ITK_ok;

		CreateInput* formCreateInput = static_cast<Teamcenter::CreateInput*> 
			(Teamcenter::BusinessObjectRegistry::instance().createInputObject("ItemRevision Master", 
			OPERATIONINPUT_CREATE));
		formCreateInput->setString("object_name", "1234567/A", false);
		formCreateInput->setString("user_data_1", "Some Value", false);
		Teamcenter::Form *pForm = dynamic_cast<Teamcenter::Form *>
			(Teamcenter::BusinessObjectRegistry::instance().createBusinessObject(formCreateInput));
		rstat = AOM_save_with_extensions(pForm->getTag());
		
		CreateInput* revCreateInput = static_cast<Teamcenter::CreateInput*>(Teamcenter::BusinessObjectRegistry::instance().createInputObject("ItemRevision", OPERATIONINPUT_CREATE));
		revCreateInput->setTag("item_master_tag", pForm->getTag(), false);
		
        CreateInput* itemCreateInput = static_cast<Teamcenter::CreateInput*> (Teamcenter::BusinessObjectRegistry::instance().createInputObject("Item", OPERATIONINPUT_CREATE));
		itemCreateInput->setString("item_id", "1234567", false);
		itemCreateInput->setString("object_name", "1234567", false);
		itemCreateInput->setTag("revision", revCreateInput->getTag(), false);

		Teamcenter::Item *pItem = dynamic_cast<Teamcenter::Item *>(Teamcenter::BusinessObjectRegistry::instance().createBusinessObject(itemCreateInput));
		rstat = AOM_save_with_extensions(pItem->getTag());
		
	}
	catch( const IFail &e )
	{
		ifail = e.ifail();
        cout << "error " << e.ifail() << endl;
        cout << e.getMessage() << endl; 
	}
    return ifail;
}