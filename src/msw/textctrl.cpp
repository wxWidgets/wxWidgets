/////////////////////////////////////////////////////////////////////////////
// Name:        msw/textctrl.cpp
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

#if wxUSE_TEXTCTRL

#ifndef WX_PRECOMP
    #include "wx/textctrl.h"
    #include "wx/settings.h"
    #include "wx/brush.h"
    #include "wx/utils.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/app.h"
#endif

#include "wx/module.h"

#if wxUSE_CLIPBOARD
    #include "wx/clipbrd.h"
#endif

#include "wx/textfile.h"

#include <windowsx.h>

#include "wx/msw/private.h"

#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

#if wxUSE_RICHEDIT && (!defined(__GNUWIN32_OLD__) || defined(__CYGWIN10__))
    #include <richedit.h>
#endif

// old mingw32 doesn't define this
#ifndef CFM_CHARSET
    #define CFM_CHARSET 0x08000000
#endif // CFM_CHARSET

#ifndef CFM_BACKCOLOR
    #define CFM_BACKCOLOR 0x04000000
#endif

// cygwin does not have these defined for richedit
#ifndef ENM_LINK
    #define ENM_LINK 0x04000000
#endif

#ifndef EM_AUTOURLDETECT
    #define EM_AUTOURLDETECT (WM_USER + 91)
#endif

#ifndef EN_LINK
    #define EN_LINK 0x070b

    typedef struct _enlink
    {
        NMHDR nmhdr;
        UINT msg;
        WPARAM wParam;
        LPARAM lParam;
        CHARRANGE chrg;
    } ENLINK;
#endif // ENLINK

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
#ifdef __WIN16__
    EVT_ERASE_BACKGROUND(wxTextCtrl::OnEraseBackground)
#endif
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

    // translate wxWin style flags to MSW ones, checking for consistency while
    // doing it
    long msStyle = ES_LEFT | WS_TABSTOP;

    if ( m_windowStyle & wxCLIP_SIBLINGS )
        msStyle |= WS_CLIPSIBLINGS;

    if ( m_windowStyle & wxTE_MULTILINE )
    {
        wxASSERT_MSG( !(m_windowStyle & wxTE_PROCESS_ENTER),
                      wxT("wxTE_PROCESS_ENTER style is ignored for multiline text controls (they always process it)") );

        msStyle |= ES_MULTILINE | ES_WANTRETURN;
        if ((m_windowStyle & wxTE_NO_VSCROLL) == 0)
            msStyle |= WS_VSCROLL;
        m_windowStyle |= wxTE_PROCESS_ENTER;
    }
    else // !multiline
    {
        // there is really no reason to not have this style for single line
        // text controls
        msStyle |= ES_AUTOHSCROLL;
    }

    if ( m_windowStyle & wxHSCROLL )
        msStyle |= WS_HSCROLL | ES_AUTOHSCROLL;

    if ( m_windowStyle & wxTE_READONLY )
        msStyle |= ES_READONLY;

    if ( m_windowStyle & wxTE_PASSWORD )
        msStyle |= ES_PASSWORD;

    if ( m_windowStyle & wxTE_AUTO_SCROLL )
        msStyle |= ES_AUTOHSCROLL;

    if ( m_windowStyle & wxTE_NOHIDESEL )
        msStyle |= ES_NOHIDESEL;

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
                wxLogError(_("Impossible to create a rich edit control, using simple text control instead. Please reinstall riched32.dll"));

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
            // Experimental: this seems to help with the scroll problem. See messages from Jekabs Andrushaitis <j.andrusaitis@konts.lv>
            // wx-dev list, entitled "[wx-dev] wxMSW-EVT_KEY_DOWN and wxMSW-wxTextCtrl" and "[wx-dev] TextCtrl (RichEdit)"
            // Unfortunately, showing the selection in blue when the control doesn't have
            // the focus is non-standard behaviour, and we need to find another workaround.
            //msStyle |= ES_NOHIDESEL ;
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
#if wxUSE_UNICODE
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

    // we need to turn '\n's into "\r\n"s for the multiline controls
    wxString valueWin;
    if ( m_windowStyle & wxTE_MULTILINE )
    {
        valueWin = wxTextFile::Translate(value, wxTextFileType_Dos);
    }
    else // single line
    {
        valueWin = value;
    }

    if ( !MSWCreateControl(windowClass, msStyle, pos, size, valueWin) )
        return FALSE;

    SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_WINDOW));

