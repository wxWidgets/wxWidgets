/////////////////////////////////////////////////////////////////////////////
// Name:        textctrl.cpp
// Purpose:     wxTextCtrl
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

#ifdef __GNUG__
    #pragma implementation "textctrl.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/textctrl.h"
    #include "wx/settings.h"
    #include "wx/brush.h"
    #include "wx/utils.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/app.h"
#endif

#if wxUSE_CLIPBOARD
    #include "wx/clipbrd.h"
#endif

#include "wx/textfile.h"

#include <windowsx.h>

#include "wx/msw/private.h"

#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

#if wxUSE_IOSTREAMH
#   include <fstream.h>
#else
#   include <fstream>
#endif

#if wxUSE_RICHEDIT && (!defined(__GNUWIN32__) || defined(wxUSE_NORLANDER_HEADERS))
    #include <richedit.h>
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

#if wxUSE_RICHEDIT

// this module initializes RichEdit DLL if needed
class wxRichEditModule : public wxModule
{
public:
    virtual bool OnInit();
    virtual void OnExit();

    // get the version currently loaded, -1 if none
    static int GetLoadedVersion() { return ms_verRichEdit; }

    // load the richedit DLL of at least of required version
    static bool Load(int version = 1);

private:
    // the handle to richedit DLL and the version of the DLL loaded
    static HINSTANCE ms_hRichEdit;

    // the DLL version loaded or -1 if none
    static int ms_verRichEdit;

    DECLARE_DYNAMIC_CLASS(wxRichEditModule)
};

HINSTANCE wxRichEditModule::ms_hRichEdit = (HINSTANCE)NULL;
int       wxRichEditModule::ms_verRichEdit = -1;

IMPLEMENT_DYNAMIC_CLASS(wxRichEditModule, wxModule)

#endif // wxUSE_RICHEDIT

// ----------------------------------------------------------------------------
// event tables and other macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTextCtrl, wxControl)

BEGIN_EVENT_TABLE(wxTextCtrl, wxControl)
    EVT_CHAR(wxTextCtrl::OnChar)
    EVT_DROP_FILES(wxTextCtrl::OnDropFiles)

    EVT_MENU(wxID_CUT, wxTextCtrl::OnCut)
    EVT_MENU(wxID_COPY, wxTextCtrl::OnCopy)
    EVT_MENU(wxID_PASTE, wxTextCtrl::OnPaste)
    EVT_MENU(wxID_UNDO, wxTextCtrl::OnUndo)
    EVT_MENU(wxID_REDO, wxTextCtrl::OnRedo)

    EVT_UPDATE_UI(wxID_CUT, wxTextCtrl::OnUpdateCut)
    EVT_UPDATE_UI(wxID_COPY, wxTextCtrl::OnUpdateCopy)
    EVT_UPDATE_UI(wxID_PASTE, wxTextCtrl::OnUpdatePaste)
    EVT_UPDATE_UI(wxID_UNDO, wxTextCtrl::OnUpdateUndo)
    EVT_UPDATE_UI(wxID_REDO, wxTextCtrl::OnUpdateRedo)
END_EVENT_TABLE()


// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

wxTextCtrl::wxTextCtrl()
{
#if wxUSE_RICHEDIT
    m_isRich = FALSE;
#endif
}

