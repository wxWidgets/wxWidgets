/////////////////////////////////////////////////////////////////////////////
// Name:        button.cpp
// Purpose:     wxButton
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "button.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/button.h"
#include "wx/brush.h"
#endif

#include "wx/msw/private.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxButton, wxControl)
#endif

#define BUTTON_HEIGHT_FROM_CHAR_HEIGHT(cy) (11*EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy)/10)

// Buttons

bool wxButton::MSWCommand(WXUINT param, WXWORD id)
{
  if (param == BN_CLICKED || (param == 1)) // 1 for accelerator
  {
    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, id);
    event.SetEventObject(this);
    ProcessCommand(event);
    return TRUE;
  }
  else return FALSE;
}

bool wxButton::Create(wxWindow *parent, wxWindowID id, const wxString& label,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
  SetName(name);
  SetValidator(validator);

  parent->AddChild((wxButton *)this);
  m_backgroundColour = parent->GetBackgroundColour() ;
  m_foregroundColour = parent->GetForegroundColour() ;

  m_windowStyle = (long&)style;

  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;

  if (id == -1)
    m_windowId = NewControlId();
  else
    m_windowId = id;

  DWORD exStyle = MakeExtendedStyle(m_windowStyle);
  HWND wx_button =
    CreateWindowEx(exStyle, "BUTTON", label, BS_PUSHBUTTON | WS_TABSTOP | WS_CHILD,
                    0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)m_windowId,
                    wxGetInstance(), NULL);

#if CTL3D
//  if (!(GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS))
//    Ctl3dSubclassCtl(wx_button);
#endif

  m_hWnd = (WXHWND)wx_button;

  // Subclass again for purposes of dialog editing mode
  SubclassWin((WXHWND)wx_button);

  SetFont(parent->GetFont());

  SetSize(x, y, width, height);
  ShowWindow(wx_button, SW_SHOW);

  return TRUE;
}

void wxButton::SetSize(int x, int y, int width, int height, int sizeFlags)
{
  int currentX, currentY;
  GetPosition(&currentX, &currentY);
  int x1 = x;
  int y1 = y;
  if (x == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    x1 = currentX;
  if (y == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    y1 = currentY;

  AdjustForParentClientOrigin(x1, y1, sizeFlags);

  int actualWidth = width;
  int actualHeight = height;
  int ww, hh;
  GetSize(&ww, &hh);

  int current_width;
  int cyf;
  char buf[300];
  GetWindowText((HWND) GetHWND(), buf, 300);
  GetTextExtent(buf, &current_width, &cyf,NULL,NULL,& GetFont());

  // If we're prepared to use the existing width, then...
  if (width == -1 && ((sizeFlags & wxSIZE_AUTO_WIDTH) != wxSIZE_AUTO_WIDTH))
    actualWidth = ww;
  else if (width == -1)
  {
    int cx;
    int cy;
    wxGetCharSize(GetHWND(), &cx, &cy,& GetFont());
    actualWidth = (int)(current_width + 3*cx) ;
  }
  
  // If we're prepared to use the existing height, then...
  if (height == -1 && ((sizeFlags & wxSIZE_AUTO_HEIGHT) != wxSIZE_AUTO_HEIGHT))
    actualHeight = hh;
  else if (height == -1)
  {
    actualHeight = BUTTON_HEIGHT_FROM_CHAR_HEIGHT(cyf);
  }

  MoveWindow((HWND) GetHWND(), x1, y1, actualWidth, actualHeight, TRUE);
}

void wxButton::SetDefault(void)
{
  wxWindow *parent = (wxWindow *)GetParent();
  if (parent)
    parent->SetDefaultItem(this);

  if (parent)
  {
    SendMessage((HWND)parent->GetHWND(), DM_SETDEFID, m_windowId, 0L);
  }

  SendMessage((HWND)GetHWND(), BM_SETSTYLE, BS_DEFPUSHBUTTON, 1L);
}

wxString wxButton::GetLabel(void) const
{
    GetWindowText((HWND) GetHWND(), wxBuffer, 300);
    return wxString(wxBuffer);
}

void wxButton::SetLabel(const wxString& label)
{
  SetWindowText((HWND) GetHWND(), (const char *) label);
}

WXHBRUSH wxButton::OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
			WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
/*
    WXHBRUSH hBrush = (WXHBRUSH) MSWDefWindowProc(message, wParam, lParam);
//    ::SetTextColor((HDC) pDC, GetSysColor(COLOR_BTNTEXT));
    ::SetTextColor((HDC) pDC, RGB(GetForegroundColour().Red(), GetForegroundColour().Green(),
        GetForegroundColour().Blue()));

	return hBrush;
*/
  // This doesn't in fact seem to make any difference at all - buttons are always
  // the same colour.
  ::SetBkColor((HDC) pDC, RGB(GetBackgroundColour().Red(), GetBackgroundColour().Green(), GetBackgroundColour().Blue()));
  ::SetTextColor((HDC) pDC, RGB(GetForegroundColour().Red(), GetForegroundColour().Green(), GetForegroundColour().Blue()));

  wxBrush *backgroundBrush = wxTheBrushList->FindOrCreateBrush(GetBackgroundColour(), wxSOLID);

  // Note that this will be cleaned up in wxApp::OnIdle, if backgroundBrush
  // has a zero usage count.
  // NOT NOW; CHANGED.
//  backgroundBrush->RealizeResource();
  return (WXHBRUSH) backgroundBrush->GetResourceHandle();
}

void wxButton::Command (wxCommandEvent & event)
{
  ProcessCommand (event);
}


