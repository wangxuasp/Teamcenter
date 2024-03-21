/* HEAD PRINT_SELECTED_OBJECTS_NAMES_FROM_CUSTOM_MOTTIF_BUTTON CCC ITK*/
/* This program will print the tags and names of all selected workspace objects.
   To add a pulldown menu with a single button to the mainmenu bar in MOTIF
   Add the following lines to wscustom.uih and recompile wsmainwindow.uil using 
   /usr/bin/X11/uil -I$IMAN_LANGUAGE/motif \wsmainwindow.uil -o wsmainwindow.uid:
-- begin copy
    value
    k_displaySelections  : 11000 + 16;

    object displaySelectionButton : XmPushButton
    {
        arguments
        {
            XmNuserData = $WSMainWindow;
            XmNlabelString = 'Display Selections';
        };
    
        callbacks
        { 
            MrmNcreateCallback  = procedure create3(k_displaySelections);
            XmNactivateCallback = procedure gizmocb(k_displaySelections);
        };
    };

    object IMAN_custom : XmCascadeButton
    {
        arguments
        {
            XmNlabelString = 'GTAC Pulldown';
        };
        controls
        {
            XmPulldownMenu
            {
                controls
                {
                    XmPushButton displaySelectionButton;
                };
            };
        };
    };
-- end copy
*/

/* Modified version of user_uif.c */     
#include <uif.h>
#include <user_exits/user_exits.h>
#include <tc/tc.h>
#include <cm.h>
#include <user_exits/epm_toolkit_utils.h>
#include <org_uif.h>

/****************************************************************************/

int USER_register_callbacks(void)
/*
 *  Description
 *
 *      User exit to allow INFOMANAGER to be customized so user callbacks
 *      registered with UIF_register_callback() will be called instead
 *      of the INFOMANAGER supplied callbacks.
 *      This is called when INFOMANAGER is being initialized including
 *      ITK programs.
 *
 *  Restrictions
 *      *** NOTE: For applications encapsulated by the IMAN team (UG, Genius
 *                SAP, etc.) use the internal exit function IE_uif_init
 *                instead of this function.
 *
 *-+
 */
{
/* I injected these many ifdef, as I didn't want to change the order,  */
/* these functions are being called for the Motif product. There might */
/* might be some dependencies.Z.I.                                     */
#ifdef MOTIF_UIF
    CM_init_application();         /* CM not provided */
#endif
    USER_ed_shell_init_module();
#ifdef MOTIF_UIF
    USER_note_init_module();       /* comment says for Motif only */
#endif
    USER_gs_shell_init_module();
    EPM_register_toolkit_handlers();

#ifdef REGISTER_FUI
    FUI_register_callbacks();
#endif
    return ITK_ok;
}

#ifdef MOTIF_UIF
/****************************************************************************/

int print_selections(UIF_message_t msg)
{
    int
        n_selected,
        ii,
        status;
    tag_t
        *selected;
    char
        name[WSO_name_size_c + 1];
        
    status = UIF_ask_folder_selections(msg.gizmo, &n_selected, &selected);
        if (status != ITK_ok)
        printf("UIF_ask_folder_selections failed status = %d\n", status);
        
    for (ii = 0; ii < n_selected; ii++)
    {
        status = WSOM_ask_name(selected[ii], name);
        if (status != ITK_ok)   
            printf("WSOM_ask_name failed status = %d\n", status);
        else
            printf("%d. tag: %d - name: %s\n", ii + 1, selected[ii], name);
    }
    MEM_free(selected);
    return status;
}
    

int GTAC_add_workspace_callbacks(UIF_gizmo_t home_folder_window)
{
    int
        status;
    status = UIF_add_callback(home_folder_window, 11016, print_selections, 
        UIF_after_callback, NULL);
    if (status != ITK_ok)
        printf("UIF_add_callbacks failed status = %d\n", status);
    return ITK_ok;
}

int USER_add_workspace_callbacks( UIF_gizmo_t home_folder_window )
/*
 *  Description
 *
 *      User exit to allow INFOMANAGER to be customized so user callbacks
 *      added to the initial home folder window with UIF_add_callback()
 *      will be called in addition to the INFOMANAGER supplied callbacks.
 *      This is called when workspace is being initialized and right after
 *      the initial main window has been created.
 *
 *  Restrictions
 *      *** NOTE: For applications encapsulated by the IMAN team (UG, Genius
 *                SAP, etc.) use the internal exit function
 *                IE_add_workspace_callbacks instead of this function.
 *
 *-+
 */
{
    int
        status;
    USER_report_init_module( home_folder_window); /* defined in user_exits.h*/


    CM_UIF_init_application( home_folder_window );

    ORG_UIF_add_workspace_callbacks( home_folder_window );

#ifdef REGISTER_FUI
    FUI_add_workspace_callbacks( home_folder_window );
#endif

    status = GTAC_add_workspace_callbacks(home_folder_window);

    return ITK_ok;
}

/****************************************************************************/

int USER_add_pse_callbacks( UIF_gizmo_t pse_window )
/*
 *  Description
 *
 *      User exit to allow adding callbacks to the PSE main window.
 *
 *  Restrictions
 *      *** NOTE: For applications encapsulated by the IMAN team (UG, Genius
 *                SAP, etc.) use the internal exit function
 *                IE_add_pse_callbacks instead of this function.
 *-+
 */
{
    return ITK_ok;
}
#endif

/****************************************************************************/

int USER_new_current_group ( tag_t group_tag, tag_t role_tag )
/*
 * Description
 *
 * user exit called when the IMAN user changes group.
 *
 */
{
    return ITK_ok;
}

