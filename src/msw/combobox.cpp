/////////////////////////////////////////////////////////////////////////////
// Name:        msw/combobox.cpp
// Purpose:     wxComboBox class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
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
#endif

#include "wx/combobox.h"
#include "wx/brush.h"
#include "wx/clipbrd.h"
#include "wx/msw/private.h"

#if wxUSE_TOOLTIPS
    #ifndef __GNUWIN32_OLD__
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
        // forward some messages to the combobox
        case WM_KEYUP:
        case WM_KEYDOWN:
        case WM_CHAR:
            {
                wxComboBox *combo = wxDynamicCast(win, wxComboBox);
                wxCHECK_MSG( combo, 0, _T("should have combo as parent") );

                if ( combo->MSWProcessEditMsg(message, wParam, lParam) )
                    return 0;
            }
            break;

#if 1
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
#endif // 0

        // deal with tooltips here
#if wxUSE_TOOLTIPS
        case WM_NOTIFY:
            {
                wxCHECK_MSG( win, 0, _T("should have a parent") );

                NMHDR* hdr = (NMHDR *)lParam;
                if ( (int)hdr->code == TTN_NEEDTEXT )
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

WXHBRUSH wxComboBox::OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
                               WXUINT message,
                               WXWPARAM wParam,
                               WXLPARAM lParam)
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

    if (!IsEnabled())
        colBack = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE);

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
            return HandleChar(wParam, lParam, TRUE /* isASCII */);

        case WM_KEYDOWN:
            return HandleKeyDown(wParam, lParam);

        case WM_KEYUP:
            return HandleKeyUp(wParam, lParam);
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
                // if sel != -1, value was initialized above (and we can't use
                // GetValue() here as it would return the old selection and we
                // want the new one)
                wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, GetId());
                event.SetString(sel == -1 ? GetValue() : value);
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

    // and now create the MSW control
    if ( !MSWCreateControl(_T("COMBOBOX"), msStyle) )
        return FALSE;

    SetSize(pos.x, pos.y, size.x, size.y);

    // A choice/combobox normally has a white background (or other, depending
    // on global settings) rather than inheriting the parent's background colour.
    SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_WINDOW));

    for ( int i = 0; i < n; i++ )
    {
        Append(choices[i]);
    }

    if ( !value.IsEmpty() )
    {
        SetValue(value);
    }

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

    return TRUE;
}

// TODO: update and clear all this horrible mess (VZ)

void wxComboBox::SetValue(const wxString& value)
{
  // If newlines are denoted by just 10, must stick 13 in front.
  int singletons = 0;
  int len = value.Length();
  int i;
  for (i = 0; i < len; i ++)
  {
    if ((i > 0) && (value[i] == 10) && (value[i-1] != 13))
      singletons ++;
  }
  if (singletons > 0)
  {
    wxChar *tmp = new wxChar[len + singletons + 1];
    int j = 0;
    for (i = 0; i < len; i ++)
    {
      if ((i > 0) && (value[i] == 10) && (value[i-1] != 13))
      {
        tmp[j] = 13;
        j ++;
      }
      tmp[j] = value[i];
      j ++;
    }
    tmp[j] = 0;
    SetWindowText(GetHwnd(), tmp);
    delete[] tmp;
  }
  else
    SetWindowText(GetHwnd(), value);
}

// Clipboard operations
void wxComboBox::Copy()
{
  HWND hWnd = GetHwnd();
  SendMessage(hWnd, WM_COPY, 0, 0L);
}

void wxComboBox::Cut()
{
  HWND hWnd = GetHwnd();
  SendMessage(hWnd, WM_CUT, 0, 0L);
}

void wxComboBox::Paste()
{
  HWND hWnd = GetHwnd();
  SendMessage(hWnd, WM_PASTE, 0, 0L);
}

void wxComboBox::SetEditable(bool editable)
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
    HWND hWnd = GetHwnd();
    long fromChar = from;
    long toChar = to;

    // Set selection and remove it
#ifdef __WIN32__
    SendMessage(hWnd, CB_SETEDITSEL, fromChar, toChar);
#else
    SendMessage(hWnd, CB_SETEDITSEL, (WPARAM)0, (LPARAM)MAKELONG(fromChar, toChar));
#endif
    SendMessage(hWnd, WM_CUT, (WPARAM)0, (LPARAM)0);

    // Now replace with 'value', by pasting.
    wxSetClipboardData(wxDF_TEXT, (wxObject *)(const wxChar *)value, 0, 0);

    // Paste into edit control
    SendMessage(hWnd, WM_PASTE, (WPARAM)0, (LPARAM)0L);
#endif
}

void wxComboBox::Remove(long from, long to)
{
    HWND hWnd = GetHwnd();
    long fromChar = from;
    long toChar = to;

    // Cut all selected text
#ifdef __WIN32__
    SendMessage(hWnd, CB_SETEDITSEL, fromChar, toChar);
#else
    SendMessage(hWnd, CB_SETEDITSEL, (WPARAM)0, (LPARAM)MAKELONG(fromChar, toChar));
#endif
    SendMessage(hWnd, WM_CUT, (WPARAM)0, (LPARAM)0);
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

#ifdef __WIN32__
    SendMessage(hWnd, CB_SETEDITSEL, (WPARAM)fromChar, (LPARAM)toChar);
//    SendMessage(hWnd, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);
#else
    // WPARAM is 0: selection is scrolled into view
    SendMessage(hWnd, CB_SETEDITSEL, (WPARAM)0, (LPARAM)MAKELONG(fromChar, toChar));
#endif
}

void wxComboBox::DoMoveWindow(int x, int y, int width, int height)
{
    // here is why this is necessary: if the width is negative, the combobox
    // window proc makes the window of the size width*height instead of
    // interpreting height in the usual manner (meaning the height of the drop
    // down list - usually the height specified in the call to MoveWindow()
    // will not change the height of combo box per se)
    //
    // this behaviour is not documented anywhere, but this is just how it is
    // here (NT 4.4) and, anyhow, the check shouldn't hurt - however without
    // the check, constraints/sizers using combos may break the height
    // constraint will have not at all the same value as expected
    if ( width < 0 )
        return;

    int cx, cy;
    wxGetCharSize(GetHWND(), &cx, &cy, &GetFont());

    // what should the height of the drop down list be? we choose 10 items by
    // default and also 10 items max (if we always use n, the list will never
    // have vertical scrollbar)
    int n = GetCount();
    if ( !n || (n > 10) )
        n = 10;

    height = (n + 1)* EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy);

    wxControl::DoMoveWindow(x, y, width, height);
}

wxSize wxComboBox::DoGetBestSize() const
{
    // the choice calculates the horz size correctly, but not the vertical
    // component: correct it
    wxSize size = wxChoice::DoGetBestSize();

    int cx, cy;
    wxGetCharSize(GetHWND(), &cx, &cy, &GetFont());
    size.y = EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy);

    return size;
}

#endif
 // wxUSE_COMBOBOX

