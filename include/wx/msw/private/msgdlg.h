///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/msgdlg.h
// Purpose:     helper functions used with native message dialog
// Author:      Rickard Westerlund
// Created:     2010-07-12
// RCS-ID:      $Id$
// Copyright:   (c) 2010 wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_PRIVATE_MSGDLG_H_
#define _WX_MSW_PRIVATE_MSGDLG_H_

#include "wx/msw/wrapcctl.h"
#include "wx/ptr_scpd.h"

// Macro to help identify if task dialogs are available.
#ifdef TD_WARNING_ICON
    #define wxHAS_MSW_TASKDIALOG
#endif

// Provides methods for creating a task dialog.
namespace wxMSWMessageDialog
{

#ifdef wxHAS_MSW_TASKDIALOG

    class wxMSWTaskDialogConfig
    {
    public:
        wxMSWTaskDialogConfig()
            : buttons(new TASKDIALOG_BUTTON[3]),
              parent(NULL),
              iconId(0),
              style(0),
              useCustomLabels(false)
            { }

        // initializes the object from a message dialog.
        wxMSWTaskDialogConfig(const wxMessageDialogBase& dlg);

        wxScopedArray<TASKDIALOG_BUTTON> buttons;
        wxWindow *parent;
        wxString caption;
        wxString message;
        wxString extendedMessage;
        long iconId;
        long style;
        bool useCustomLabels;
        wxString btnYesLabel;
        wxString btnNoLabel;
        wxString btnOKLabel;
        wxString btnCancelLabel;

        // Will create a task dialog with it's paremeters for it's creation
        // stored in the provided TASKDIALOGCONFIG parameter.
        // NOTE: The wxMSWTaskDialogConfig object needs to remain accessible
        // during the subsequent call to TaskDialogIndirect().
        void MSWCommonTaskDialogInit(TASKDIALOGCONFIG &tdc);

        // Used by MSWCommonTaskDialogInit() to add a regular button or a
        // button with a custom label if used.
        void AddTaskDialogButton(TASKDIALOGCONFIG &tdc,
                                 int btnCustomId,
                                 int btnCommonId,
                                 const wxString& customLabel);
    }; // class wxMSWTaskDialogConfig

#endif // wxHAS_MSW_TASKDIALOG

    bool HasNativeTaskDialog();

    // Translates standard MSW button IDs like IDCANCEL into an equivalent
    // wx constant such as wxCANCEL.
    int MSWTranslateReturnCode(int msAns);
}; // namespace wxMSWMessageDialog

#endif // _WX_MSW_PRIVATE_MSGDLG_H_
