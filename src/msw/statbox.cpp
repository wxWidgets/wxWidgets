/////////////////////////////////////////////////////////////////////////////
// Name:        statbox.cpp
// Purpose:     wxStaticBox
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "statbox.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/dcclient.h"
#include "wx/app.h"
#endif

#include "wx/statbox.h"
#include "wx/msw/private.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxStaticBox, wxControl)

BEGIN_EVENT_TABLE(wxStaticBox, wxControl)
	EVT_ERASE_BACKGROUND(wxStaticBox::OnEraseBackground)
END_EVENT_TABLE()

#endif

/*
 * Group box
 */
 
bool wxStaticBox::Create(wxWindow *parent, const wxWindowID id,
           const wxString& label,
           const wxPoint& pos,
           const wxSize& size,
           const long style,
           const wxString& name)
{
  SetName(name);

  if (parent) parent->AddChild(this);

  SetBackgroundColour(parent->GetDefaultBackgroundColour()) ;
  SetForegroundColour(parent->GetDefaultForegroundColour()) ;

  if ( id == -1 )
  	m_windowId = (int)NewControlId();
  else
	m_windowId = id;

  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;

  m_windowStyle = style;

  long msStyle = BS_GROUPBOX | WS_CHILD | WS_VISIBLE ; // GROUP_FLAGS;

  bool want3D;
  WXDWORD exStyle = Determine3DEffects(0, &want3D) ;

  HWND wx_button =
    CreateWindowEx(exStyle, "BUTTON", (const char *)label, msStyle,
                    0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)m_windowId,
                    wxGetInstance(), NULL);
#if CTL3D
  if (want3D)
  {
    Ctl3dSubclassCtl(wx_button);
	  m_useCtl3D = TRUE;
  }
#endif

  m_hWnd = (WXHWND)wx_button;

  // Subclass again for purposes of dialog editing mode
  SubclassWin(GetHWND());

  SetFont(* parent->GetFont());

  SetSize(x, y, width, height);
  ShowWindow(wx_button, SW_SHOW);

  return TRUE;
}

void wxStaticBox::SetLabel(const wxString& label)
{
  SetWindowText((HWND)m_hWnd, (const char *)label);
}

void wxStaticBox::SetSize(const int x, const int y, const int width, const int height, const int sizeFlags)
{
  int currentX, currentY;
  GetPosition(&currentX, &currentY);

  int x1 = x;
  int y1 = y;
  int w1 = width;
  int h1 = height;

  if (x == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    x1 = currentX;
  if (y == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    y1 = currentY;

  // If we're prepared to use the existing size, then...
  if (width == -1 && height == -1 && ((sizeFlags & wxSIZE_AUTO) != wxSIZE_AUTO))
  {
    GetSize(&w1, &h1);
  }

  char buf[300];

  float current_width;

  int cx;
  int cy;
  float cyf;

  HWND button = (HWND)m_hWnd;
  wxGetCharSize(GetHWND(), &cx, &cy,GetFont());

  GetWindowText(button, buf, 300);
  GetTextExtent(buf, &current_width, &cyf,NULL,NULL,GetFont());
  if (w1 < 0)
   w1 = (int)(current_width + 3*cx) ;
  if (h1<0)
    h1 = (int)(cyf*EDIT_CONTROL_FACTOR) ;
  MoveWindow(button, x1, y1, w1, h1, TRUE);

#if WXWIN_COMPATIBILITY
  GetEventHandler()->OldOnSize(width, height);
#else
  wxSizeEvent event(wxSize(width, height), m_windowId);
  event.eventObject = this;
  GetEventHandler()->ProcessEvent(event);
#endif
}

WXHBRUSH wxStaticBox::OnCtlColor(const WXHDC pDC, const WXHWND pWnd, const WXUINT nCtlColor,
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

// Shouldn't erase the whole window, since the static box must only paint its
// outline.
void wxStaticBox::OnEraseBackground(wxEraseEvent& event)
{
  wxWindow *parent = GetParent();
  if ( parent && parent->GetHWND() && (::GetWindowLong((HWND) parent->GetHWND(), GWL_STYLE) & WS_CLIPCHILDREN) )
  {
        // TODO: May in fact need to generate a paint event for inside this
        // control's rectangle, otherwise all controls are going to be clipped -
        // ugh.
        HBRUSH hBrush = ::CreateSolidBrush(PALETTERGB(GetBackgroundColour().Red(), GetBackgroundColour().Green(), GetBackgroundColour().Blue()));
        int mode = ::SetMapMode((HDC) event.GetDC()->GetHDC(), MM_TEXT);

        RECT rect;

        ::GetClientRect((HWND) GetHWND(), &rect);
        ::FillRect ((HDC) event.GetDC()->GetHDC(), &rect, hBrush);
        ::DeleteObject(hBrush);
        ::SetMapMode((HDC) event.GetDC()->GetHDC(), mode);
  }
  else
	Default();
}

long wxStaticBox::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
	// TODO: somehow, this has to accept mouse clicks in user interface edit mode,
	// but not otherwise. Only there is no longer a UI edit mode...

	// It worked before because the message could be processed if not in UI
	// edit mode. We have to find some way of distinguishing this.
	// Maybe this class can have an AcceptMouseEvents(bool) function; a sort of
	// kludge... or, we can search for an active event table entry that will
	// intercept mouse events, and if one exists (that isn't the default),
	// skip the code below. Too time consuming though.
	// Perhaps it's ok to do the default thing *anyway* because the title or edge
	// of the window may still be active!
	if (nMsg == WM_NCHITTEST)
            return Default();

	return wxControl::MSWWindowProc(nMsg, wParam, lParam);
}

