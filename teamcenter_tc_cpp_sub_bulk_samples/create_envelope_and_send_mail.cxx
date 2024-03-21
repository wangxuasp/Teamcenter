
#include <iostream>
#include <sa/user.h>
#include <tc/envelope.h>
#include <tc/tc.h>

#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <mld/logging/TcMainLogger.hxx>
#include <base_utils/ScopedSmPtr.hxx>

using namespace std;
using namespace Teamcenter;
using Teamcenter::Main::logger;

static void create_envelope_and_send_mail(void)
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {	
		scoped_smptr<char> user_name_string;
		tag_t user_tag = NULLTAG;
		stat = POM_get_user(&user_name_string, &user_tag);

		tag_t envelope = NULLTAG;
		stat = MAIL_create_envelope("1234567/A", "1234567/A", &envelope);
		
		stat = MAIL_initialize_envelope(envelope,"1234567/A", "1234567/A");
		stat = MAIL_add_envelope_receiver(envelope, user_tag);
		stat = MAIL_send_envelope(envelope);

		tag_t mailbox = NULLTAG;
		stat = SA_ask_user_mailbox(user_tag, &mailbox);
	}
	catch( const IFail &ex )
    {
		logger()->error( ex.ifail(), ex.getMessage());
		cout << ex.getMessage()  << endl;
        cout << "error " << ex.ifail() << endl;        
    }
}
