
#include <bom/bom.h>
#include <pie/pie.h>
#include <tccore/aom_prop.h>

#include <string>
#include <iostream>
#include <sstream>

#include <Error_Exception.hxx>
#include <base_utils/IFail.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;
using Teamcenter::Main::logger;

int create_plmxml_export_file_of_structure(tag_t rev)
{
    int ifail = ITK_ok;
    ResultStatus rstat;
    try
    {
        scoped_smptr<char> item_id;
        rstat = AOM_ask_value_string(rev, "item_id", &item_id);
        
        ostringstream xml_file;
        xml_file << "C:\\temp\\" << item_id.get()<< ".xml";
        
        ostringstream log_file;
        log_file << "C:\\temp\\" << item_id.get()<< ".log";
        
        tag_t window = NULLTAG;
        rstat = BOM_create_window(&window);
        
        tag_t line = NULLTAG;
        rstat = BOM_set_window_top_line(window, NULLTAG, rev, NULLTAG, &line);
        
        tag_t session_tag = NULLTAG;
        rstat = PIE_create_session(&session_tag);       
        rstat = PIE_session_set_file(session_tag, xml_file.str().c_str());
        rstat = PIE_session_set_log_file(session_tag, log_file.str().c_str());
        
        char name[] = "ConfiguredDataFilesExportDefault";
        char context[] = "DEFAULT_PIE_CONTEXT_STRING";
        int n_modes = 0;
        scoped_smptr<tag_t> modes;
        rstat = PIE_find_transfer_mode2(name, context, &n_modes, &modes);
        
        rstat = PIE_session_set_transfer_mode(session_tag, modes[0]);
        
        int n_objects = 2;
        tag_t objects[] = {window, rev};
        rstat = PIE_session_export_objects(session_tag, n_objects,  objects);       
        rstat = PIE_delete_session(session_tag);
        rstat = BOM_close_window(window);   
    }
    catch( const IFail &e )
    {
        ifail = e.ifail();
        cout << "error " << e.ifail() << endl;
        cout << e.getMessage() << endl;
        logger()->error(e.ifail(),e.getMessage().c_str());
    }
    return ifail;
}