#if wxUSE_RICHEDIT
    if (m_isRich)
    {
        // have to enable events manually
        LPARAM mask = ENM_CHANGE | ENM_DROPFILES | ENM_SELCHANGE | ENM_UPDATE;

        if ( m_windowStyle & wxTE_AUTO_URL )
        {
            mask |= ENM_LINK;

            ::SendMessage(GetHwnd(), EM_AUTOURLDETECT, TRUE, 0);
        }

        ::SendMessage(GetHwnd(), EM_SETEVENTMASK, 0, mask);
    }
#endif // wxUSE_RICHEDIT

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

        int len = GetWindowTextLength(GetHwnd());
        if ( len )
        {
            // alloc one extra WORD as needed by the control
            wxChar *p = str.GetWriteBuf(++len);

            TEXTRANGE textRange;
            textRange.chrg.cpMin = 0;
            textRange.chrg.cpMax = -1;
            textRange.lpstrText = p;

            (void)SendMessage(GetHwnd(), EM_GETTEXTRANGE, 0, (LPARAM)&textRange);

            // believe it or not, but EM_GETTEXTRANGE uses just CR ('\r') for
            // the newlines which is neither Unix nor Windows style (Win95 with
            // riched20.dll shows this behaviour) - convert it to something
            // reasonable
            for ( ; *p; p++ )
            {
                if ( *p == _T('\r') )
                    *p = _T('\n');
            }

            str.UngetWriteBuf();
        }
        //else: no text at all, leave the string empty

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

        SetWindowText(GetHwnd(), valueDos.c_str());

        // for compatibility with the GTK and because it is more logical, we
        // move the cursor to the end of the text after SetValue()

        // GRG, Jun/2000: Changed this back after a lot of discussion
        //   in the lists. wxWindows 2.2 will have a set of flags to
        //   customize this behaviour.
        //SetInsertionPointEnd();

        AdjustSpaceLimit();
    }
}

void wxTextCtrl::WriteText(const wxString& value)
{
    wxString valueDos = wxTextFile::Translate(value, wxTextFileType_Dos);

#if wxUSE_RICHEDIT
    // ensure that the new text will be in the default style
    if ( IsRich() &&
            (m_defaultStyle.HasFont() || m_defaultStyle.HasTextColour()) )
    {
        long start, end;
        GetSelection(&start, &end);
        SetStyle(start, end, m_defaultStyle );
    }
#endif // wxUSE_RICHEDIT

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
    return (from != to) ;
}

bool wxTextCtrl::CanCut() const
{
    // Can cut if there's a selection
    long from, to;
    GetSelection(& from, & to);
    return (from != to) && (IsEditable());
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

#if wxUSE_RICHEDIT
    if ( !m_isRich)
#endif
    {
        static const wxChar *nothing = _T("");
        SendMessage(hWnd, EM_REPLACESEL, 0, (LPARAM)nothing);
    }
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
    }
    else
#endif // rich/!rich
    {
        DWORD dwStart, dwEnd;
        WPARAM wParam = (WPARAM) &dwStart; // receives starting position
        LPARAM lParam = (LPARAM) &dwEnd;   // receives ending position

        ::SendMessage(GetHwnd(), EM_GETSEL, wParam, lParam);

        *from = dwStart;
        *to = dwEnd;
    }
}

