/////////////////////////////////////////////////////////////////////////////
// Name:        radiobut.cpp
// Purpose:     wxRadioButton
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "radiobut.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/setup.h"
#include "wx/radiobut.h"
#endif

#include "wx/msw/private.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxRadioButton, wxControl)
// IMPLEMENT_DYNAMIC_CLASS(wxBitmapRadioButton, wxRadioButton)
#endif

bool wxRadioButton::MSWCommand(WXUINT param, WXWORD id)
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

  long groupStyle = 0;
  if (m_windowStyle & wxRB_GROUP)
    groupStyle = WS_GROUP;

//  long msStyle = groupStyle | RADIO_FLAGS;
  long msStyle = groupStyle | BS_AUTORADIOBUTTON | WS_CHILD | WS_VISIBLE ;

  bool want3D;
  WXDWORD exStyle = Determine3DEffects(0, &want3D) ;

  // Even with extended styles, need to combine with WS_BORDER
  // for them to look right.
/*
  if ( want3D || wxStyleHasBorder(m_windowStyle) )
    msStyle |= WS_BORDER;
*/

  m_hWnd = (WXHWND) CreateWindowEx(exStyle, RADIO_CLASS, (const char *)label,
                          msStyle,0,0,0,0,
                          (HWND) parent->GetHWND(), (HMENU)m_windowId, wxGetInstance(), NULL);

  wxCHECK_MSG( m_hWnd, FALSE, "Failed to create radiobutton" );

#if CTL3D
  if (want3D)
  {
    Ctl3dSubclassCtl((HWND) m_hWnd);
   m_useCtl3D = TRUE;
  }
#endif

  SetFont(parent->GetFont());

  // Subclass again for purposes of dialog editing mode
  SubclassWin((WXHWND)m_hWnd);

//  SetValue(value);

  // start GRW fix
  if (label != "")
  {
    int label_width, label_height;
    GetTextExtent(label, &label_width, &label_height, NULL, NULL, & GetFont());
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

  return TRUE;
}


void wxRadioButton::SetLabel(const wxString& label)
{
  SetWindowText((HWND) GetHWND(), (const char *)label);
}

void wxRadioButton::SetValue(bool value)
{
// Following necessary for Win32s, because Win32s translate BM_SETCHECK
  SendMessage((HWND) GetHWND(), BM_SETCHECK, (WPARAM)value, 0L);
}

// Get single selection
bool wxRadioButton::GetValue(void) const
{
  return (SendMessage((HWND) GetHWND(), BM_GETCHECK, 0, 0L) != 0);
}

WXHBRUSH wxRadioButton::OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
      WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
#if CTL3D
  if ( m_useCtl3D )
  {
    HBRUSH hbrush = Ctl3dCtlColorEx(message, wParam, lParam);
    return (WXHBRUSH) hbrush;
  }
#endif

  if (GetParent()->GetTransparentBackground())
    SetBkMode((HDC) pDC, TRANSPARENT);
  else
    SetBkMode((HDC) pDC, OPAQUE);

  ::SetBkColor((HDC) pDC, RGB(GetBackgroundColour().Red(), GetBackgroundColour().Green(), GetBackgroundColour().Blue()));
  ::SetTextColor((HDC) pDC, RGB(GetForegroundColour().Red(), GetForegroundColour().Green(), GetForegroundColour().Blue()));

  wxBrush *backgroundBrush = wxTheBrushList->FindOrCreateBrush(GetBackgroundColour(), wxSOLID);

  // Note that this will be cleaned up in wxApp::OnIdle, if backgroundBrush
  // has a zero usage count.
//  backgroundBrush->RealizeResource();
  return (WXHBRUSH) backgroundBrush->GetResourceHandle();
}

void wxRadioButton::Command (wxCommandEvent & event)
{
  SetValue ( (event.m_commandInt != 0) );
  ProcessCommand (event);
}


// Not implemented
#if 0
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

//  long msStyle = groupStyle | RADIO_FLAGS;
  long msStyle = groupStyle | BS_RADIOBUTTON | WS_CHILD | WS_VISIBLE ;

  m_hWnd = (WXHWND) CreateWindowEx(MakeExtendedStyle(m_windowStyle), RADIO_CLASS, "toggle",
                          msStyle,0,0,0,0,
                          (HWND) parent->GetHWND(), (HMENU)m_windowId, wxGetInstance(), NULL);

  wxCHECK_MSG( m_hWnd, "Failed to create radio button", FALSE );

#if CTL3D
  if (!(GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS))
  {
    Ctl3dSubclassCtl((HWND) GetHWND());
    m_useCtl3D = TRUE;
  }
#endif

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
  SendMessage((HWND) GetHWND(), BM_SETCHECK, (WPARAM)value, 0L);
}

// Get single selection, for single choice list items
bool wxBitmapRadioButton::GetValue(void) const
{
  return (bool)SendMessage((HWND) GetHWND(), BM_GETCHECK, 0, 0L);
}

#endif
