#include <iostream>

#include <pom/pom/pom.h>
#include <tccore/aom.h>
#include <tccore/item.h>
#include <tccore/part.h>

#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>

using namespace std;

logical is_object_descendant_of_class(tag_t object, char *class_name)
{
    int ifail = ITK_ok;
    ResultStatus rstat;
    logical verdict = FALSE;
    try
    {
        tag_t parent_class = NULLTAG;
        rstat = POM_class_id_of_class(class_name, &parent_class);

        tag_t object_class = NULLTAG;
        rstat = POM_class_of_instance(object, &object_class);
        
        rstat = POM_is_descendant(parent_class, object_class, &verdict);
    }
    catch( const IFail &e )
    {
        cout << "error " << e.ifail() << endl;
        cout << e.getMessage() << endl;
    }
    return (verdict);
}

int associate_design_revision_to_part_revision(tag_t design_rev)
{
    int ifail = ITK_ok;
    ResultStatus rstat;
    try
    {       
        if (is_object_descendant_of_class(design_rev, "Design_0_Revision_alt") )
        {       
            tag_t part = NULLTAG;
            tag_t part_rev = NULLTAG;
            rstat = PART_create_part ("Part-9195075" , "" , NULLTAG , "Part-9195075", 
                "Part" , "A" , true , 1, &part, &part_rev );        
            rstat = ITEM_save_item(part);
                        
            tag_t rel = NULLTAG;
            rstat = PART_rev_associate_design_rev(part_rev, design_rev, &rel);  

            rstat = AOM_unload(part);
            rstat = AOM_unload(part_rev);
            rstat = AOM_unload(design_rev); 
        }
    }
    catch( const IFail &e )
    {
        cout << "error " << e.ifail() << endl;
        cout << e.getMessage() << endl; 
    }
    return ifail;
}
 