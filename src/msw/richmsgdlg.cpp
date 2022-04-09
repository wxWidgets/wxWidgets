/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/richmsgdlg.cpp
// Purpose:     wxRichMessageDialog
// Author:      Rickard Westerlund
// Created:     2010-07-04
// Copyright:   (c) 2010 wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_RICHMSGDLG

#include "wx/richmsgdlg.h"
#include "wx/modalhook.h"

#ifndef WX_PRECOMP
    #include "wx/msw/private.h"
    #include "wx/utils.h"                   // for wxWindowDisabler
#endif

// This will define wxHAS_MSW_TASKDIALOG if we have support for it in the
// headers we use.
#include "wx/msw/private/msgdlg.h"

// ----------------------------------------------------------------------------
// wxRichMessageDialog
// ----------------------------------------------------------------------------

int wxRichMessageDialog::ShowModal()
{
    WX_HOOK_MODAL_DIALOG();

#ifdef wxHAS_MSW_TASKDIALOG
    using namespace wxMSWMessageDialog;

    if ( HasNativeTaskDialog() )
    {
        wxWindowDisabler disableOthers(this, GetParent());

        // create a task dialog
        WinStruct<TASKDIALOGCONFIG> tdc;
        wxMSWTaskDialogConfig wxTdc(*this);

        wxTdc.MSWCommonTaskDialogInit( tdc );

        // add a checkbox
        if ( !m_checkBoxText.empty() )
        {
            tdc.pszVerificationText = m_checkBoxText.t_str();
            if ( m_checkBoxValue )
                tdc.dwFlags |= TDF_VERIFICATION_FLAG_CHECKED;
        }

        // add collapsible footer
        if ( !m_detailedText.empty() )
            tdc.pszExpandedInformation = m_detailedText.t_str();

        // Add footer text
        if ( !m_footerText.empty() )
        {
            tdc.pszFooter = m_footerText.t_str();
            switch ( m_footerIcon )
            {
                case wxICON_INFORMATION:
                    tdc.pszFooterIcon = TD_INFORMATION_ICON;
                    break;
                case wxICON_WARNING:
                    tdc.pszFooterIcon = TD_WARNING_ICON;
                    break;
                case wxICON_ERROR:
                    tdc.pszFooterIcon = TD_ERROR_ICON;
                    break;
                case wxICON_AUTH_NEEDED:
                    tdc.pszFooterIcon = TD_SHIELD_ICON;
                    break;
            }
        }

        TaskDialogIndirect_t taskDialogIndirect = GetTaskDialogIndirectFunc();
        if ( !taskDialogIndirect )
            return wxID_CANCEL;

        // create the task dialog, process the answer and return it.
        BOOL checkBoxChecked;
        int msAns;
        HRESULT hr = taskDialogIndirect( &tdc, &msAns, NULL, &checkBoxChecked );
        if ( FAILED(hr) )
        {
            wxLogApiError( "TaskDialogIndirect", hr );
            return wxID_CANCEL;
        }
        m_checkBoxValue = checkBoxChecked != FALSE;

        // In case only an "OK" button was specified we actually created a
        // "Cancel" button (see comment in MSWCommonTaskDialogInit). This
        // results in msAns being IDCANCEL while we want IDOK (just like
        // how the native MessageBox function does with only an "OK" button).
        if ( (msAns == IDCANCEL)
            && !(GetMessageDialogStyle() & (wxYES_NO|wxCANCEL)) )
        {
            msAns = IDOK;
        }

        return MSWTranslateReturnCode( msAns );
    }
#endif // wxHAS_MSW_TASKDIALOG

    // use the generic version when task dialog is't available at either
    // compile or run-time.
    return wxGenericRichMessageDialog::ShowModal();
}

#endif // wxUSE_RICHMSGDLG
