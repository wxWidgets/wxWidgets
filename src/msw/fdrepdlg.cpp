/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/fdrepdlg.cpp
// Purpose:     wxFindReplaceDialog class
// Author:      Markus Greither
// Modified by: 31.07.01: VZ: integrated into wxWindows
// Created:     23/03/2001
// RCS-ID:
// Copyright:   (c) Markus Greither
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "fdrepdlg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_FINDREPLDLG

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

#include "wx/msw/private.h"

#if !defined(__WIN32__) || defined(__SALFORDC__) || defined(__WXWINE__)
    #include <commdlg.h>
#endif

#include "wx/fdrepdlg.h"

// ----------------------------------------------------------------------------
// functions prototypes
// ----------------------------------------------------------------------------

LRESULT APIENTRY FindReplaceWindowProc(HWND hwnd, WXUINT nMsg,
                                       WPARAM wParam, LPARAM lParam);

UINT CALLBACK wxFindReplaceDialogHookProc(HWND hwnd,
                                          UINT uiMsg,
                                          WPARAM wParam,
                                          LPARAM lParam);

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxFindReplaceDialog, wxDialog)

IMPLEMENT_DYNAMIC_CLASS(wxFindDialogEvent, wxCommandEvent)

// ----------------------------------------------------------------------------
// wxFindReplaceDialogImpl: the internals of wxFindReplaceDialog
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxFindReplaceDialogImpl
{
public:
    wxFindReplaceDialogImpl(wxFindReplaceDialog *dialog, int flagsWX);
    ~wxFindReplaceDialogImpl();

    void InitFindWhat(const wxString& str);
    void InitReplaceWith(const wxString& str);

    void SubclassDialog(HWND hwnd);

    static UINT GetFindDialogMessage() { return ms_msgFindDialog; }

    // only for passing to ::FindText or ::ReplaceText
    FINDREPLACE *GetPtrFindReplace() { return &m_findReplace; }

private:
    void InitString(const wxString& str, LPTSTR *ppStr, WORD *pLen);

    // the owner of the dialog
    HWND m_hwndOwner;

    // the previous window proc of our owner
    WNDPROC m_oldParentWndProc;

    // the find replace data used by the dialog
    FINDREPLACE m_findReplace;

    // registered Message for Dialog
    static UINT ms_msgFindDialog;
};

UINT wxFindReplaceDialogImpl::ms_msgFindDialog = 0;

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxFindReplaceDialogImpl
// ----------------------------------------------------------------------------

wxFindReplaceDialogImpl::wxFindReplaceDialogImpl(wxFindReplaceDialog *dialog,
                                                 int flagsWX)
{
    // get the identifier for the find dialog message if we don't have it yet
    if ( !ms_msgFindDialog )
    {
        ms_msgFindDialog = ::RegisterWindowMessage(FINDMSGSTRING);

        if ( !ms_msgFindDialog )
        {
            wxLogLastError(_T("RegisterWindowMessage(FINDMSGSTRING)"));
        }
    }

    m_hwndOwner = NULL;
    m_oldParentWndProc = NULL;

    wxZeroMemory(m_findReplace);

    // translate the flags

    // always set this to be able to set the title
    int flags = FR_ENABLEHOOK;

    if ( flagsWX & wxFR_NOMATCHCASE)
        flags |= FR_NOMATCHCASE;
    if ( flagsWX & wxFR_NOWHOLEWORD)
        flags |= FR_NOWHOLEWORD;
    if ( flagsWX & wxFR_NOUPDOWN)
        flags |= FR_NOUPDOWN;
    if ( flagsWX & wxFR_DOWN)
        flags |= FR_DOWN;

    m_findReplace.lStructSize = sizeof(FINDREPLACE);
    m_findReplace.hwndOwner = GetHwndOf(dialog->GetParent());
    m_findReplace.Flags = flags;

    m_findReplace.lCustData = (LPARAM)dialog;
    m_findReplace.lpfnHook = wxFindReplaceDialogHookProc;
}

void wxFindReplaceDialogImpl::InitString(const wxString& str,
                                         LPTSTR *ppStr, WORD *pLen)
{
    size_t len = str.length() + 1;
    if ( len < 80 )
    {
        // MSDN docs say that the buffer must be at least 80 chars
        len = 80;
    }

    *ppStr = new wxChar[len];
    wxStrcpy(*ppStr, str);
    *pLen = len;
}

