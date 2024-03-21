/* 
 *   This file contains the implementation of the example in the 
 *       Server Customization Best Practices Guide - Section 15.1 
 *          Implement Extension to a COTS Property that does not 
 *           Implement Custom Getter and Setter Operation          
 */
#include <A4_properties/A4_registerPropExtension.hxx>
#include <iostream>
#include <time.h>
#include <ug_va_copy.h>
#include <tccore/tctype.h>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;
using Teamcenter::Main::logger;

#pragma warning(disable: 4100)
#pragma warning(disable: 4189)

int A4_registerPropExtension( METHOD_message_t * msg, va_list args)
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
        va_list largs;
        va_copy( largs, args);
        tag_t type_tag = va_arg( largs,tag_t);
        va_end( largs );

        char* typeName;
        ifail = TCTYPE_ask_name2(type_tag, &typeName);

        if( strcmp(typeName, "ItemRevision") == 0)
        {
            METHOD_id_t  method;
            stat = METHOD_find_prop_method(typeName, "IMAN_specification", PROP_set_value_tags_msg, &method);
            if (method.id != NULLTAG)
            {
                stat = METHOD_add_pre_condition(method, myPropSetterValidateFn, NULL);
            }
        }
        MEM_free(typeName);
    }
    catch( const IFail &ex )
    {
        logger()->error( ex.ifail(), ex.getMessage());
        cout << ex.getMessage()  << endl;
        cout << "error " << ex.ifail() << endl;
    }
    return ifail;
}

int myPropSetterValidateFn(METHOD_message_t *msg, va_list args)
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
        va_list largs;
        va_copy( largs, args );
        int n_secondaryObjTags = va_arg(largs,  int);
        const tag_t* secondaryObjTags = va_arg(largs, tag_t*);
        va_end( largs );

        tag_t objTag = NULLTAG;
        METHOD_PROP_MESSAGE_OBJECT(msg, objTag);
        printf("\n\t objTag: %u", objTag);

        //Given objTag, secondaryObjTags
        //Implement custom validation code
    }
    catch( const IFail &ex )
    {
        logger()->error( ex.ifail(), ex.getMessage());
        cout << ex.getMessage()  << endl;
        cout << "error " << ex.ifail() << endl;
    }
    return ifail;
}

