/* HEAD  DETACH_ITEM_REV_ATTACHMENTS CCC ITK */
#include<stdlib.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <sa/am.h> 
#include <tccore/item.h>
#include <tccore/workspaceobject.h>
#include <sa/sa.h> 
#include <pom/pom/pom.h>

#define EXIT_FAILURE 1
#define ITK_CALL(x) {           \
    int stat;                     \
    char *err_string;             \
    if( (stat = (x)) != ITK_ok)   \
    {                             \
    EMH_get_error_string (NULLTAG, stat, &err_string);                 \
    printf ("ERROR: %d ERROR MSG: %s.\n", stat, err_string);           \
    printf ("FUNCTION: %s\nFILE: %s LINE: %d\n",#x, __FILE__, __LINE__); \
    if(err_string) MEM_free(err_string);                                \
    exit (EXIT_FAILURE);                                                   \
    }                                                                    \
}

static void report_attachment_information(ITEM_attached_object_tag_t attach)
{
    char
        *id_string,
        *class_name,
        wso_type[WSO_name_size_c +1],
        *relation;
    tag_t
        class;
    
    ITK_CALL(WSOM_ask_object_id_string(attach.attachment, &id_string));
    ITK_CALL(POM_class_of_instance(attach.attachment, &class));
    ITK_CALL(POM_name_of_class(class, &class_name));            
    ITK_CALL(WSOM_ask_object_type(attach.attachment, wso_type));
    ITK_CALL(ITEM_ask_attachment_name(attach.attachment_type, &relation)); 
    printf("\t%s\n\t  Class name: %s\n\t  WSOM Type: %s\n\t  Relation: %s\n", 
        id_string, class_name, wso_type, relation);
    if (id_string) MEM_free(id_string);
    if (class_name) MEM_free(class_name);
    if (relation)  MEM_free(relation);
}

static logical do_it(void)
{
    int
        n_revs,
        ii, 
        n_rev_attach,
        jj;
    char
        item_name[ITEM_name_size_c +1],
        *id_string,
        *class_name;
    tag_t
        item,
        *revs,
        class;         
    ITEM_attached_object_tag_t
        *rev_attachs;
	
    while (printf("\nEnter the Item ID: ") && gets(item_name) &&
        strlen(item_name))
    {
        ITK_CALL(ITEM_find_item(item_name, &item));   
        ITK_CALL(ITEM_list_all_revs(item, &n_revs, &revs));
	    printf("\nNumber of Item Revisions = %d.\n", n_revs);    
        for (ii = 0; ii < n_revs; ii++)
        {
            
            ITK_CALL(WSOM_ask_object_id_string(revs[ii], &id_string));
            ITK_CALL(POM_class_of_instance(revs[ii], &class));
            ITK_CALL(POM_name_of_class(class, &class_name));            
            printf("%d.  %s\t%s\n", ii+1, id_string, class_name);

            ITK_CALL(ITEM_list_all_attachments_tag(revs[ii], &n_rev_attach,
                &rev_attachs));
            printf("\tNumber of Revision Attachments = %d \n", n_rev_attach);
            for (jj = 0; jj < n_rev_attach; jj++)
            {
                report_attachment_information(rev_attachs[jj]);
                ITK_CALL(POM_class_of_instance(rev_attachs[jj].attachment, &class));
                ITK_CALL(POM_name_of_class(class, &class_name));            

                if (strcmp(class_name, "Form") )
                {
                    ITK_CALL(ITEM_detach_rev_object_tag(revs[ii], 
                        rev_attachs[jj].attachment, 
                        rev_attachs[jj].attachment_type));
                }
                if (class_name) MEM_free(class_name); 
            }
            if (id_string) (MEM_free(id_string);       
            if (rev_attachs) MEM_free(rev_attachs);
            
            ITK_CALL(ITEM_list_all_attachments_tag(revs[ii], &n_rev_attach,
                &rev_attachs));        
            printf("\n number of Revision attachments is %d \n\n", n_rev_attach);          
        }        
        if (revs) MEM_free(revs);
    }
}

int ITK_user_main(int argc, char* argv[])
{
    int
        status = 0; 
    char
        *message;
    
    ITK_CALL(ITK_initialize_text_services( 0 ));
    status = ITK_auto_login();
   
    if (  (status != ITK_ok)) printf("iMAN login NOT successful.\n");
    else
    {
        printf("iMAN login successful.\n\n");
        ITK_CALL(ITK_set_journalling(TRUE));
        do_it();
    }
    ITK_exit_module(TRUE);
    return status;
}