void wxFindReplaceDialogImpl::InitFindWhat(const wxString& str)
{
    InitString(str, &m_findReplace.lpstrFindWhat, &m_findReplace.wFindWhatLen);
}

void wxFindReplaceDialogImpl::InitReplaceWith(const wxString& str)
{
    InitString(str,
               &m_findReplace.lpstrReplaceWith,
               &m_findReplace.wReplaceWithLen);
}

void wxFindReplaceDialogImpl::SubclassDialog(HWND hwnd)
{
    m_hwndOwner = hwnd;
    m_oldParentWndProc = (WNDPROC)::SetWindowLong
                                    (
                                        hwnd,
                                        GWL_WNDPROC,
                                        (LONG)FindReplaceWindowProc
                                    );

    // save it elsewhere to access it from FindReplaceWindowProc()
    (void)::SetWindowLong(hwnd, GWL_USERDATA, (LONG)m_oldParentWndProc);
}

wxFindReplaceDialogImpl::~wxFindReplaceDialogImpl()
{
    delete [] m_findReplace.lpstrFindWhat;
    delete [] m_findReplace.lpstrReplaceWith;

    if ( m_hwndOwner )
    {
        ::SetWindowLong(m_hwndOwner, GWL_WNDPROC, (LONG)m_oldParentWndProc);
    }
}

// ----------------------------------------------------------------------------
// Window Proc for handling RegisterWindowMessage(FINDMSGSTRING)
// ----------------------------------------------------------------------------

LRESULT APIENTRY FindReplaceWindowProc(HWND hwnd, WXUINT nMsg,
                                       WPARAM wParam, LPARAM lParam)
{
    if ( nMsg == wxFindReplaceDialogImpl::GetFindDialogMessage() )
    {
        FINDREPLACE *pFR = (FINDREPLACE *)lParam;
        wxFindReplaceDialog *dialog = (wxFindReplaceDialog *)pFR->lCustData;

        // map flags from Windows
        wxEventType evtType;

        bool replace = FALSE;
        if ( pFR->Flags & FR_DIALOGTERM )
        {
            evtType = wxEVT_COMMAND_FIND_CLOSE;
        }
        else if ( pFR->Flags & FR_FINDNEXT )
        {
            evtType = wxEVT_COMMAND_FIND_NEXT;
        }
        else if ( pFR->Flags & FR_REPLACE )
        {
            evtType = wxEVT_COMMAND_REPLACE;

            replace = TRUE;
        }
        else if ( pFR->Flags & FR_REPLACEALL )
        {
            evtType = wxEVT_COMMAND_REPLACE_ALL;

            replace = TRUE;
        }
        else
        {
            wxFAIL_MSG( _T("unknown find dialog event") );

            return 0;
        }

        wxUint32 flags = 0;
        if ( pFR->Flags & FR_DOWN )
            flags |= wxFR_DOWN;
        if ( pFR->Flags & FR_WHOLEWORD )
            flags |= wxFR_WHOLEWORD;
        if ( pFR->Flags & FR_MATCHCASE )
            flags |= wxFR_MATCHCASE;

        wxFindDialogEvent event(evtType, dialog->GetId());
        event.SetFlags(flags);
        event.SetFindString(pFR->lpstrFindWhat);
        if ( replace )
        {
            event.SetReplaceString(pFR->lpstrReplaceWith);
        }

        (void)dialog->GetEventHandler()->ProcessEvent(event);
    }

    WNDPROC wndProc = (WNDPROC)::GetWindowLong(hwnd, GWL_USERDATA);

    return ::CallWindowProc(wndProc, hwnd, nMsg, wParam, lParam);
};

// ----------------------------------------------------------------------------
// Find/replace dialog hook proc
// ----------------------------------------------------------------------------

UINT CALLBACK wxFindReplaceDialogHookProc(HWND hwnd,
                                          UINT uiMsg,
                                          WPARAM WXUNUSED(wParam),
                                          LPARAM lParam)
{
    if ( uiMsg == WM_INITDIALOG )
    {
        FINDREPLACE *pFR = (FINDREPLACE *)lParam;
        wxFindReplaceDialog *dialog = (wxFindReplaceDialog *)pFR->lCustData;

        ::SetWindowText(hwnd, dialog->GetTitle());

        // don't return FALSE from here or the dialog won't be shown
        return TRUE;
    }

    return 0;
}

// ============================================================================
// wxFindReplaceDialog implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxFindReplaceDialog ctors/dtor
// ----------------------------------------------------------------------------

