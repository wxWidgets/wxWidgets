/////////////////////////////////////////////////////////////////////////////
// Name:        bmpbuttn.cpp
// Purpose:     wxBitmapButton
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "bmpbuttn.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/bmpbuttn.h"
#endif

#include "wx/msw/private.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxBitmapButton, wxButton)
#endif

#define BUTTON_HEIGHT_FACTOR (EDIT_CONTROL_FACTOR * 1.1)

bool wxBitmapButton::Create(wxWindow *parent, wxWindowID id, const wxBitmap& bitmap,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
  m_buttonBitmap = bitmap;
  SetName(name);
  SetValidator(validator);

  parent->AddChild(this);

  m_backgroundColour = parent->GetDefaultBackgroundColour() ;
  m_foregroundColour = parent->GetDefaultForegroundColour() ;
  m_windowStyle = style;
  m_marginX = 0;
  m_marginY = 0;

  if ( style & wxBU_AUTODRAW )
  {
	m_marginX = wxDEFAULT_BUTTON_MARGIN;
	m_marginY = wxDEFAULT_BUTTON_MARGIN;
  }

  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;

  if (id == -1)
    m_windowId = NewControlId();
  else
    m_windowId = id;

  if ( width == -1 && bitmap.Ok())
	width = bitmap.GetWidth() + 2*m_marginX;

  if ( height == -1 && bitmap.Ok())
	height = bitmap.GetHeight() + 2*m_marginY;

  HWND wx_button =
    CreateWindowEx(0, "BUTTON", "", BS_OWNERDRAW | WS_TABSTOP | WS_CHILD,
                    0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)m_windowId,
                    wxGetInstance(), NULL);

  m_hWnd = (WXHWND)wx_button;

  // Subclass again for purposes of dialog editing mode
  SubclassWin((WXHWND)wx_button);

  // TODO? If in future we have a facility for having a label as well
  // as a bitmap, set the font.
//  SetFont(parent->GetFont()) ;

  SetSize(x, y, width, height);
  ShowWindow(wx_button, SW_SHOW);

  return TRUE;
}

void wxBitmapButton::SetBitmapLabel(const wxBitmap& bitmap)
{
  m_buttonBitmap = bitmap;
}

bool wxBitmapButton::MSWOnDraw(WXDRAWITEMSTRUCT *item)
{
	long style = GetWindowLong((HWND) GetHWND(), GWL_STYLE);
#if defined(__WIN95__)
	if (style & BS_BITMAP)
	{
		// Should we call Default() here?
//		Default();

		// Let default procedure draw the bitmap, which is defined
		// in the Windows resource.
		return FALSE;
	}
#endif

    LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT) item;

	wxBitmap* bitmap = &m_buttonBitmap;

	UINT state = lpDIS->itemState;
	if ((state & ODS_SELECTED) && m_buttonBitmapSelected.Ok())
		bitmap = &m_buttonBitmapSelected;
	else if ((state & ODS_FOCUS) && m_buttonBitmapFocus.Ok())
		bitmap = &m_buttonBitmapFocus;
	else if ((state & ODS_DISABLED) && m_buttonBitmapDisabled.Ok())
		bitmap = &m_buttonBitmapDisabled;

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

	// Draw the face, if auto-drawing
	if ( GetWindowStyleFlag() & wxBU_AUTODRAW )
		DrawFace((WXHDC) hDC, lpDIS->rcItem.left, lpDIS->rcItem.top, lpDIS->rcItem.right, lpDIS->rcItem.bottom,
            ((state & ODS_SELECTED) == ODS_SELECTED));

	// Centre the bitmap in the control area
	int x1 = (int) (x + ((width - bitmap->GetWidth()) / 2));
	int y1 = (int) (y + ((height - bitmap->GetHeight()) / 2));

	if ( (state & ODS_SELECTED) && (GetWindowStyleFlag() & wxBU_AUTODRAW) )
	{
		x1 ++;
		y1 ++;
	}

	::BitBlt(hDC, x1, y1, bitmap->GetWidth(), bitmap->GetHeight(), memDC, 0, 0, SRCCOPY);

	if ( (state & ODS_DISABLED) && (GetWindowStyleFlag() & wxBU_AUTODRAW) )
		DrawButtonDisable( (WXHDC) hDC, lpDIS->rcItem.left, lpDIS->rcItem.top, lpDIS->rcItem.right, lpDIS->rcItem.bottom, TRUE ) ;
	else if ( (state & ODS_FOCUS) && (GetWindowStyleFlag() & wxBU_AUTODRAW) )
		DrawButtonFocus( (WXHDC) hDC, lpDIS->rcItem.left, lpDIS->rcItem.top, lpDIS->rcItem.right, lpDIS->rcItem.bottom, ((state & ODS_SELECTED) == ODS_SELECTED));

	::SelectObject(memDC, old);

    ::DeleteDC(memDC);

    return TRUE;
}

