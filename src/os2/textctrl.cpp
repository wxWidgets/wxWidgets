/////////////////////////////////////////////////////////////////////////////
// Name:        textctrl.cpp
// Purpose:     wxTextCtrl
// Author:      David Webster
// Modified by:
// Created:     10/17/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/textctrl.h"
    #include "wx/settings.h"
    #include "wx/brush.h"
    #include "wx/utils.h"
    #include "wx/log.h"
#endif

#if wxUSE_CLIPBOARD
    #include "wx/app.h"
    #include "wx/clipbrd.h"
#endif

#include "wx/textfile.h"

#include "wx/os2/private.h"

#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

#if wxUSE_IOSTREAMH
#   include <fstream.h>
#else
#   include <fstream>
#endif

#if !USE_SHARED_LIBRARY

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

#endif // USE_SHARED_LIBRARY

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

wxTextCtrl::wxTextCtrl()
{
}

bool wxTextCtrl::Create(wxWindow *parent, wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
#if wxUSE_VALIDATORS
                        const wxValidator& validator,
#endif
                        const wxString& name)
{
    // base initialization
    if ( !CreateBase(parent, id, pos, size, style, validator, name) )
        return FALSE;

    // Validator was set in CreateBase
    //SetValidator(validator);
    if ( parent )
        parent->AddChild(this);

    // set colours
    SetupColours();

    // translate wxWin style flags to MSW ones, checking for consistency while
    // doing it
// TODO:
/*
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
    const wxChar *windowClass = wxT("EDIT");

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
*/
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

    SetSize(pos.x, pos.y, size.x, size.y);

    return TRUE;
}

// Make sure the window style (etc.) reflects the HWND style (roughly)
void wxTextCtrl::AdoptAttributesFromHWND()
{
  wxWindow::AdoptAttributesFromHWND();

  HWND hWnd = GetHwnd();
  // TODO:
  /*
  long style = GetWindowLong(hWnd, GWL_STYLE);

  if (style & ES_MULTILINE)
    m_windowStyle |= wxTE_MULTILINE;
  if (style & ES_PASSWORD)
    m_windowStyle |= wxTE_PASSWORD;
  if (style & ES_READONLY)
    m_windowStyle |= wxTE_READONLY;
  if (style & ES_WANTRETURN)
    m_windowStyle |= wxTE_PROCESS_ENTER;
  */
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
    return wxGetWindowText(GetHWND());
}

void wxTextCtrl::SetValue(const wxString& value)
{
// TODO:
/*
    wxString valueDos = wxTextFile::Translate(value, wxTextFileType_Dos);

    SetWindowText(GetHwnd(), valueDos);

    AdjustSpaceLimit();
*/
}

void wxTextCtrl::WriteText(const wxString& value)
{
// TODO:
/*
    wxString valueDos = wxTextFile::Translate(value, wxTextFileType_Dos);

    SendMessage(GetHwnd(), EM_REPLACESEL, 0, (LPARAM)valueDos.c_str());

    AdjustSpaceLimit();
*/
}

void wxTextCtrl::AppendText(const wxString& text)
{
// TODO:
/*
    SetInsertionPointEnd();
    WriteText(text);
*/
}

void wxTextCtrl::Clear()
{
//    SetWindowText(GetHwnd(), wxT(""));
}

// ----------------------------------------------------------------------------
// Clipboard operations
// ----------------------------------------------------------------------------

void wxTextCtrl::Copy()
{
    if (CanCopy())
    {
        HWND hWnd = GetHwnd();
//        SendMessage(hWnd, WM_COPY, 0, 0L);
    }
}

void wxTextCtrl::Cut()
{
    if (CanCut())
    {
        HWND hWnd = GetHwnd();
//        SendMessage(hWnd, WM_CUT, 0, 0L);
    }
}

void wxTextCtrl::Paste()
{
    if (CanPaste())
    {
        HWND hWnd = GetHwnd();
//        SendMessage(hWnd, WM_PASTE, 0, 0L);
    }
}

bool wxTextCtrl::CanCopy() const
{
    // Can copy if there's a selection
    long from, to;
//    GetSelection(& from, & to);
    return (from != to);
}

bool wxTextCtrl::CanCut() const
{
    // Can cut if there's a selection
    long from, to;
//    GetSelection(& from, & to);
    return (from != to);
}

bool wxTextCtrl::CanPaste() const
{
    if (!IsEditable())
        return FALSE;

    // Standard edit control: check for straight text on clipboard
    bool isTextAvailable = FALSE;
//TODO:
/*
    if ( ::OpenClipboard(GetHwndOf(wxTheApp->GetTopWindow())) )
    {
        isTextAvailable = (::IsClipboardFormatAvailable(CF_TEXT) != 0);
        ::CloseClipboard();
    }
*/
    return isTextAvailable;
}