bool wxTextCtrl::Create(wxWindow *parent, wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxValidator& validator,
                        const wxString& name)
{
    // base initialization
    if ( !CreateBase(parent, id, pos, size, style, validator, name) )
        return FALSE;

    if ( parent )
        parent->AddChild(this);

    // set colours
    SetupColours();

    // translate wxWin style flags to MSW ones, checking for consistency while
    // doing it
    long msStyle = ES_LEFT | WS_VISIBLE | WS_CHILD | WS_TABSTOP;
    if ( m_windowStyle & wxTE_MULTILINE )
    {
        wxASSERT_MSG( !(m_windowStyle & wxTE_PROCESS_ENTER),
                      wxT("wxTE_PROCESS_ENTER style is ignored for multiline "
                          "text controls (they always process it)") );

        msStyle |= ES_MULTILINE | ES_WANTRETURN;
        if ((m_windowStyle & wxTE_NO_VSCROLL) == 0)
            msStyle |= WS_VSCROLL;
        m_windowStyle |= wxTE_PROCESS_ENTER;
    }
    else
        msStyle |= ES_AUTOHSCROLL;

    if (m_windowStyle & wxHSCROLL)
        msStyle |= (WS_HSCROLL | ES_AUTOHSCROLL);

    if (m_windowStyle & wxTE_READONLY)
        msStyle |= ES_READONLY;

    if (m_windowStyle & wxHSCROLL)
        msStyle |= (WS_HSCROLL | ES_AUTOHSCROLL);
    if (m_windowStyle & wxTE_PASSWORD) // hidden input
        msStyle |= ES_PASSWORD;

    // we always want the characters and the arrows
    m_lDlgCode = DLGC_WANTCHARS | DLGC_WANTARROWS;

    // we may have several different cases:
    // 1. normal case: both TAB and ENTER are used for dialog navigation
    // 2. ctrl which wants TAB for itself: ENTER is used to pass to the next
    //    control in the dialog
    // 3. ctrl which wants ENTER for itself: TAB is used for dialog navigation
    // 4. ctrl which wants both TAB and ENTER: Ctrl-ENTER is used to pass to
    //    the next control
    if ( m_windowStyle & wxTE_PROCESS_ENTER )
        m_lDlgCode |= DLGC_WANTMESSAGE;
    if ( m_windowStyle & wxTE_PROCESS_TAB )
        m_lDlgCode |= DLGC_WANTTAB;

    // do create the control - either an EDIT or RICHEDIT
    wxString windowClass = wxT("EDIT");

#if wxUSE_RICHEDIT
    if ( m_windowStyle & wxTE_RICH )
    {
        static bool s_errorGiven = FALSE;   // MT-FIXME

        // only give the error msg once if the DLL can't be loaded
        if ( !s_errorGiven )
        {
            // first try to load the RichEdit DLL (will do nothing if already
            // done)
            if ( !wxRichEditModule::Load() )
            {
                wxLogError(_("Impossible to create a rich edit control, "
                             "using simple text control instead. Please "
                             "reinstall riched32.dll"));

                s_errorGiven = TRUE;
            }
        }

        if ( s_errorGiven )
        {
            m_isRich = FALSE;
        }
        else
        {
            msStyle |= ES_AUTOVSCROLL;
            m_isRich = TRUE;

            int ver = wxRichEditModule::GetLoadedVersion();
            if ( ver == 1 )
            {
                windowClass = wxT("RICHEDIT");
            }
            else
            {
#ifndef RICHEDIT_CLASS
                wxString RICHEDIT_CLASS;
                RICHEDIT_CLASS.Printf(_T("RichEdit%d0"), ver);
#ifdef wxUSE_UNICODE
                RICHEDIT_CLASS += _T('W');
#else // ANSI
                RICHEDIT_CLASS += _T('A');
#endif // Unicode/ANSI
#endif // !RICHEDIT_CLASS

                windowClass = RICHEDIT_CLASS;
            }
        }
    }
    else
        m_isRich = FALSE;
#endif // wxUSE_RICHEDIT

    bool want3D;
    WXDWORD exStyle = Determine3DEffects(WS_EX_CLIENTEDGE, &want3D);

    // Even with extended styles, need to combine with WS_BORDER for them to
    // look right.
    if ( want3D || wxStyleHasBorder(m_windowStyle) )
        msStyle |= WS_BORDER;

    // NB: don't use pos and size as CreateWindowEx arguments because they
    //     might be -1 in which case we should use the default values (and
    //     SetSize called below takes care of it)
    m_hWnd = (WXHWND)::CreateWindowEx(exStyle,
                                      windowClass,
                                      NULL,
                                      msStyle,
                                      0, 0, 0, 0,
                                      GetHwndOf(parent),
                                      (HMENU)m_windowId,
                                      wxGetInstance(),
                                      NULL);

    wxCHECK_MSG( m_hWnd, FALSE, wxT("Failed to create text ctrl") );

#if wxUSE_CTL3D
    if ( want3D )
    {
        Ctl3dSubclassCtl(GetHwnd());
        m_useCtl3D = TRUE;
    }
#endif

#if wxUSE_RICHEDIT
    if (m_isRich)
    {
        // Have to enable events
        ::SendMessage(GetHwnd(), EM_SETEVENTMASK, 0,
                      ENM_CHANGE | ENM_DROPFILES | ENM_SELCHANGE | ENM_UPDATE);
    }
#endif

    SubclassWin(GetHWND());

    // set font, position, size and initial value
    wxFont& fontParent = parent->GetFont();
    if ( fontParent.Ok() )
    {
        SetFont(fontParent);
    }
    else
    {
        SetFont(wxSystemSettings::GetSystemFont(wxSYS_SYSTEM_FONT));
    }

    // Causes a crash for Symantec C++ and WIN32 for some reason
#if !(defined(__SC__) && defined(__WIN32__))
    if ( !value.IsEmpty() )
    {
        SetValue(value);
    }
#endif

    SetSize(pos.x, pos.y, size.x, size.y);

    return TRUE;
}