wxString wxTextCtrl::GetStringSelection() const
{
    // the base class version works correctly for the rich text controls
    // because there the lines are terminated with just '\r' which means that
    // the string length is not changed in the result of the translations doen
    // in GetValue() but for the normal ones when we replace "\r\n" with '\n'
    // we break the indices
#if wxUSE_RICHEDIT
    if ( m_isRich )
        return wxTextCtrlBase::GetStringSelection();
#endif // wxUSE_RICHEDIT

    long from, to;
    GetSelection(&from, &to);

    wxString str;
    if ( from < to )
    {
        str = wxGetWindowText(GetHWND()).Mid(from, to - from);

        // and now that we have the correct selection, convert it to the
        // correct format
        str = wxTextFile::Translate(str, wxTextFileType_Unix);
    }

    return str;
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
    HWND hWnd = GetHwnd();
    long fromChar = from;
    long toChar = to;

    // Set selection and remove it
#ifdef __WIN32__
    SendMessage(hWnd, EM_SETSEL, fromChar, toChar);
    SendMessage(hWnd, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)value.c_str());
#else
    SendMessage(hWnd, EM_SETSEL, (WPARAM)0, (LPARAM)MAKELONG(fromChar, toChar));
    SendMessage(hWnd, EM_REPLACESEL, (WPARAM)0, (LPARAM)value.c_str());
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
    SendMessage(hWnd, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)"");
#else
    SendMessage(hWnd, EM_SETSEL, (WPARAM)0, (LPARAM)MAKELONG(fromChar, toChar));
    SendMessage(hWnd, EM_REPLACESEL, (WPARAM)0, (LPARAM)"");
#endif
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

    // there must be at least enough place for the length WORD in the
    // buffer
    len += sizeof(WORD);

    wxString str;
    wxChar *buf = str.GetWriteBuf(len);

    *(WORD *)buf = (WORD)len;
    len = (size_t)::SendMessage(GetHwnd(), EM_GETLINE, lineNo, (LPARAM)buf);
    buf[len] = 0;

    str.UngetWriteBuf(len);

    return str;
}

void wxTextCtrl::SetMaxLength(unsigned long len)
{
    ::SendMessage(GetHwnd(), EM_LIMITTEXT, len, 0);
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

// ----------------------------------------------------------------------------
// kbd input processing
// ----------------------------------------------------------------------------

bool wxTextCtrl::MSWShouldPreProcessMessage(WXMSG* pMsg)
{
    MSG *msg = (MSG *)pMsg;

    // check for our special keys here: if we don't do it and the parent frame
    // uses them as accelerators, they wouldn't work at all, so we disable
    // usual preprocessing for them
    if ( msg->message == WM_KEYDOWN )
    {
        WORD vkey = msg->wParam;
        if ( (HIWORD(msg->lParam) & KF_ALTDOWN) == KF_ALTDOWN )
        {
            if ( vkey == VK_BACK )
                return FALSE;
        }
        else // no Alt
        {
            if ( wxIsCtrlDown() )
            {
                switch ( vkey )
                {
                    case 'C':
                    case 'V':
                    case 'X':
                    case VK_INSERT:
                    case VK_DELETE:
                    case VK_HOME:
                    case VK_END:
                        return FALSE;
                }
            }
            else if ( wxIsShiftDown() )
            {
                if ( vkey == VK_INSERT || vkey == VK_DELETE )
                    return FALSE;
            }
        }
    }

    return wxControl::MSWShouldPreProcessMessage(pMsg);
}

void wxTextCtrl::OnChar(wxKeyEvent& event)
{
    switch ( event.KeyCode() )
    {
        case WXK_RETURN:
            if ( !(m_windowStyle & wxTE_MULTILINE) )
            {
                wxCommandEvent event(wxEVT_COMMAND_TEXT_ENTER, m_windowId);
                InitCommandEvent(event);
                event.SetString(GetValue());
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
            {
                wxNavigationKeyEvent eventNav;
                eventNav.SetDirection(!event.ShiftDown());
                eventNav.SetWindowChange(event.ControlDown());
                eventNav.SetEventObject(this);

                if ( GetParent()->GetEventHandler()->ProcessEvent(eventNav) )
                    return;
            }
            break;
    }

    // no, we didn't process it
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
                InitCommandEvent(event);
                event.SetString(GetValue());
                ProcessCommand(event);
            }
            break;

        case EN_MAXTEXT:
            // the text size limit has been hit - increase it
            if ( !AdjustSpaceLimit() )
            {
                wxCommandEvent event(wxEVT_COMMAND_TEXT_MAXLEN, m_windowId);
                InitCommandEvent(event);
                event.SetString(GetValue());
                ProcessCommand(event);
            }
            break;

            // the other notification messages are not processed
        case EN_UPDATE:
        case EN_ERRSPACE:
        case EN_HSCROLL:
        case EN_VSCROLL:
            return FALSE;
        default:
            return FALSE;
    }

    // processed
    return TRUE;
}

