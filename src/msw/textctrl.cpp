/////////////////////////////////////////////////////////////////////////////
// Name:        msw/textctrl.cpp
// Purpose:     wxTextCtrl
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
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
    #include "wx/menu.h"
#endif

#include "wx/module.h"

#if wxUSE_CLIPBOARD
    #include "wx/clipbrd.h"
#endif

#include "wx/textfile.h"

#include <windowsx.h>

#include "wx/msw/private.h"
#include "wx/msw/winundef.h"

#include <string.h>
#include <stdlib.h>

#ifndef __WXWINCE__
#include <sys/types.h>
#endif

#if wxUSE_RICHEDIT

// old mingw32 has richedit stuff directly in windows.h and doesn't have
// richedit.h at all
#if !defined(__GNUWIN32_OLD__) || defined(__CYGWIN10__)
    #include <richedit.h>
#endif

#include "wx/msw/missing.h"

#endif // wxUSE_RICHEDIT

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

#if wxUSE_RICHEDIT

DWORD CALLBACK wxRichEditStreamIn(DWORD dwCookie, BYTE *buf, LONG cb, LONG *pcb);

#endif // wxUSE_RICHEDIT

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

#if wxUSE_RICHEDIT

// this module initializes RichEdit DLL(s) if needed
class wxRichEditModule : public wxModule
{
public:
    virtual bool OnInit();
    virtual void OnExit();

    // load the richedit DLL of at least of required version
    static bool Load(int version = 1);

private:
    // the handles to richedit 1.0 and 2.0 (or 3.0) DLLs
    static HINSTANCE ms_hRichEdit[2];

    DECLARE_DYNAMIC_CLASS(wxRichEditModule)
};

HINSTANCE wxRichEditModule::ms_hRichEdit[2] = { NULL, NULL };

IMPLEMENT_DYNAMIC_CLASS(wxRichEditModule, wxModule)

#endif // wxUSE_RICHEDIT

// ----------------------------------------------------------------------------
// event tables and other macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTextCtrl, wxControl)

BEGIN_EVENT_TABLE(wxTextCtrl, wxControl)
    EVT_CHAR(wxTextCtrl::OnChar)
    EVT_DROP_FILES(wxTextCtrl::OnDropFiles)

#if wxUSE_RICHEDIT
    EVT_RIGHT_UP(wxTextCtrl::OnRightClick)
#endif

    EVT_MENU(wxID_CUT, wxTextCtrl::OnCut)
    EVT_MENU(wxID_COPY, wxTextCtrl::OnCopy)
    EVT_MENU(wxID_PASTE, wxTextCtrl::OnPaste)
    EVT_MENU(wxID_UNDO, wxTextCtrl::OnUndo)
    EVT_MENU(wxID_REDO, wxTextCtrl::OnRedo)
    EVT_MENU(wxID_CLEAR, wxTextCtrl::OnDelete)
    EVT_MENU(wxID_SELECTALL, wxTextCtrl::OnSelectAll)

    EVT_UPDATE_UI(wxID_CUT, wxTextCtrl::OnUpdateCut)
    EVT_UPDATE_UI(wxID_COPY, wxTextCtrl::OnUpdateCopy)
    EVT_UPDATE_UI(wxID_PASTE, wxTextCtrl::OnUpdatePaste)
    EVT_UPDATE_UI(wxID_UNDO, wxTextCtrl::OnUpdateUndo)
    EVT_UPDATE_UI(wxID_REDO, wxTextCtrl::OnUpdateRedo)
    EVT_UPDATE_UI(wxID_CLEAR, wxTextCtrl::OnUpdateDelete)
    EVT_UPDATE_UI(wxID_SELECTALL, wxTextCtrl::OnUpdateSelectAll)
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

void wxTextCtrl::Init()
{
#if wxUSE_RICHEDIT
    m_verRichEdit = 0;
#endif // wxUSE_RICHEDIT

    m_privateContextMenu = NULL;
    m_suppressNextUpdate = FALSE;
}

