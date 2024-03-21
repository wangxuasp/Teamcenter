
#include <tc/emh.h>
#include <tc/tc.h>
#include <tc/tc_util.h>
#include <property/legacy_counter.h>


#define SITE_A_init_value 		1
#define SITE_A_max_value 		19
#define SITE_A_new_init_value	40

#define SITE_B_init_value 		20
#define SITE_B_max_value 		29
#define SITE_B_new_init_value	50

#define SITE_C_init_value 		30
#define SITE_C_max_value 		39
#define SITE_C_new_init_value	60

static void create_legacy_counters(char *counterName, int initialValue)
{
	int ifail = ITK_ok;
	ifail = PROPERTY_legacy_create_id_counter(counterName);
	if (ifail == ITK_ok)
	{
		if (initialValue > 1)
			set_legacy_counter_inital_value(counterName, initialValue);	
	}
}	
	
static void set_legacy_counter_inital_value(char *counterName, int initialValue)
{

	PROPERTY_select_counter(counterName);
	int next_id = 0;
	do 
	{	
		PROPERTY_load_current_number(0, &next_id);
	} 
	while( next_id < initialValue - 1);	
}

static int get_legacy_counter_next_id(char *counterName, int maxValue, int newInitialValue)
{
	PROPERTY_select_counter(counterName);
	int next_id = 0;
	PROPERTY_load_current_number(0, &next_id);
		
	if (next_id == maxValue)
		set_legacy_counter_inital_value(counterName, newInitialValue);
	
	return next_id;
}

static void test_it()
{	
	int n_loops = 25;
	
	create_legacy_counters("SiteACounter", SITE_A_init_value);
	printf("\n\n Site A:\n");
	for (int ii = 0; ii < n_loops; ii++)
	{
		int next_id = get_legacy_counter_next_id("SiteACounter", SITE_A_max_value, SITE_A_new_init_value);
		printf("\t %d  ", next_id);
		if(next_id == SITE_A_max_value) printf("\n");
	}
	printf("\n");
	
	create_legacy_counters("SiteBCounter", SITE_B_init_value);
	printf("\n\n Site B:\n");
	for (int ii = 0; ii < n_loops; ii++)
	{
		int next_id = get_legacy_counter_next_id("SiteBCounter", SITE_B_max_value, SITE_B_new_init_value);
		printf("\t %d  ", next_id);
		if(next_id == SITE_B_max_value) printf("\n");
	}
	printf("\n");	
	
	create_legacy_counters("SiteCCounter", SITE_C_init_value);
	printf("\n\n Site C:\n");
	for (int ii = 0; ii < n_loops; ii++)
	{
		int next_id = get_legacy_counter_next_id("SiteCCounter", SITE_C_max_value, SITE_C_new_init_value);
		printf("\t %d  ", next_id);
		if(next_id == SITE_C_max_value) printf("\n");
	}
	printf("\n");
}

int ITK_user_main(int argc, char* argv[])
{
    ITK_initialize_text_services(ITK_BATCH_TEXT_MODE);
    if (ITK_auto_login () != ITK_ok)
    {
        TC_printf ("Login failed \n");
        return (!ITK_ok);
    }
    ITK_set_journalling(TRUE);
	
	test_it();

    ITK_exit_module(FALSE);

    return ITK_ok;
}