// Make sure the window style (etc.) reflects the HWND style (roughly)
void wxTextCtrl::AdoptAttributesFromHWND()
{
  wxWindow::AdoptAttributesFromHWND();

  HWND hWnd = GetHwnd();
  long style = GetWindowLong(hWnd, GWL_STYLE);

  // retrieve the style to see whether this is an edit or richedit ctrl
#if wxUSE_RICHEDIT
  wxChar buf[256];

  GetClassName(hWnd, buf, WXSIZEOF(buf));

  if ( wxStricmp(buf, wxT("EDIT")) == 0 )
    m_isRich = FALSE;
  else
    m_isRich = TRUE;
#endif // wxUSE_RICHEDIT

  if (style & ES_MULTILINE)
    m_windowStyle |= wxTE_MULTILINE;
  if (style & ES_PASSWORD)
    m_windowStyle |= wxTE_PASSWORD;
  if (style & ES_READONLY)
    m_windowStyle |= wxTE_READONLY;
  if (style & ES_WANTRETURN)
    m_windowStyle |= wxTE_PROCESS_ENTER;
}

void wxTextCtrl::SetupColours()
{
    // FIXME why is bg colour not inherited from parent?
    SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_WINDOW));
    SetForegroundColour(GetParent()->GetForegroundColour());
}

// ----------------------------------------------------------------------------
// set/get the controls text
// ----------------------------------------------------------------------------

wxString wxTextCtrl::GetValue() const
{
    // we can't use wxGetWindowText() (i.e. WM_GETTEXT internally) for
    // retrieving more than 64Kb under Win9x
#if wxUSE_RICHEDIT
    if ( m_isRich )
    {
        wxString str;

        int len = GetWindowTextLength(GetHwnd()) + 1;
        wxChar *p = str.GetWriteBuf(len);

        TEXTRANGE textRange;
        textRange.chrg.cpMin = 0;
        textRange.chrg.cpMax = -1;
        textRange.lpstrText = p;

        (void)SendMessage(GetHwnd(), EM_GETTEXTRANGE, 0, (LPARAM)&textRange);

        // believe it or not, but EM_GETTEXTRANGE uses just CR ('\r') for the
        // newlines which is neither Unix nor Windows style (Win95 with
        // riched20.dll shows this behaviour) - convert it to something
        // reasonable
        for ( ; *p; p++ )
        {
            if ( *p == _T('\r') )
                *p = _T('\n');
        }

        str.UngetWriteBuf();

        return str;
    }
#endif // wxUSE_RICHEDIT

    // WM_GETTEXT uses standard DOS CR+LF (\r\n) convention - convert to the
    // same one as above for consitency
    wxString str = wxGetWindowText(GetHWND());

    return wxTextFile::Translate(str, wxTextFileType_Unix);
}

void wxTextCtrl::SetValue(const wxString& value)
{
    // if the text is long enough, it's faster to just set it instead of first
    // comparing it with the old one (chances are that it will be different
    // anyhow, this comparison is there to avoid flicker for small single-line
    // edit controls mostly)
    if ( (value.length() > 0x400) || (value != GetValue()) )
    {
        wxString valueDos = wxTextFile::Translate(value, wxTextFileType_Dos);

        SetWindowText(GetHwnd(), valueDos);

        AdjustSpaceLimit();
    }
}

