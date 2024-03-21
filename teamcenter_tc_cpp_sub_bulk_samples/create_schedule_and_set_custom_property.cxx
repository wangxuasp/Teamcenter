#include <iostream>
#include <time.h>
#include <tc/emh.h>
#include <tc/tc.h>
#include <tc/tc_startup.h>
#include <schmgt/schmgt_bridge_itk.h>
#include <tccore/aom_prop.h>

#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;
using Teamcenter::Main::logger;

static void get_current_date_and_time(char *date_time)
{
    time_t the_time;
    struct tm *time_ptr;
    char *time_format = "%Y%m%d-%H%M%S";

    the_time = time((time_t *)0);
    time_ptr = localtime (&the_time);
    strftime(date_time, 128, time_format, time_ptr);
}

int ITK_user_main(int argc, char* argv[])
{
    int ifail = ITK_ok;

	ITK_initialize_text_services(ITK_BATCH_TEXT_MODE);           
	ITK_auto_login();                                            
	ITK_set_journalling(TRUE);
	
	ResultStatus stat;
	try
	{
		date_t start_date;
		stat = ITK_string_to_date( "27-Jun-2016 13:48", &start_date);  

		date_t finish_date;
		stat = ITK_string_to_date( "27-Jun-2019 13:48", &finish_date);  

		/* Ensure unique id */
		char schedule_id[128 + 1] ={"\0"};
		get_current_date_and_time(schedule_id);
		
		/*  Set COTS property */
		AttributeUpdateContainer_t  attrUpdates[2];
		attrUpdates[0].attrType = 7;
		attrUpdates[0].attrName = "gov_classification";
		attrUpdates[0].attrValue = "Top Secret";
		
		/*  Set custom property */
		attrUpdates[1].attrType = 7;
		attrUpdates[1].attrName = "a3_custom_prop";
		attrUpdates[1].attrValue = "Custom Value";
		
		TypedAttributeUpdateContainer_t  otherAttrUpdates[2];
		
		/* Valid objectTypes are "ScheduleTaskType" or "ScheduleType" */
		otherAttrUpdates[0].objectType = "ScheduleType";
		otherAttrUpdates[0].updatesSize = 1;
		otherAttrUpdates[0].updates[0] = attrUpdates[0];
		
		/* Valid objectTypes are "ScheduleTaskType" or "ScheduleType" */
		otherAttrUpdates[1].objectType = "ScheduleType";
		otherAttrUpdates[1].updatesSize = 1;
		otherAttrUpdates[1].updates[0] = attrUpdates[1];

		SCHMGT_new_schedule_container_t creInput;
		creInput.name = schedule_id;
		creInput.description = schedule_id;
		creInput.id = schedule_id;
		creInput.customer_name = "Acme";
		creInput.customer_number = NULL;
		creInput.start_date = start_date;
		creInput.finish_date = finish_date;
		creInput.priority = 3;
		creInput.dates_linked = false;  
		creInput.published = true;   
		creInput.notifications_enabled = false;
		creInput.percent_linked = false;  
		creInput.is_template = false;   
		creInput.is_public = true;  
		creInput.type = "A3Schedule";  	
		creInput.bill_code = NULL;
		creInput.bill_sub_code = NULL;
		creInput.bill_type = NULL;
		creInput.bill_rate = NULLTAG;
		creInput.other_attributes_size = 0; 
		creInput.typed_attribute_container_size = 2; 
		creInput.typed_other_attributes[0] = otherAttrUpdates[0];
		creInput.typed_other_attributes[1] = otherAttrUpdates[1];

		scoped_smptr<tag_t> schedules;
		stat = SCHMGT_create_new_schedule(&creInput, 1, &schedules);  
		tag_t schedule_tag = schedules[0];  // only one in this example
			
		scoped_smptr<char> str;
		stat = AOM_ask_value_string(schedule_tag, "gov_classification", &str);
		cout << endl << str.get() << endl;
		
		stat = AOM_ask_value_string(schedule_tag, "a3_custom_prop", &str);
		cout << endl << str.get() << endl;
	}
	catch( const IFail &e )
	{
		logger()->error( e.ifail(), e.getMessage());
		cout << e.getMessage()  << endl;
        cout << "error " << e.ifail() << endl;     
	}	
    ITK_exit_module(FALSE);     	
    return ITK_ok;
}
