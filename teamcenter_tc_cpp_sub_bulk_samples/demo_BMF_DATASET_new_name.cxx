
#include <A2irs/A2bmf_DATASET_new_name.hxx>
#include <string.h>
#include <ug_va_copy.h>

/* 
    BMIDE equivalent to user exit USER_new_dataset_name 
    Define and Assign extension rule on 
        Dataset - object_name - BMF_DATASET_new_name - BaseAction
        
    Note: This extension rule MUST be defined and assigned to Business Object Type Item.

*/
int A2bmf_DATASET_new_name( METHOD_message_t * msg, va_list args )
{
    int ifail = ITK_ok;
    
    va_list largs;
    va_copy( largs, args );
    tag_t  owner_tag = va_arg(largs, tag_t);
    tag_t  ds_type_tag = va_arg(largs, tag_t);
    tag_t  relation_tag = va_arg(largs, tag_t);
    const char* basis_name  = va_arg(largs, const char *);
    char **dataset_name = va_arg(largs, char **);
    logical *mod = va_arg(largs, logical*);
    va_end( largs );
    
    if (/* your logic here */)
    {
        *mod = TRUE;
        *dataset_name = (char *)MEM_alloc(sizeof(char) * strlen("NewValue"));
        strcpy(*dataset_name, "NewValue");
    )
    return ifail;
}
