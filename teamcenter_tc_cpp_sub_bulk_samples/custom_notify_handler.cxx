/* EPM_register_action_handler("-CR-notify", "", custom_notify_handler); */ 

#include <iostream>
#include <time.h>
#include <tc/tc.h>
#include <ug_va_copy.h>
#include <epm/epm.h>
#include <epm/epm_task_template_itk.h>
#include <epm/epm_toolkit_tc_utils.h>
#include <ict/ict_userservice.h>
#include <sa/sa.h>
#include <stdio.h>
#include <tc/emh.h>
#include <tc/preferences.h>
#include <tc/tc_startup.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/method.h>
#include <Error_Exception.hxx>
#include <base_utils/IFail.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;

static int get_TO_email_arg(EPM_action_message_t msg, char to[SA_email_size_c + 1]);
static int get_FROM_email_arg(EPM_action_message_t msg, char from[SA_email_size_c + 1]);
static int get_SUBJECT_arg (EPM_action_message_t msg, char subject[100]);
static int get_MAIL_SERVER_arg(EPM_action_message_t msg, char server[100]);
static int create_BODY_file(EPM_action_message_t msg, char body[IMF_filename_size_c + 1]);
       
int custom_notify_handler(EPM_action_message_t msg)
{
    int ifail = ITK_ok;
    ResultStatus stat;   
    try
    {
        /* tc_mail_smtp arguments and character length limits */
        char to[SA_email_size_c + 1] = "";
        char subject[100] = "";
        char server[100] = "";
        char body[200] = "";
        char user[SA_email_size_c + 1]= "";

        /* Populate the "to" argument */
        get_TO_email_arg(msg, to);

        /* Populate the "user" argument */
        get_FROM_email_arg(msg, user);
        
        /* Populate the "subject" argument */
        get_SUBJECT_arg(msg, subject);
        
        /* Populate the "server" argument */
        get_MAIL_SERVER_arg(msg, server);
        
        /* Create email body file */
        strcpy(body, "C:\\Temp\\body.txt");
        create_BODY_file(msg, body);
        create_DEBUG_BODY_file(msg, body);
    
        char cmd[256] = "";
        sprintf(cmd, "tc_mail_smtp -to=%s -user=%s -subject=%s -server=%s -port=25 -body=%s",
            to, user, subject, server, body);        
    
        system(cmd);
    }
    catch(IFail & ex)
    {
        cout << "error " << ex.ifail() << endl;
        cout << ex.getMessage() << endl;
    }
    return ifail;
}

int get_TO_email_arg(EPM_action_message_t msg, char to[SA_email_size_c + 1])
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
        tag_t resp_party = NULLTAG; 
        stat = EPM_ask_responsible_party(msg.task, &resp_party);
        if (resp_party != NULLTAG)
        {
            scoped_smptr<char> name;
            stat = SA_ask_user_person_name2(resp_party, &name);
            
            scoped_smptr<char> email;
            stat = EPM_get_person_email_addr(name.get(), &email);
            strcpy(to, email.get());
        }
    }
    catch(IFail & ex)
    {
        cout << "error " << ex.ifail() << endl;
        cout << ex.getMessage() << endl;
    }
    return ifail;
}

int get_FROM_email_arg(EPM_action_message_t /*msg*/, char user[100 + 1])
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
        scoped_smptr<char> user_name;
        tag_t user_tag = NULLTAG;   
        stat = POM_get_user(&user_name, &user_tag);
        if (user_tag != NULLTAG)
        {
            scoped_smptr<char> name;
            stat = SA_ask_user_person_name2(user_tag, &name);
            
            scoped_smptr<char> email;
            stat = EPM_get_person_email_addr(name.get(), &email);
            strcpy(user, email.get());          
        }
    }
    catch(IFail & ex)
    {
        cout << "error " << ex.ifail() << endl;
        cout << ex.getMessage() << endl;
    }
    return ifail;
}

int get_SUBJECT_arg(EPM_action_message_t /*msg*/, char subject[100])
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {       
        time_t the_time;
        struct tm *time_ptr;
        char *time_format = "%Y-%m-%d %H.%M";

        the_time = time((time_t *)0);
        time_ptr = localtime (&the_time);

        char date_time[128 + 1] = {"\0"};
        strftime(date_time, 128, time_format, time_ptr);
        sprintf(subject, "\"%s\"", date_time);
    }
    catch(IFail & ex)
    {
        cout << "error " << ex.ifail() << endl;
        cout << ex.getMessage() << endl;
    }
    return ifail;
}

int get_MAIL_SERVER_arg(EPM_action_message_t /*msg*/, char server[100])
{
    int ifail = ITK_ok;
    ResultStatus stat;
    try
    {
        scoped_smptr<char> value;
        stat = PREF_ask_char_value("Mail_server_name",  0, &value);
        strcpy(server, value.get());    
    }
    catch(IFail & ex)
    {
        cout << "error " << ex.ifail() << endl;
        cout << ex.getMessage() << endl;
    }
    return ifail;
}

int create_BODY_file(EPM_action_message_t msg, char body[200])
{
    int ifail = ITK_ok;
    ResultStatus stat;  
    try
    {
        scoped_smptr<char> name;
        stat = EPM_ask_name2(msg.task, &name);

        scoped_smptr<char> action;
        stat = EPM_ask_action_string2(msg.action, &action);

        scoped_smptr<char> state;
        stat = EPM_ask_state_string2(msg.proposed_state, &state);

        FILE *the_file;     
        the_file = fopen(body, "wb+");
        
        fprintf(the_file, "Task: %s\n", name.get());        
        fprintf(the_file, "Action: %s\n", action.get());        
        fprintf(the_file, "State: %s\n", state.get());

        fclose(the_file);   
    }
    catch(IFail & ex)
    {
        cout << "error " << ex.ifail() << endl;
        cout << ex.getMessage() << endl;
    }
    return ifail;       
}

