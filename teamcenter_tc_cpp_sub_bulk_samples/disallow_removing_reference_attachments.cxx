#include <epm/epm.h>

EPM_decision_t disallow_removing_reference_attachments( EPM_rule_message_t msg )
{
    EPM_state_t state;
    EPM_ask_state( msg.task, &state );

    // Allow initial dettachments before job is initiated
    if( state == EPM_unassigned || state == EPM_pending )
    {
        return EPM_go;
    }

    if( msg.proposed_action == EPM_remove_attachment_action
        && msg.tag == EPM_reference_attachment )
    {
        return EPM_nogo;
    }
    return EPM_go;
}
