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
  SetValidator(validator);

  parent->AddChild(this);

  m_backgroundColour = parent->GetBackgroundColour() ;
  m_foregroundColour = parent->GetForegroundColour() ;
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

  m_hWnd = (WXHWND)CreateWindowEx
                   (
                    0,
                    wxT("BUTTON"),
                    wxT(""),
                    WS_VISIBLE | WS_TABSTOP | WS_CHILD | BS_OWNERDRAW ,
                    0, 0, 0, 0, 
                    GetWinHwnd(parent),
                    (HMENU)m_windowId,
                    wxGetInstance(),
                    NULL
                   );

  // Subclass again for purposes of dialog editing mode
  SubclassWin(m_hWnd);

  SetFont(parent->GetFont()) ;

  SetSize(x, y, width, height);

  return TRUE;
}

void wxBitmapButton::SetBitmapLabel(const wxBitmap& bitmap)
{
  m_buttonBitmap = bitmap;
}

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

    // choose the bitmap to use depending on the buttons state
    wxBitmap* bitmap;

    UINT state = lpDIS->itemState;
    bool isSelected = (state & ODS_SELECTED) != 0;
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

    // draw it on the memory DC
    HDC hDC = lpDIS->hDC;
    HDC memDC = ::CreateCompatibleDC(hDC);

    HBITMAP old = (HBITMAP) ::SelectObject(memDC, (HBITMAP) bitmap->GetHBITMAP());

    if (!old)
    {
        wxLogLastError(_T("SelectObject"));

        return FALSE;
    }

    int x = lpDIS->rcItem.left;
    int y = lpDIS->rcItem.top;
    int width = lpDIS->rcItem.right - x;
    int height = lpDIS->rcItem.bottom - y;

    int wBmp = bitmap->GetWidth(),
        hBmp = bitmap->GetHeight();

    // Draw the face, if auto-drawing
    bool autoDraw = (GetWindowStyleFlag() & wxBU_AUTODRAW) != 0;
    if ( autoDraw )
    {
        DrawFace((WXHDC) hDC,
                 lpDIS->rcItem.left, lpDIS->rcItem.top,
                 lpDIS->rcItem.right, lpDIS->rcItem.bottom,
                 isSelected);
    }

    // Centre the bitmap in the control area
    int x1 = x + (width - wBmp) / 2;
    int y1 = y + (height - hBmp) / 2;

    if ( isSelected && autoDraw )
    {
        x1++;
        y1++;
    }

    BOOL ok;

    // no MaskBlt() under Win16
#ifdef __WIN32__
    wxMask *mask = bitmap->GetMask();
    if ( mask )
    {
        // the fg ROP is applied for the pixels of the mask bitmap which are 1
        // (for a wxMask this means that this is a non transparent pixel), the
        // bg ROP is applied for all the others

        wxColour colBg = GetBackgroundColour();
        HBRUSH hbrBackground =
            ::CreateSolidBrush(RGB(colBg.Red(), colBg.Green(), colBg.Blue()));
        HBRUSH hbrOld = (HBRUSH)::SelectObject(hDC, hbrBackground);

        ok = ::MaskBlt(
                       hDC, x1, y1, wBmp, hBmp,                 // dst
                       memDC, 0, 0,                             // src
                       (HBITMAP)mask->GetMaskBitmap(), 0, 0,    // mask
                       MAKEROP4(SRCCOPY,                        // fg ROP
                                PATCOPY)                        // bg ROP
                      );

        ::SelectObject(hDC, hbrOld);
        ::DeleteObject(hbrBackground);
    }
    else
#endif // Win32
    {
        ok = ::BitBlt(hDC, x1, y1, wBmp, hBmp,  // dst
                      memDC, 0, 0,              // src
                      SRCCOPY);                 // ROP
    }

    if ( !ok )
    {
        wxLogLastError(_T("Mask/BitBlt()"));
    }

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

    ::SelectObject(memDC, old);

    ::DeleteDC(memDC);

    return TRUE;
}

void wxBitmapButton::DrawFace( WXHDC dc, int left, int top, int right, int bottom, bool sel )
{
    HPEN oldp;
    HBRUSH oldb ;

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

    oldp = (HPEN) SelectObject( (HDC) dc, GetStockObject( NULL_PEN ) ) ;
    oldb = (HBRUSH) SelectObject( (HDC) dc, brushFace ) ;
    Rectangle( (HDC) dc, left, top, right, bottom ) ;
    SelectObject( (HDC) dc, penBorder) ;
    MoveToEx((HDC) dc,left+1,top,NULL);LineTo((HDC) dc,right-1,top);
    MoveToEx((HDC) dc,left,top+1,NULL);LineTo((HDC) dc,left,bottom-1);
    MoveToEx((HDC) dc,left+1,bottom-1,NULL);LineTo((HDC) dc,right-1,bottom-1);
    MoveToEx((HDC) dc,right-1,top+1,NULL);LineTo((HDC) dc,right-1,bottom-1);

    SelectObject( (HDC) dc, penShadow) ;
    if (sel)
    {
        MoveToEx((HDC) dc,left+1    ,bottom-2   ,NULL) ;
        LineTo((HDC) dc,  left+1    ,top+1) ;
        LineTo((HDC) dc,  right-2   ,top+1) ;
    }
    else
    {
        MoveToEx((HDC) dc,left+1    ,bottom-2   ,NULL) ;
        LineTo((HDC) dc,  right-2   ,bottom-2) ;
        LineTo((HDC) dc,  right-2   ,top) ;
        MoveToEx((HDC) dc,left+2    ,bottom-3   ,NULL) ;
        LineTo((HDC) dc,  right-3   ,bottom-3) ;
        LineTo((HDC) dc,  right-3   ,top+1) ;

        SelectObject( (HDC) dc, penLight) ;

        MoveToEx((HDC) dc,left+1    ,bottom-2   ,NULL) ;
        LineTo((HDC) dc,  left+1    ,top+1) ;
        LineTo((HDC) dc,  right-2   ,top+1) ;
    }
    SelectObject((HDC) dc,oldp) ;
    SelectObject((HDC) dc,oldb) ;

    DeleteObject(penBorder);
    DeleteObject(penLight);
    DeleteObject(penShadow);
    DeleteObject(brushFace);
}

// VZ: should be at the very least less than wxDEFAULT_BUTTON_MARGIN
#define FOCUS_MARGIN 3

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
    HBRUSH  old = (HBRUSH) SelectObject( (HDC) dc, wxDisableButtonBrush ) ;

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

    ::SelectObject( (HDC) dc, old ) ;
}

void wxBitmapButton::SetDefault()
{
    wxButton::SetDefault();
}
