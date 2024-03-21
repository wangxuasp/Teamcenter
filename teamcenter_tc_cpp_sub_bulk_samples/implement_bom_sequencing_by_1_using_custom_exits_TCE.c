/*HEAD IMPLEMENT_BOM_SEQUENCING_BY_1_USING_CUSTOM_EXITS CCC UFUN */
/*
    %TC_ROOT%\sample\compile -DIPLIB=none gtac_ask_new_seqno.c
    %TC_ROOT%\sample\link_custom_exits GTAC

    cp GTAC.dll %TC_BIN%

    %TC_BIN%\preferences_manager -u=infodba -p=infodba -mode=import -action=merge -preference=TC_customization_libraries -scope=SITE -values=GTAC

*/

#include <ict/ict_userservice.h>
#include <user_exits/user_exits.h>
#include <tccore/custom.h>
#include <itk/mem.h>
#include <tc/tc.h>
#include <ps/ps.h>
#include <tc/emh.h>
#include <tc/preferences.h>
#include <stdarg.h>

static void ECHO(char *format, ...)
{
    char msg[1000];
    va_list args;
    va_start(args, format);
    vsprintf(msg, format, args);
    va_end(args);
    printf(msg);
    TC_write_syslog(msg);
}

#define IFERR_ABORT(X)  (report_error( __FILE__, __LINE__, #X, X, TRUE))
#define IFERR_REPORT(X) (report_error( __FILE__, __LINE__, #X, X, FALSE))

static int report_error(char *file, int line, char *call, int status,
    logical exit_on_error)
{
    if (status != ITK_ok)
    {
        int
            n_errors = 0,
            *severities = NULL,
            *statuses = NULL;
        char
            **messages;

        EMH_ask_errors(&n_errors, &severities, &statuses, &messages);
        if (n_errors > 0)
        {
            ECHO("\n%s\n", messages[n_errors-1]);
            EMH_clear_errors();
        }
        else
        {
            char *error_message_string;
            EMH_get_error_string (NULLTAG, status, &error_message_string);
            ECHO("\n%s\n", error_message_string);
        }

        ECHO("error %d at line %d in %s\n", status, line, file);
        ECHO("%s\n", call);

        if (exit_on_error)
        {
            ECHO("Exiting program!\n");
            exit (status);
        }
    }

    return status;
}


/* Initial sequence number */
#define DEFAULT_SEQUENCE_NO "1"

extern char *GTAC_ask_for_new_sequence_no ( tag_t parent_bom_revision )
{
    int ifail     = ITK_ok;

    if (parent_bom_revision == null_tag)
    {
        char *my_string = (char *)MEM_alloc ( strlen ( DEFAULT_SEQUENCE_NO ) + 1 );
        return strcpy ( my_string, DEFAULT_SEQUENCE_NO );
    }
    else
    {
        int n_occs;
        tag_t *occs;
        int ifail = PS_list_occurrences_of_bvr(parent_bom_revision, &n_occs, &occs);
        if (ifail == ITK_ok)
        {
            if (n_occs == 0)
            {
                char *my_string = (char *)MEM_alloc ( strlen ( DEFAULT_SEQUENCE_NO ) + 1 );
                return strcpy ( my_string, DEFAULT_SEQUENCE_NO );
            }
            else
            {
                int i, max_sequence_no = 0;

                char *sequence_no;
                EMH_clear_errors ();
                for (i = 0; i < n_occs; i++)
                {
                    ifail = PS_ask_seq_no (parent_bom_revision, occs[i], &sequence_no);
                    if (ifail == ITK_ok)
                    {
                        int val = atoi(sequence_no);
                        MEM_free(sequence_no);
                        if (val > max_sequence_no)
                        {
                            max_sequence_no = val;
                        }
                    }
                    else if (ifail != PS_no_seq_no)
                    {
                        MEM_free (occs);
                        /* could return DEFAULT_SEQUENCE_NO, but since other */
                        /* occurrences have no sequence number I doubt this  */
                        /* I doubt this should.                              */
                        return NULL;
                    }
                    else
                    {
                        EMH_clear_last_error(PS_no_seq_no); /* NKM 21/8/98 - should clear valid error */
                    }
                }
                MEM_free(occs);

                sequence_no = (char *)MEM_alloc(15);    /* big enough to hold most numbers */
                sprintf(sequence_no, "%d", ++max_sequence_no);
                return sequence_no;
            }
        }
        else
        {
            return NULL;
        }
    }
}

