
#include <iostream>

#include <tc/tc.h>
#include <pom/pom/pom.h>

#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>

using namespace std;

logical is_ItemRevision(tag_t object_tag)
{
	int ifail = ITK_ok;
	ResultStatus rstat;
	logical verdict = FALSE;
	try
	{
		tag_t rev_class = NULLTAG;
		rstat = POM_class_id_of_class("ItemRevision", &rev_class);

		tag_t class_tag = NULLTAG;
		rstat = POM_class_of_instance(object_tag, &class_tag);
		rstat = POM_is_descendant(rev_class, class_tag, &verdict);
	}
	catch( const IFail &e )
	{
		cout << "error " << e.ifail() << endl;
		cout << e.getMessage() << endl;
	}
	return (verdict);
}
