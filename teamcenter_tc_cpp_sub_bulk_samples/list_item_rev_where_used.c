/* HEAD LIST_ITEM_REV_WHERE_USED CCC ITK */
#include<stdlib.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <sa/am.h> 
#include <tccore/item.h>
#include <tccore/workspaceobject.h>
#include <sa/sa.h> 
#include <ps/ps.h>

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

static logical do_it(void)
{
    int
        n_revs,
        ii, 
        n_parents,
        jj,
        *levels;
    char
        item_name[ITEM_name_size_c +1],
        *id_string,
        *class_name;
    tag_t
        item, 
        class,
        *revs,
        *parents;         
	
    while (printf("\nEnter the Item ID: ") && gets(item_name) &&
        strlen(item_name))
    {
        ITK_CALL(ITEM_find_item(item_name, &item));   
        ITK_CALL(ITEM_list_all_revs(item, &n_revs, &revs));
        printf("\nNumber of Item Revisions = %d\n", n_revs);    
        for (ii = 0; ii < n_revs; ii++)
        {
            ITK_CALL(WSOM_ask_object_id_string(revs[ii], &id_string));
            ITK_CALL(PS_where_used_all(revs[ii], PS_where_used_all_levels, 
                &n_parents, &levels, &parents));
            ITK_CALL(POM_class_of_instance(revs[ii], &class));
            ITK_CALL(POM_name_of_class(class, &class_name));            
            printf("%d.  %s\t%s\n", ii+1, id_string, class_name); 
            printf("\tNumber of Parents = %d \n", n_parents);
           
            for (jj = 0; jj < n_parents; jj++)
            {
                ITK_CALL(WSOM_ask_object_id_string(parents[jj], &id_string));
                ITK_CALL(POM_class_of_instance(parents[jj], &class));
                ITK_CALL(POM_name_of_class(class, &class_name));            
                printf("\t%d.  %s\t%s\n", jj+1, id_string, class_name); 
            }
            if (levels) MEM_free(levels);
            if (id_string) MEM_free(id_string);
            if (parents) MEM_free(parents); 
            if (class_name) MEM_free(class_name);      
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
