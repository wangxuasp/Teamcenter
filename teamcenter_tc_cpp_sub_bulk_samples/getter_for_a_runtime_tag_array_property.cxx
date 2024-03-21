
/**
 * Getter for a Tag Array Property
 * @param values - Parameter value
 * @param isNull - Returns true for an array element if the parameter value at that location is null
 * @return - Status. 0 if successful
 */
int  A2FormImpl::getA2_runtime_tagsBase( std::vector<tag_t> & values, std::vector<int> & isNull ) const
{
    ECHO("\n A2FormImpl::getA2_runtime_tagsBase \n");
    int ifail = ITK_ok;

    int n_objects = 0;
    tag_t* objects = NULL;
    get_newstuff_folder_contents(&n_objects, &objects);
    
    values.resize(n_objects);
    isNull.resize(n_objects);
    for (int ii = 0; ii < n_objects; ii++)
    {                  
        isNull[ii] = false;
        values[ii] = objects[ii];
    }   
    return ifail;
}
