/////////////////////////////////////////////////////////////////////////////
// Name:        combobox.cpp
// Purpose:     wxComboBox class
// Author:      David Webster
// Modified by:
// Created:     10/13/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/combobox.h"

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/setup.h"
#endif

#if wxUSE_COMBOBOX

#include "wx/combobox.h"
#include "wx/clipbrd.h"
#include "wx/os2/private.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxComboBox, wxControl)
#endif

bool wxComboBox::OS2Command(WXUINT param, WXWORD WXUNUSED(id))
{
  // TODO:
  /*
  if (param == CBN_SELCHANGE)
  {
    wxCommandEvent event(wxEVT_COMMAND_COMBOBOX_SELECTED, m_windowId);
    event.SetInt(GetSelection());
    event.SetEventObject(this);
    event.SetString(GetStringSelection());
    ProcessCommand(event);

    return TRUE;
  }
  else if (param == CBN_EDITCHANGE)
  {
    wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, m_windowId);
    event.SetString(GetValue());
    event.SetEventObject(this);
    ProcessCommand(event);

    return TRUE;
  }
  else
      return FALSE;
*/
  return FALSE;
}

bool wxComboBox::Create(wxWindow *parent, wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,
                        const wxSize& size,
                        int n, const wxString choices[],
                        long style,
#if wxUSE_VALIDATORS
#  if defined(__VISAGECPP__)
                        const wxValidator* validator,
#  else
                        const wxValidator& validator,
#  endif
#endif
                        const wxString& name)
{
  SetName(name);
#if wxUSE_VALIDATORS
  SetValidator(validator);
#endif
  if (parent) parent->AddChild(this);
  SetBackgroundColour(parent->GetBackgroundColour()) ;
  SetForegroundColour(parent->GetForegroundColour()) ;

  m_windowStyle = style;

  if ( id == -1 )
    m_windowId = (int)NewControlId();
  else
    m_windowId = id;

  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;
// TODO:
/*
  long msStyle = WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
      CBS_NOINTEGRALHEIGHT;

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

  m_hWnd = (WXHWND)::CreateWindowEx(exStyle, wxT("COMBOBOX"), NULL,
                   msStyle,
                   0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)m_windowId,
                   wxGetInstance(), NULL);

  wxCHECK_MSG( m_hWnd, FALSE, wxT("Failed to create combobox") );

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
*/
  return TRUE;
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
//    SetWindowText(GetHwnd(), tmp);
    delete[] tmp;
  }
//  else
//    SetWindowText(GetHwnd(), value);
}

// Clipboard operations
void wxComboBox::Copy()
{
  HWND hWnd = GetHwnd();
//  SendMessage(hWnd, WM_COPY, 0, 0L);
}

void wxComboBox::Cut()
{
  HWND hWnd = GetHwnd();
//  SendMessage(hWnd, WM_CUT, 0, 0L);
}

void wxComboBox::Paste()
{
  HWND hWnd = GetHwnd();
//  SendMessage(hWnd, WM_PASTE, 0, 0L);
}

void wxComboBox::SetEditable(bool editable)
{
  // Can't implement in MSW?
//  HWND hWnd = GetHwnd();
//  SendMessage(hWnd, EM_SETREADONLY, (WPARAM)!editable, (LPARAM)0L);
}

void wxComboBox::SetInsertionPoint(long pos)
{
/*
  HWND hWnd = GetHwnd();
  SendMessage(hWnd, EM_SETSEL, pos, pos);
  SendMessage(hWnd, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);
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
  DWORD Pos=(DWORD)SendMessage(GetHwnd(), EM_GETSEL, 0, 0L);
  return Pos&0xFFFF;
*/
  return 0;
}

long wxComboBox::GetLastPosition() const
{
/*
    HWND hWnd = GetHwnd();

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
    HWND hWnd = GetHwnd();
    long fromChar = from;
    long toChar = to;

    // Set selection and remove it
//    SendMessage(hWnd, CB_SETEDITSEL, fromChar, toChar);
//    SendMessage(hWnd, WM_CUT, (WPARAM)0, (LPARAM)0);

    // Now replace with 'value', by pasting.
    wxSetClipboardData(wxDF_TEXT, (wxObject *)(const wxChar *)value, 0, 0);

    // Paste into edit control
//    SendMessage(hWnd, WM_PASTE, (WPARAM)0, (LPARAM)0L);
#endif
}

void wxComboBox::Remove(long from, long to)
{
    HWND hWnd = GetHwnd();
    long fromChar = from;
    long toChar = to;

    // Cut all selected text
//    SendMessage(hWnd, CB_SETEDITSEL, fromChar, toChar);
//    SendMessage(hWnd, WM_CUT, (WPARAM)0, (LPARAM)0);
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

//    SendMessage(hWnd, CB_SETEDITSEL, (WPARAM)fromChar, (LPARAM)toChar);
//    SendMessage(hWnd, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);
}

void wxComboBox::DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags)
{
    wxControl::DoSetSize(x, y, width, height, sizeFlags);
}

#endif
 // wxUSE_COMBOBOX

