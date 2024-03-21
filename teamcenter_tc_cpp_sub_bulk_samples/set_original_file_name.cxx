
#include <iostream>
#include <stdio.h>
#include <ae/ae.h>
#include <ae/dataset.h>
#include <ae/datasettype.h>
#include <ae/tool.h>
#include <sa/tcfile.h>
#include <tc/emh.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tcinit/tcinit.h>
#include <Error_Exception.hxx>
#include <base_utils/IFail.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;

int set_dataset_named_reference_name(tag_t dataset_tag)
{
    int ifail = ITK_ok;
    try
    {   
        ResultStatus stat;

        AE_reference_type_t refType;
        
        tag_t tFile = NULLTAG;
        stat = AE_ask_dataset_named_ref2(dataset_tag, "UGPART", &refType, &tFile);
        
        stat = AOM_refresh(tFile, TRUE);
        stat = IMF_set_original_file_name2(tFile, "new_name.prt");
        stat = AOM_save(tFile);
        stat = AOM_refresh(tFile, FALSE);
        stat = AOM_unlock(tFile);
    }
    catch(IFail & ex)
    {
        return ex.ifail();
    }
    return ifail;
}