extern int GTAC_ask_new_seqno(int *decision, va_list args)
{
    enum SeqModeType { SeqModeNew, SeqModeExisting, SeqModeNone };

    TC_preference_search_scope_t old_scope;
    char* seqno_mode_pref;
    enum SeqModeType seqno_mode;
    int ifail;

    /***********  va_list for USER_ask_new_seqno ***********/
    tag_t  parent_bom_rev = va_arg(args, tag_t);
    tag_t  item = va_arg(args, tag_t);
    char  **seqno = va_arg(args, char **);
    /***********************************************************/

    ECHO("GTAC_ask_new_seqno\n");
    *decision = ONLY_CURRENT_CUSTOMIZATION;

    *seqno = NULL;
    if (item != NULLTAG)
    {
        tag_t item_class;
        tag_t passed_class;
        logical is_sub_class;

        if( (ifail = POM_class_id_of_class( "Item", &item_class )) != ITK_ok)
        {
            return ifail;
        }
        if( (ifail = POM_class_of_instance( item, &passed_class )) != ITK_ok)
        {
            return ifail;
        }
        if( (ifail = POM_is_descendant(item_class, passed_class, &is_sub_class)) != ITK_ok)
        {
           return ifail;
        }
        if (!is_sub_class && item_class != passed_class)
        {
            /* Item is actually an itemrev, need to change it to item */
            if ( (ifail = ITEM_ask_item_of_rev( item, &item )) != ITK_ok)
            {
                return ifail;
            }
        }
    }

    /* first try and retrieve the preference */
    if ( (ifail = PREF_ask_search_scope( &old_scope )) != ITK_ok)
    {
        return ifail;
    }
    if ( (ifail = PREF_set_search_scope( TC_preference_all )) != ITK_ok)
    {
        return ifail;
    }
    if ( (ifail = PREF_ask_char_value( "PS_new_seqno_mode", 0, &seqno_mode_pref )) != ITK_ok)
    {
        PREF_set_search_scope( old_scope );
        return ifail;
    }
    if ( (ifail = PREF_set_search_scope( old_scope )) != ITK_ok)
    {
        return ifail;
    }

    if ( seqno_mode_pref == NULL )
    {
        seqno_mode = SeqModeNew;
    }
    else if ( tc_strcasecmp(seqno_mode_pref, "new") == 0)
    {
        seqno_mode = SeqModeNew;
    }
    else if ( tc_strcasecmp(seqno_mode_pref, "existing") == 0)
    {
        seqno_mode = SeqModeExisting;
    }
    else if ( tc_strcasecmp(seqno_mode_pref, "none") == 0)
    {
        seqno_mode = SeqModeNone;
    }
    else
    {
        static logical moaned = false;
        if (!moaned)
        {
            TC_write_syslog ("****Preference PS_new_seqno_mode was set to '%s', it should be new/existing/none; assuming new ***\n", seqno_mode_pref);
            moaned = true;
        }
       seqno_mode = SeqModeNew;
    }

    MEM_free(seqno_mode_pref);

    switch (seqno_mode)
    {
        int n_occs;
        tag_t* occs;

    case SeqModeNew:
        *seqno = GTAC_ask_for_new_sequence_no( parent_bom_rev );
        return ITK_ok;

    case SeqModeExisting:
        if (parent_bom_rev == NULLTAG)
        {
            *seqno = GTAC_ask_for_new_sequence_no( parent_bom_rev );
            return ITK_ok;
        }
        ifail = PS_list_occurrences_of_bvr(parent_bom_rev, &n_occs, &occs);
        if (ifail == ITK_ok)
        {
            if ((n_occs == 0) || (item == NULLTAG))
            {
                MEM_free(occs);
                *seqno = GTAC_ask_for_new_sequence_no( parent_bom_rev );
                return ITK_ok;
            }
            else
            {
                int i;
                tag_t child_item;
                tag_t child_bv;
                /* For each occurence, get the item, find out if the item
                *  is the same as the one we're trying to add, and if it is
                *  pass back its seqno, else call the old function
                */
                for (i = 0; i < n_occs; i++)
                {
                    logical bvr_is_precise;

                    if ( (ifail = PS_ask_occurrence_child( parent_bom_rev,
                                                           occs[i],
                                                           &child_item,
                                                           &child_bv )) != ITK_ok)
                    {
                        MEM_free(occs);
                        return ifail;
                    }

                    if ( (ifail = PS_ask_is_bvr_precise( parent_bom_rev, &bvr_is_precise )) != ITK_ok)
                    {
                        MEM_free(occs);
                        return ifail;
                    }

                    if (bvr_is_precise)  /*Then child_item will be a revision*/
                    {
                        if ( (ifail = ITEM_ask_item_of_rev( child_item, &child_item )) != ITK_ok )
                        {
                            MEM_free(occs);
                            return ifail;
                        }
                    }

                    if (child_item == item)
                    {
                        /* We have found the item, so return the occ's seqno */
                        ifail = PS_ask_seq_no ( parent_bom_rev, occs[i], seqno) ;
                        if (ifail == PS_no_seq_no)
                        {
                            EMH_clear_errors();
                            MEM_free(occs);
                            seqno = NULL;

                            return ITK_ok;
                        }
                        MEM_free(occs);
                        return ifail;
                    }
                }
                /* No occurance of this item exists, so go back to old function */
                MEM_free(occs);
                *seqno = GTAC_ask_for_new_sequence_no( parent_bom_rev );
                return ITK_ok;
            }
        }
        else
        {
            return ifail;
        }

    case SeqModeNone:
        *seqno = (char *)MEM_alloc ( sizeof ( char ) );
        *seqno [ 0 ] = '\0';
        return ITK_ok;

    default:
        TC_report_serious_error ( __FILE__, __LINE__, "Should never get to here" );
        *seqno = (char *)MEM_alloc ( sizeof ( char ) );
        *seqno [ 0 ] = '\0';
        return ITK_ok;
    }

    return ITK_ok;
}

/* This function name must be <library_name>_register_callbacks */
extern DLLAPI int GTAC_register_callbacks()
{
    ECHO("GTAC_register_callbacks\n");

    IFERR_REPORT(CUSTOM_register_exit( "GTAC", "USER_ask_new_seqno",
        (CUSTOM_EXIT_ftn_t) GTAC_ask_new_seqno));
    return ITK_ok;
}