WXHBRUSH wxTextCtrl::OnCtlColor(WXHDC pDC, WXHWND WXUNUSED(pWnd), WXUINT WXUNUSED(nCtlColor),
#if wxUSE_CTL3D
                               WXUINT message,
                               WXWPARAM wParam,
                               WXLPARAM lParam
#else
                               WXUINT WXUNUSED(message),
                               WXWPARAM WXUNUSED(wParam),
                               WXLPARAM WXUNUSED(lParam)
#endif
    )
{
#if wxUSE_CTL3D
    if ( m_useCtl3D )
    {
        HBRUSH hbrush = Ctl3dCtlColorEx(message, wParam, lParam);
        return (WXHBRUSH) hbrush;
    }
#endif // wxUSE_CTL3D

    HDC hdc = (HDC)pDC;
    if (GetParent()->GetTransparentBackground())
        SetBkMode(hdc, TRANSPARENT);
    else
        SetBkMode(hdc, OPAQUE);

    wxColour colBack = GetBackgroundColour();

    if (!IsEnabled() && (GetWindowStyle() & wxTE_MULTILINE) == 0)
        colBack = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE);

    ::SetBkColor(hdc, wxColourToRGB(colBack));
    ::SetTextColor(hdc, wxColourToRGB(GetForegroundColour()));

    wxBrush *brush = wxTheBrushList->FindOrCreateBrush(colBack, wxSOLID);

    return (WXHBRUSH)brush->GetResourceHandle();
}

// In WIN16, need to override normal erasing because
// Ctl3D doesn't use the wxWindows background colour.
#ifdef __WIN16__
void wxTextCtrl::OnEraseBackground(wxEraseEvent& event)
{
    wxColour col(m_backgroundColour);

#if wxUSE_CTL3D
    if (m_useCtl3D)
        col = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_WINDOW);
#endif

    RECT rect;
    ::GetClientRect(GetHwnd(), &rect);

    COLORREF ref = PALETTERGB(col.Red(),
                              col.Green(),
                              col.Blue());
    HBRUSH hBrush = ::CreateSolidBrush(ref);
    if ( !hBrush )
        wxLogLastError(wxT("CreateSolidBrush"));

    HDC hdc = (HDC)event.GetDC()->GetHDC();

    int mode = ::SetMapMode(hdc, MM_TEXT);

    ::FillRect(hdc, &rect, hBrush);
    ::DeleteObject(hBrush);
    ::SetMapMode(hdc, mode);

}
#endif

bool wxTextCtrl::AdjustSpaceLimit()
{
#ifndef __WIN16__
    unsigned int limit = ::SendMessage(GetHwnd(), EM_GETLIMITTEXT, 0, 0);

    // HACK: we try to automatically extend the limit for the amount of text
    //       to allow (interactively) entering more than 64Kb of text under
    //       Win9x but we shouldn't reset the text limit which was previously
    //       set explicitly with SetMaxLength()
    //
    //       we could solve this by storing the limit we set in wxTextCtrl but
    //       to save space we prefer to simply test here the actual limit
    //       value: we consider that SetMaxLength() can only be called for
    //       values < 32Kb
    if ( limit < 0x8000 )
    {
        // we've got more text than limit set by SetMaxLength()
        return FALSE;
    }

    unsigned int len = ::GetWindowTextLength(GetHwnd());
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

    // we changed the limit
    return TRUE;
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
        hText *= wxMax(GetNumberOfLines(), 5);
    }
    //else: for single line control everything is ok

    return wxSize(wText, hText);
}