void wxTextCtrl::WriteText(const wxString& value)
{
    wxString valueDos = wxTextFile::Translate(value, wxTextFileType_Dos);

    SendMessage(GetHwnd(), EM_REPLACESEL, 0, (LPARAM)valueDos.c_str());

    AdjustSpaceLimit();
}

void wxTextCtrl::AppendText(const wxString& text)
{
    SetInsertionPointEnd();
    WriteText(text);
}

void wxTextCtrl::Clear()
{
    SetWindowText(GetHwnd(), wxT(""));
}

// ----------------------------------------------------------------------------
// Clipboard operations
// ----------------------------------------------------------------------------

void wxTextCtrl::Copy()
{
    if (CanCopy())
    {
        HWND hWnd = GetHwnd();
        SendMessage(hWnd, WM_COPY, 0, 0L);
    }
}

void wxTextCtrl::Cut()
{
    if (CanCut())
    {
        HWND hWnd = GetHwnd();
        SendMessage(hWnd, WM_CUT, 0, 0L);
    }
}

void wxTextCtrl::Paste()
{
    if (CanPaste())
    {
        HWND hWnd = GetHwnd();
        SendMessage(hWnd, WM_PASTE, 0, 0L);
    }
}

bool wxTextCtrl::CanCopy() const
{
    // Can copy if there's a selection
    long from, to;
    GetSelection(& from, & to);
    return (from != to);
}

bool wxTextCtrl::CanCut() const
{
    // Can cut if there's a selection
    long from, to;
    GetSelection(& from, & to);
    return (from != to);
}

bool wxTextCtrl::CanPaste() const
{
#if wxUSE_RICHEDIT
    if (m_isRich)
    {
        int dataFormat = 0; // 0 == any format
        return (::SendMessage( GetHwnd(), EM_CANPASTE, (WPARAM) (UINT) dataFormat, 0) != 0);
    }
#endif
    if (!IsEditable())
        return FALSE;

    // Standard edit control: check for straight text on clipboard
    bool isTextAvailable = FALSE;
    if ( ::OpenClipboard(GetHwndOf(wxTheApp->GetTopWindow())) )
    {
        isTextAvailable = (::IsClipboardFormatAvailable(CF_TEXT) != 0);
        ::CloseClipboard();
    }

    return isTextAvailable;
}

// ----------------------------------------------------------------------------
// Accessors
// ----------------------------------------------------------------------------

void wxTextCtrl::SetEditable(bool editable)
{
    HWND hWnd = GetHwnd();
    SendMessage(hWnd, EM_SETREADONLY, (WPARAM)!editable, (LPARAM)0L);
}

void wxTextCtrl::SetInsertionPoint(long pos)
{
    HWND hWnd = GetHwnd();
#ifdef __WIN32__
#if wxUSE_RICHEDIT
    if ( m_isRich)
    {
        CHARRANGE range;
        range.cpMin = pos;
        range.cpMax = pos;
        SendMessage(hWnd, EM_EXSETSEL, 0, (LPARAM) &range);
        SendMessage(hWnd, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);
    }
    else
#endif // wxUSE_RICHEDIT
    {
        SendMessage(hWnd, EM_SETSEL, pos, pos);
        SendMessage(hWnd, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);
    }
#else // Win16
    SendMessage(hWnd, EM_SETSEL, 0, MAKELPARAM(pos, pos));
#endif // Win32/16

    static const char *nothing = "";
    SendMessage(hWnd, EM_REPLACESEL, 0, (LPARAM)nothing);
}

void wxTextCtrl::SetInsertionPointEnd()
{
    long pos = GetLastPosition();
    SetInsertionPoint(pos);
}

long wxTextCtrl::GetInsertionPoint() const
{
#if wxUSE_RICHEDIT
    if (m_isRich)
    {
        CHARRANGE range;
        range.cpMin = 0;
        range.cpMax = 0;
        SendMessage(GetHwnd(), EM_EXGETSEL, 0, (LPARAM) &range);
        return range.cpMin;
    }
#endif

    DWORD Pos = (DWORD)SendMessage(GetHwnd(), EM_GETSEL, 0, 0L);
    return Pos & 0xFFFF;
}

