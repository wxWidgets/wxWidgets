/////////////////////////////////////////////////////////////////////////////
// Name:        bmpbuttn.cpp
// Purpose:     wxBitmapButton
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
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
    #include "wx/log.h"
    #include "wx/dcmemory.h"
#endif

#include "wx/msw/private.h"

IMPLEMENT_DYNAMIC_CLASS(wxBitmapButton, wxButton)

#define BUTTON_HEIGHT_FACTOR (EDIT_CONTROL_FACTOR * 1.1)

bool wxBitmapButton::Create(wxWindow *parent, wxWindowID id, const wxBitmap& bitmap,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
  m_buttonBitmap = bitmap;
  SetName(name);

#if wxUSE_VALIDATORS
  SetValidator(validator);
#endif // wxUSE_VALIDATORS

  parent->AddChild(this);

  m_backgroundColour = parent->GetBackgroundColour();
  m_foregroundColour = parent->GetForegroundColour();
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

	long msStyle = WS_VISIBLE | WS_TABSTOP | WS_CHILD | BS_OWNERDRAW ;

#ifdef __WIN32__
    if(m_windowStyle & wxBU_LEFT)
        msStyle |= BS_LEFT;
    if(m_windowStyle & wxBU_RIGHT)
        msStyle |= BS_RIGHT;
    if(m_windowStyle & wxBU_TOP)
        msStyle |= BS_TOP;
    if(m_windowStyle & wxBU_BOTTOM)
        msStyle |= BS_BOTTOM;
#endif

  m_hWnd = (WXHWND)CreateWindowEx
                   (
                    0,
                    wxT("BUTTON"),
                    wxT(""),
                    msStyle,
                    0, 0, 0, 0, 
                    GetWinHwnd(parent),
                    (HMENU)m_windowId,
                    wxGetInstance(),
                    NULL
                   );

  // Subclass again for purposes of dialog editing mode
  SubclassWin(m_hWnd);

  SetFont(parent->GetFont());

  SetSize(x, y, width, height);

  return TRUE;
}

void wxBitmapButton::SetBitmapLabel(const wxBitmap& bitmap)
{
  m_buttonBitmap = bitmap;
}

// VZ: should be at the very least less than wxDEFAULT_BUTTON_MARGIN
#define FOCUS_MARGIN 3

bool wxBitmapButton::MSWOnDraw(WXDRAWITEMSTRUCT *item)
{
#if defined(__WIN95__)
    long style = GetWindowLong((HWND) GetHWND(), GWL_STYLE);
    if (style & BS_BITMAP)
    {
        // Let default procedure draw the bitmap, which is defined
        // in the Windows resource.
        return FALSE;
    }
#endif

    LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT) item;
    HDC hDC                = lpDIS->hDC;
    UINT state             = lpDIS->itemState;
    bool isSelected        = (state & ODS_SELECTED) != 0;
    bool autoDraw          = (GetWindowStyleFlag() & wxBU_AUTODRAW) != 0;


    // choose the bitmap to use depending on the button state
    wxBitmap* bitmap;

    if ( isSelected && m_buttonBitmapSelected.Ok() )
        bitmap = &m_buttonBitmapSelected;
    else if ((state & ODS_FOCUS) && m_buttonBitmapFocus.Ok())
        bitmap = &m_buttonBitmapFocus;
    else if ((state & ODS_DISABLED) && m_buttonBitmapDisabled.Ok())
        bitmap = &m_buttonBitmapDisabled;
    else
        bitmap = &m_buttonBitmap;

    if ( !bitmap->Ok() )
        return FALSE;

    // centre the bitmap in the control area
    int x      = lpDIS->rcItem.left;
    int y      = lpDIS->rcItem.top;
    int width  = lpDIS->rcItem.right - x;
    int height = lpDIS->rcItem.bottom - y;
    int wBmp   = bitmap->GetWidth();
    int hBmp   = bitmap->GetHeight();

	int x1,y1;
	
    if(m_windowStyle & wxBU_LEFT)
        x1 = x + (FOCUS_MARGIN+1);
    else if(m_windowStyle & wxBU_RIGHT)
        x1 = x + (width - wBmp) - (FOCUS_MARGIN+1);
    else
        x1 = x + (width - wBmp) / 2;

    if(m_windowStyle & wxBU_TOP)
        y1 = y + (FOCUS_MARGIN+1);
    else if(m_windowStyle & wxBU_BOTTOM)
        y1 = y + (height - hBmp) - (FOCUS_MARGIN+1);
    else
        y1 = y + (height - hBmp) / 2;

    if ( isSelected && autoDraw )
    {
        x1++;
        y1++;
    }

    // draw the face, if auto-drawing
    if ( autoDraw )
    {
        DrawFace((WXHDC) hDC,
                 lpDIS->rcItem.left, lpDIS->rcItem.top,
                 lpDIS->rcItem.right, lpDIS->rcItem.bottom,
                 isSelected);
    }

    // draw the bitmap
    wxDC dst;
    dst.SetHDC((WXHDC) hDC, FALSE);
    dst.DrawBitmap(*bitmap, x1, y1, TRUE);
    
    // draw focus / disabled state, if auto-drawing
    if ( (state & ODS_DISABLED) && autoDraw )
    {
        DrawButtonDisable((WXHDC) hDC,
                          lpDIS->rcItem.left, lpDIS->rcItem.top,
                          lpDIS->rcItem.right, lpDIS->rcItem.bottom,
                          TRUE);
    }
    else if ( (state & ODS_FOCUS) && autoDraw )
    {
        DrawButtonFocus((WXHDC) hDC,
                        lpDIS->rcItem.left,
                        lpDIS->rcItem.top,
                        lpDIS->rcItem.right,
                        lpDIS->rcItem.bottom,
                        isSelected);
    }

    return TRUE;
}

