/////////////////////////////////////////////////////////////////////////////
// Name:        msw/combobox.cpp
// Purpose:     wxComboBox class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
#pragma implementation "combobox.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_COMBOBOX

#ifndef WX_PRECOMP
    #include "wx/settings.h"
    #include "wx/log.h"
    // for wxEVT_COMMAND_TEXT_ENTER
    #include "wx/textctrl.h"
#endif

#include "wx/combobox.h"
#include "wx/brush.h"
#include "wx/clipbrd.h"
#include "wx/msw/private.h"

#if wxUSE_TOOLTIPS
    #if !defined(__GNUWIN32_OLD__) || defined(__CYGWIN10__)
        #include <commctrl.h>
    #endif
    #include "wx/tooltip.h"
#endif // wxUSE_TOOLTIPS

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxComboBox, wxControl)

// ----------------------------------------------------------------------------
// function prototypes
// ----------------------------------------------------------------------------

LRESULT APIENTRY _EXPORT wxComboEditWndProc(HWND hWnd,
                                            UINT message,
                                            WPARAM wParam,
                                            LPARAM lParam);

// ---------------------------------------------------------------------------
// global vars
// ---------------------------------------------------------------------------

// the pointer to standard radio button wnd proc
static WXFARPROC gs_wndprocEdit = (WXFARPROC)NULL;

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wnd proc for subclassed edit control
// ----------------------------------------------------------------------------

LRESULT APIENTRY _EXPORT wxComboEditWndProc(HWND hWnd,
                                            UINT message,
                                            WPARAM wParam,
                                            LPARAM lParam)
{
    HWND hwndCombo = ::GetParent(hWnd);
    wxWindow *win = wxFindWinFromHandle((WXHWND)hwndCombo);

    switch ( message )
    {
        // forward some messages to the combobox to generate the appropriate
        // wxEvents from them
        case WM_KEYUP:
        case WM_KEYDOWN:
        case WM_CHAR:
        case WM_SETFOCUS:
        case WM_KILLFOCUS:
            {
                wxComboBox *combo = wxDynamicCast(win, wxComboBox);
                if ( !combo )
                {
                    // we can get WM_KILLFOCUS while our parent is already half
                    // destroyed and hence doesn't look like a combobx any
                    // longer, check for it to avoid bogus assert failures
                    if ( !win->IsBeingDeleted() )
                    {
                        wxFAIL_MSG( _T("should have combo as parent") );
                    }
                }
                else if ( combo->MSWProcessEditMsg(message, wParam, lParam) )
                {
                    // handled by parent
                    return 0;
                }
            }
            break;

        case WM_GETDLGCODE:
            {
                wxCHECK_MSG( win, 0, _T("should have a parent") );

                if ( win->GetWindowStyle() & wxPROCESS_ENTER )
                {
                    // need to return a custom dlg code or we'll never get it
                    return DLGC_WANTMESSAGE;
                }
            }
            break;

        // deal with tooltips here
#if wxUSE_TOOLTIPS && defined(TTN_NEEDTEXT)
        case WM_NOTIFY:
            {
                wxCHECK_MSG( win, 0, _T("should have a parent") );

                NMHDR* hdr = (NMHDR *)lParam;
                if ( hdr->code == TTN_NEEDTEXT )
                {
                    wxToolTip *tooltip = win->GetToolTip();
                    if ( tooltip )
                    {
                        TOOLTIPTEXT *ttt = (TOOLTIPTEXT *)lParam;
                        ttt->lpszText = (wxChar *)tooltip->GetTip().c_str();
                    }

                    // processed
                    return 0;
                }
            }
            break;
#endif // wxUSE_TOOLTIPS
    }

    return ::CallWindowProc(CASTWNDPROC gs_wndprocEdit, hWnd, message, wParam, lParam);
}

WXHBRUSH wxComboBox::OnCtlColor(WXHDC pDC, WXHWND WXUNUSED(pWnd), WXUINT WXUNUSED(nCtlColor),
                               WXUINT WXUNUSED(message),
                               WXWPARAM WXUNUSED(wParam),
                               WXLPARAM WXUNUSED(lParam)
    )
{
    HDC hdc = (HDC)pDC;
    if (GetParent()->GetTransparentBackground())
        SetBkMode(hdc, TRANSPARENT);
    else
        SetBkMode(hdc, OPAQUE);

    wxColour colBack = GetBackgroundColour();

    if (!IsEnabled())
        colBack = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);

    ::SetBkColor(hdc, wxColourToRGB(colBack));
    ::SetTextColor(hdc, wxColourToRGB(GetForegroundColour()));

    wxBrush *brush = wxTheBrushList->FindOrCreateBrush(colBack, wxSOLID);

    return (WXHBRUSH)brush->GetResourceHandle();
}