long wxTextCtrl::GetLastPosition() const
{
    HWND hWnd = GetHwnd();

    // Will always return a number > 0 (according to docs)
    int noLines = (int)SendMessage(hWnd, EM_GETLINECOUNT, (WPARAM)0, (LPARAM)0L);

    // This gets the char index for the _beginning_ of the last line
    int charIndex = (int)SendMessage(hWnd, EM_LINEINDEX, (WPARAM)(noLines-1), (LPARAM)0L);

    // Get number of characters in the last line. We'll add this to the character
    // index for the last line, 1st position.
    int lineLength = (int)SendMessage(hWnd, EM_LINELENGTH, (WPARAM)charIndex, (LPARAM)0L);

    return (long)(charIndex + lineLength);
}

// If the return values from and to are the same, there is no
// selection.
void wxTextCtrl::GetSelection(long* from, long* to) const
{
#if wxUSE_RICHEDIT
    if (m_isRich)
    {
        CHARRANGE charRange;
        ::SendMessage(GetHwnd(), EM_EXGETSEL, 0, (LPARAM) (CHARRANGE*) & charRange);

        *from = charRange.cpMin;
        *to = charRange.cpMax;

        return;
    }
#endif
    DWORD dwStart, dwEnd;
    WPARAM wParam = (WPARAM) (DWORD*) & dwStart; // receives starting position
    LPARAM lParam = (LPARAM) (DWORD*) & dwEnd;   // receives ending position

    ::SendMessage(GetHwnd(), EM_GETSEL, wParam, lParam);

    *from = dwStart;
    *to = dwEnd;
}

bool wxTextCtrl::IsEditable() const
{
    long style = ::GetWindowLong(GetHwnd(), GWL_STYLE);

    return ((style & ES_READONLY) == 0);
}

// ----------------------------------------------------------------------------
// Editing
// ----------------------------------------------------------------------------

void wxTextCtrl::Replace(long from, long to, const wxString& value)
{
#if wxUSE_CLIPBOARD
    HWND hWnd = GetHwnd();
    long fromChar = from;
    long toChar = to;

    // Set selection and remove it
#ifdef __WIN32__
    SendMessage(hWnd, EM_SETSEL, fromChar, toChar);
#else
    SendMessage(hWnd, EM_SETSEL, (WPARAM)0, (LPARAM)MAKELONG(fromChar, toChar));
#endif
    SendMessage(hWnd, WM_CUT, (WPARAM)0, (LPARAM)0);

    // Now replace with 'value', by pasting.
    wxSetClipboardData(wxDF_TEXT, (wxObject *) (const wxChar *)value, 0, 0);

    // Paste into edit control
    SendMessage(hWnd, WM_PASTE, (WPARAM)0, (LPARAM)0L);
#else
    wxFAIL_MSG("wxTextCtrl::Replace not implemented if wxUSE_CLIPBOARD is 0.");
#endif
}

void wxTextCtrl::Remove(long from, long to)
{
    HWND hWnd = GetHwnd();
    long fromChar = from;
    long toChar = to;

    // Cut all selected text
#ifdef __WIN32__
    SendMessage(hWnd, EM_SETSEL, fromChar, toChar);
#else
    SendMessage(hWnd, EM_SETSEL, (WPARAM)0, (LPARAM)MAKELONG(fromChar, toChar));
#endif
    SendMessage(hWnd, WM_CUT, (WPARAM)0, (LPARAM)0);
}

void wxTextCtrl::SetSelection(long from, long to)
{
    HWND hWnd = GetHwnd();
    long fromChar = from;
    long toChar = to;

    // if from and to are both -1, it means (in wxWindows) that all text should
    // be selected. Translate into Windows convention
    if ((from == -1) && (to == -1))
    {
      fromChar = 0;
      toChar = -1;
    }

#ifdef __WIN32__
    SendMessage(hWnd, EM_SETSEL, (WPARAM)fromChar, (LPARAM)toChar);
    SendMessage(hWnd, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);
#else
    // WPARAM is 0: selection is scrolled into view
    SendMessage(hWnd, EM_SETSEL, (WPARAM)0, (LPARAM)MAKELONG(fromChar, toChar));
#endif
}

