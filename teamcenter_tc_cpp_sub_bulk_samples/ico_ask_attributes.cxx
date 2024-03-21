#include <plms_common.hxx>
#include <iostream>
#include <ics/ics.h>
#include <ics/ics2.h>
#include <tc/tc.h>
#include <tc/emh.h>

#include <Error_Exception.hxx>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <metaframework/BusinessObjectRef.hxx>

static int ico_ask_attributes(tag_t tWso)
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
        int  nIdAttrs;
        int* iIdAttrs;
        int* nValAttrs;
        char*** cpValAttrs;
        stat = ICS_ico_ask_attributes(tWso, &nIdAttrs, &iIdAttrs, &nValAttrs, &cpValAttrs);
        
        printf("\n Number of Attribute %d \n", nIdAttrs);

        for(int ii = 0; ii < nIdAttrs; ii++)
            printf("\t%d - %s\n", iIdAttrs[ii], *cpValAttrs[ii]);

        MEM_free(cpValAttrs);
    }
    catch( const IFail &e )
    {
        cout << "error " << e.ifail() << endl;
        cout << e.getMessage() << endl;
    }
    return ifail;
}

