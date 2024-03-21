/*
    This example is based on Teamcenter Help section
        "Intelligent part numbering use case and example"
*/
#include <iostream>
#include <tc/tc.h>
#include <property/idgeneration.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/tctype.h>
#include <base_utils/TcResultStatus.hxx>
#include <Error_Exception.hxx>
#include <base_utils/IFail.hxx>

using namespace std;

static int generate_intelligent_id()
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
        tag_t idGenTypeTag = NULLTAG;
        stat= TCTYPE_ask_type( "A6_IdGeneratorForPartNumber", &idGenTypeTag);

        tag_t idGenCreateInputTag = NULLTAG;
        stat = TCTYPE_construct_create_input(idGenTypeTag, &idGenCreateInputTag);
                
        tag_t idGenTag = NULLTAG;
        stat= TCTYPE_create_object( idGenCreateInputTag, &idGenTag );
        
        /* All properties set below must have been added to the 
           A6_IdGeneratorForPartNumber CreateInput   */
        stat = AOM_set_value_string(idGenTag, "a6_Depart", "BS");
        stat = AOM_set_value_string(idGenTag, "a6_MakeBuy", "Make"); 
        stat = AOM_set_value_string(idGenTag, "a6_ProductLine", "CV");      
        stat = AOM_set_value_string(idGenTag, "a6_Unit", "AP");     

        char *generated_id = NULL;          
        stat = IDGR_generate_id_for_type("A6_Item", "item_id", idGenTag, &generated_id); 
        cout << "\n generated_id: " << generated_id << endl;
    }
    catch( const IFail &e )
    {
        cout << "error " << e.ifail() << endl;
        cout << e.getMessage() << endl;
    }
    return ifail;   
}