// GRG Feb/2000, support for bmp buttons with Win95/98 standard LNF

#if defined(__WIN95__)

void wxBitmapButton::DrawFace( WXHDC dc, int left, int top, int right, int bottom, bool sel )
{
    HPEN oldp;
    HPEN penHiLight;
    HPEN penLight;
    HPEN penShadow;
    HPEN penDkShadow;
    HBRUSH brushFace;

    // create needed pens and brush
    penHiLight  = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_3DHILIGHT));
    penLight    = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_3DLIGHT));
    penShadow   = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_3DSHADOW));
    penDkShadow = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_3DDKSHADOW));
    brushFace   = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));

    // draw the rectangle
    RECT rect;
    rect.left   = left;
    rect.right  = right;
    rect.top    = top;
    rect.bottom = bottom;
    FillRect((HDC) dc, &rect, brushFace);

    // draw the border
    oldp = (HPEN) SelectObject( (HDC) dc, sel? penDkShadow : penHiLight);
    MoveToEx((HDC) dc, left, top, NULL); LineTo((HDC) dc, right-1, top);
    MoveToEx((HDC) dc, left, top+1, NULL); LineTo((HDC) dc, left, bottom-1);

    SelectObject( (HDC) dc, sel? penShadow : penLight);
    MoveToEx((HDC) dc, left+1, top+1, NULL); LineTo((HDC) dc, right-2, top+1);
    MoveToEx((HDC) dc, left+1, top+2, NULL); LineTo((HDC) dc, left+1, bottom-2);

    SelectObject( (HDC) dc, sel? penLight : penShadow);
    MoveToEx((HDC) dc, left+1, bottom-2, NULL); LineTo((HDC) dc, right-1, bottom-2);
    MoveToEx((HDC) dc, right-2, bottom-3, NULL); LineTo((HDC) dc, right-2, top);

    SelectObject( (HDC) dc, sel? penHiLight : penDkShadow);
    MoveToEx((HDC) dc, left, bottom-1, NULL); LineTo((HDC) dc, right+2, bottom-1);
    MoveToEx((HDC) dc, right-1, bottom-2, NULL); LineTo((HDC) dc, right-1, top-1);

    // delete allocated resources
    SelectObject((HDC) dc,oldp);
    DeleteObject(penHiLight);
    DeleteObject(penLight);
    DeleteObject(penShadow);
    DeleteObject(penDkShadow);
    DeleteObject(brushFace);
}

