/////////////////////////////////////////////////////////////////////////////
// Name:        radiobut.cpp
// Purpose:     wxRadioButton
// Author:      David Webster
// Modified by:
// Created:     10/12/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/setup.h"
#include "wx/radiobut.h"
#include "wx/brush.h"
#endif

#include "wx/msw/private.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxRadioButton, wxControl)
#endif

bool wxRadioButton::OS2Command(WXUINT param, WXWORD id)
{
  if (param == BN_CLICKED)
  {
    wxCommandEvent event(wxEVT_COMMAND_RADIOBUTTON_SELECTED, m_windowId);
    event.SetEventObject( this );
    ProcessCommand(event);
    return TRUE;
  }
  else return FALSE;
}

bool wxRadioButton::Create(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
    SetName(name);
    SetValidator(validator);

    if (parent) parent->AddChild(this);

    SetBackgroundColour(parent->GetBackgroundColour());
    SetForegroundColour(parent->GetForegroundColour());

    if ( id == -1 )
        m_windowId = (int)NewControlId();
    else
        m_windowId = id;

    int x = pos.x;
    int y = pos.y;
    int width = size.x;
    int height = size.y;

    m_windowStyle = style ;

// TODO create radiobutton
/*
  long groupStyle = 0;
  if (m_windowStyle & wxRB_GROUP)
    groupStyle = WS_GROUP;

//  long msStyle = groupStyle | RADIO_FLAGS;
  long msStyle = groupStyle | BS_AUTORADIOBUTTON | WS_CHILD | WS_VISIBLE ;

  bool want3D;
  WXDWORD exStyle = Determine3DEffects(0, &want3D) ;

  m_hWnd = (WXHWND) CreateWindowEx(exStyle, RADIO_CLASS, (const wxChar *)label,
                          msStyle,0,0,0,0,
                          (HWND) parent->GetHWND(), (HMENU)m_windowId, wxGetInstance(), NULL);

  wxCHECK_MSG( m_hWnd, FALSE, wxT("Failed to create radiobutton") );


  SetFont(parent->GetFont());

  // Subclass again for purposes of dialog editing mode
  SubclassWin((WXHWND)m_hWnd);

//  SetValue(value);
*/

  // start GRW fix
  if (label != wxT(""))
  {
    int label_width, label_height;
    GetTextExtent(label, &label_width, &label_height, NULL, NULL, & this->GetFont());
    if (width < 0)
      width = (int)(label_width + RADIO_SIZE);
    if (height<0)
    {
      height = (int)(label_height);
      if (height < RADIO_SIZE)
        height = RADIO_SIZE;
    }
  }
  else
  {
    if (width < 0)
      width = RADIO_SIZE;
    if (height < 0)
      height = RADIO_SIZE;
  }
  // end GRW fix

  SetSize(x, y, width, height);
  return FALSE;
}

void wxRadioButton::SetLabel(const wxString& label)
{
    // TODO
}

void wxRadioButton::SetValue(bool value)
{
    // TODO
}

// Get single selection, for single choice list items
bool wxRadioButton::GetValue() const
{
    // TODO
    return FALSE;
}

void wxRadioButton::Command (wxCommandEvent & event)
{
  SetValue ( (event.m_commandInt != 0) );
  ProcessCommand (event);
}

bool wxBitmapRadioButton::Create(wxWindow *parent, wxWindowID id,
       const wxBitmap *bitmap,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
  SetName(name);
  SetValidator(validator);

  if (parent) parent->AddChild(this);
  SetBackgroundColour(parent->GetBackgroundColour());
  SetForegroundColour(parent->GetForegroundColour());

  if ( id == -1 )
    m_windowId = (int)NewControlId();
  else
  m_windowId = id;

  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;
  m_windowStyle = style ;

  long groupStyle = 0;
  if (m_windowStyle & wxRB_GROUP)
    groupStyle = WS_GROUP;

// TODO:
/*
//  long msStyle = groupStyle | RADIO_FLAGS;
//  long msStyle = groupStyle | BS_RADIOBUTTON | WS_CHILD | WS_VISIBLE ;

  m_hWnd = (WXHWND) CreateWindowEx(MakeExtendedStyle(m_windowStyle), RADIO_CLASS, "toggle",
                          msStyle,0,0,0,0,
                          (HWND) parent->GetHWND(), (HMENU)m_windowId, wxGetInstance(), NULL);

  wxCHECK_MSG( m_hWnd, "Failed to create radio button", FALSE );

*/
  // Subclass again for purposes of dialog editing mode
  SubclassWin(GetHWND());

  SetSize(x, y, width, height);

  return TRUE;
}

void wxBitmapRadioButton::SetLabel(const wxBitmap *bitmap)
{
}

void wxBitmapRadioButton::SetValue(bool value)
{
// Following necessary for Win32s, because Win32s translate BM_SETCHECK
//  SendMessage((HWND) GetHWND(), BM_SETCHECK, (WPARAM)value, 0L);
}

// Get single selection, for single choice list items
bool wxBitmapRadioButton::GetValue(void) const
{
//  return (bool)SendMessage((HWND) GetHWND(), BM_GETCHECK, 0, 0L);
    return FALSE;
}

