/////////////////////////////////////////////////////////////////////////////
// Name:        combobox.cpp
// Purpose:     wxComboBox class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "combobox.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/setup.h"
#endif

#if wxUSE_COMBOBOX

#include "wx/combobox.h"
#include "wx/clipbrd.h"
#include "wx/msw/private.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxComboBox, wxControl)
#endif

bool wxComboBox::MSWCommand(WXUINT param, WXWORD WXUNUSED(id))
{
  if (param == CBN_SELCHANGE)
  {
    wxCommandEvent event(wxEVT_COMMAND_COMBOBOX_SELECTED, m_windowId);
    event.SetInt(GetSelection());
    event.SetEventObject(this);
    event.SetString(copystring(GetStringSelection()));
    ProcessCommand(event);
    delete[] event.GetString();
    return TRUE;
  }
  else if (param == CBN_EDITCHANGE)
  {
    wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, m_windowId);
    event.SetString(copystring(GetValue()));
    event.SetEventObject(this);
    ProcessCommand(event);
    delete[] event.GetString();
    return TRUE;
  }
  else return FALSE;
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
  SetName(name);
  SetValidator(validator);
  if (parent) parent->AddChild(this);
  SetBackgroundColour(parent->GetBackgroundColour()) ;
  SetForegroundColour(parent->GetForegroundColour()) ;
  m_noStrings = 0;

  m_windowStyle = style;

  if ( id == -1 )
    m_windowId = (int)NewControlId();
  else
    m_windowId = id;

  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;

  long msStyle = WS_CHILD | WS_HSCROLL | WS_VSCROLL |
                 WS_TABSTOP | WS_VISIBLE | CBS_NOINTEGRALHEIGHT;

  if (m_windowStyle & wxCB_READONLY)
    msStyle |= CBS_DROPDOWNLIST;
  else if (m_windowStyle & wxCB_SIMPLE)
    msStyle |= CBS_SIMPLE; // A list (shown always) and edit control
  else
    msStyle |= CBS_DROPDOWN;

  if (m_windowStyle & wxCB_SORT)
    msStyle |= CBS_SORT;

  bool want3D;
  WXDWORD exStyle = Determine3DEffects(WS_EX_CLIENTEDGE, &want3D) ;

  // Even with extended styles, need to combine with WS_BORDER
  // for them to look right.
  if ( want3D || wxStyleHasBorder(m_windowStyle) )
    msStyle |= WS_BORDER;

  m_hWnd = (WXHWND)::CreateWindowEx(exStyle, "COMBOBOX", NULL,
                   msStyle,
                   0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)m_windowId,
                   wxGetInstance(), NULL);

  wxCHECK_MSG( m_hWnd, FALSE, "Failed to create combobox" );

/*
#if wxUSE_CTL3D
  if (want3D)
  {
    Ctl3dSubclassCtl(wx_combo);
    m_useCtl3D = TRUE;
  }
#endif
*/

  // Subclass again for purposes of dialog editing mode
  SubclassWin(m_hWnd);

  SetFont(parent->GetFont());
  int i;
  for (i = 0; i < n; i++)
  {
    Append(choices[i]);
  }

  SetSelection(i);

  SetSize(x, y, width, height);
  if ( !value.IsEmpty() )
  {
    SetValue(value);
  }

  return TRUE;
}

wxString wxComboBox::GetValue() const
{
  return wxGetWindowText(GetHWND());
}

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
    char *tmp = new char[len + singletons + 1];
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
    SetWindowText((HWND) GetHWND(), tmp);
    delete[] tmp;
  }
  else
    SetWindowText((HWND) GetHWND(), (const char *)value);
}

// Clipboard operations
void wxComboBox::Copy()
{
  HWND hWnd = (HWND) GetHWND();
  SendMessage(hWnd, WM_COPY, 0, 0L);
}

void wxComboBox::Cut()
{
  HWND hWnd = (HWND) GetHWND();
  SendMessage(hWnd, WM_CUT, 0, 0L);
}

void wxComboBox::Paste()
{
  HWND hWnd = (HWND) GetHWND();
  SendMessage(hWnd, WM_PASTE, 0, 0L);
}

void wxComboBox::SetEditable(bool editable)
{
  // Can't implement in MSW?
//  HWND hWnd = (HWND) GetHWND();
//  SendMessage(hWnd, EM_SETREADONLY, (WPARAM)!editable, (LPARAM)0L);
}

void wxComboBox::SetInsertionPoint(long pos)
{
/*
  HWND hWnd = (HWND) GetHWND();
#ifdef __WIN32__
  SendMessage(hWnd, EM_SETSEL, pos, pos);
  SendMessage(hWnd, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);
#else
  SendMessage(hWnd, EM_SETSEL, 0, MAKELPARAM(pos, pos));
#endif
  char *nothing = "";
  SendMessage(hWnd, EM_REPLACESEL, 0, (LPARAM)nothing);
*/
}

void wxComboBox::SetInsertionPointEnd()
{
/*
  long pos = GetLastPosition();
  SetInsertionPoint(pos);
*/
}

long wxComboBox::GetInsertionPoint() const
{
/*
  DWORD Pos=(DWORD)SendMessage((HWND) GetHWND(), EM_GETSEL, 0, 0L);
  return Pos&0xFFFF;
*/
  return 0;
}

long wxComboBox::GetLastPosition() const
{
/*
    HWND hWnd = (HWND) GetHWND();

    // Will always return a number > 0 (according to docs)
    int noLines = (int)SendMessage(hWnd, EM_GETLINECOUNT, (WPARAM)0, (LPARAM)0L);

    // This gets the char index for the _beginning_ of the last line
    int charIndex = (int)SendMessage(hWnd, EM_LINEINDEX, (WPARAM)(noLines-1), (LPARAM)0L);
    
    // Get number of characters in the last line. We'll add this to the character
    // index for the last line, 1st position.
    int lineLength = (int)SendMessage(hWnd, EM_LINELENGTH, (WPARAM)charIndex, (LPARAM)0L);

    return (long)(charIndex + lineLength);
*/
  return 0;
}

void wxComboBox::Replace(long from, long to, const wxString& value)
{
#if wxUSE_CLIPBOARD
    HWND hWnd = (HWND) GetHWND();
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
    wxSetClipboardData(wxDF_TEXT, (wxObject *)(const char *)value, 0, 0);

    // Paste into edit control
    SendMessage(hWnd, WM_PASTE, (WPARAM)0, (LPARAM)0L);
#endif
}

void wxComboBox::Remove(long from, long to)
{
    HWND hWnd = (HWND) GetHWND();
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
    HWND hWnd = (HWND) GetHWND();
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

#endif
 // wxUSE_COMBOBOX