bool wxTextCtrl::LoadFile(const wxString& file)
{
    if ( wxTextCtrlBase::LoadFile(file) )
    {
        // update the size limit if needed
        AdjustSpaceLimit();

        return TRUE;
    }

    return FALSE;
}

bool wxTextCtrl::IsModified() const
{
    return (SendMessage(GetHwnd(), EM_GETMODIFY, 0, 0) != 0);
}

// Makes 'unmodified'
void wxTextCtrl::DiscardEdits()
{
    SendMessage(GetHwnd(), EM_SETMODIFY, FALSE, 0L);
}

int wxTextCtrl::GetNumberOfLines() const
{
    return (int)SendMessage(GetHwnd(), EM_GETLINECOUNT, (WPARAM)0, (LPARAM)0);
}

long wxTextCtrl::XYToPosition(long x, long y) const
{
    HWND hWnd = GetHwnd();

    // This gets the char index for the _beginning_ of this line
    int charIndex = (int)SendMessage(hWnd, EM_LINEINDEX, (WPARAM)y, (LPARAM)0);
    return (long)(x + charIndex);
}

bool wxTextCtrl::PositionToXY(long pos, long *x, long *y) const
{
    HWND hWnd = GetHwnd();

    // This gets the line number containing the character
    int lineNo;
#if wxUSE_RICHEDIT
    if ( m_isRich )
    {
        lineNo = (int)SendMessage(hWnd, EM_EXLINEFROMCHAR, 0, (LPARAM)pos);
    }
    else
#endif // wxUSE_RICHEDIT
        lineNo = (int)SendMessage(hWnd, EM_LINEFROMCHAR, (WPARAM)pos, 0);

    if ( lineNo == -1 )
    {
        // no such line
        return FALSE;
    }

    // This gets the char index for the _beginning_ of this line
    int charIndex = (int)SendMessage(hWnd, EM_LINEINDEX, (WPARAM)lineNo, (LPARAM)0);
    if ( charIndex == -1 )
    {
        return FALSE;
    }

    // The X position must therefore be the different between pos and charIndex
    if ( x )
        *x = (long)(pos - charIndex);
    if ( y )
        *y = (long)lineNo;

    return TRUE;
}

void wxTextCtrl::ShowPosition(long pos)
{
    HWND hWnd = GetHwnd();

    // To scroll to a position, we pass the number of lines and characters
    // to scroll *by*. This means that we need to:
    // (1) Find the line position of the current line.
    // (2) Find the line position of pos.
    // (3) Scroll by (pos - current).
    // For now, ignore the horizontal scrolling.

    // Is this where scrolling is relative to - the line containing the caret?
    // Or is the first visible line??? Try first visible line.
//    int currentLineLineNo1 = (int)SendMessage(hWnd, EM_LINEFROMCHAR, (WPARAM)-1, (LPARAM)0L);

    int currentLineLineNo = (int)SendMessage(hWnd, EM_GETFIRSTVISIBLELINE, (WPARAM)0, (LPARAM)0L);

    int specifiedLineLineNo = (int)SendMessage(hWnd, EM_LINEFROMCHAR, (WPARAM)pos, (LPARAM)0L);

    int linesToScroll = specifiedLineLineNo - currentLineLineNo;

    if (linesToScroll != 0)
      (void)SendMessage(hWnd, EM_LINESCROLL, (WPARAM)0, (LPARAM)linesToScroll);
}

int wxTextCtrl::GetLineLength(long lineNo) const
{
    long charIndex = XYToPosition(0, lineNo);
    int len = (int)SendMessage(GetHwnd(), EM_LINELENGTH, charIndex, 0);
    return len;
}

wxString wxTextCtrl::GetLineText(long lineNo) const
{
    size_t len = (size_t)GetLineLength(lineNo) + 1;
    char *buf = (char *)malloc(len);
    *(WORD *)buf = len;
    int noChars = (int)SendMessage(GetHwnd(), EM_GETLINE, lineNo, (LPARAM)buf);
    buf[noChars] = 0;

    wxString str(buf);

    free(buf);

    return str;
}

