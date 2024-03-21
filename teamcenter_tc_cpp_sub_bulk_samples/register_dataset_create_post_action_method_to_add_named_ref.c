
#include <tc/tc.h>
#include <user_exits/user_exits.h>
#include <tccore/tctype.h>
#include <ict/ict_userservice.h>
#include <tccore/custom.h>
#include <tccore/tcaehelper.h>
#include <user_exits/aiws_ext.h>
#include <tc/preferences.h>
#include <ss/ss_errors.h>
#include <sa/tcfile.h>
#include <tccore/workspaceobject.h>
#include <ae/ae.h>
#include <tccore/method.h>
#include <tccore/item_msg.h>
#include <tc/tc_arguments.h>
#include <ae/dataset.h>
#include <ae/dataset_msg.h>
#include <ss/ss_const.h>
#include <user_exits/user_exits.h>

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

extern int GTAC_post_action(METHOD_message_t *m, va_list args)
{
	tag_t 
		dsTypeTag,
		dsTag,
		tool = NULLTAG,
		imported_file = NULLTAG;
    char  
		native_file_spec[WSO_name_size_c +1] = "c:\\TEMP\\test.txt",
		*dsName,
		*dsDesc,
		*dsId,
		*dsRev; 
    IMF_file_t 
		file_descriptor;    dsTypeTag   = va_arg( args, tag_t );

    dsName      = va_arg( args, const char * );
    dsDesc      = va_arg( args, const char * );
    dsId        = va_arg( args, const char * );
    dsRev       = va_arg( args, const char * );
    dsTag       = va_arg( args, tag_t );

	ITK_CALL(AE_ask_datasettype_def_tool(dsTypeTag, &tool));
	if (tool == NULLTAG) printf("Tool not found!\n");
	ITK_CALL(AE_set_dataset_tool(dsTag, tool));  

	ITK_CALL(IMF_import_file(native_file_spec, dsName, SS_TEXT, 
		&imported_file, &file_descriptor));
	ITK_CALL(AOM_save(imported_file));
   
    ITK_CALL(AE_add_dataset_named_ref(dsTag, "TEXT", AE_PART_OF, 
        imported_file)); 
       
	return ITK_ok;

}
extern void GTAC_register_method()
{
    int ec = ITK_ok,
		number_of_arguments = 4;
	METHOD_id_t
		 method;
	TC_argument_list_t 
		*user_args = NULL;

	fprintf(stdout, "\n\nGTAC_register_method...\n");

	ec = METHOD_find_method("UGPART", AE_create_dataset_msg, &method);
	if (ec) fprintf(stdout, "Error %d - METHOD_find_method\n", ec);
	
	if (method.id != NULLTAG)
	{		
	    ec = METHOD_add_action(method,  METHOD_post_action_type, GTAC_post_action, NULL);
	    if (ec) fprintf(stdout, "Error %d - METHOD_add_action\n", ec);
		printf("Method post action registered\n");
	}
	else	fprintf(stdout, "Method NOT found!\n", ec);
}

