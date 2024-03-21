
#include <iostream>

#include <pom/pom/pom.h>
#include <tccore/aom_prop.h>
#include <tccore/workspaceobject.h>

#include <base_utils/IFail.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <base_utils/TcResultStatus.hxx>

using namespace std;
using namespace Teamcenter;

int remove_release_status(tag_t wso_tag)
{
    int ifail = ITK_ok;
    ResultStatus rstat;
    try
    {       
        int n_statuses = 0;
        scoped_smptr<tag_t> statuses;
        rstat = WSOM_ask_release_status_list(wso_tag, &n_statuses, &statuses);
        if (n_statuses > 0)
        {
            rstat = ITK_set_bypass(true);
            
            tag_t attr = NULLTAG;
            rstat = POM_attr_id_of_attr( "release_status_list", "WorkspaceObject", 
                &attr);
            if (n_statuses == 1)
            {
                rstat = POM_refresh_instances_any_class(1, &wso_tag, 
                    POM_modify_lock);
                rstat = POM_clear_attr(1, &wso_tag, attr);
                
                logical unload = true; 
                rstat = POM_save_instances(1, &wso_tag, unload);
            
                rstat = POM_refresh_instances_any_class(1, statuses.get(), 
                    POM_delete_lock);
                rstat = POM_delete_instances(1, statuses.get() );
            }
            else
            {
                for (int ii = 0; ii < n_statuses; ii++)
                {
                    scoped_smptr<char> name;
                    rstat = AOM_ask_value_string(statuses[ii], "name", &name);
                    if (strcmp(name.get(), "A2ReleaseStatusC") == 0)
                    {
                        cout << name.get() << endl; 
                        tag_t status_to_delete = statuses[ii];
                        rstat = POM_refresh_instances_any_class(1, &wso_tag, 
                            POM_modify_lock);
                        rstat = POM_remove_from_attr(1, &wso_tag, attr, ii, 1);
                
                        logical unload = true; 
                        rstat = POM_save_instances(1, &wso_tag, 
                            unload);                        
                        
                        rstat = POM_refresh_instances_any_class(1, &status_to_delete, 
                            POM_delete_lock);
                        rstat = POM_delete_instances(1, 
                            &status_to_delete);                     
                    }
                }       
            }
        }
    }
    catch( const IFail &e )
    {
        cout << "error " << e.ifail() << endl;
        cout << e.getMessage() << endl; 
    }
    return ifail;
}
 
