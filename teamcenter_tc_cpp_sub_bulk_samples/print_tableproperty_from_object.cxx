#include <tc/tc.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <fclasses/tc_date.h>

using namespace Teamcenter;

int print_tableproperty_from_object (tag_t object_tag, const char* table_property_name)
{
    int ifail = ITK_ok;
    ResultStatus status;

    int n_table_rows = 0;
    tag_t* table_rows = 0;

    date_t date_val;
    scoped_smptr<char> date_string;
    scoped_smptr<char> code_string;

    try
    {
        status = AOM_ask_table_rows(object_tag, table_property_name, &n_table_rows, &table_rows);
        for (int i=0; i<n_table_rows; i++)
        {
            status = AOM_ask_value_date( table_rows[i], "a3new_date", &date_val);
            status = DATE_date_to_string ( date_val, "%d-%b-%Y %H:%M:%S", &date_string );
            printf("Date: %s\n", date_string.get());
            status = AOM_ask_value_string( table_rows[i], "a3new_date_code", &code_string);
            printf("Code: %s\n", code_string.get());
        }
    }
    catch (const IFail &ex)
    {
        ifail = ex.ifail();
    }
    return ifail;
}
