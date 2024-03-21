#include <constants/constants.h>
#include <tccore/Item.hxx>
#include <metaframework/CreateInput.hxx>
#include <metaframework/BusinessObjectRegistry.hxx>

using namespace std;

static void createItemAndRevisionWithDifferentNames(void)
{
    int ifail = ITK_ok;

    char type_name[] = "Item";
	Teamcenter::CreateInput* itemCreateInput = 
		static_cast<Teamcenter::CreateInput*>
			(Teamcenter::BusinessObjectRegistry::instance().
				createInputObject(type_name, OPERATIONINPUT_CREATE));

    //Revision type name is not always <item type name>Revision
    char *revision_type_name = 0;
    CONSTANTS_get_type_constant_value("ItemRevision", type_name, &revision_type_name);

    Teamcenter::CreateInput* revCreateInput = static_cast<Teamcenter::CreateInput*>(Teamcenter::BusinessObjectRegistry::instance().createInputObject(revision_type_name, OPERATIONINPUT_CREATE));
    
    MEM_free(revision_type_name);

    // set Item properties
    itemCreateInput->setString("item_id", "1234567", false);
	itemCreateInput->setString("object_name", "My Item Name", false);
	itemCreateInput->setString("object_desc", "My Item Description", false);
    itemCreateInput->setTag("revision", revCreateInput->getTag(), false);

    // set Revision properties
    revCreateInput->setString("item_revision_id", "00", false);
	revCreateInput->setString("object_name", "My Revision Name", false);
	revCreateInput->setString("object_desc", "My Revision Description", false);

    Teamcenter::Item *pItem = dynamic_cast<Teamcenter::Item *>(Teamcenter::BusinessObjectRegistry::instance().createBusinessObject(itemCreateInput));

    ifail = AOM_save_with_extensions(pItem->getTag());

    // If the Revision is not saved it will inherit the Item's Name and Description.
    ifail = AOM_save_with_extensions(revCreateInput->getTag());
}

