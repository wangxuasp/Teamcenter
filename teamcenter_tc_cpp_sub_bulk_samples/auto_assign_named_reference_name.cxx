#include <ae/dataset.h>

#include <iostream>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>

using namespace Teamcenter;
using namespace std;

int auto_assign_named_reference_name(tag_t ds_tag)
{
    int ifail = ITK_ok;
	ResultStatus rstat;
	try
	{
		rstat = AOM_refresh(ds_tag, true);
		
		char os_path[] = "C:\\Users\\infodba\\Desktop\\junk.txt";	

		/* Let TC assign the name of the file in the volume */
		char  auto_assign[] = ""; 
		
		rstat = AE_import_named_ref(ds_tag, "Text", os_path, auto_assign, SS_TEXT);
		
		rstat = AOM_save(ds_tag);
		rstat = AOM_refresh(ds_tag, FALSE);
		rstat = AOM_unload(ds_tag);
	}
	catch( const IFail &e )
	{
        cout << "error " << e.ifail() << endl;
        cout << e.getMessage() << endl; 		
	}
    return ifail;
}
