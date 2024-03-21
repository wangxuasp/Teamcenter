
#include <A2gtac/A2AuditEPMTaskReject.hxx>

#include <string.h>
#include <pom/pom/pom.h>
#include <tccore/aom_prop.h>

/* disable unreferenced formal parameter errors */
#pragma warning(disable: 4100)



int A2AuditEPMTaskReject(tag_t EPMPerformSignoffTaskTag,
                         int secondaryObjectCount, /* not used */
                         tag_t* secondaryObjectTags, /* not used */
                         char** secondaryQualifiers, /* not used */
                         tag_t eventType, /* not used */
                         int paramCount,
                         char** paramNames,
                         char** paramValues,
                         int errorCode, /* not used */
                         const char* errorMessage, /* not used */
                         tag_t Fnd0WorkflowAuditTag)
{
    int ifail = ITK_ok;
    if ( paramCount ==0 ) return ITK_ok;

    tag_t signoffTag = NULLTAG;
    for ( int ii = 0; ii < paramCount; ii++ )
    {
        if(strcmp(paramNames[ii],"signoff") == 0)
        {
            char* signoffUID=paramValues[ii];
            ifail = POM_string_to_tag(signoffUID, &signoffTag);
            if(ifail != ITK_ok) return ifail;
            break;
        }
    }

    if (signoffTag == NULLTAG)
    {
        return ITK_ok;
    }

    tag_t auditClassTag = NULLTAG;
    ifail = POM_class_of_instance(Fnd0WorkflowAuditTag, &auditClassTag);
    if(ifail != ITK_ok) return ifail;

    char* auditClassName = NULL;
    ifail = POM_name_of_class (auditClassTag, &auditClassName);
    if(ifail != ITK_ok) return ifail;

    tag_t signoffClassTag = NULLTAG;
    ifail = POM_class_of_instance( signoffTag, &signoffClassTag);
    if(ifail != ITK_ok) return ifail;

    char* signoffClassName = NULL;
    ifail = POM_name_of_class (signoffClassTag, &signoffClassName);
    if(ifail != ITK_ok) return ifail;
    
    logical exists = false;
    ifail = POM_attr_exists("a2_custom_property", signoffClassName, &exists);
    if(ifail != ITK_ok) return ifail;
    if (exists == TRUE)
    {
        char* propVal = NULL;
        ifail = AOM_ask_value_string (signoffTag, "a2_custom_property" ,&propVal);
        if(ifail != ITK_ok) return ifail;
        
        exists=false;
        ifail = POM_attr_exists("a2_signoff_custom_property", auditClassName, &exists);
        if(ifail != ITK_ok) return ifail;
        if (exists == TRUE)
        {
            ifail = AOM_set_value_string(Fnd0WorkflowAuditTag, 
                        "a2_signoff_custom_property", propVal);
            if(ifail != ITK_ok) return ifail;
        }
        if(propVal) MEM_free(propVal);
    }
    if (auditClassName) MEM_free(auditClassName);
    if (signoffClassName) MEM_free(signoffClassName);
    return ifail;
}