wxTextCtrl::~wxTextCtrl()
{
    if (m_privateContextMenu)
    {
        delete m_privateContextMenu;
        m_privateContextMenu = NULL;
    }
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

    // translate wxWin style flags to MSW ones
    WXDWORD msStyle = MSWGetCreateWindowFlags();

    // do create the control - either an EDIT or RICHEDIT
    wxString windowClass = wxT("EDIT");

#if wxUSE_RICHEDIT
    if ( m_windowStyle & wxTE_AUTO_URL )
    {
        // automatic URL detection only works in RichEdit 2.0+
        m_windowStyle |= wxTE_RICH2;
    }

    if ( m_windowStyle & wxTE_RICH2 )
    {
        // using richedit 2.0 implies using wxTE_RICH
        m_windowStyle |= wxTE_RICH;
    }

    // we need to load the richedit DLL before creating the rich edit control
    if ( m_windowStyle & wxTE_RICH )
    {
        static bool s_errorGiven = FALSE;// MT-FIXME

        // Which version do we need? Use 1.0 by default because it is much more
        // like the the standard EDIT or 2.0 if explicitly requested, but use
        // only 2.0 in Unicode mode as 1.0 doesn't support Unicode at all
        //
        // TODO: RichEdit 3.0 is apparently capable of emulating RichEdit 1.0
        //       (and thus EDIT) much better than RichEdit 2.0 so we probably
        //       should use 3.0 if available as it is the best of both worlds -
        //       but as I can't test it right now I don't do it (VZ)
#if wxUSE_UNICODE
        const int verRichEdit = 2;
#else // !wxUSE_UNICODE
        int verRichEdit = m_windowStyle & wxTE_RICH2 ? 2 : 1;
#endif // wxUSE_UNICODE/!wxUSE_UNICODE

        // only give the error msg once if the DLL can't be loaded
        if ( !s_errorGiven )
        {
            // try to load the RichEdit DLL (will do nothing if already done)
            if ( !wxRichEditModule::Load(verRichEdit) )
            {
#if !wxUSE_UNICODE
                // try another version?
                verRichEdit = 3 - verRichEdit; // 1 <-> 2

                if ( !wxRichEditModule::Load(verRichEdit) )
#endif // wxUSE_UNICODE
                {
                    wxLogError(_("Impossible to create a rich edit control, using simple text control instead. Please reinstall riched32.dll"));

                    s_errorGiven = TRUE;
                }
            }
        }

        // have we managed to load any richedit version?
        if ( !s_errorGiven )
        {
            m_verRichEdit = verRichEdit;
            if ( m_verRichEdit == 1 )
            {
                windowClass = wxT("RICHEDIT");
            }
            else
            {
#ifndef RICHEDIT_CLASS
                wxString RICHEDIT_CLASS;
                RICHEDIT_CLASS.Printf(_T("RichEdit%d0"), m_verRichEdit);
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

    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

#if wxUSE_RICHEDIT
    if ( IsRich() )
    {
        // enable the events we're interested in: we want to get EN_CHANGE as
        // for the normal controls
        LPARAM mask = ENM_CHANGE;

        if ( GetRichVersion() == 1 )
        {
            // we also need EN_MSGFILTER for richedit 1.0 for the reasons
            // explained in its handler
           mask |= ENM_MOUSEEVENTS;
        }
        else if ( m_windowStyle & wxTE_AUTO_URL )
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
    long style = ::GetWindowLong(hWnd, GWL_STYLE);

    // retrieve the style to see whether this is an edit or richedit ctrl
#if wxUSE_RICHEDIT
    wxString classname = wxGetWindowClass(GetHWND());

    if ( classname.IsSameAs(_T("EDIT"), FALSE /* no case */) )
    {
        m_verRichEdit = 0;
    }
    else // rich edit?
    {
        wxChar c;
        if ( wxSscanf(classname, _T("RichEdit%d0%c"), &m_verRichEdit, &c) != 2 )
        {
            wxLogDebug(_T("Unknown edit control '%s'."), classname.c_str());

            m_verRichEdit = 0;
        }
    }
#endif // wxUSE_RICHEDIT

    if (style & ES_MULTILINE)
        m_windowStyle |= wxTE_MULTILINE;
    if (style & ES_PASSWORD)
        m_windowStyle |= wxTE_PASSWORD;
    if (style & ES_READONLY)
        m_windowStyle |= wxTE_READONLY;
    if (style & ES_WANTRETURN)
        m_windowStyle |= wxTE_PROCESS_ENTER;
    if (style & ES_CENTER)
        m_windowStyle |= wxTE_CENTRE;
    if (style & ES_RIGHT)
        m_windowStyle |= wxTE_RIGHT;
}

WXDWORD wxTextCtrl::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    long msStyle = wxControl::MSWGetStyle(style, exstyle);

    // styles which we alaways add by default
    if ( style & wxTE_MULTILINE )
    {
        wxASSERT_MSG( !(style & wxTE_PROCESS_ENTER),
                      wxT("wxTE_PROCESS_ENTER style is ignored for multiline text controls (they always process it)") );

        msStyle |= ES_MULTILINE | ES_WANTRETURN;
        if ( !(style & wxTE_NO_VSCROLL) )
        {
            // always adjust the vertical scrollbar automatically if we have it
            msStyle |= WS_VSCROLL | ES_AUTOVSCROLL;

#if wxUSE_RICHEDIT
            // we have to use this style for the rich edit controls because
            // without it the vertical scrollbar never appears at all in
            // richedit 3.0 because of our ECO_NOHIDESEL hack (search for it)
            if ( style & wxTE_RICH2 )
            {
                msStyle |= ES_DISABLENOSCROLL;
            }
#endif // wxUSE_RICHEDIT
        }

        style |= wxTE_PROCESS_ENTER;
    }
    else // !multiline
    {
        // there is really no reason to not have this style for single line
        // text controls
        msStyle |= ES_AUTOHSCROLL;
    }

    // styles which we add depending on the specified wxWindows styles
    if ( style & wxHSCROLL )
    {
        // automatically scroll the control horizontally as necessary
        msStyle |= WS_HSCROLL;// | ES_AUTOHSCROLL;
    }

    if ( style & wxTE_READONLY )
        msStyle |= ES_READONLY;

    if ( style & wxTE_PASSWORD )
        msStyle |= ES_PASSWORD;

    if ( style & wxTE_NOHIDESEL )
        msStyle |= ES_NOHIDESEL;

    // note that we can't do do "& wxTE_LEFT" as wxTE_LEFT == 0
    if ( style & wxTE_CENTRE )
        msStyle |= ES_CENTER;
    else if ( style & wxTE_RIGHT )
        msStyle |= ES_RIGHT;
    else
        msStyle |= ES_LEFT; // ES_LEFT if 0 as well but for consistency...

    return msStyle;
}

void wxTextCtrl::SetWindowStyleFlag(long style)
{
#if wxUSE_RICHEDIT
    // we have to deal with some styles separately because they can't be
    // changed by simply calling SetWindowLong(GWL_STYLE) but can be changed
    // using richedit-specific EM_SETOPTIONS
    if ( IsRich() &&
            ((style & wxTE_NOHIDESEL) != (GetWindowStyle() & wxTE_NOHIDESEL)) )
    {
        bool set = (style & wxTE_NOHIDESEL) != 0;

        ::SendMessage(GetHwnd(), EM_SETOPTIONS, set ? ECOOP_OR : ECOOP_AND,
                      set ? ECO_NOHIDESEL : ~ECO_NOHIDESEL);
    }
#endif // wxUSE_RICHEDIT

    wxControl::SetWindowStyleFlag(style);
}

// ----------------------------------------------------------------------------
// set/get the controls text
// ----------------------------------------------------------------------------

wxString wxTextCtrl::GetValue() const
{
    // range 0..-1 is special for GetRange() and means to retrieve all text
    return GetRange(0, -1);
}

wxString wxTextCtrl::GetRange(long from, long to) const
{
    wxString str;

    if ( from >= to && to != -1 )
    {
        // nothing to retrieve
        return str;
    }

#if wxUSE_RICHEDIT
    if ( IsRich() )
    {
        int len = GetWindowTextLength(GetHwnd());
        if ( len > from )
        {
            // alloc one extra WORD as needed by the control
            wxChar *p = str.GetWriteBuf(++len);

            TEXTRANGE textRange;
            textRange.chrg.cpMin = from;
            textRange.chrg.cpMax = to == -1 ? len : to;
            textRange.lpstrText = p;

            (void)SendMessage(GetHwnd(), EM_GETTEXTRANGE, 0, (LPARAM)&textRange);

            if ( m_verRichEdit > 1 )
            {
                // RichEdit 2.0 uses just CR ('\r') for the newlines which is
                // neither Unix nor Windows style - convert it to something
                // reasonable
                for ( ; *p; p++ )
                {
                    if ( *p == _T('\r') )
                        *p = _T('\n');
                }
            }

            str.UngetWriteBuf();

            if ( m_verRichEdit == 1 )
            {
                // convert to the canonical form - see comment below
                str = wxTextFile::Translate(str, wxTextFileType_Unix);
            }
        }
        //else: no text at all, leave the string empty
    }
    else
#endif // wxUSE_RICHEDIT
    {
        // retrieve all text
        str = wxGetWindowText(GetHWND());

        // need only a range?
        if ( from < to )
        {
            str = str.Mid(from, to - from);
        }

        // WM_GETTEXT uses standard DOS CR+LF (\r\n) convention - convert to the
        // canonical one (same one as above) for consistency with the other kinds
        // of controls and, more importantly, with the other ports
        str = wxTextFile::Translate(str, wxTextFileType_Unix);
    }

    return str;
}

void wxTextCtrl::SetValue(const wxString& value)
{
    // if the text is long enough, it's faster to just set it instead of first
    // comparing it with the old one (chances are that it will be different
    // anyhow, this comparison is there to avoid flicker for small single-line
    // edit controls mostly)
    if ( (value.length() > 0x400) || (value != GetValue()) )
    {
        DoWriteText(value, FALSE /* not selection only */);
    }

    // we should reset the modified flag even if the value didn't really change

    // mark the control as being not dirty - we changed its text, not the
    // user
    DiscardEdits();

    // for compatibility, don't move the cursor when doing SetValue()
    SetInsertionPoint(0);
}

#if wxUSE_RICHEDIT && (!wxUSE_UNICODE || wxUSE_UNICODE_MSLU)

DWORD CALLBACK wxRichEditStreamIn(DWORD dwCookie, BYTE *buf, LONG cb, LONG *pcb)
{
    *pcb = 0;

    wchar_t *wbuf = (wchar_t *)buf;
    const wchar_t *wpc = *(const wchar_t **)dwCookie;
    while ( cb && *wpc )
    {
        *wbuf++ = *wpc++;

        cb -= sizeof(wchar_t);
        (*pcb) += sizeof(wchar_t);
    }

    *(const wchar_t **)dwCookie = wpc;

    return 0;
}

// from utils.cpp
extern WXDLLIMPEXP_BASE long wxEncodingToCodepage(wxFontEncoding encoding);

#if wxUSE_UNICODE_MSLU
bool wxTextCtrl::StreamIn(const wxString& value,
                          wxFontEncoding WXUNUSED(encoding),
                          bool selectionOnly)
{
    const wchar_t *wpc = value.c_str();
#else // !wxUSE_UNICODE_MSLU
bool wxTextCtrl::StreamIn(const wxString& value,
                          wxFontEncoding encoding,
                          bool selectionOnly)
{
    // we have to use EM_STREAMIN to force richedit control 2.0+ to show any
    // text in the non default charset -- otherwise it thinks it knows better
    // than we do and always shows it in the default one

    // first get the Windows code page for this encoding
    long codepage = wxEncodingToCodepage(encoding);
    if ( codepage == -1 )
    {
        // unknown encoding
        return FALSE;
    }

    // next translate to Unicode using this code page
    int len = ::MultiByteToWideChar(codepage, 0, value, -1, NULL, 0);

#if wxUSE_WCHAR_T
    wxWCharBuffer wchBuf(len);
#else
    wchar_t *wchBuf = (wchar_t *)malloc((len + 1)*sizeof(wchar_t));
#endif

    if ( !::MultiByteToWideChar(codepage, 0, value, -1,
                                (wchar_t *)(const wchar_t *)wchBuf, len) )
    {
        wxLogLastError(_T("MultiByteToWideChar"));
    }

    // finally, stream it in the control
    const wchar_t *wpc = wchBuf;
#endif // wxUSE_UNICODE_MSLU

    EDITSTREAM eds;
    wxZeroMemory(eds);
    eds.dwCookie = (DWORD)&wpc;
    // the cast below is needed for broken (very) old mingw32 headers
    eds.pfnCallback = (EDITSTREAMCALLBACK)wxRichEditStreamIn;

    // we're going to receive 2 EN_CHANGE notifications if we got any selection
    // (same problem as in DoWriteText())
    if ( selectionOnly && HasSelection() )
    {
        // so suppress one of them
        m_suppressNextUpdate = TRUE;
    }

    ::SendMessage(GetHwnd(), EM_STREAMIN,
                  SF_TEXT |
                  SF_UNICODE |
                  (selectionOnly ? SFF_SELECTION : 0),
                  (LPARAM)&eds);

    if ( eds.dwError )
    {
        wxLogLastError(_T("EM_STREAMIN"));
    }

#if !wxUSE_WCHAR_T
    free(wchBuf);
#endif // !wxUSE_WCHAR_T

    return TRUE;
}

#endif // wxUSE_RICHEDIT

void wxTextCtrl::WriteText(const wxString& value)
{
    DoWriteText(value);
}

void wxTextCtrl::DoWriteText(const wxString& value, bool selectionOnly)
{
    wxString valueDos;
    if ( m_windowStyle & wxTE_MULTILINE )
        valueDos = wxTextFile::Translate(value, wxTextFileType_Dos);
    else
        valueDos = value;

#if wxUSE_RICHEDIT
    // there are several complications with the rich edit controls here
    bool done = FALSE;
    if ( IsRich() )
    {
        // first, ensure that the new text will be in the default style
        if ( !m_defaultStyle.IsDefault() )
        {
            long start, end;
            GetSelection(&start, &end);
            SetStyle(start, end, m_defaultStyle);
        }

#if wxUSE_UNICODE_MSLU
        // RichEdit doesn't have Unicode version of EM_REPLACESEL on Win9x,
        // but EM_STREAMIN works
        if ( wxUsingUnicowsDll() && GetRichVersion() > 1 )
        {
           done = StreamIn(valueDos, wxFONTENCODING_SYSTEM, selectionOnly);
        }
#endif // wxUSE_UNICODE_MSLU

#if !wxUSE_UNICODE
        // next check if the text we're inserting must be shown in a non
        // default charset -- this only works for RichEdit > 1.0
        if ( GetRichVersion() > 1 )
        {
            wxFont font = m_defaultStyle.GetFont();
            if ( !font.Ok() )
                font = GetFont();

            if ( font.Ok() )
            {
               wxFontEncoding encoding = font.GetEncoding();
               if ( encoding != wxFONTENCODING_SYSTEM )
               {
                   done = StreamIn(valueDos, encoding, selectionOnly);
               }
            }
        }
#endif // !wxUSE_UNICODE
    }

    if ( !done )
#endif // wxUSE_RICHEDIT
    {
        // in some cases we get 2 EN_CHANGE notifications after the SendMessage
        // call below which is confusing for the client code and so should be
        // avoided
        //
        // these cases are: (a) plain EDIT controls if EM_REPLACESEL is used
        // and there is a non empty selection currently and (b) rich text
        // controls in any case
        if (
#if wxUSE_RICHEDIT
            IsRich() ||
#endif // wxUSE_RICHEDIT
            (selectionOnly && HasSelection()) )
        {
            m_suppressNextUpdate = TRUE;
        }

        ::SendMessage(GetHwnd(), selectionOnly ? EM_REPLACESEL : WM_SETTEXT,
                      0, (LPARAM)valueDos.c_str());

        // OTOH, non rich text controls don't generate any events at all when
        // we use WM_SETTEXT -- have to emulate them here
        if ( !selectionOnly
#if wxUSE_RICHEDIT
                && !IsRich()
#endif // wxUSE_RICHEDIT
           )
        {
            // Windows already sends an update event for single-line
            // controls.
            if ( m_windowStyle & wxTE_MULTILINE )
                SendUpdateEvent();
        }
    }

    AdjustSpaceLimit();
}

void wxTextCtrl::AppendText(const wxString& text)
{
    SetInsertionPointEnd();

    WriteText(text);

#if wxUSE_RICHEDIT
    if ( IsMultiLine() && GetRichVersion() > 1 )
    {
        // setting the caret to the end and showing it simply doesn't work for
        // RichEdit 2.0 -- force it to still do what we want
        ::SendMessage(GetHwnd(), EM_LINESCROLL, 0, GetNumberOfLines());
    }
#endif // wxUSE_RICHEDIT
}

void wxTextCtrl::Clear()
{
    ::SetWindowText(GetHwnd(), wxEmptyString);

#if wxUSE_RICHEDIT
    if ( !IsRich() )
#endif // wxUSE_RICHEDIT
    {
        // rich edit controls send EN_UPDATE from WM_SETTEXT handler themselves
        // but the normal ones don't -- make Clear() behaviour consistent by
        // always sending this event

        // Windows already sends an update event for single-line
        // controls.
        if ( m_windowStyle & wxTE_MULTILINE )
            SendUpdateEvent();
    }
}

#ifdef __WIN32__

bool wxTextCtrl::EmulateKeyPress(const wxKeyEvent& event)
{
    SetFocus();

    size_t lenOld = GetValue().length();

    wxUint32 code = event.GetRawKeyCode();
    ::keybd_event(code, 0, 0 /* key press */, 0);
    ::keybd_event(code, 0, KEYEVENTF_KEYUP, 0);

    // assume that any alphanumeric key changes the total number of characters
    // in the control - this should work in 99% of cases
    return GetValue().length() != lenOld;
}

#endif // __WIN32__

// ----------------------------------------------------------------------------
// Clipboard operations
// ----------------------------------------------------------------------------

void wxTextCtrl::Copy()
{
    if (CanCopy())
    {
        ::SendMessage(GetHwnd(), WM_COPY, 0, 0L);
    }
}

void wxTextCtrl::Cut()
{
    if (CanCut())
    {
        ::SendMessage(GetHwnd(), WM_CUT, 0, 0L);
    }
}

void wxTextCtrl::Paste()
{
    if (CanPaste())
    {
        ::SendMessage(GetHwnd(), WM_PASTE, 0, 0L);
    }
}

bool wxTextCtrl::HasSelection() const
{
    long from, to;
    GetSelection(&from, &to);
    return from != to;
}

bool wxTextCtrl::CanCopy() const
{
    // Can copy if there's a selection
    return HasSelection();
}

bool wxTextCtrl::CanCut() const
{
    return CanCopy() && IsEditable();
}

bool wxTextCtrl::CanPaste() const
{
    if ( !IsEditable() )
        return FALSE;

#if wxUSE_RICHEDIT
    if ( IsRich() )
    {
        UINT cf = 0; // 0 == any format

        return ::SendMessage(GetHwnd(), EM_CANPASTE, cf, 0) != 0;
    }
#endif // wxUSE_RICHEDIT

    // Standard edit control: check for straight text on clipboard
    if ( !::OpenClipboard(GetHwndOf(wxTheApp->GetTopWindow())) )
        return FALSE;

    bool isTextAvailable = ::IsClipboardFormatAvailable(CF_TEXT) != 0;
    ::CloseClipboard();

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
    DoSetSelection(pos, pos);
}

void wxTextCtrl::SetInsertionPointEnd()
{
    // we must not do anything if the caret is already there because calling
    // SetInsertionPoint() thaws the controls if Freeze() had been called even
    // if it doesn't actually move the caret anywhere and so the simple fact of
    // doing it results in horrible flicker when appending big amounts of text
    // to the control in a few chunks (see DoAddText() test in the text sample)
    if ( GetInsertionPoint() == GetLastPosition() )
        return;

    long pos;

#if wxUSE_RICHEDIT
    if ( m_verRichEdit == 1 )
    {
        // we don't have to waste time calling GetLastPosition() in this case
        pos = -1;
    }
    else // !RichEdit 1.0
#endif // wxUSE_RICHEDIT
    {
        pos = GetLastPosition();
    }

    SetInsertionPoint(pos);
}

long wxTextCtrl::GetInsertionPoint() const
{
#if wxUSE_RICHEDIT
    if ( IsRich() )
    {
        CHARRANGE range;
        range.cpMin = 0;
        range.cpMax = 0;
        SendMessage(GetHwnd(), EM_EXGETSEL, 0, (LPARAM) &range);
        return range.cpMin;
    }
#endif // wxUSE_RICHEDIT

    DWORD Pos = (DWORD)SendMessage(GetHwnd(), EM_GETSEL, 0, 0L);
    return Pos & 0xFFFF;
}

long wxTextCtrl::GetLastPosition() const
{
    int numLines = GetNumberOfLines();
    long posStartLastLine = XYToPosition(0, numLines - 1);

    long lenLastLine = GetLengthOfLineContainingPos(posStartLastLine);

    return posStartLastLine + lenLastLine;
}

// If the return values from and to are the same, there is no
// selection.
void wxTextCtrl::GetSelection(long* from, long* to) const
{
#if wxUSE_RICHEDIT
    if ( IsRich() )
    {
        CHARRANGE charRange;
        ::SendMessage(GetHwnd(), EM_EXGETSEL, 0, (LPARAM) &charRange);

        *from = charRange.cpMin;
        *to = charRange.cpMax;
    }
    else
#endif // !wxUSE_RICHEDIT
    {
        DWORD dwStart, dwEnd;
        ::SendMessage(GetHwnd(), EM_GETSEL, (WPARAM)&dwStart, (LPARAM)&dwEnd);

        *from = dwStart;
        *to = dwEnd;
    }
}

bool wxTextCtrl::IsEditable() const
{
    // strangely enough, we may be called before the control is created: our
    // own Create() calls MSWGetStyle() which calls AcceptsFocus() which calls
    // us
    if ( !m_hWnd )
        return TRUE;

    long style = ::GetWindowLong(GetHwnd(), GWL_STYLE);

    return (style & ES_READONLY) == 0;
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------

void wxTextCtrl::SetSelection(long from, long to)
{
    // if from and to are both -1, it means (in wxWindows) that all text should
    // be selected - translate into Windows convention
    if ( (from == -1) && (to == -1) )
    {
        from = 0;
        to = -1;
    }

    DoSetSelection(from, to);
}

void wxTextCtrl::DoSetSelection(long from, long to, bool scrollCaret)
{
    HWND hWnd = GetHwnd();

#ifdef __WIN32__
#if wxUSE_RICHEDIT
    if ( IsRich() )
    {
        CHARRANGE range;
        range.cpMin = from;
        range.cpMax = to;
        SendMessage(hWnd, EM_EXSETSEL, 0, (LPARAM) &range);
    }
    else
#endif // wxUSE_RICHEDIT
    {
        SendMessage(hWnd, EM_SETSEL, (WPARAM)from, (LPARAM)to);
    }

    if ( scrollCaret )
    {
#if wxUSE_RICHEDIT
        // richedit 3.0 (i.e. the version living in riched20.dll distributed
        // with Windows 2000 and beyond) doesn't honour EM_SCROLLCARET when
        // emulating richedit 2.0 unless the control has focus or ECO_NOHIDESEL
        // option is set (but it does work ok in richedit 1.0 mode...)
        //
        // so to make it work we either need to give focus to it here which
        // will probably create many problems (dummy focus events; window
        // containing the text control being brought to foreground
        // unexpectedly; ...) or to temporarily set ECO_NOHIDESEL which may
        // create other problems too -- and in fact it does because if we turn
        // on/off this style while appending the text to the control, the
        // vertical scrollbar never appears in it even if we append tons of
        // text and to work around this the only solution I found was to use
        // ES_DISABLENOSCROLL
        //
        // this is very ugly but I don't see any other way to make this work
        if ( GetRichVersion() > 1 )
        {
            if ( !HasFlag(wxTE_NOHIDESEL) )
            {
                ::SendMessage(GetHwnd(), EM_SETOPTIONS,
                              ECOOP_OR, ECO_NOHIDESEL);
            }
            //else: everything is already ok
        }
#endif // wxUSE_RICHEDIT

        SendMessage(hWnd, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);

#if wxUSE_RICHEDIT
        // restore ECO_NOHIDESEL if we changed it
        if ( GetRichVersion() > 1 && !HasFlag(wxTE_NOHIDESEL) )
        {
            ::SendMessage(GetHwnd(), EM_SETOPTIONS,
                          ECOOP_AND, ~ECO_NOHIDESEL);
        }
#endif // wxUSE_RICHEDIT
    }
#else // Win16
    // WPARAM is 0: selection is scrolled into view
    SendMessage(hWnd, EM_SETSEL, (WPARAM)0, (LPARAM)MAKELONG(from, to));
#endif // Win32/16
}

// ----------------------------------------------------------------------------
// Editing
// ----------------------------------------------------------------------------

void wxTextCtrl::Replace(long from, long to, const wxString& value)
{
    // Set selection and remove it
    DoSetSelection(from, to, FALSE /* don't scroll caret into view */);

    SendMessage(GetHwnd(), EM_REPLACESEL,
#ifdef __WIN32__
                TRUE,
#else
                FALSE,
#endif
                (LPARAM)value.c_str());
}

void wxTextCtrl::Remove(long from, long to)
{
    Replace(from, to, wxEmptyString);
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
    return SendMessage(GetHwnd(), EM_GETMODIFY, 0, 0) != 0;
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
    // This gets the char index for the _beginning_ of this line
    long charIndex = SendMessage(GetHwnd(), EM_LINEINDEX, (WPARAM)y, (LPARAM)0);

    return charIndex + x;
}

bool wxTextCtrl::PositionToXY(long pos, long *x, long *y) const
{
    HWND hWnd = GetHwnd();

    // This gets the line number containing the character
    long lineNo;
#if wxUSE_RICHEDIT
    if ( IsRich() )
    {
        lineNo = SendMessage(hWnd, EM_EXLINEFROMCHAR, 0, (LPARAM)pos);
    }
    else
#endif // wxUSE_RICHEDIT
    {
        lineNo = SendMessage(hWnd, EM_LINEFROMCHAR, (WPARAM)pos, 0);
    }

    if ( lineNo == -1 )
    {
        // no such line
        return FALSE;
    }

    // This gets the char index for the _beginning_ of this line
    long charIndex = SendMessage(hWnd, EM_LINEINDEX, (WPARAM)lineNo, (LPARAM)0);
    if ( charIndex == -1 )
    {
        return FALSE;
    }

    // The X position must therefore be the different between pos and charIndex
    if ( x )
        *x = pos - charIndex;
    if ( y )
        *y = lineNo;

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

long wxTextCtrl::GetLengthOfLineContainingPos(long pos) const
{
    return ::SendMessage(GetHwnd(), EM_LINELENGTH, (WPARAM)pos, 0);
}

int wxTextCtrl::GetLineLength(long lineNo) const
{
    long pos = XYToPosition(0, lineNo);

    return GetLengthOfLineContainingPos(pos);
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
    return ::SendMessage(GetHwnd(), EM_CANUNDO, 0, 0) != 0;
}

bool wxTextCtrl::CanRedo() const
{
    return ::SendMessage(GetHwnd(), EM_CANUNDO, 0, 0) != 0;
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
        WORD vkey = (WORD) msg->wParam;
        if ( (HIWORD(msg->lParam) & KF_ALTDOWN) == KF_ALTDOWN )
        {
            if ( vkey == VK_BACK )
                return FALSE;
        }
        else // no Alt
        {
            // we want to process some Ctrl-foo and Shift-bar but no key
            // combinations without either Ctrl or Shift nor with both of them
            // pressed
            const int ctrl = wxIsCtrlDown(),
                      shift = wxIsShiftDown();
            switch ( ctrl + shift )
            {
                default:
                    wxFAIL_MSG( _T("how many modifiers have we got?") );
                    // fall through

                case 0:
                case 2:
                    break;

                case 1:
                    // either Ctrl or Shift pressed
                    if ( ctrl )
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
                    else // Shift is pressed
                    {
                        if ( vkey == VK_INSERT || vkey == VK_DELETE )
                            return FALSE;
                    }
            }
        }
    }

    return wxControl::MSWShouldPreProcessMessage(pMsg);
}

void wxTextCtrl::OnChar(wxKeyEvent& event)
{
    switch ( event.GetKeyCode() )
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

long wxTextCtrl::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    long lRc = wxTextCtrlBase::MSWWindowProc(nMsg, wParam, lParam);

    if ( nMsg == WM_GETDLGCODE )
    {
        // we always want the chars and the arrows: the arrows for navigation
        // and the chars because we want Ctrl-C to work even in a read only
        // control
        long lDlgCode = DLGC_WANTCHARS | DLGC_WANTARROWS;

        if ( IsEditable() )
        {
            // we may have several different cases:
            // 1. normal case: both TAB and ENTER are used for dlg navigation
            // 2. ctrl which wants TAB for itself: ENTER is used to pass to the
            //    next control in the dialog
            // 3. ctrl which wants ENTER for itself: TAB is used for dialog
            //    navigation
            // 4. ctrl which wants both TAB and ENTER: Ctrl-ENTER is used to go
            //    to the next control

            // the multiline edit control should always get <Return> for itself
            if ( HasFlag(wxTE_PROCESS_ENTER) || HasFlag(wxTE_MULTILINE) )
                lDlgCode |= DLGC_WANTMESSAGE;

            if ( HasFlag(wxTE_PROCESS_TAB) )
                lDlgCode |= DLGC_WANTTAB;

            lRc |= lDlgCode;
        }
        else // !editable
        {
            // NB: use "=", not "|=" as the base class version returns the
            //     same flags is this state as usual (i.e. including
            //     DLGC_WANTMESSAGE). This is strange (how does it work in the
            //     native Win32 apps?) but for now live with it.
            lRc = lDlgCode;
        }
    }

    return lRc;
}

// ----------------------------------------------------------------------------
// text control event processing
// ----------------------------------------------------------------------------

bool wxTextCtrl::SendUpdateEvent()
{
    // is event reporting suspended?
    if ( m_suppressNextUpdate )
    {
        // do process the next one
        m_suppressNextUpdate = FALSE;

        return FALSE;
    }

    wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, GetId());
    InitCommandEvent(event);
    event.SetString(GetValue());

    return ProcessCommand(event);
}

bool wxTextCtrl::MSWCommand(WXUINT param, WXWORD WXUNUSED(id))
{
    switch ( param )
    {
        case EN_SETFOCUS:
        case EN_KILLFOCUS:
            {
                wxFocusEvent event(param == EN_KILLFOCUS ? wxEVT_KILL_FOCUS
                                                         : wxEVT_SET_FOCUS,
                                   m_windowId);
                event.SetEventObject(this);
                GetEventHandler()->ProcessEvent(event);
            }
            break;

        case EN_CHANGE:
            SendUpdateEvent();
            break;

        case EN_MAXTEXT:
            // the text size limit has been hit -- try to increase it
            if ( !AdjustSpaceLimit() )
            {
                wxCommandEvent event(wxEVT_COMMAND_TEXT_MAXLEN, m_windowId);
                InitCommandEvent(event);
                event.SetString(GetValue());
                ProcessCommand(event);
            }
            break;

            // the other edit notification messages are not processed
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
    wxColour colBack = GetBackgroundColour();

    if (!IsEnabled() && (GetWindowStyle() & wxTE_MULTILINE) == 0)
        colBack = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);

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
        col = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
#endif

    RECT rect;
    ::GetClientRect(GetHwnd(), &rect);

    COLORREF ref = wxColourToRGB(col);
    HBRUSH hBrush = ::CreateSolidBrush(ref);
    if ( !hBrush )
        wxLogLastError(wxT("CreateSolidBrush"));

    HDC hdc = (HDC)event.GetDC()->GetHDC();

    int mode = ::SetMapMode(hdc, MM_TEXT);

    ::FillRect(hdc, &rect, hBrush);
    ::DeleteObject(hBrush);
    ::SetMapMode(hdc, mode);

}
#endif // Win16

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
        if ( IsRich() )
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
    // we don't want focus if we can't be edited unless we're a multiline
    // control because then it might be still nice to get focus from keyboard
    // to be able to scroll it without mouse
    return (IsEditable() || IsMultiLine()) && wxControl::AcceptsFocus();
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

void wxTextCtrl::OnDelete(wxCommandEvent& event)
{
    long from, to;
    GetSelection(& from, & to);
    if (from != -1 && to != -1)
        Remove(from, to);
}

void wxTextCtrl::OnSelectAll(wxCommandEvent& event)
{
    SetSelection(-1, -1);
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

void wxTextCtrl::OnUpdateDelete(wxUpdateUIEvent& event)
{
    long from, to;
    GetSelection(& from, & to);
    event.Enable(from != -1 && to != -1 && from != to && IsEditable()) ;
}

void wxTextCtrl::OnUpdateSelectAll(wxUpdateUIEvent& event)
{
    event.Enable(GetLastPosition() > 0);
}

void wxTextCtrl::OnRightClick(wxMouseEvent& event)
{
#if wxUSE_RICHEDIT
    if (IsRich())
    {
        if (!m_privateContextMenu)
        {
            m_privateContextMenu = new wxMenu;
            m_privateContextMenu->Append(wxID_UNDO, _("&Undo"));
            m_privateContextMenu->Append(wxID_REDO, _("&Redo"));
            m_privateContextMenu->AppendSeparator();
            m_privateContextMenu->Append(wxID_CUT, _("Cu&t"));
            m_privateContextMenu->Append(wxID_COPY, _("&Copy"));
            m_privateContextMenu->Append(wxID_PASTE, _("&Paste"));
            m_privateContextMenu->Append(wxID_CLEAR, _("&Delete"));
            m_privateContextMenu->AppendSeparator();
            m_privateContextMenu->Append(wxID_SELECTALL, _("Select &All"));
        }
        PopupMenu(m_privateContextMenu, event.GetPosition());
        return;
    }
    else
#endif
    event.Skip();
}

// the rest of the file only deals with the rich edit controls
#if wxUSE_RICHEDIT

// ----------------------------------------------------------------------------
// EN_LINK processing
// ----------------------------------------------------------------------------

bool wxTextCtrl::MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result)
{
    NMHDR *hdr = (NMHDR* )lParam;
    switch ( hdr->code )
    {
       case EN_MSGFILTER:
            {
                const MSGFILTER *msgf = (MSGFILTER *)lParam;
                UINT msg = msgf->msg;

                // this is a bit crazy but richedit 1.0 sends us all mouse
                // events _except_ WM_LBUTTONUP (don't ask me why) so we have
                // generate the wxWin events for this message manually
                //
                // NB: in fact, this is still not totally correct as it does
                //     send us WM_LBUTTONUP if the selection was cleared by the
                //     last click -- so currently we get 2 events in this case,
                //     but as I don't see any obvious way to check for this I
                //     leave this code in place because it's still better than
                //     not getting left up events at all
                if ( msg == WM_LBUTTONUP )
                {
                    WXUINT flags = msgf->wParam;
                    int x = GET_X_LPARAM(msgf->lParam),
                        y = GET_Y_LPARAM(msgf->lParam);

                    HandleMouseEvent(msg, x, y, flags);
                }
            }

            // return TRUE to process the event (and FALSE to ignore it)
            return TRUE;

        case EN_LINK:
            {
                const ENLINK *enlink = (ENLINK *)hdr;

                switch ( enlink->msg )
                {
                    case WM_SETCURSOR:
                        // ok, so it is hardcoded - do we really nee to
                        // customize it?
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
            }
            return TRUE;
    }
    
    // not processed, leave it to the base class
    return wxTextCtrlBase::MSWOnNotify(idCtrl, lParam, result);
}

// ----------------------------------------------------------------------------
// colour setting for the rich edit controls
// ----------------------------------------------------------------------------

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

#if wxUSE_RICHEDIT

bool wxTextCtrl::SetStyle(long start, long end, const wxTextAttr& style)
{
    if ( !IsRich() )
    {
        // can't do it with normal text control
        return FALSE;
    }

    // the richedit 1.0 doesn't handle setting background colour, so don't
    // even try to do anything if it's the only thing we want to change
    if ( m_verRichEdit == 1 && !style.HasFont() && !style.HasTextColour() &&
        !style.HasLeftIndent() && !style.HasRightIndent() && !style.HasAlignment() &&
        !style.HasTabs() )
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
    {
        DoSetSelection(start, end, FALSE /* don't scroll caret into view */);
    }

    // initialize CHARFORMAT struct
#if wxUSE_RICHEDIT2
    CHARFORMAT2 cf;
#else
    CHARFORMAT cf;
#endif

    wxZeroMemory(cf);

    // we can't use CHARFORMAT2 with RichEdit 1.0, so pretend it is a simple
    // CHARFORMAT in that case
#if wxUSE_RICHEDIT2
    if ( m_verRichEdit == 1 )
    {
        // this is the only thing the control is going to grok
        cf.cbSize = sizeof(CHARFORMAT);
    }
    else
#endif
    {
        // CHARFORMAT or CHARFORMAT2
        cf.cbSize = sizeof(cf);
    }

    if ( style.HasFont() )
    {
        // VZ: CFM_CHARSET doesn't seem to do anything at all in RichEdit 2.0
        //     but using it doesn't seem to hurt neither so leaving it for now

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
    if ( m_verRichEdit != 1 && style.HasBackgroundColour() )
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

    // now do the paragraph formatting
    PARAFORMAT2 pf;
    wxZeroMemory(pf);
    // we can't use PARAFORMAT2 with RichEdit 1.0, so pretend it is a simple
    // PARAFORMAT in that case
#if wxUSE_RICHEDIT2
    if ( m_verRichEdit == 1 )
    {
        // this is the only thing the control is going to grok
        pf.cbSize = sizeof(PARAFORMAT);
    }
    else
#endif
    {
        // PARAFORMAT or PARAFORMAT2
        pf.cbSize = sizeof(pf);
    }

    if (style.HasAlignment())
    {
        pf.dwMask |= PFM_ALIGNMENT;
        if (style.GetAlignment() == wxTEXT_ALIGNMENT_RIGHT)
            pf.wAlignment = PFA_RIGHT;
        else if (style.GetAlignment() == wxTEXT_ALIGNMENT_CENTRE)
            pf.wAlignment = PFA_CENTER;
        else if (style.GetAlignment() == wxTEXT_ALIGNMENT_JUSTIFIED)
            pf.wAlignment = PFA_JUSTIFY;
        else
            pf.wAlignment = PFA_LEFT;
    }

    if (style.HasLeftIndent())
    {
        pf.dwMask |= PFM_STARTINDENT;

        // Convert from 1/10 mm to TWIPS
        pf.dxStartIndent = (int) (((double) style.GetLeftIndent()) * mm2twips / 10.0) ;

        // TODO: do we need to specify dxOffset?
    }

    if (style.HasRightIndent())
    {
        pf.dwMask |= PFM_RIGHTINDENT;

        // Convert from 1/10 mm to TWIPS
        pf.dxRightIndent = (int) (((double) style.GetRightIndent()) * mm2twips / 10.0) ;
    }

    if (style.HasTabs())
    {
        pf.dwMask |= PFM_TABSTOPS;

        const wxArrayInt& tabs = style.GetTabs();

        pf.cTabCount = wxMin(tabs.GetCount(), MAX_TAB_STOPS);
        size_t i;
        for (i = 0; i < (size_t) pf.cTabCount; i++)
        {
            // Convert from 1/10 mm to TWIPS
            pf.rgxTabs[i] = (int) (((double) tabs[i]) * mm2twips / 10.0) ;
        }
    }

    if (pf.dwMask != 0)
    {
        // do format the selection
        bool ok = ::SendMessage(GetHwnd(), EM_SETPARAFORMAT,
            0, (LPARAM) &pf) != 0;
        if ( !ok )
        {
            wxLogDebug(_T("SendMessage(EM_SETPARAFORMAT, 0) failed"));
        }
    }

    if ( changeSel )
    {
        // restore the original selection
        DoSetSelection(startOld, endOld, FALSE);
    }

    return ok;
}

bool wxTextCtrl::SetDefaultStyle(const wxTextAttr& style)
{
    if ( !wxTextCtrlBase::SetDefaultStyle(style) )
        return FALSE;

    // we have to do this or the style wouldn't apply for the text typed by the
    // user
    long posLast = GetLastPosition();
    SetStyle(posLast, posLast, m_defaultStyle);

    return TRUE;
}

bool wxTextCtrl::GetStyle(long position, wxTextAttr& style)
{
    if ( !IsRich() )
    {
        // can't do it with normal text control
        return FALSE;
    }

    // initialize CHARFORMAT struct
#if wxUSE_RICHEDIT2
    CHARFORMAT2 cf;
#else
    CHARFORMAT cf;
#endif

    wxZeroMemory(cf);

    // we can't use CHARFORMAT2 with RichEdit 1.0, so pretend it is a simple
    // CHARFORMAT in that case
#if wxUSE_RICHEDIT2
    if ( m_verRichEdit == 1 )
    {
        // this is the only thing the control is going to grok
        cf.cbSize = sizeof(CHARFORMAT);
    }
    else
#endif
    {
        // CHARFORMAT or CHARFORMAT2
        cf.cbSize = sizeof(cf);
    }
    // we can only change the format of the selection, so select the range we
    // want and restore the old selection later
    long startOld, endOld;
    GetSelection(&startOld, &endOld);

    // but do we really have to change the selection?
    bool changeSel = position != startOld || position != endOld;

    if ( changeSel )
    {
        DoSetSelection(position, position, FALSE /* don't scroll caret into view */);
    }

    // get the selection formatting
    (void) ::SendMessage(GetHwnd(), EM_GETCHARFORMAT,
                            SCF_SELECTION, (LPARAM)&cf) ;

    LOGFONT lf;
    lf.lfHeight = cf.yHeight;
    lf.lfWidth = 0;
    lf.lfCharSet = ANSI_CHARSET; // FIXME: how to get correct charset?
    lf.lfClipPrecision = 0;
    lf.lfEscapement = 0;
    wxStrcpy(lf.lfFaceName, cf.szFaceName);
    if (cf.dwEffects & CFE_ITALIC)
        lf.lfItalic = TRUE;
    lf.lfOrientation = 0;
    lf.lfPitchAndFamily = cf.bPitchAndFamily;
    lf.lfQuality = 0;
    if (cf.dwEffects & CFE_STRIKEOUT)
        lf.lfStrikeOut = TRUE;
    if (cf.dwEffects & CFE_UNDERLINE)
        lf.lfUnderline = TRUE;
    if (cf.dwEffects & CFE_BOLD)
        lf.lfWeight = FW_BOLD;

    wxFont font = wxCreateFontFromLogFont(& lf);
    if (font.Ok())
    {
        style.SetFont(font);
    }
    style.SetTextColour(wxColour(cf.crTextColor));

#if wxUSE_RICHEDIT2
    if ( m_verRichEdit != 1 )
    {
        // cf.dwMask |= CFM_BACKCOLOR;
        style.SetBackgroundColour(wxColour(cf.crBackColor));
    }
#endif // wxUSE_RICHEDIT2

    // now get the paragraph formatting
    PARAFORMAT2 pf;
    wxZeroMemory(pf);
    // we can't use PARAFORMAT2 with RichEdit 1.0, so pretend it is a simple
    // PARAFORMAT in that case
#if wxUSE_RICHEDIT2
    if ( m_verRichEdit == 1 )
    {
        // this is the only thing the control is going to grok
        pf.cbSize = sizeof(PARAFORMAT);
    }
    else
#endif
    {
        // PARAFORMAT or PARAFORMAT2
        pf.cbSize = sizeof(pf);
    }

    // do format the selection
    (void) ::SendMessage(GetHwnd(), EM_GETPARAFORMAT, 0, (LPARAM) &pf) ;

    style.SetLeftIndent( (int) ((double) pf.dxStartIndent * twips2mm * 10.0) );
    style.SetRightIndent( (int) ((double) pf.dxRightIndent * twips2mm * 10.0) );

    if (pf.wAlignment == PFA_CENTER)
        style.SetAlignment(wxTEXT_ALIGNMENT_CENTRE);
    else if (pf.wAlignment == PFA_RIGHT)
        style.SetAlignment(wxTEXT_ALIGNMENT_RIGHT);
    else if (pf.wAlignment == PFA_JUSTIFY)
        style.SetAlignment(wxTEXT_ALIGNMENT_JUSTIFIED);
    else
        style.SetAlignment(wxTEXT_ALIGNMENT_LEFT);

    wxArrayInt tabStops;
    size_t i;
    for (i = 0; i < (size_t) pf.cTabCount; i++)
    {
        tabStops[i] = (int) ((double) (pf.rgxTabs[i] & 0xFFFF) * twips2mm * 10.0) ;
    }

    if ( changeSel )
    {
        // restore the original selection
        DoSetSelection(startOld, endOld, FALSE);
    }

    return TRUE;
}

#endif

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
    for ( size_t i = 0; i < WXSIZEOF(ms_hRichEdit); i++ )
    {
        if ( ms_hRichEdit[i] )
        {
            ::FreeLibrary(ms_hRichEdit[i]);
        }
    }
}

/* static */
bool wxRichEditModule::Load(int version)
{
    // we don't support loading richedit 3.0 as I don't know how to distinguish
    // it from 2.0 anyhow
    wxCHECK_MSG( version == 1 || version == 2, FALSE,
                 _T("incorrect richedit control version requested") );

    // make it the index in the array
    version--;

    if ( ms_hRichEdit[version] == (HINSTANCE)-1 )
    {
        // we had already tried to load it and failed
        return FALSE;
    }

    if ( ms_hRichEdit[version] )
    {
        // we've already got this one
        return TRUE;
    }

    wxString dllname = version ? _T("riched20") : _T("riched32");
    dllname += _T(".dll");

    ms_hRichEdit[version] = ::LoadLibrary(dllname);

    if ( !ms_hRichEdit[version] )
    {
        wxLogSysError(_("Could not load Rich Edit DLL '%s'"), dllname.c_str());

        ms_hRichEdit[version] = (HINSTANCE)-1;

        return FALSE;
    }

    return TRUE;
}

#endif // wxUSE_RICHEDIT

#endif // wxUSE_TEXTCTRL
