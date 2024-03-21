/*HEAD REPORT_PSE_OCCURRENCE_CHILDREN CCC ITK */
#include<stdlib.h>
#include <bom/bom.h>
#include <cfm/cfm.h>
#include <tc/tc.h>
#include <tccore/item.h>
#include <ps/ps_errors.h>

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

static void do_it(void)
{
    int
        n_bvs = 0,
        ii = 0,
        n_bvrs = 0,
        jj = 0,
        n_bvr_occs = 0,
        kk = 0;
    char
        *item_id = ITK_ask_cli_argument("-i="),
        *seq_no = NULL,
        *child_item_id = NULL;
    tag_t 
        item = NULLTAG,
        *bvs = NULL,
        *bvrs = NULL,
        *occurrences = NULL,
        child_item = NULLTAG,
        child_bom_view = NULLTAG;
        
        ITK_CALL(ITEM_find_item (item_id, &item));        
        if (!item) 
        {
            printf("\tItem %s does not exist!\n", item_id);
            exit(EXIT_FAILURE); 
        }
        
        ITK_CALL(ITEM_list_bom_views( item, &n_bvs, &bvs));
        printf("%s has...\n\t%d BomViews.\n", item_id, n_bvs);
        for (ii = 0; ii < n_bvs; ii++)
        {
            ITK_CALL(PS_list_bvrs_of_bom_view(bvs[ii], &n_bvrs, &bvrs));
            printf("\t%d BomView Revisions.\n", n_bvrs);
            for (jj = 0; jj < n_bvrs; jj++)
            {
                ITK_CALL(PS_list_occurrences_of_bvr(bvrs[jj], &n_bvr_occs, 
                    &occurrences));
                printf("\tBomView Revision #%d has %d occurrences.\n", jj+1, n_bvr_occs);
                for (kk = 0;  kk < n_bvr_occs; kk++)
                {
                    ITK_CALL(PS_ask_occurrence_child(bvrs[jj], occurrences[kk],
                        &child_item, &child_bom_view));
                    ITK_CALL(WSOM_ask_id_string(child_item, &child_item_id));
                    ITK_CALL(PS_ask_seq_no(bvrs[jj], occurrences[kk], &seq_no));  
                    printf("\t\t%s - seq no: %s\n", child_item_id, seq_no);
                }
                if (n_bvr_occs) MEM_free(occurrences);
                if (child_item_id) MEM_free(child_item_id);
                if (seq_no) MEM_free(seq_no);
            }
            if (n_bvrs) MEM_free(bvrs);                       
        }
        if (n_bvs) MEM_free(bvs);
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
