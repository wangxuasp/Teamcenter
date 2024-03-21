
#include <iostream>
#include <base_utils/TcResultStatus.hxx>
#include <base_utils/IFail.hxx>
#include <metaframework/BusinessObjectRef.hxx>

using namespace Teamcenter;
using namespace std;

static void get_default_child_properties_of_object(tag_t object_tag)
{
	/*
		This gets the child objects defined in preferences 
		<TYPE>_DefaultChildProperties and <TYPE>_DefaultPseudoFolder
	*/
	int ifail = ITK_ok;
	ResultStatus stat;
	try
	{
		BusinessObjectRef< BusinessObject> parent( object_tag );
		vector< string > propNames;
		vector< tag_t > childTags;
		vector< string > childPropNames;
		stat = parent->fnd0GetChildren(&propNames, &childTags, &childPropNames);
		cout << "\n  Number of Children: " << childTags.size() << endl << endl;
		for (int ii = 0; ii < childTags.size(); ii++)
		{
			BusinessObjectRef< BusinessObject> child( childTags[ii] );
			string objectString;
			bool isNull = false;
			stat = child->getObject_string(objectString, isNull);			
			cout << "  Child Prop: " << childPropNames[ii] << endl;
			cout << "   Object ID: " << objectString << endl << endl;		
		}
	}
	catch( const IFail &ex )
	{
		printf("\n\n\n");
		cout << "error " << ex.ifail() << endl;
		cout << ex.getMessage() << endl;	
	}
}