#else

void wxBitmapButton::DrawFace( WXHDC dc, int left, int top, int right, int bottom, bool sel )
{
    HPEN oldp;
    HPEN penBorder;
    HPEN penLight;
    HPEN penShadow;
    HBRUSH brushFace;

    // create needed pens and brush
    penBorder = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_WINDOWFRAME));
    penShadow = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));
    penLight  = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_BTNHIGHLIGHT));
    brushFace = CreateSolidBrush(COLOR_BTNFACE);

    // draw the rectangle
    RECT rect;
    rect.left   = left;
    rect.right  = right;
    rect.top    = top;
    rect.bottom = bottom;
    FillRect((HDC) dc, &rect, brushFace);

    // draw the border
    oldp = (HPEN) SelectObject( (HDC) dc, penBorder);
    MoveToEx((HDC) dc,left+1,top,NULL);LineTo((HDC) dc,right-1,top);
    MoveToEx((HDC) dc,left,top+1,NULL);LineTo((HDC) dc,left,bottom-1);
    MoveToEx((HDC) dc,left+1,bottom-1,NULL);LineTo((HDC) dc,right-1,bottom-1);
    MoveToEx((HDC) dc,right-1,top+1,NULL);LineTo((HDC) dc,right-1,bottom-1);

    SelectObject( (HDC) dc, penShadow);
    if (sel)
    {
        MoveToEx((HDC) dc,left+1    ,bottom-2   ,NULL);
        LineTo((HDC) dc,  left+1    ,top+1);
        LineTo((HDC) dc,  right-2   ,top+1);
    }
    else
    {
        MoveToEx((HDC) dc,left+1    ,bottom-2   ,NULL);
        LineTo((HDC) dc,  right-2   ,bottom-2);
        LineTo((HDC) dc,  right-2   ,top);

        MoveToEx((HDC) dc,left+2    ,bottom-3   ,NULL);
        LineTo((HDC) dc,  right-3   ,bottom-3);
        LineTo((HDC) dc,  right-3   ,top+1);

        SelectObject( (HDC) dc, penLight);

        MoveToEx((HDC) dc,left+1    ,bottom-2   ,NULL);
        LineTo((HDC) dc,  left+1    ,top+1);
        LineTo((HDC) dc,  right-2   ,top+1);
    }

    // delete allocated resources
    SelectObject((HDC) dc,oldp);
    DeleteObject(penBorder);
    DeleteObject(penLight);
    DeleteObject(penShadow);
    DeleteObject(brushFace);
}

#endif // defined(__WIN95__)


void wxBitmapButton::DrawButtonFocus( WXHDC dc, int left, int top, int right, int bottom, bool sel )
{
    RECT rect;
    rect.left = left;
    rect.top = top;
    rect.right = right;
    rect.bottom = bottom;
    InflateRect( &rect, - FOCUS_MARGIN, - FOCUS_MARGIN );

    // GRG: the focus rectangle should not move when the button is pushed!
/*
    if ( sel )
        OffsetRect( &rect, 1, 1 );
*/
    DrawFocusRect( (HDC) dc, &rect );
}

extern HBRUSH wxDisableButtonBrush;
void wxBitmapButton::DrawButtonDisable( WXHDC dc, int left, int top, int right, int bottom, bool with_marg )
{
    HBRUSH  old = (HBRUSH) SelectObject( (HDC) dc, wxDisableButtonBrush );

    // VZ: what's this?? there is no such ROP AFAIK
#ifdef __SALFORDC__
    DWORD dwRop = 0xFA0089L;
#else
    DWORD dwRop = 0xFA0089UL;
#endif

    if ( with_marg )
    {
        left += m_marginX;
        top += m_marginY;
        right -= 2 * m_marginX;
        bottom -= 2 * m_marginY;
    }

    ::PatBlt( (HDC) dc, left, top, right, bottom, dwRop);

    ::SelectObject( (HDC) dc, old );
}

void wxBitmapButton::SetDefault()
{
    wxButton::SetDefault();
}
