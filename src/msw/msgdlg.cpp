/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/msgdlg.cpp
// Purpose:     wxMessageDialog
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_MSGDLG

#include "wx/msgdlg.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/dialog.h"
    #include "wx/hashmap.h"
#endif

#include "wx/msw/private.h"

// For MB_TASKMODAL
#ifdef __WXWINCE__
#include "wx/msw/wince/missing.h"
#endif

IMPLEMENT_CLASS(wxMessageDialog, wxDialog)

// there can potentially be one message box per thread so we use a hash map
// with thread ids as keys and (currently shown) message boxes as values
//
// TODO: replace this with wxTLS once it's available
WX_DECLARE_HASH_MAP(unsigned long, wxMessageDialog *,
                    wxIntegerHash, wxIntegerEqual,
                    wxMessageDialogMap);

namespace
{

wxMessageDialogMap& HookMap()
{
    static wxMessageDialogMap s_Map;

    return s_Map;
}

} // anonymous namespace

/* static */
WXLRESULT wxCALLBACK
wxMessageDialog::HookFunction(int code, WXWPARAM wParam, WXLPARAM lParam)
{
    // Find the thread-local instance of wxMessageDialog
    const DWORD tid = ::GetCurrentThreadId();
    wxMessageDialogMap::iterator node = HookMap().find(tid);
    wxCHECK_MSG( node != HookMap().end(), false,
                    wxT("bogus thread id in wxMessageDialog::Hook") );

    wxMessageDialog *  const wnd = node->second;

    const HHOOK hhook = (HHOOK)wnd->m_hook;
    const LRESULT rc = ::CallNextHookEx(hhook, code, wParam, lParam);

    if ( code == HC_ACTION && lParam )
    {
        const CWPRETSTRUCT * const s = (CWPRETSTRUCT *)lParam;

        if ( s->message == HCBT_ACTIVATE )
        {
            // we won't need this hook any longer
            ::UnhookWindowsHookEx(hhook);
            wnd->m_hook = NULL;
            HookMap().erase(tid);

            if ( wnd->GetMessageDialogStyle() & wxCENTER )
            {
                wnd->SetHWND(s->hwnd);
                wnd->Center(); // center on parent
                wnd->SetHWND(NULL);
            }
            //else: default behaviour, center on screen
        }
    }

    return rc;
}

int wxMessageDialog::ShowModal()
{
    if ( !wxTheApp->GetTopWindow() )
    {
        // when the message box is shown from wxApp::OnInit() (i.e. before the
        // message loop is entered), this must be done or the next message box
        // will never be shown - just try putting 2 calls to wxMessageBox() in
        // OnInit() to see it
        while ( wxTheApp->Pending() )
            wxTheApp->Dispatch();
    }

    // use the top level window as parent if none specified
    if ( !m_parent )
        m_parent = FindSuitableParent();
    HWND hWnd = m_parent ? GetHwndOf(m_parent) : NULL;

    // translate wx style in MSW
    unsigned int msStyle = MB_OK;
    const long wxStyle = GetMessageDialogStyle();
    if (wxStyle & wxYES_NO)
    {
#if !(defined(__SMARTPHONE__) && defined(__WXWINCE__))
        if (wxStyle & wxCANCEL)
            msStyle = MB_YESNOCANCEL;
        else
#endif // !(__SMARTPHONE__ && __WXWINCE__)
            msStyle = MB_YESNO;

        if (wxStyle & wxNO_DEFAULT)
            msStyle |= MB_DEFBUTTON2;
    }

    if (wxStyle & wxOK)
    {
        if (wxStyle & wxCANCEL)
            msStyle = MB_OKCANCEL;
        else
            msStyle = MB_OK;
    }
    if (wxStyle & wxICON_EXCLAMATION)
        msStyle |= MB_ICONEXCLAMATION;
    else if (wxStyle & wxICON_HAND)
        msStyle |= MB_ICONHAND;
    else if (wxStyle & wxICON_INFORMATION)
        msStyle |= MB_ICONINFORMATION;
    else if (wxStyle & wxICON_QUESTION)
        msStyle |= MB_ICONQUESTION;

    if ( wxStyle & wxSTAY_ON_TOP )
        msStyle |= MB_TOPMOST;

#ifndef __WXWINCE__
    if ( wxTheApp->GetLayoutDirection() == wxLayout_RightToLeft )
        msStyle |= MB_RTLREADING | MB_RIGHT;
#endif

    if (hWnd)
        msStyle |= MB_APPLMODAL;
    else
        msStyle |= MB_TASKMODAL;

    // per MSDN documentation for MessageBox() we can prefix the message with 2
    // right-to-left mark characters to tell the function to use RTL layout
    // (unfortunately this only works in Unicode builds)
    wxString message = GetFullMessage();
#if wxUSE_UNICODE
    if ( wxTheApp->GetLayoutDirection() == wxLayout_RightToLeft )
    {
        // NB: not all compilers support \u escapes
        static const wchar_t wchRLM = 0x200f;
        message.Prepend(wxString(wchRLM, 2));
    }
#endif // wxUSE_UNICODE

    // install the hook if we need to position the dialog in a non-default way
    if ( wxStyle & wxCENTER )
    {
        const DWORD tid = ::GetCurrentThreadId();
        m_hook = ::SetWindowsHookEx(WH_CALLWNDPROCRET,
                                    &wxMessageDialog::HookFunction, NULL, tid);
        HookMap()[tid] = this;
    }

    // do show the dialog
    int msAns = MessageBox(hWnd, message.wx_str(), m_caption.wx_str(), msStyle);
    int ans;
    switch (msAns)
    {
        default:
            wxFAIL_MSG(_T("unexpected ::MessageBox() return code"));
            // fall through

        case IDCANCEL:
            ans = wxID_CANCEL;
            break;
        case IDOK:
            ans = wxID_OK;
            break;
        case IDYES:
            ans = wxID_YES;
            break;
        case IDNO:
            ans = wxID_NO;
            break;
    }
    return ans;
}

#endif // wxUSE_MSGDLG