// ----------------------------------------------------------------------------
// Accessors
// ----------------------------------------------------------------------------

void wxTextCtrl::SetEditable(bool editable)
{
    HWND hWnd = GetHwnd();
//    SendMessage(hWnd, EM_SETREADONLY, (WPARAM)!editable, (LPARAM)0L);
}

void wxTextCtrl::SetInsertionPoint(long pos)
{
// TODO:
/*
    HWND hWnd = GetHwnd();
    {
        SendMessage(hWnd, EM_SETSEL, pos, pos);
        SendMessage(hWnd, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);
    }
    static const char *nothing = "";
    SendMessage(hWnd, EM_REPLACESEL, 0, (LPARAM)nothing);
*/
}

void wxTextCtrl::SetInsertionPointEnd()
{
// TODO:
/*
    long pos = GetLastPosition();
    SetInsertionPoint(pos);
*/
}

long wxTextCtrl::GetInsertionPoint() const
{
// TODO:
/*
    DWORD Pos = (DWORD)SendMessage(GetHwnd(), EM_GETSEL, 0, 0L);
    return Pos & 0xFFFF;
*/
    return 0;
}

long wxTextCtrl::GetLastPosition() const
{
    HWND hWnd = GetHwnd();

// TODO:
/*
    // Will always return a number > 0 (according to docs)
    int noLines = (int)SendMessage(hWnd, EM_GETLINECOUNT, (WPARAM)0, (LPARAM)0L);

    // This gets the char index for the _beginning_ of the last line
    int charIndex = (int)SendMessage(hWnd, EM_LINEINDEX, (WPARAM)(noLines-1), (LPARAM)0L);

    // Get number of characters in the last line. We'll add this to the character
    // index for the last line, 1st position.
    int lineLength = (int)SendMessage(hWnd, EM_LINELENGTH, (WPARAM)charIndex, (LPARAM)0L);

    return (long)(charIndex + lineLength);
*/
    return 0L;
}

// If the return values from and to are the same, there is no
// selection.
void wxTextCtrl::GetSelection(long* from, long* to) const
{
    DWORD dwStart, dwEnd;
    MPARAM wParam = (MPARAM) (DWORD*) & dwStart; // receives starting position
    MPARAM lParam = (MPARAM) (DWORD*) & dwEnd;   // receives ending position

//    ::SendMessage(GetHwnd(), EM_GETSEL, wParam, lParam);

    *from = dwStart;
    *to = dwEnd;
}

bool wxTextCtrl::IsEditable() const
{
// TODO:
/*
    long style = ::GetWindowLong(GetHwnd(), GWL_STYLE);

    return ((style & ES_READONLY) == 0);
*/
    return FALSE;
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
//    SendMessage(hWnd, EM_SETSEL, fromChar, toChar);
//    SendMessage(hWnd, WM_CUT, (WPARAM)0, (LPARAM)0);

    // Now replace with 'value', by pasting.
    wxSetClipboardData(wxDF_TEXT, (wxObject *) (const wxChar *)value, 0, 0);

    // Paste into edit control
//    SendMessage(hWnd, WM_PASTE, (WPARAM)0, (LPARAM)0L);
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
//    SendMessage(hWnd, EM_SETSEL, fromChar, toChar);
//    SendMessage(hWnd, WM_CUT, (WPARAM)0, (LPARAM)0);
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

//    SendMessage(hWnd, EM_SETSEL, (WPARAM)fromChar, (LPARAM)toChar);
//    SendMessage(hWnd, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);
}

bool wxTextCtrl::LoadFile(const wxString& file)
{
// TODO:
/*
    if ( wxTextCtrlBase::LoadFile(file) )
    {
        // update the size limit if needed
        AdjustSpaceLimit();

        return TRUE;
    }
*/
    return FALSE;
}

bool wxTextCtrl::IsModified() const
{
//    return (SendMessage(GetHwnd(), EM_GETMODIFY, 0, 0) != 0);
    return FALSE;
}

// Makes 'unmodified'
void wxTextCtrl::DiscardEdits()
{
//    SendMessage(GetHwnd(), EM_SETMODIFY, FALSE, 0L);
}

int wxTextCtrl::GetNumberOfLines() const
{
//    return (int)SendMessage(GetHwnd(), EM_GETLINECOUNT, (WPARAM)0, (LPARAM)0);
    return 0;
}

long wxTextCtrl::XYToPosition(long x, long y) const
{
    HWND hWnd = GetHwnd();

    // This gets the char index for the _beginning_ of this line
// TODO:
/*
    int charIndex = (int)SendMessage(hWnd, EM_LINEINDEX, (WPARAM)y, (LPARAM)0);
    return (long)(x + charIndex);
*/
    return 0L;
}

