#include <tc/emh.h>
#include <tc/tc.h>
#include <tc/tc_startup.h>
#include <tccore/aom.h>

#include <base_utils/TcResultStatus.hxx>
#include <base_utils/IFail.hxx>
#include <metaframework/BusinessObjectRef.hxx>
#include <sa/Person.hxx>

using namespace Teamcenter;

using namespace std;

int createPerson()
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
        char typeName[] = "Person";
        CreateInput* personCreInput = static_cast<CreateInput*>
            (BusinessObjectRegistry::instance().
                createInputObject(typeName, OPERATIONINPUT_CREATE));

        bool isNull = false;
        stat = personCreInput->setString("user_name", "Doe, John", isNull);

        char address[] = "100 Main Street";
        stat = personCreInput->setString("PA1", address, isNull);

        char city[] = "HomeTown";
        stat = personCreInput->setString("PA2", city, isNull);

        char state[] = "Ohio";
        stat = personCreInput->setString("PA3", state, isNull);

        char zip_code[] = "12345";
        stat = personCreInput->setString("PA4", zip_code, isNull);

        char country[] = "USA";
        stat = personCreInput->setString("PA5", country, isNull);

        char organization[] = "Engineering";
        stat = personCreInput->setString("PA6", organization, isNull);

        char employee_number[] = "7654321";
        stat = personCreInput->setString("PA7", employee_number, isNull);

        char internal_mail_code[] = "B32";
        stat = personCreInput->setString("PA8", internal_mail_code, isNull);

        char email[] = "john.doe@amce.com";
        stat = personCreInput->setString("PA9", email, isNull);

        char phone[] = "(800) 987-6543";
        stat = personCreInput->setString("PA10", phone, isNull);

        stat = personCreInput->setString("locale", "en_US", isNull);
        stat = personCreInput->setString("timezone", "America / Los_Angeles", isNull);

        Person* pPerson = static_cast<Person*>
            (BusinessObjectRegistry::instance().
                createBusinessObject(personCreInput));

        tag_t PersonTag = pPerson->getTag();
        stat = AOM_save_with_extensions(PersonTag);
    }
    catch (const IFail &e)
    {
        cout << "error " << e.ifail() << endl;
        cout << e.getMessage() << endl;
    }
    return ifail;
}
