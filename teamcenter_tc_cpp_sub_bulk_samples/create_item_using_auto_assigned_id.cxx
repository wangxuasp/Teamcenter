
#include <tccore/Item.hxx>
#include <metaframework/CreateInput.hxx>
#include <metaframework/BusinessObjectRegistry.hxx>

using namespace std;

static int createItemUsingAutoAssignedId(void)
{
    int ifail = ITK_ok;

    char type_name[] = "Item";
	Teamcenter::CreateInput* itemCreateInput = static_cast<Teamcenter::CreateInput*>(Teamcenter::BusinessObjectRegistry::instance().createInputObject(type_name, OPERATIONINPUT_CREATE));
    
    itemCreateInput->setString("object_name", "My Item Name", false);
   
    Teamcenter::Item *pItem = dynamic_cast<Teamcenter::Item *>(Teamcenter::BusinessObjectRegistry::instance().createBusinessObject(itemCreateInput));
    
    ifail = AOM_save_with_extensions(pItem->getTag());

    return ifail;
}
