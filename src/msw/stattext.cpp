/////////////////////////////////////////////////////////////////////////////
// Name:        stattext.cpp
// Purpose:     wxStaticText
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "stattext.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/event.h"
#include "wx/app.h"
#include "wx/brush.h"
#endif

#include "wx/stattext.h"
#include "wx/msw/private.h"
#include <stdio.h>

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxStaticText, wxControl)
#endif

bool wxStaticText::Create(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
  SetName(name);
  if (parent) parent->AddChild(this);

  SetBackgroundColour(parent->GetBackgroundColour()) ;
  SetForegroundColour(parent->GetForegroundColour()) ;

  if ( id == -1 )
    m_windowId = (int)NewControlId();
  else
  m_windowId = id;

  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;

  m_windowStyle = style;

  long msStyle = WS_CHILD|WS_VISIBLE;
  if (m_windowStyle & wxALIGN_CENTRE)
    msStyle |= SS_CENTER;
  else if (m_windowStyle & wxALIGN_RIGHT)
    msStyle |= SS_RIGHT;
  else
    msStyle |= SS_LEFT;

  // Even with extended styles, need to combine with WS_BORDER
  // for them to look right.
  if ( wxStyleHasBorder(m_windowStyle) )
    msStyle |= WS_BORDER;

  m_hWnd = (WXHWND)::CreateWindowEx(MakeExtendedStyle(m_windowStyle), "STATIC", (const char *)label,
                         msStyle,
                         0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)m_windowId,
                         wxGetInstance(), NULL);

  wxCHECK_MSG( m_hWnd, FALSE, "Failed to create static ctrl" );

#if wxUSE_CTL3D
/*
  if (!(GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS))
    Ctl3dSubclassCtl(static_item);
*/
#endif

  SubclassWin(m_hWnd);

  SetFont(parent->GetFont());
  SetSize(x, y, width, height);

  return TRUE;
}

void wxStaticText::SetSize(int x, int y, int width, int height, int sizeFlags)
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

  char buf[300];
  int current_width;
  int cyf;

  ::GetWindowText((HWND) GetHWND(), buf, 300);
  GetTextExtent(buf, &current_width, &cyf, NULL, NULL, & this->GetFont());

  int ww, hh;
  GetSize(&ww, &hh);

  // If we're prepared to use the existing width, then...
  if (width == -1 && ((sizeFlags & wxSIZE_AUTO_WIDTH) != wxSIZE_AUTO_WIDTH))
    actualWidth = ww;
  else if (width == -1)
  {
    int cx;
    int cy;
    wxGetCharSize(GetHWND(), &cx, &cy, & this->GetFont());
    actualWidth = (int)(current_width + cx) ;
  }

  // If we're prepared to use the existing height, then...
  if (height == -1 && ((sizeFlags & wxSIZE_AUTO_HEIGHT) != wxSIZE_AUTO_HEIGHT))
    actualHeight = hh;
  else if (height == -1)
  {
    actualHeight = (int)(cyf) ;
  }

  MoveWindow((HWND) GetHWND(), x1, y1, actualWidth, actualHeight, TRUE);
}

void wxStaticText::SetLabel(const wxString& label)
{
  int w, h;
  RECT rect;

  wxWindow *parent = GetParent();
  GetWindowRect((HWND) GetHWND(), &rect);

  // Since we now have the absolute screen coords,
  // if there's a parent we must subtract its top left corner
  POINT point;
  point.x = rect.left;
  point.y = rect.top;
  if (parent)
  {
    ::ScreenToClient((HWND) parent->GetHWND(), &point);
  }

  GetTextExtent(label, &w, &h, NULL, NULL, & this->GetFont());
  MoveWindow((HWND) GetHWND(), point.x, point.y, (int)(w + 10), (int)h,
             TRUE);
  SetWindowText((HWND) GetHWND(), (const char *)label);
}

WXHBRUSH wxStaticText::OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
      WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
/*
#if wxUSE_CTL3D
  if ( m_useCtl3D )
  {
    HBRUSH hbrush = Ctl3dCtlColorEx(message, wParam, lParam);

    if (hbrush != (HBRUSH) 0)
      return hbrush;
    else
      return (HBRUSH)MSWDefWindowProc(message, wParam, lParam);
  }
#endif
*/

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

long wxStaticText::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
  // Ensure that static items get messages. Some controls don't like this
  // message to be intercepted (e.g. RichEdit), hence the tests.
  if (nMsg == WM_NCHITTEST)
    return (long)HTCLIENT;

  return wxWindow::MSWWindowProc(nMsg, wParam, lParam);
}