bool wxTextCtrl::PositionToXY(long pos, long *x, long *y) const
{
    HWND hWnd = GetHwnd();

    // This gets the line number containing the character
    int lineNo;
//    lineNo = (int)SendMessage(hWnd, EM_LINEFROMCHAR, (WPARAM)pos, 0);

    if ( lineNo == -1 )
    {
        // no such line
        return FALSE;
    }

    // This gets the char index for the _beginning_ of this line
    int charIndex = 0; // TODO: (int)SendMessage(hWnd, EM_LINEINDEX, (WPARAM)lineNo, (LPARAM)0);
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

// TODO:
/*
    int currentLineLineNo = (int)SendMessage(hWnd, EM_GETFIRSTVISIBLELINE, (WPARAM)0, (LPARAM)0L);

    int specifiedLineLineNo = (int)SendMessage(hWnd, EM_LINEFROMCHAR, (WPARAM)pos, (LPARAM)0L);

    int linesToScroll = specifiedLineLineNo - currentLineLineNo;

    if (linesToScroll != 0)
      (void)SendMessage(hWnd, EM_LINESCROLL, (WPARAM)0, (LPARAM)linesToScroll);
*/
}

int wxTextCtrl::GetLineLength(long lineNo) const
{
// TODO:
/*

    long charIndex = XYToPosition(0, lineNo);
    int len = (int)SendMessage(GetHwnd(), EM_LINELENGTH, charIndex, 0);
    return len;
*/
    return 0;
}

wxString wxTextCtrl::GetLineText(long lineNo) const
{
    size_t len = (size_t)GetLineLength(lineNo) + 1;
    char *buf = (char *)malloc(len);
    *(WORD *)buf = len;
    int noChars = 0; // TODO:(int)SendMessage(GetHwnd(), EM_GETLINE, lineNo, (LPARAM)buf);
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
//        ::SendMessage(GetHwnd(), EM_UNDO, 0, 0);
    }
}

void wxTextCtrl::Redo()
{
    if (CanRedo())
    {
        // Same as Undo, since Undo undoes the undo, i.e. a redo.
//        ::SendMessage(GetHwnd(), EM_UNDO, 0, 0);
    }
}

bool wxTextCtrl::CanUndo() const
{
//    return (::SendMessage(GetHwnd(), EM_CANUNDO, 0, 0) != 0);
    return FALSE;
}

bool wxTextCtrl::CanRedo() const
{
//    return (::SendMessage(GetHwnd(), EM_CANUNDO, 0, 0) != 0);
    return FALSE;
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

WXHBRUSH wxTextCtrl::OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
                                WXUINT message, WXWPARAM wParam,
                                WXLPARAM lParam)
{
    HDC hdc = (HDC)pDC;
// TODO:
/*
    SetBkMode(hdc, GetParent()->GetTransparentBackground() ? TRANSPARENT
                                                           : OPAQUE);

    ::SetBkColor(hdc, RGB(GetBackgroundColour().Red(), GetBackgroundColour().Green(), GetBackgroundColour().Blue()));
    ::SetTextColor(hdc, RGB(GetForegroundColour().Red(), GetForegroundColour().Green(), GetForegroundColour().Blue()));
*/
    wxBrush *backgroundBrush = wxTheBrushList->FindOrCreateBrush(GetBackgroundColour(), wxSOLID);

    return (WXHBRUSH) backgroundBrush->GetResourceHandle();
}

void wxTextCtrl::OnChar(wxKeyEvent& event)
{
    switch ( event.KeyCode() )
    {
// TODO:
/*
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
*/
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

bool wxTextCtrl::OS2Command(WXUINT param, WXWORD WXUNUSED(id))
{
    switch (param)
    {
// TODO:
/*
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

        case EN_ERRSPACE:
            // the text size limit has been hit - increase it
            AdjustSpaceLimit();
            break;

            // the other notification messages are not processed
        case EN_UPDATE:
        case EN_MAXTEXT:
        case EN_HSCROLL:
        case EN_VSCROLL:
*/
        default:
            return FALSE;
    }

    // processed
    return TRUE;
}

void wxTextCtrl::AdjustSpaceLimit()
{
// TODO:
/*
    unsigned int len = ::GetWindowTextLength(GetHwnd()),
    limit = ::SendMessage(GetHwnd(), EM_GETLIMITTEXT, 0, 0);
    if ( len > limit )
    {
        limit = len + 0x8000;    // 32Kb

        if ( limit > 0xffff )
            ::SendMessage(GetHwnd(), EM_LIMITTEXT, 0, limit);
        else
            ::SendMessage(GetHwnd(), EM_LIMITTEXT, limit, 0);
    }
*/
}

bool wxTextCtrl::AcceptsFocus() const
{
    // we don't want focus if we can't be edited
    return IsEditable() && wxControl::AcceptsFocus();
}

wxSize wxTextCtrl::DoGetBestSize()
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

