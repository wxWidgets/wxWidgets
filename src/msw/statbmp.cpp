/////////////////////////////////////////////////////////////////////////////
// Name:        statbmp.cpp
// Purpose:     wxStaticBitmap
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "statbmp.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/statbmp.h"
#endif

#include <stdio.h>
#include "wx/msw/private.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxStaticBitmap, wxControl)
#endif

/*
 * wxStaticBitmap
 */

bool wxStaticBitmap::Create(wxWindow *parent, const wxWindowID id,
           const wxBitmap& bitmap,
           const wxPoint& pos,
           const wxSize& size,
           const long style,
           const wxString& name)
{
  m_messageBitmap = bitmap;
  SetName(name);
  if (parent) parent->AddChild(this);

  m_backgroundColour = parent->GetDefaultBackgroundColour() ;
  m_foregroundColour = parent->GetDefaultForegroundColour() ;

  if ( id == -1 )
  	m_windowId = (int)NewControlId();
  else
	m_windowId = id;

  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;

  if ( width < 0 && bitmap.Ok() )
	width = bitmap.GetWidth();
  if ( height < 0 && bitmap.Ok() )
	height = bitmap.GetHeight();

  m_windowStyle = style;

  // Use an ownerdraw button to produce a static bitmap, since there's
  // no ownerdraw static.
  // TODO: perhaps this should be a static item, with style SS_BITMAP.
  HWND static_item =
    CreateWindowEx(0, "BUTTON", "", BS_OWNERDRAW | WS_TABSTOP | WS_CHILD,
                    0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)m_windowId,
                    wxGetInstance(), NULL);
  m_hWnd = (WXHWND) static_item;

  // Subclass again for purposes of dialog editing mode
  SubclassWin((WXHWND) static_item);
  SetSize(x, y, width, height);
  return TRUE;
}

void wxStaticBitmap::SetSize(const int x, const int y, const int width, const int height, const int sizeFlags)
{
  int currentX, currentY;
  GetPosition(&currentX, &currentY);
  int x1 = x;
  int y1 = y;

  if (x == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    x1 = currentX;
  if (y == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    y1 = currentY;

  int actualWidth = width;
  int actualHeight = height;

  int ww, hh;
  GetSize(&ww, &hh);

  // If we're prepared to use the existing width, then...
  if (width == -1 && ((sizeFlags & wxSIZE_AUTO_WIDTH) != wxSIZE_AUTO_WIDTH))
    actualWidth = ww;
  else actualWidth = width;

  // If we're prepared to use the existing height, then...
  if (height == -1 && ((sizeFlags & wxSIZE_AUTO_HEIGHT) != wxSIZE_AUTO_HEIGHT))
    actualHeight = hh;
  else actualHeight = height;

  MoveWindow((HWND) GetHWND(), x1, y1, actualWidth, actualHeight, TRUE);

  if (!((width == -1) && (height == -1)))
  {
#if WXWIN_COMPATIBILITY
    GetEventHandler()->OldOnSize(actualWidth, actualHeight);
#else
    wxSizeEvent event(wxSize(actualWidth, actualHeight), m_windowId);
    event.eventObject = this;
    GetEventHandler()->ProcessEvent(event);
#endif
  }
}

void wxStaticBitmap::SetBitmap(const wxBitmap& bitmap)
{
  m_messageBitmap = bitmap;

  int x, y;
  int w, h;
  GetPosition(&x, &y);
  GetSize(&w, &h);
  RECT rect;
  rect.left = x; rect.top = y; rect.right = x + w; rect.bottom = y + h;

  if ( bitmap.Ok() )
  	MoveWindow((HWND) GetHWND(), x, y, bitmap.GetWidth(), bitmap.GetHeight(),
             FALSE);
  
  InvalidateRect((HWND) GetParent()->GetHWND(), &rect, TRUE);
}

bool wxStaticBitmap::MSWOnDraw(WXDRAWITEMSTRUCT *item)
{
	long style = GetWindowLong((HWND) GetHWND(), GWL_STYLE);
#ifdef __WIN32__
	if ((style & 0xFF) == SS_BITMAP)
	{
		// Should we call Default() here?
//		Default();

		// Let default procedure draw the bitmap, which is defined
		// in the Windows resource.
		return FALSE;
	}
#endif

    LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT) item;

	wxBitmap* bitmap = &m_messageBitmap;
	if ( !bitmap->Ok() )
		return FALSE;

	HDC hDC = lpDIS->hDC;
	HDC memDC = ::CreateCompatibleDC(hDC);

	HBITMAP old = ::SelectObject(memDC, (HBITMAP) bitmap->GetHBITMAP());

	if (!old)
		return FALSE;

	RECT rect = lpDIS->rcItem;

	int x = lpDIS->rcItem.left;
	int y = lpDIS->rcItem.top;
	int width = lpDIS->rcItem.right - x;
	int height = lpDIS->rcItem.bottom - y;

	// Centre the bitmap in the control area
	int x1 = (int) (x + ((width - bitmap->GetWidth()) / 2));
	int y1 = (int) (y + ((height - bitmap->GetHeight()) / 2));

	::BitBlt(hDC, x1, y1, bitmap->GetWidth(), bitmap->GetHeight(), memDC, 0, 0, SRCCOPY);

	::SelectObject(memDC, old);

    ::DeleteDC(memDC);

    return TRUE;
}

long wxStaticBitmap::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
  // Ensure that static items get messages. Some controls don't like this
  // message to be intercepted (e.g. RichEdit), hence the tests.
  if (nMsg == WM_NCHITTEST)
    return (long)HTCLIENT;

  return wxWindow::MSWWindowProc(nMsg, wParam, lParam);
}

