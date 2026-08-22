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
#include "wx/weakref.h"

#ifndef WX_PRECOMP
    #include "wx/msw/private.h"
    #include "wx/utils.h"                   // for wxWindowDisabler
#endif

#include "wx/msw/private/msgdlg.h"

// ----------------------------------------------------------------------------
// wxRichMessageDialog
// ----------------------------------------------------------------------------

int wxRichMessageDialog::ShowModal()
{
    using namespace wxMSWMessageDialog;

    // Install exactly one hook for the public operation, including when a
    // failed native attempt continues through the generic fallback.
    WX_HOOK_MODAL_DIALOG();

    if ( HasNativeTaskDialog() )
    {
        const wxWeakRef<wxWindow> requestedParent(GetParent());
        wxWindowDisabler disableOthers(this, GetParentForModalDialog());

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

        // create the task dialog, process the answer and return it.
        BOOL checkBoxChecked = FALSE;
        int msAns = IDCANCEL;
        const HRESULT hr = InvokeTaskDialogIndirect(
            &tdc, &msAns, nullptr, &checkBoxChecked);
        if ( FAILED(hr) )
        {
            wxLogApiError( "TaskDialogIndirect", hr );
            if ( m_parent && !requestedParent.get() )
                m_parent = nullptr;
        }
        else
        {
            m_checkBoxValue = checkBoxChecked != FALSE;

            // In case only an "OK" button was specified we actually created
            // a "Cancel" button (see MSWCommonTaskDialogInit). Translate its
            // Escape/close result back to the public OK-only contract.
            if ( (msAns == IDCANCEL) &&
                    !(GetMessageDialogStyle() & (wxYES_NO | wxCANCEL)) )
            {
                msAns = IDOK;
            }

            if ( m_parent && !requestedParent.get() )
                m_parent = nullptr;

            return MSWTranslateReturnCode(msAns);
        }
    }

    // Use the generic version when the task dialog isn't available or when
    // its invocation failed. A native setup failure must not silently change
    // the public operation into Cancel.
    return DoShowModal();
}

#endif // wxUSE_RICHMSGDLG