void wxBitmapButton::DrawFace( WXHDC dc, int left, int top, int right, int bottom, bool sel )
{
  HPEN	oldp;
  HBRUSH	oldb ;

  HPEN penBorder;
  HPEN penLight;
  HPEN penShadow;
  HBRUSH brushFace;
  COLORREF ms_color;

	ms_color = GetSysColor(COLOR_WINDOWFRAME) ;
	penBorder = CreatePen(PS_SOLID,0,ms_color) ;

	ms_color = GetSysColor(COLOR_BTNSHADOW) ;
	penShadow = CreatePen(PS_SOLID,0,ms_color) ;

	ms_color = GetSysColor(COLOR_BTNHIGHLIGHT) ;
	penLight = CreatePen(PS_SOLID,0,ms_color) ;

	ms_color = GetSysColor(COLOR_BTNFACE) ;
	brushFace = CreateSolidBrush(ms_color) ;

	oldp = SelectObject( (HDC) dc, GetStockObject( NULL_PEN ) ) ;
	oldb = SelectObject( (HDC) dc, brushFace ) ;
	Rectangle( (HDC) dc, left, top, right, bottom ) ;
	SelectObject( (HDC) dc, penBorder) ;
        MoveToEx((HDC) dc,left+1,top,NULL);LineTo((HDC) dc,right-1,top);
        MoveToEx((HDC) dc,left,top+1,NULL);LineTo((HDC) dc,left,bottom-1);
        MoveToEx((HDC) dc,left+1,bottom-1,NULL);LineTo((HDC) dc,right-1,bottom-1);
        MoveToEx((HDC) dc,right-1,top+1,NULL);LineTo((HDC) dc,right-1,bottom-1);

	SelectObject( (HDC) dc, penShadow) ;
	if (sel)
	{
		MoveToEx((HDC) dc,left+1	,bottom-2	,NULL) ;
		LineTo((HDC) dc,  left+1	,top+1) ;
		LineTo((HDC) dc,  right-2	,top+1) ;
	}
	else
	{
		MoveToEx((HDC) dc,left+1	,bottom-2	,NULL) ;
		LineTo((HDC) dc,  right-2	,bottom-2) ;
		LineTo((HDC) dc,  right-2	,top) ;
		MoveToEx((HDC) dc,left+2	,bottom-3	,NULL) ;
		LineTo((HDC) dc,  right-3	,bottom-3) ;
		LineTo((HDC) dc,  right-3	,top+1) ;

		SelectObject( (HDC) dc, penLight) ;

		MoveToEx((HDC) dc,left+1	,bottom-2	,NULL) ;
		LineTo((HDC) dc,  left+1	,top+1) ;
		LineTo((HDC) dc,  right-2	,top+1) ;
	}
	SelectObject((HDC) dc,oldp) ;
	SelectObject((HDC) dc,oldb) ;

  DeleteObject(penBorder);
  DeleteObject(penLight);
  DeleteObject(penShadow);
  DeleteObject(brushFace);
}

#define FOCUS_MARGIN 6

void wxBitmapButton::DrawButtonFocus( WXHDC dc, int left, int top, int right, int bottom, bool sel )
{
	RECT rect;
	rect.left = left;
	rect.top = top;
	rect.right = right;
	rect.bottom = bottom;
	InflateRect( &rect, - FOCUS_MARGIN, - FOCUS_MARGIN ) ;
	if ( sel )
		OffsetRect( &rect, 1, 1 ) ;
	DrawFocusRect( (HDC) dc, &rect ) ;
}

extern HBRUSH wxDisableButtonBrush;
void wxBitmapButton::DrawButtonDisable( WXHDC dc, int left, int top, int right, int bottom, bool with_marg )
{
	HBRUSH	old = SelectObject( (HDC) dc, wxDisableButtonBrush ) ;

	if ( with_marg )
		::PatBlt( (HDC) dc, left + m_marginX, top + m_marginY,
			right - 2 * m_marginX, bottom - 2 * m_marginY,
				0xfa0089ul ) ;
	else	::PatBlt( (HDC) dc, left, top, right, bottom, 0xfa0089ul ) ;

	::SelectObject( (HDC) dc, old ) ;
}

