#include <stdio.h>
#include <ug_va_copy.h>
#include <metaframework/CreateInput.hxx>

extern "C" DLLAPI int  A2_gtac_extension_rule(METHOD_message_t *m, va_list args)
{
    printf("\n\n A2_gtac_extension_rule \n\n");

	int ifail = ITK_ok;

    va_list largs;
    va_copy( largs, args );
    Teamcenter::CreateInput *creInput = va_arg(largs, Teamcenter::CreateInput*);
    va_end( largs );

    tag_t item_tag = msg->object_tag;

    tag_t rev_tag = NULLTAG;
    std::string revision = "revision";
    itemCreInput->getCompoundObjectTag(revision, rev_tag);

    tag_t master_tag = NULLTAG;
    std::string IMAN_master_form = "IMAN_master_form";
    itemCreInput->getCompoundObjectTag(IMAN_master_form, master_tag);

    tag_t revCreInputTag = NULLTAG;
    bool isNull = false;
    ifail = itemCreInput->getTag("revision", revCreInputTag, isNull);

    return ifail;
}

