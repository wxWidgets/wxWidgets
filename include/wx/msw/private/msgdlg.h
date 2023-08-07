///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/msgdlg.h
// Purpose:     helper functions used with native message dialog
// Author:      Rickard Westerlund
// Created:     2010-07-12
// Copyright:   (c) 2010 wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_PRIVATE_MSGDLG_H_
#define _WX_MSW_PRIVATE_MSGDLG_H_

#include "wx/msw/wrapcctl.h"
#include "wx/scopedarray.h"

// Provides methods for creating a task dialog.
namespace wxMSWMessageDialog
{

    class wxMSWTaskDialogConfig
    {
    public:
        enum { MAX_BUTTONS = 4  };

        wxMSWTaskDialogConfig()
            : buttons(new TASKDIALOG_BUTTON[MAX_BUTTONS]),
              parent(nullptr),
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
        wxString btnHelpLabel;

        // Will create a task dialog with its parameters for its creation
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


    typedef HRESULT (WINAPI *TaskDialogIndirect_t)(const TASKDIALOGCONFIG *,
                                                   int *, int *, BOOL *);

    // Return the pointer to TaskDialogIndirect(). It can return a null pointer
    // if the task dialog is not available, which may happen even under modern
    // OS versions when using comctl32.dll v5, as it happens if the application
    // doesn't provide a manifest specifying that it wants to use v6.
    TaskDialogIndirect_t GetTaskDialogIndirectFunc();

    // Return true if the task dialog is available, but we don't actually need
    // to show it yet (if we do, then GetTaskDialogIndirectFunc() should be
    // used directly).
    bool HasNativeTaskDialog();

    // Translates standard MSW button IDs like IDCANCEL into an equivalent
    // wx constant such as wxCANCEL.
    int MSWTranslateReturnCode(int msAns);
}; // namespace wxMSWMessageDialog

#endif // _WX_MSW_PRIVATE_MSGDLG_H_