// ----------------------------------------------------------------------------
// Undo/redo
// ----------------------------------------------------------------------------

void wxTextCtrl::Undo()
{
    if (CanUndo())
    {
        ::SendMessage(GetHwnd(), EM_UNDO, 0, 0);
    }
}

void wxTextCtrl::Redo()
{
    if (CanRedo())
    {
        // Same as Undo, since Undo undoes the undo, i.e. a redo.
        ::SendMessage(GetHwnd(), EM_UNDO, 0, 0);
    }
}

bool wxTextCtrl::CanUndo() const
{
    return (::SendMessage(GetHwnd(), EM_CANUNDO, 0, 0) != 0);
}

bool wxTextCtrl::CanRedo() const
{
    return (::SendMessage(GetHwnd(), EM_CANUNDO, 0, 0) != 0);
}

// ----------------------------------------------------------------------------
// implemenation details
// ----------------------------------------------------------------------------

void wxTextCtrl::Command(wxCommandEvent & event)
{
    SetValue(event.GetString());
    ProcessCommand (event);
}

void wxTextCtrl::OnDropFiles(wxDropFilesEvent& event)
{
    // By default, load the first file into the text window.
    if (event.GetNumberOfFiles() > 0)
    {
        LoadFile(event.GetFiles()[0]);
    }
}

void wxTextCtrl::OnChar(wxKeyEvent& event)
{
    switch ( event.KeyCode() )
    {
        case WXK_RETURN:
            if ( !(m_windowStyle & wxTE_MULTILINE) )
            {
                wxCommandEvent event(wxEVT_COMMAND_TEXT_ENTER, m_windowId);
                event.SetEventObject( this );
                if ( GetEventHandler()->ProcessEvent(event) )
                    return;
            }
            //else: multiline controls need Enter for themselves

            break;

        case WXK_TAB:
            // always produce navigation event - even if we process TAB
            // ourselves the fact that we got here means that the user code
            // decided to skip processing of this TAB - probably to let it
            // do its default job.
            //
            // NB: Notice that Ctrl-Tab is handled elsewhere and Alt-Tab is
            //     handled by Windows
            {
                wxNavigationKeyEvent eventNav;
                eventNav.SetDirection(!event.ShiftDown());
                eventNav.SetWindowChange(FALSE);
                eventNav.SetEventObject(this);

                if ( GetEventHandler()->ProcessEvent(eventNav) )
                    return;
            }
            break;

        default:
            event.Skip();
            return;
    }

    // don't just call event.Skip() because this will cause TABs and ENTERs
    // be passed upwards and we don't always want this - instead process it
    // right here

    // FIXME
    event.Skip();
}

bool wxTextCtrl::MSWCommand(WXUINT param, WXWORD WXUNUSED(id))
{
    switch (param)
    {
        case EN_SETFOCUS:
        case EN_KILLFOCUS:
            {
                wxFocusEvent event(param == EN_KILLFOCUS ? wxEVT_KILL_FOCUS
                        : wxEVT_SET_FOCUS,
                        m_windowId);
                event.SetEventObject( this );
                GetEventHandler()->ProcessEvent(event);
            }
            break;

        case EN_CHANGE:
            {
                wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, m_windowId);
                wxString val(GetValue());
                if ( !val.IsNull() )
                    event.m_commandString = WXSTRINGCAST val;
                event.SetEventObject( this );
                ProcessCommand(event);
            }
            break;

        case EN_MAXTEXT:
            // the text size limit has been hit - increase it
            AdjustSpaceLimit();
            break;

            // the other notification messages are not processed
        case EN_UPDATE:
        case EN_ERRSPACE:
        case EN_HSCROLL:
        case EN_VSCROLL:
        default:
            return FALSE;
    }

    // processed
    return TRUE;
}

