/*
    Item::fnd0postCloneAction operation is executed two ways.
    
    1) In Manufacturing Process Planner select File-> New-> From Template 
    
    2) ITK program calls one of the following ITK APIs:
        ME_create_process_from_template
        ME_create_plant_from_template 
        ME_create_product_from_template
*/
int  A2MEProcessImpl::fnd0postCloneActionBase( const tag_t *originalItem )
{
	ECHO("\n\n\t A2MEProcessImpl::fnd0postCloneActionBase \n");
    int ifail = ITK_ok;

    tag_t type = NULLTAG;
    IFERR_REPORT(TCTYPE_ask_object_type(*originalItem, &type));

    char type_name[TCTYPE_name_size_c+1] = "";
    IFERR_REPORT(TCTYPE_ask_name(type, type_name));

    char *name = NULL;
    IFERR_REPORT(WSOM_ask_object_id_string(*originalItem, &name));
    ECHO("\t\t\t %s - %s \n", name, type_name);

    tag_t cloneTag = this->getA2MEProcess()->getTag();
    IFERR_REPORT(TCTYPE_ask_object_type(cloneTag, &type));
    IFERR_REPORT(TCTYPE_ask_name(type, type_name));
    IFERR_REPORT(WSOM_ask_object_id_string(cloneTag, &name));
    ECHO("\t\t\t %s - %s \n", name, type_name);
    if (name) MEM_free(name);

    char *user_name_string = NULL;
    tag_t user_tag = NULLTAG;
    IFERR_REPORT(POM_get_user(&user_name_string, &user_tag));
    MEM_free(user_name_string);

    tag_t homeFolderTag = NULLTAG;
    IFERR_REPORT(SA_ask_user_home_folder(user_tag, &homeFolderTag));
    IFERR_REPORT(AOM_refresh(homeFolderTag, TRUE));
    IFERR_REPORT(FL_insert(homeFolderTag, cloneTag, 999));
    IFERR_REPORT(AOM_save(homeFolderTag));
    IFERR_REPORT(AOM_refresh(homeFolderTag, FALSE));
    IFERR_REPORT(AOM_unload(homeFolderTag));

    return ifail;
}