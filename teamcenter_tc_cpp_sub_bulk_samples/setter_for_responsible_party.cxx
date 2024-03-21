
#include <iostream>
#include <stdarg.h>
#include <ug_va_copy.h>
#include <ict/ict_userservice.h>
#include <sa/sa.h>
#include <tc/tc.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/tctype.h>

#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;
using Teamcenter::Main::logger;

extern DLLAPI int setResponsible_partyBase(METHOD_message_t *msg, va_list args);

/****************** Must be assigned to EPMTask parent type ****************** /
int A4_EPMTask_IMANTYPE_init_intl_props_PostAction( METHOD_message_t * /*msg*/, va_list args )
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
        char type_name[] = "EPMDoTask";
        char property_name[] = "responsible_party";
        char message[] = "PROP_set_value_tag";
        METHOD_function_t custom_method = setResponsible_partyBase;
        
        va_list largs;
        va_copy( largs, args);
        tag_t type_tag = va_arg( largs,tag_t);
        va_end( largs );
 
        tag_t target_type_tag = NULLTAG;        
        stat = TCTYPE_ask_type (type_name , &target_type_tag);
 
        if(type_tag == target_type_tag)
        { 
            METHOD_id_t  method;
            stat = METHOD_find_prop_method(type_name, property_name,  message, &method);
            if (method.id != NULLTAG)
            { 
                stat = METHOD_add_pre_condition(method, custom_method, NULL);
            }
        }
    }
    catch( const IFail &ex )
    {
        logger()->error( ex.ifail(), ex.getMessage());
        cout << ex.getMessage()  << endl;
        cout << "error " << ex.ifail() << endl;
    }
    return ifail;
}

int setResponsible_partyBase(METHOD_message_t *msg, va_list args)
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
        va_list  largs;
        va_copy(largs, args);
        va_arg(largs, tag_t);
        const tag_t value = va_arg(largs, tag_t);
        logical is_null = va_arg(largs, int) != 0;
        va_end( largs );

        const char *prop_name = NULL;
        METHOD_PROP_MESSAGE_PROP_NAME(msg, prop_name);
        
        tag_t  object_tag = NULLTAG;
        METHOD_PROP_MESSAGE_OBJECT(msg, object_tag);

        tag_t type_tag = NULLTAG;
        stat = TCTYPE_ask_object_type(object_tag, &type_tag);

        scoped_smptr<char> type_name;
        stat = TCTYPE_ask_name2(type_tag, &type_name);

        scoped_smptr<char> name;
        stat = AOM_ask_name(object_tag, &name);
        
        printf("\n  Name: %s \t Type: %s \t Property: %s\n", 
            name.get(), type_name.get(), prop_name);

        /*  your code here */ 

    }
    catch(IFail & ex)
    {
        cout << "error " << ex.ifail() << endl;
        cout << ex.getMessage() << endl;
    }   
    return  ifail;
}