// ----------------------------------------------------------------------------
// standard handlers for standard edit menu events
// ----------------------------------------------------------------------------

void wxTextCtrl::OnCut(wxCommandEvent& WXUNUSED(event))
{
    Cut();
}

void wxTextCtrl::OnCopy(wxCommandEvent& WXUNUSED(event))
{
    Copy();
}

void wxTextCtrl::OnPaste(wxCommandEvent& WXUNUSED(event))
{
    Paste();
}

void wxTextCtrl::OnUndo(wxCommandEvent& WXUNUSED(event))
{
    Undo();
}

void wxTextCtrl::OnRedo(wxCommandEvent& WXUNUSED(event))
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

// the rest of the file only deals with the rich edit controls
#if wxUSE_RICHEDIT

// ----------------------------------------------------------------------------
// EN_LINK processing
// ----------------------------------------------------------------------------

bool wxTextCtrl::MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result)
{
    NMHDR *hdr = (NMHDR* )lParam;
    if ( hdr->code == EN_LINK )
    {
        ENLINK *enlink = (ENLINK *)hdr;

        switch ( enlink->msg )
        {
            case WM_SETCURSOR:
                // ok, so it is hardcoded - do we really nee to customize it?
                ::SetCursor(GetHcursorOf(wxCursor(wxCURSOR_HAND)));
                *result = TRUE;
                break;

            case WM_MOUSEMOVE:
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_LBUTTONDBLCLK:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
            case WM_RBUTTONDBLCLK:
                // send a mouse event
                {
                    static const wxEventType eventsMouse[] =
                    {
                        wxEVT_MOTION,
                        wxEVT_LEFT_DOWN,
                        wxEVT_LEFT_UP,
                        wxEVT_LEFT_DCLICK,
                        wxEVT_RIGHT_DOWN,
                        wxEVT_RIGHT_UP,
                        wxEVT_RIGHT_DCLICK,
                    };

                    // the event ids are consecutive
                    wxMouseEvent
                        evtMouse(eventsMouse[enlink->msg - WM_MOUSEMOVE]);

                    InitMouseEvent(evtMouse,
                                   GET_X_LPARAM(enlink->lParam),
                                   GET_Y_LPARAM(enlink->lParam),
                                   enlink->wParam);

                    wxTextUrlEvent event(m_windowId, evtMouse,
                                         enlink->chrg.cpMin,
                                         enlink->chrg.cpMax);

                    InitCommandEvent(event);

                    *result = ProcessCommand(event);
                }
                break;
        }

        return TRUE;
    }

    // not processed
    return FALSE;
}

// ----------------------------------------------------------------------------
// colour setting for the rich edit controls
// ----------------------------------------------------------------------------

// Watcom C++ doesn't define this
#ifndef SCF_ALL
#define SCF_ALL 0x0004
#endif

bool wxTextCtrl::SetBackgroundColour(const wxColour& colour)
{
    if ( !wxTextCtrlBase::SetBackgroundColour(colour) )
    {
        // colour didn't really change
        return FALSE;
    }

    if ( IsRich() )
    {
        // rich edit doesn't use WM_CTLCOLOR, hence we need to send
        // EM_SETBKGNDCOLOR additionally
        ::SendMessage(GetHwnd(), EM_SETBKGNDCOLOR, 0, wxColourToRGB(colour));
    }

    return TRUE;
}

bool wxTextCtrl::SetForegroundColour(const wxColour& colour)
{
    if ( !wxTextCtrlBase::SetForegroundColour(colour) )
    {
        // colour didn't really change
        return FALSE;
    }

    if ( IsRich() )
    {
        // change the colour of everything
        CHARFORMAT cf;
        wxZeroMemory(cf);
        cf.cbSize = sizeof(cf);
        cf.dwMask = CFM_COLOR;
        cf.crTextColor = wxColourToRGB(colour);
        ::SendMessage(GetHwnd(), EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);
    }

    return TRUE;
}