void wxTextCtrl::AdjustSpaceLimit()
{
#ifndef __WIN16__
    unsigned int len = ::GetWindowTextLength(GetHwnd()),
    limit = ::SendMessage(GetHwnd(), EM_GETLIMITTEXT, 0, 0);
    if ( len >= limit )
    {
        limit = len + 0x8000;    // 32Kb

#if wxUSE_RICHEDIT
        if ( m_isRich )
        {
            // as a nice side effect, this also allows passing limit > 64Kb
            ::SendMessage(GetHwnd(), EM_EXLIMITTEXT, 0, limit);
        }
        else
#endif // wxUSE_RICHEDIT
        {
            if ( limit > 0xffff )
            {
                // this will set it to a platform-dependent maximum (much more
                // than 64Kb under NT)
                limit = 0;
            }

            ::SendMessage(GetHwnd(), EM_LIMITTEXT, limit, 0);
        }
    }
#endif // !Win16
}

bool wxTextCtrl::AcceptsFocus() const
{
    // we don't want focus if we can't be edited
    return IsEditable() && wxControl::AcceptsFocus();
}

wxSize wxTextCtrl::DoGetBestSize() const
{
    int cx, cy;
    wxGetCharSize(GetHWND(), &cx, &cy, &GetFont());

    int wText = DEFAULT_ITEM_WIDTH;

    int hText = EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy);
    if ( m_windowStyle & wxTE_MULTILINE )
    {
        hText *= wxMin(GetNumberOfLines(), 5);
    }
    //else: for single line control everything is ok

    return wxSize(wText, hText);
}

// ----------------------------------------------------------------------------
// standard handlers for standard edit menu events
// ----------------------------------------------------------------------------

void wxTextCtrl::OnCut(wxCommandEvent& event)
{
    Cut();
}

void wxTextCtrl::OnCopy(wxCommandEvent& event)
{
    Copy();
}

void wxTextCtrl::OnPaste(wxCommandEvent& event)
{
    Paste();
}

void wxTextCtrl::OnUndo(wxCommandEvent& event)
{
    Undo();
}

void wxTextCtrl::OnRedo(wxCommandEvent& event)
{
    Redo();
}

void wxTextCtrl::OnUpdateCut(wxUpdateUIEvent& event)
{
    event.Enable( CanCut() );
}

void wxTextCtrl::OnUpdateCopy(wxUpdateUIEvent& event)
{
    event.Enable( CanCopy() );
}

void wxTextCtrl::OnUpdatePaste(wxUpdateUIEvent& event)
{
    event.Enable( CanPaste() );
}

void wxTextCtrl::OnUpdateUndo(wxUpdateUIEvent& event)
{
    event.Enable( CanUndo() );
}

void wxTextCtrl::OnUpdateRedo(wxUpdateUIEvent& event)
{
    event.Enable( CanRedo() );
}

// ----------------------------------------------------------------------------
// wxRichEditModule
// ----------------------------------------------------------------------------

#if wxUSE_RICHEDIT

bool wxRichEditModule::OnInit()
{
    // don't do anything - we will load it when needed
    return TRUE;
}

void wxRichEditModule::OnExit()
{
    if ( ms_hRichEdit )
    {
        FreeLibrary(ms_hRichEdit);
    }
}

/* static */
bool wxRichEditModule::Load(int version)
{
    wxCHECK_MSG( version >= 1 && version <= 3, FALSE,
                 _T("incorrect richedit control version requested") );

    if ( version <= ms_verRichEdit )
    {
        // we've already got this or better
        return TRUE;
    }

    if ( ms_hRichEdit )
    {
        ::FreeLibrary(ms_hRichEdit);
    }

    // always try load riched20.dll first - like this we won't have to reload
    // it later if we're first asked for RE 1 and then for RE 2 or 3
    wxString dllname = _T("riched20.dll");
    ms_hRichEdit = ::LoadLibrary(dllname);
    ms_verRichEdit = 2; // no way to tell if it's 2 or 3, assume 2

    if ( !ms_hRichEdit && (version == 1) )
    {
        // fall back to RE 1
        dllname = _T("riched32.dll");
        ms_hRichEdit = ::LoadLibrary(dllname);
        ms_verRichEdit = 1;
    }

    if ( !ms_hRichEdit )
    {
        wxLogSysError(_("Could not load Rich Edit DLL '%s'"), dllname.c_str());

        ms_verRichEdit = -1;

        return FALSE;
    }

    return TRUE;
}

#endif // wxUSE_RICHEDIT

