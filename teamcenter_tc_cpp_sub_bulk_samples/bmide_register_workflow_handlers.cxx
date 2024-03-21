
/*
    Define and assign an extension rule to 
    Session - BMF_SESSION_register_epm_handlers - BaseAction
*/
#include <A5wf/A5RegisterEPMHandlers.hxx>

int A5RegisterEPMHandlers( METHOD_message_t * /*msg*/, va_list /*args*/ )
{
	int ifail = ITK_ok;
	EPM_register_rule_handler("-Custom Rule handler", "", custom_rule_handler);
	EPM_register_action_handler("-Custom Action Handler", "", custom_action_handler);
	return ifail;
}

int custom_action_handler(EPM_action_message_t msg)
{
	int ifail = ITK_ok;

	/* your code here */

	return ifail;
}

EPM_decision_t custom_rule_handler(EPM_rule_message_t msg)
{
    EPM_decision_t decision = EPM_go;

    /* your code here */

    return decision;
}