// ----------------------------------------------------------------------------
// styling support for rich edit controls
// ----------------------------------------------------------------------------

bool wxTextCtrl::SetStyle(long start, long end, const wxTextAttr& style)
{
    if ( !IsRich() )
    {
        // can't do it with normal text control
        return FALSE;
    }

    // the rich text control doesn't handle setting background colour, so don't
    // even try if it's the only thing we want to change
    if ( wxRichEditModule::GetLoadedVersion() < 2 &&
         !style.HasFont() && !style.HasTextColour() )
    {
        // nothing to do: return TRUE if there was really nothing to do and
        // FALSE if we failed to set bg colour
        return !style.HasBackgroundColour();
    }

    // order the range if needed
    if ( start > end )
    {
        long tmp = start;
        start = end;
        end = tmp;
    }

    // we can only change the format of the selection, so select the range we
    // want and restore the old selection later
    long startOld, endOld;
    GetSelection(&startOld, &endOld);

    // but do we really have to change the selection?
    bool changeSel = start != startOld || end != endOld;

    if ( changeSel )
        SendMessage(GetHwnd(), EM_SETSEL, (WPARAM) start, (LPARAM) end);

    // initialize CHARFORMAT struct
#if wxUSE_RICHEDIT2
    CHARFORMAT2 cf;
#else
    CHARFORMAT cf;
#endif
    wxZeroMemory(cf);
    cf.cbSize = sizeof(cf);

    if ( style.HasFont() )
    {
        cf.dwMask |= CFM_FACE | CFM_SIZE | CFM_CHARSET |
                     CFM_ITALIC | CFM_BOLD | CFM_UNDERLINE;

        // fill in data from LOGFONT but recalculate lfHeight because we need
        // the real height in twips and not the negative number which
        // wxFillLogFont() returns (this is correct in general and works with
        // the Windows font mapper, but not here)
        LOGFONT lf;
        wxFillLogFont(&lf, &style.GetFont());
        cf.yHeight = 20*style.GetFont().GetPointSize(); // 1 pt = 20 twips
        cf.bCharSet = lf.lfCharSet;
        cf.bPitchAndFamily = lf.lfPitchAndFamily;
        wxStrncpy( cf.szFaceName, lf.lfFaceName, WXSIZEOF(cf.szFaceName) );

        // also deal with underline/italic/bold attributes: note that we must
        // always set CFM_ITALIC &c bits in dwMask, even if we don't set the
        // style to allow clearing it
        if ( lf.lfItalic )
        {
            cf.dwEffects |= CFE_ITALIC;
        }

        if ( lf.lfWeight == FW_BOLD )
        {
            cf.dwEffects |= CFE_BOLD;
        }

        if ( lf.lfUnderline )
        {
            cf.dwEffects |= CFE_UNDERLINE;
        }

        // strikeout fonts are not supported by wxWindows
    }

    if ( style.HasTextColour() )
    {
        cf.dwMask |= CFM_COLOR;
        cf.crTextColor = wxColourToRGB(style.GetTextColour());
    }

#if wxUSE_RICHEDIT2
    if ( wxRichEditModule::GetLoadedVersion() > 1 && style.HasBackgroundColour() )
    {
        cf.dwMask |= CFM_BACKCOLOR;
        cf.crBackColor = wxColourToRGB(style.GetBackgroundColour());
    }
#endif // wxUSE_RICHEDIT2

    // do format the selection
    bool ok = ::SendMessage(GetHwnd(), EM_SETCHARFORMAT,
                            SCF_SELECTION, (LPARAM)&cf) != 0;
    if ( !ok )
    {
        wxLogDebug(_T("SendMessage(EM_SETCHARFORMAT, SCF_SELECTION) failed"));
    }

    if ( changeSel )
    {
        // restore the original selection
        SendMessage(GetHwnd(), EM_SETSEL, (WPARAM)startOld, (LPARAM)endOld);
    }

    return ok;
}

// ----------------------------------------------------------------------------
// wxRichEditModule
// ----------------------------------------------------------------------------

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

#endif // wxUSE_TEXTCTRL