void wxFindReplaceDialog::Init()
{
    m_impl = NULL;
    m_FindReplaceData = NULL;

    // as we're created in the hidden state, bring the internal flag in sync
    m_isShown = FALSE;
}

wxFindReplaceDialog::wxFindReplaceDialog(wxWindow *parent,
                                         wxFindReplaceData *data,
                                         const wxString &title)
                   : m_FindReplaceData(data)
{
    Init();

    (void)Create(parent, data, title);
}

wxFindReplaceDialog::~wxFindReplaceDialog()
{
    delete m_impl;
}

bool wxFindReplaceDialog::Create(wxWindow *parent,
                                 wxFindReplaceData *data,
                                 const wxString &title)
{
    m_FindReplaceData = data;
    m_parent = parent;

    SetTitle(title);

    // we must have a parent as it will get the messages from us
    return parent != NULL;
}

// ----------------------------------------------------------------------------
// wxFindReplaceDialog data access
// ----------------------------------------------------------------------------

void wxFindReplaceDialog::SetData(wxFindReplaceData *data)
{
    delete m_FindReplaceData;
    m_FindReplaceData = data;
}

// ----------------------------------------------------------------------------
// wxFindReplaceData show/hide
// ----------------------------------------------------------------------------

bool wxFindReplaceDialog::Show(bool show)
{
    if ( !wxWindowBase::Show(show) )
    {
        // visibility status didn't change
        return FALSE;
    }

    // do we already have the dialog window?
    if ( m_hWnd )
    {
        // yes, just use it
        return ::ShowWindow(GetHwnd(), show ? SW_SHOW : SW_HIDE);
    }

    if ( !show )
    {
        // well, it doesn't exist which is as good as being hidden
        return TRUE;
    }

    wxCHECK_MSG( m_FindReplaceData, FALSE, _T("call Create() first!") );

    wxASSERT_MSG( !m_impl, _T("why don't we have the window then?") );

    int flagsWX = m_FindReplaceData->GetFlags();

    m_impl = new wxFindReplaceDialogImpl(this, flagsWX);

    m_impl->InitFindWhat(m_FindReplaceData->GetFindString());

    if ( flagsWX & wxFR_REPLACEDIALOG)
    {
        m_impl->InitFindWhat(m_FindReplaceData->GetReplaceString());
    }

    // call the right function to show the dialog which does what we want
    HWND (*pfn)(FINDREPLACE *) = flagsWX & wxFR_REPLACEDIALOG ? ::ReplaceText
                                                              : ::FindText;
    m_hWnd = (WXHWND)(*pfn)(m_impl->GetPtrFindReplace());
    if ( !m_hWnd )
    {
        wxLogError(_("Failed to create the standard find/replace dialog (error code %d)"),
                   ::CommDlgExtendedError());

        delete m_impl;
        m_impl = NULL;

        return FALSE;
    }

    // subclass parent window in order to get FINDMSGSTRING message
    m_impl->SubclassDialog(GetHwndOf(m_parent));

    if ( !::ShowWindow((HWND)m_hWnd, SW_SHOW) )
    {
        wxLogLastError(_T("ShowWindow(find dialog)"));
    }

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxFindReplaceDialog title handling
// ----------------------------------------------------------------------------

// we set the title of this dialog in our jook proc but for now don't crash in
// the base class version because of m_hWnd == 0

void wxFindReplaceDialog::SetTitle( const wxString& title)
{
    m_title = title;
}

wxString wxFindReplaceDialog::GetTitle() const
{
    return m_title;
}

// ----------------------------------------------------------------------------
// wxFindReplaceDialog position/size
// ----------------------------------------------------------------------------

void wxFindReplaceDialog::DoSetSize(int WXUNUSED(x), int WXUNUSED(y),
                                    int WXUNUSED(width), int WXUNUSED(height),
                                    int WXUNUSED(sizeFlags))
{
    // ignore - we can't change the size of this standard dialog
    return;
}

// NB: of course, both of these functions are completely bogus, but it's better
//     than nothing
void wxFindReplaceDialog::DoGetSize(int *width, int *height) const
{
    // the standard dialog size
    if ( width )
        *width = 225;
    if ( height )
        *height = 324;
}

void wxFindReplaceDialog::DoGetClientSize(int *width, int *height) const
{
    // the standard dialog size
    if ( width )
        *width = 219;
    if ( height )
        *height = 299;
}

#endif // wxUSE_FINDREPLDLG

