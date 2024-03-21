/*HEAD CUSTOM_SUBSCRIPTION_HANDLER CCC ITK */
/*
     %TC_ROOT%\sample\compile -DIPLIB=libuser_exits custom_subscription_handler.c
     %TC_ROOT%\sample\link_user_exits

     %TC_BIN%\install_handlers -f=create -id=CustomEventId -funcname=CUSTOM_event_handler -functype=1 -execmode=1  -retrycount=5 -retryinterval=10 -override=true

    THIS IS NOT SUPPORTED AS A CUSTOM EXIT (a.k.a. Supplier Custom Hook)
    MUST BE ADDED TO libuser_exits.dll/so/sl
*/

#include <tc/emh.h>
#include <tc/tc.h>
#include <sub_mgr/subscription.h>
#include <user_exits/user_exits.h>
#include <ict/ict_userservice.h>
#include <stdarg.h>

extern DLLAPI int CUSTOM_event_handler(void* message, int nArgs, char** args)
{
    //TcSubscriptionActionMsg_t* msg = (TcSubscriptionActionMsg_t*)message;
    TcSubscriptionActionMsg_t* msg = (TcSubscriptionActionMsg_t*)message;

    /* TC_write_syslog will go to subsucriptionmgr syslog not tcserver syslog */
    TC_write_syslog("\n\n\n Running CUSTOM_event_handler \n\n\n");

    system("cmd /c \"date /t >> %TC_TMP_DIR%\\_______custom_event.log\"");
    return ITK_ok;
}

