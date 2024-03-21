
#include <time.h>
#include <schmgt/schmgt_itk.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tc/tc.h>
#include <tc/tc_startup.h>

#include <iostream>
#include <base_utils/TcResultStatus.hxx>
#include <base_utils/IFail.hxx>

using namespace std;

int updateScheduleTaskActualDates(tag_t tScheduleTask)
{
    int ifail = ITK_ok;
	ResultStatus status;
	try
	{
		ITK_set_bypass( TRUE );
		char *name = NULL;
        status = WSOM_ask_name2(tScheduleTask, &name);
		cout << "\n    Schedule Task Name:  " << name << "\n" << endl;
		MEM_free(name);
		
		char* state = NULL;
		status = AOM_UIF_ask_value(tScheduleTask, "fnd0state", &state);
		
		char* start = NULL;
		status = AOM_UIF_ask_value(tScheduleTask, "actual_start_date", &start);
		
		char*  finish = NULL;
		status = AOM_UIF_ask_value(tScheduleTask, "actual_finish_date", &finish);
		
		cout << "\t Before:  " << state << "\t" << start << "\t" << finish << endl;
		
		date_t today;
		time_t t = time(0);
		struct tm * now = localtime( & t );
		char szDate[100];
		strftime(szDate,strlen("DD-MMM-YYYY HH:MM")+1,"%d-%b-%Y %H:%M",now);
		
		date_t start_date;
		status = ITK_string_to_date(szDate, &start_date);
		
		date_t finish_date;
		status = ITK_string_to_date(szDate, &finish_date);

		status = AOM_refresh(tScheduleTask, TRUE);
		status = SCHMGT_update_task_exec_data(tScheduleTask, true, start_date, true, finish_date, 0, -1, -1, -1, -1);
		status = AOM_save(tScheduleTask);
		AOM_refresh(tScheduleTask, FALSE);

		status = AOM_UIF_ask_value(tScheduleTask, "fnd0state", &state);		
		status = AOM_UIF_ask_value(tScheduleTask, "actual_start_date", &start);		
		status = AOM_UIF_ask_value(tScheduleTask, "actual_finish_date", &finish);	
		status = AOM_unload(tScheduleTask);
		
		cout << "\t  After:  " << state << "\t" << start << "\t" << finish << endl;

		MEM_free(state);
		MEM_free(start);
		MEM_free(finish);		
	}
	catch( const IFail &e )
	{
		ifail = e.ifail();
		cout << "\n    error " << ifail << endl;
		cout << "    "<< e.getMessage() <<endl;
	}
	
    return ifail;
}