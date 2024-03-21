
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <epm/epm.h>

/* disable unreferenced formal parameter errors */
#pragma warning(disable: 4100)

int set_signoff_custom_property(EPM_action_message_t message)
{
	int ifail = ITK_ok;
    int numSignoffs = 0;
    tag_t *signoffTags = NULL;
    ifail = EPM_ask_attachments(message.task, EPM_signoff_attachment,
            &numSignoffs, &signoffTags);
    if (ifail == ITK_ok && numSignoffs > 0)
    {
        for( int ii = 0; ii < numSignoffs; ii++)
        {
            ifail = AOM_refresh(signoffTags[0], TRUE);
            if(ifail != ITK_ok) return ifail;

            ifail = AOM_set_value_string(signoffTags[0],
                "a2_custom_property", "some value");
            if(ifail != ITK_ok) return ifail;

            ifail = AOM_save(signoffTags[0]);
            if(ifail != ITK_ok) return ifail;

            ifail = AOM_unlock(signoffTags[0]);
            if(ifail != ITK_ok) return ifail;

            ifail = AOM_unload(signoffTags[0]);
            if(ifail != ITK_ok) return ifail;
            MEM_free(signoffTags);
        }
    }
    return ITK_ok;
}

int A2SetSignoffCustomProperty( METHOD_message_t *msg, va_list args )
{
	int ifail = ITK_ok;
	ifail = EPM_register_action_handler("Set-Signoff-Custom-Property", 
            "Placement: Complete on select-signoff-team", 
            set_signoff_custom_property);
	return ifail;
}
