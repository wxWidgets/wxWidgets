/////////////////////////////////////////////////////////////////////////////
// Name:        statbox.cpp
// Purpose:     wxStaticBox
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:       wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "statbox.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/window.h"
#include "wx/msw/private.h"

#ifndef WX_PRECOMP
#include "wx/app.h"
#include "wx/dcclient.h"
#endif

#include "wx/statbox.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxStaticBox, wxControl)

BEGIN_EVENT_TABLE(wxStaticBox, wxControl)
    EVT_ERASE_BACKGROUND(wxStaticBox::OnEraseBackground)
END_EVENT_TABLE()

#endif

/*
 * Group box
 */
 
bool wxStaticBox::Create(wxWindow *parent, wxWindowID id,
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

  long msStyle = BS_GROUPBOX | WS_CHILD | WS_VISIBLE ; // GROUP_FLAGS;

  bool want3D;
  WXDWORD exStyle = Determine3DEffects(0, &want3D) ;

  HWND wx_button =
    CreateWindowEx(exStyle, _T("BUTTON"), (const wxChar *)label, msStyle,
                    0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)m_windowId,
                    wxGetInstance(), NULL);
#if wxUSE_CTL3D
  if (want3D)
  {
    Ctl3dSubclassCtl(wx_button);
      m_useCtl3D = TRUE;
  }
#endif

  m_hWnd = (WXHWND)wx_button;

  // Subclass again for purposes of dialog editing mode
  SubclassWin(GetHWND());

  SetFont(parent->GetFont());

  SetSize(x, y, width, height);
  ShowWindow(wx_button, SW_SHOW);

  return TRUE;
}

wxSize wxStaticBox::DoGetBestSize()
{
    int cx, cy;
    wxGetCharSize(GetHWND(), &cx, &cy, &GetFont());

    int wBox;
    GetTextExtent(wxGetWindowText(m_hWnd), &wBox, &cy);

    wBox += 3*cx;
    int hBox = EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy);

    return wxSize(wBox, hBox);
}

void wxStaticBox::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    wxControl::DoSetSize(x, y, width, height, sizeFlags);

    return;

    // the static box should always be on the bottom of the Z-order, otherwise
    // it may hide controls which are positioned inside it
    if ( !::SetWindowPos(GetHwnd(), HWND_TOP, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE) )
    {
        wxLogLastError(_T("SetWindowPos"));
    }
}

WXHBRUSH wxStaticBox::OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
            WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
#if wxUSE_CTL3D
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
    // If we don't have this (call Default()), we don't paint the background properly.
    // If we do have this, we seem to overwrite enclosed controls.
    // Is it the WS_CLIPCHILDREN style that's causing the problems?
    // Probably - without this style, the background of the window will show through,
    // so the control doesn't have to paint it. The window background will always be
    // painted before all other controls, therefore there are no problems with
    // controls being hidden by the static box.
    // So, if we could specify wxCLIP_CHILDREN in window, or not, we could optimise painting better.
    // We would assume wxCLIP_CHILDREN in a frame and a scrolled window, but not in a panel.
    // Is this too platform-specific?? What else can we do? Not a lot, since we have to pass
    // this information from arbitrary wxWindow derivatives, and it depends on what you wish to
    // do with the windows.
    // Alternatively, just make sure that wxStaticBox is always at the back! There are probably
    // few other circumstances where it matters about child clipping. But what about painting onto
    // to panel, inside a groupbox? Doesn't appear, because the box wipes it out.
  wxWindow *parent = 0; //GetParent();
  if ( parent && parent->GetHWND() && (::GetWindowLong((HWND) parent->GetHWND(), GWL_STYLE) & WS_CLIPCHILDREN) )
  {
        // TODO: May in fact need to generate a paint event for inside this
        // control's rectangle, otherwise all controls are going to be clipped -
        // ugh.
        HBRUSH hBrush = ::CreateSolidBrush(PALETTERGB(GetBackgroundColour().Red(), GetBackgroundColour().Green(), GetBackgroundColour().Blue()));
        int mode = ::SetMapMode((HDC) event.GetDC()->GetHDC(), MM_TEXT);

        RECT rect;

        ::GetClientRect(GetHwnd(), &rect);
        ::FillRect ((HDC) event.GetDC()->GetHDC(), &rect, hBrush);
        ::DeleteObject(hBrush);
        ::SetMapMode((HDC) event.GetDC()->GetHDC(), mode);
  }
  else
  {
    event.Skip();
  }
}

long wxStaticBox::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    if ( nMsg == WM_NCHITTEST)
    {
        int xPos = LOWORD(lParam);  // horizontal position of cursor
        int yPos = HIWORD(lParam);  // vertical position of cursor

        ScreenToClient(&xPos, &yPos);

        // Make sure you can drag by the top of the groupbox, but let
        // other (enclosed) controls get mouse events also
        if (yPos < 10)
            return (long)HTCLIENT;
    }

    return wxControl::MSWWindowProc(nMsg, wParam, lParam);
}

