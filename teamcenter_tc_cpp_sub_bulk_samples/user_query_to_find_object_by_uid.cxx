/*
*    Teamcenter Ouery Builder Definition
*        Name: Find Object by UID
*        Search Type: POM_object
*        Modifiable Query Types: Local Query
*        
*        Search Critrea
*           Attribute: timestamp
*           User Entry L10N Key: UID
*           User Entry Name: UID
*           operator =
*        Create
*
*    Execute utility to convert "Local Query" to "User Query"
*
*       %TC_BIN%\tc_set_query_where_run -query="Find Object by UID" -run=query
*    
*    Define and assign a BMIDE extension rule
*       Business Object Name: ImanQuery
*       Business Object Or Property: Type
*       Operation Name: BMF_QRY_custom_execute
*       Extension Point: BaseAction
*       
*/      
#include <A3_extension_rules/A3_ImanQuery.hxx>

int A3_ImanQuery( METHOD_message_t * /*msg*/, va_list args )
{
    int ifail = ITK_ok;

    va_list largs;
    va_copy( largs, args );
    char  *qName = va_arg(largs, char*);
    int   num_args = va_arg (largs, int);
    char  **names = va_arg (largs, char **);
    char  **values = va_arg (largs, char **);
    int   *num_found = va_arg (largs, int*);
    tag_t **found = va_arg (largs, tag_t **);
    va_end( largs );

    if (strcmp(qName, "Find Object by UID") == 0)
    {
        tag_t tObject = NULLTAG;
        ITK__convert_uid_to_tag(values[0], &tObject);

        *num_found = 1;
        *found = (tag_t *) MEM_alloc(*num_found * sizeof(tag_t));
        (*found)[0] = tObject;
    }
    return ifail;
}