// ----------------------------------------------------------------------------
// wxComboBox
// ----------------------------------------------------------------------------

bool wxComboBox::MSWProcessEditMsg(WXUINT msg, WXWPARAM wParam, WXLPARAM lParam)
{
    switch ( msg )
    {
        case WM_CHAR:
            // for compatibility with wxTextCtrl, generate a special message
            // when Enter is pressed
            if ( wParam == VK_RETURN )
            {
                wxCommandEvent event(wxEVT_COMMAND_TEXT_ENTER, m_windowId);
                InitCommandEvent(event);
                event.SetString(GetValue());
                event.SetInt(GetSelection());
                ProcessCommand(event);
            }

            return HandleChar(wParam, lParam, TRUE /* isASCII */);

        case WM_KEYDOWN:
            return HandleKeyDown(wParam, lParam);

        case WM_KEYUP:
            return HandleKeyUp(wParam, lParam);

        case WM_SETFOCUS:
            return HandleSetFocus((WXHWND)wParam);

        case WM_KILLFOCUS:
            return HandleKillFocus((WXHWND)wParam);
    }

    return FALSE;
}

bool wxComboBox::MSWCommand(WXUINT param, WXWORD WXUNUSED(id))
{
    wxString value;
    int sel = -1;
    switch ( param )
    {
        case CBN_SELCHANGE:
            sel = GetSelection();
            if ( sel > -1 )
            {
                value = GetString(sel);

                wxCommandEvent event(wxEVT_COMMAND_COMBOBOX_SELECTED, GetId());
                event.SetInt(sel);
                event.SetEventObject(this);
                event.SetString(value);
                ProcessCommand(event);
            }
            else
            {
                break;
            }

            // fall through: for compability with wxGTK, also send the text
            // update event when the selection changes (this also seems more
            // logical as the text does change)

        case CBN_EDITCHANGE:
            {
                wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, GetId());

                // if sel != -1, value was initialized above (and we can't use
                // GetValue() here as it would return the old selection and we
                // want the new one)
                if ( sel == -1 )
                {
                    value = GetValue();
                }
                else // we're synthesizing text updated event from sel change
                {
                    // we need to do this because the user code expects
                    // wxComboBox::GetValue() to return the new value from
                    // "text updated" handler but it hadn't been updated yet
                    SetValue(value);
                }

                event.SetString(value);
                event.SetEventObject(this);
                ProcessCommand(event);
            }
            break;
    }

    // there is no return value for the CBN_ notifications, so always return
    // FALSE from here to pass the message to DefWindowProc()
    return FALSE;
}

WXHWND wxComboBox::GetEditHWND() const
{
    // this function should not be called for wxCB_READONLY controls, it is
    // the callers responsability to check this
    wxASSERT_MSG( !(GetWindowStyle() & wxCB_READONLY),
                  _T("read-only combobox doesn't have any edit control") );

    POINT pt;
    pt.x = pt.y = 4;
    HWND hwndEdit = ::ChildWindowFromPoint(GetHwnd(), pt);
    if ( !hwndEdit || hwndEdit == GetHwnd() )
    {
        wxFAIL_MSG(_T("not read only combobox without edit control?"));
    }

    return (WXHWND)hwndEdit;
}

bool wxComboBox::Create(wxWindow *parent, wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,
                        const wxSize& size,
                        int n, const wxString choices[],
                        long style,
                        const wxValidator& validator,
                        const wxString& name)
{
    // pretend that wxComboBox is hidden while it is positioned and resized and
    // show it only right before leaving this method because otherwise there is
    // some noticeable flicker while the control rearranges itself
    m_isShown = FALSE;

    // first create wxWin object
    if ( !CreateControl(parent, id, pos, size, style, validator, name) )
        return FALSE;

    // get the right style
    long msStyle = WS_TABSTOP | WS_VSCROLL | WS_HSCROLL |
                   CBS_AUTOHSCROLL | CBS_NOINTEGRALHEIGHT /* | WS_CLIPSIBLINGS */;
    if ( style & wxCB_READONLY )
        msStyle |= CBS_DROPDOWNLIST;
    else if ( style & wxCB_SIMPLE )
        msStyle |= CBS_SIMPLE; // A list (shown always) and edit control
    else
        msStyle |= CBS_DROPDOWN;

    if ( style & wxCB_SORT )
        msStyle |= CBS_SORT;

    if ( style & wxCLIP_SIBLINGS )
        msStyle |= WS_CLIPSIBLINGS;


    // and now create the MSW control
    if ( !MSWCreateControl(_T("COMBOBOX"), msStyle) )
        return FALSE;

    // A choice/combobox normally has a white background (or other, depending
    // on global settings) rather than inheriting the parent's background colour.
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

    for ( int i = 0; i < n; i++ )
    {
        Append(choices[i]);
    }

    if ( !value.IsEmpty() )
    {
        SetValue(value);
    }

    // do this after appending the values to the combobox so that autosizing
    // works correctly
    SetSize(pos.x, pos.y, size.x, size.y);

    // a (not read only) combobox is, in fact, 2 controls: the combobox itself
    // and an edit control inside it and if we want to catch events from this
    // edit control, we must subclass it as well
    if ( !(style & wxCB_READONLY) )
    {
        gs_wndprocEdit = (WXFARPROC)::SetWindowLong
                                      (
                                        (HWND)GetEditHWND(),
                                        GWL_WNDPROC,
                                        (LPARAM)wxComboEditWndProc
                                      );
    }

    // and finally, show the control
    Show(TRUE);

    return TRUE;
}

