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

// there is no hook support under CE so we can't use the code for message box
// positioning there
#ifndef __WXWINCE__
    #define wxUSE_MSGBOX_HOOK 1
#else
    #define wxUSE_MSGBOX_HOOK 0
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/dialog.h"
    #if wxUSE_MSGBOX_HOOK
        #include "wx/hashmap.h"
    #endif
#endif

#include "wx/msw/private.h"
#include "wx/msw/private/button.h"
#include "wx/msw/private/metrics.h"

#if wxUSE_MSGBOX_HOOK
    #include "wx/fontutil.h"
#endif

// For MB_TASKMODAL
#ifdef __WXWINCE__
    #include "wx/msw/wince/missing.h"
#endif

IMPLEMENT_CLASS(wxMessageDialog, wxDialog)

#if wxUSE_MSGBOX_HOOK

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

    if ( code == HCBT_ACTIVATE )
    {
        // we won't need this hook any longer
        ::UnhookWindowsHookEx(hhook);
        wnd->m_hook = NULL;
        HookMap().erase(tid);

        wnd->SetHWND((HWND)wParam);

        // centre the message box on its parent if requested
        if ( wnd->GetMessageDialogStyle() & wxCENTER )
            wnd->Center(); // center on parent
        //else: default behaviour, center on screen

        // also update the labels if necessary
        if ( wnd->HasCustomLabels() )
            wnd->AdjustButtonLabels();

        // there seems to be no reason to leave it set
        wnd->SetHWND(NULL);
    }

    return rc;
}

namespace
{

// helper of AdjustButtonLabels(): set window position expressed in screen
// coordinates, whether the window is child or top level
void MoveWindowToScreenRect(HWND hwnd, RECT rc)
{
    if ( const HWND hwndParent = ::GetAncestor(hwnd, GA_PARENT) )
    {
        // map to parent window coordinates (notice that a RECT is laid out as
        // 2 consecutive POINTs)
        ::MapWindowPoints(HWND_DESKTOP, hwndParent,
                          reinterpret_cast<POINT *>(&rc), 2);
    }

    ::MoveWindow(hwnd,
                 rc.left, rc.top,
                 rc.right - rc.left, rc.bottom - rc.top,
                 FALSE);
}

// helper of AdjustButtonLabels(): move the given window by dx
//
// works for both child and top level windows
void OffsetWindow(HWND hwnd, int dx)
{
    RECT rc = wxGetWindowRect(hwnd);

    rc.left += dx;
    rc.right += dx;

    MoveWindowToScreenRect(hwnd, rc);
}

} // anonymous namespace

void wxMessageDialog::AdjustButtonLabels()
{
    // changing the button labels is the easy part but we also need to ensure
    // that the buttons are big enough for the label strings and increase their
    // size (and hence the size of the message box itself) if they are not

    // TODO-RTL: check whether this works correctly in RTL

    // the order in this array is the one in which buttons appear in the
    // message box
    const static struct ButtonAccessors
    {
        int id;
        wxString (wxMessageDialog::*getter)() const;
    }
    buttons[] =
    {
        { IDYES,    &wxMessageDialog::GetYesLabel    },
        { IDNO,     &wxMessageDialog::GetNoLabel     },
        { IDOK,     &wxMessageDialog::GetOKLabel     },
        { IDCANCEL, &wxMessageDialog::GetCancelLabel },
    };

    // this contains the amount by which we increased the message box width
    int dx = 0;

    const NONCLIENTMETRICS& ncm = wxMSWImpl::GetNonClientMetrics();
    const wxFont fontMsgBox(wxNativeFontInfo(ncm.lfMessageFont));

    // we want to use this font in GetTextExtent() calls below but we don't
    // want to send WM_SETFONT to the message box, who knows how is it going to
    // react to it (right now it doesn't seem to do anything but what if this
    // changes)
    wxWindowBase::SetFont(fontMsgBox);

    for ( unsigned n = 0; n < WXSIZEOF(buttons); n++ )
    {
        const HWND hwndBtn = ::GetDlgItem(GetHwnd(), buttons[n].id);
        if ( !hwndBtn )
            continue;   // it's ok, not all buttons are always present

        const wxString label = (this->*buttons[n].getter)();
        const wxSize sizeLabel = wxWindowBase::GetTextExtent(label);

        // check if the button is big enough for this label
        RECT rc = wxGetWindowRect(hwndBtn);
        const int widthOld = rc.right - rc.left;
        const int widthNew = wxMSWButton::GetFittingSize(this, sizeLabel).x;
        const int dw = widthNew - widthOld;
        if ( dw > 0 )
        {
            // we need to resize the button
            rc.right += dw;
            MoveWindowToScreenRect(hwndBtn, rc);

            // and also move all the other buttons
            for ( unsigned m = n + 1; m < WXSIZEOF(buttons); m++ )
            {
                const HWND hwndBtnNext = ::GetDlgItem(GetHwnd(), buttons[m].id);
                if ( hwndBtnNext )
                    OffsetWindow(hwndBtnNext, dw);
            }

            dx += dw;
        }

        ::SetWindowText(hwndBtn, label.wx_str());
    }


    // resize the message box itself if needed
    if ( dx )
        OffsetWindow(GetHwnd(), dx);

    // surprisingly, we don't need to resize the static text control, it seems
    // to adjust itself to the new size, at least under Windows 2003
    // (TODO: test if this happens on older Windows versions)
}

#endif // wxUSE_MSGBOX_HOOK


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

#if wxUSE_MSGBOX_HOOK
    // install the hook if we need to position the dialog in a non-default way
    // or change the labels
    if ( (wxStyle & wxCENTER) || HasCustomLabels() )
    {
        const DWORD tid = ::GetCurrentThreadId();
        m_hook = ::SetWindowsHookEx(WH_CBT,
                                    &wxMessageDialog::HookFunction, NULL, tid);
        HookMap()[tid] = this;
    }
#endif // wxUSE_MSGBOX_HOOK

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