void wxComboBox::SetValue(const wxString& value)
{
    if ( HasFlag(wxCB_READONLY) )
        SetStringSelection(value);
    else
        SetWindowText(GetHwnd(), value.c_str());
}

// Clipboard operations
void wxComboBox::Copy()
{
  SendMessage(GetHwnd(), WM_COPY, 0, 0L);
}

void wxComboBox::Cut()
{
  SendMessage(GetHwnd(), WM_CUT, 0, 0L);
}

void wxComboBox::Paste()
{
  SendMessage(GetHwnd(), WM_PASTE, 0, 0L);
}

void wxComboBox::SetEditable(bool WXUNUSED(editable))
{
  // Can't implement in MSW?
//  HWND hWnd = GetHwnd();
//  SendMessage(hWnd, EM_SETREADONLY, (WPARAM)!editable, (LPARAM)0L);
}

void wxComboBox::SetInsertionPoint(long pos)
{
    if ( GetWindowStyle() & wxCB_READONLY )
        return;

#ifdef __WIN32__
    HWND hWnd = GetHwnd();
    ::SendMessage(hWnd, CB_SETEDITSEL, 0, MAKELPARAM(pos, pos));
    HWND hEditWnd = (HWND) GetEditHWND() ;
    if ( hEditWnd )
    {
        // Scroll insertion point into view
        SendMessage(hEditWnd, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);
        // Why is this necessary? (Copied from wxTextCtrl::SetInsertionPoint)
        SendMessage(hEditWnd, EM_REPLACESEL, 0, (LPARAM)_T(""));
    }
#endif // __WIN32__
}

void wxComboBox::SetInsertionPointEnd()
{
    // setting insertion point doesn't make sense for read only comboboxes
    if ( !(GetWindowStyle() & wxCB_READONLY) )
    {
        long pos = GetLastPosition();
        SetInsertionPoint(pos);
    }
}

long wxComboBox::GetInsertionPoint() const
{
#ifdef __WIN32__
    DWORD Pos=(DWORD)SendMessage(GetHwnd(), CB_GETEDITSEL, 0, 0L);
    return Pos&0xFFFF;
#else
    return 0;
#endif
}

long wxComboBox::GetLastPosition() const
{
    HWND hEditWnd = (HWND) GetEditHWND();

    // Get number of characters in the last (only) line. We'll add this to the character
    // index for the last line, 1st position.
    int lineLength = (int)SendMessage(hEditWnd, EM_LINELENGTH, (WPARAM) 0, (LPARAM)0L);

    return (long)(lineLength);
}

void wxComboBox::Replace(long from, long to, const wxString& value)
{
#if wxUSE_CLIPBOARD
    Remove(from, to);

    // Now replace with 'value', by pasting.
    wxSetClipboardData(wxDF_TEXT, (wxObject *)(const wxChar *)value, 0, 0);

    // Paste into edit control
    SendMessage(GetHwnd(), WM_PASTE, (WPARAM)0, (LPARAM)0L);
#endif
}

void wxComboBox::Remove(long from, long to)
{
    // Set selection and remove it
    SetSelection(from, to);
    SendMessage(GetHwnd(), WM_CUT, (WPARAM)0, (LPARAM)0);
}

void wxComboBox::SetSelection(long from, long to)
{
    HWND hWnd = GetHwnd();
    long fromChar = from;
    long toChar = to;
    // if from and to are both -1, it means
    // (in wxWindows) that all text should be selected.
    // This translates into Windows convention
    if ((from == -1) && (to == -1))
    {
      fromChar = 0;
      toChar = -1;
    }

    if (
#ifdef __WIN32__
    SendMessage(hWnd, CB_SETEDITSEL, (WPARAM)0, (LPARAM)MAKELONG(fromChar, toChar))
#else // Win16
    SendMessage(hWnd, CB_SETEDITSEL, (WPARAM)fromChar, (LPARAM)toChar)
#endif
        == CB_ERR )
    {
        wxLogDebug(_T("CB_SETEDITSEL failed"));
    }
}

#endif
 // wxUSE_COMBOBOX

