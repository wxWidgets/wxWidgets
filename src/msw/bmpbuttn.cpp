/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/bmpbuttn.cpp
// Purpose:     wxBitmapButton
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "bmpbuttn.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_BMPBUTTON

#ifndef WX_PRECOMP
    #include "wx/bmpbuttn.h"
    #include "wx/log.h"
    #include "wx/dcmemory.h"
#endif

#include "wx/msw/private.h"
#include "wx/image.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#if wxUSE_EXTENDED_RTTI

WX_DEFINE_FLAGS( wxBitmapButtonStyle )

wxBEGIN_FLAGS( wxBitmapButtonStyle )
    // new style border flags, we put them first to
    // use them for streaming out
    wxFLAGS_MEMBER(wxBORDER_SIMPLE)
    wxFLAGS_MEMBER(wxBORDER_SUNKEN)
    wxFLAGS_MEMBER(wxBORDER_DOUBLE)
    wxFLAGS_MEMBER(wxBORDER_RAISED)
    wxFLAGS_MEMBER(wxBORDER_STATIC)
    wxFLAGS_MEMBER(wxBORDER_NONE)

    // old style border flags
    wxFLAGS_MEMBER(wxSIMPLE_BORDER)
    wxFLAGS_MEMBER(wxSUNKEN_BORDER)
    wxFLAGS_MEMBER(wxDOUBLE_BORDER)
    wxFLAGS_MEMBER(wxRAISED_BORDER)
    wxFLAGS_MEMBER(wxSTATIC_BORDER)
    wxFLAGS_MEMBER(wxBORDER)

    // standard window styles
    wxFLAGS_MEMBER(wxTAB_TRAVERSAL)
    wxFLAGS_MEMBER(wxCLIP_CHILDREN)
    wxFLAGS_MEMBER(wxTRANSPARENT_WINDOW)
    wxFLAGS_MEMBER(wxWANTS_CHARS)
    wxFLAGS_MEMBER(wxFULL_REPAINT_ON_RESIZE)
    wxFLAGS_MEMBER(wxALWAYS_SHOW_SB )
    wxFLAGS_MEMBER(wxVSCROLL)
    wxFLAGS_MEMBER(wxHSCROLL)

    wxFLAGS_MEMBER(wxBU_AUTODRAW)
    wxFLAGS_MEMBER(wxBU_LEFT)
    wxFLAGS_MEMBER(wxBU_RIGHT)
    wxFLAGS_MEMBER(wxBU_TOP)
    wxFLAGS_MEMBER(wxBU_BOTTOM)
wxEND_FLAGS( wxBitmapButtonStyle )

IMPLEMENT_DYNAMIC_CLASS_XTI(wxBitmapButton, wxButton,"wx/bmpbuttn.h")

wxBEGIN_PROPERTIES_TABLE(wxBitmapButton)
    wxPROPERTY_FLAGS( WindowStyle , wxBitmapButtonStyle , long , SetWindowStyleFlag , GetWindowStyleFlag , EMPTY_MACROVALUE, 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // style
wxEND_PROPERTIES_TABLE()

wxBEGIN_HANDLERS_TABLE(wxBitmapButton)
wxEND_HANDLERS_TABLE()

wxCONSTRUCTOR_5( wxBitmapButton , wxWindow* , Parent , wxWindowID , Id , wxBitmap , Bitmap , wxPoint , Position , wxSize , Size )

#else
IMPLEMENT_DYNAMIC_CLASS(wxBitmapButton, wxButton)
#endif

BEGIN_EVENT_TABLE(wxBitmapButton, wxBitmapButtonBase)
    EVT_SYS_COLOUR_CHANGED(wxBitmapButton::OnSysColourChanged)
END_EVENT_TABLE()

/*
TODO PROPERTIES :

long "style" , wxBU_AUTODRAW
bool "default" , 0
bitmap "selected" ,
bitmap "focus" ,
bitmap "disabled" ,
*/

#define BUTTON_HEIGHT_FACTOR (EDIT_CONTROL_FACTOR * 1.1)

bool wxBitmapButton::Create(wxWindow *parent, wxWindowID id,
    const wxBitmap& bitmap,
    const wxPoint& pos,
    const wxSize& size, long style,
    const wxValidator& wxVALIDATOR_PARAM(validator),
    const wxString& name)
{
    m_bmpNormal = bitmap;
    SetName(name);

#if wxUSE_VALIDATORS
    SetValidator(validator);
#endif // wxUSE_VALIDATORS

    parent->AddChild(this);

    m_backgroundColour = parent->GetBackgroundColour();
    m_foregroundColour = parent->GetForegroundColour();
    m_windowStyle = style;

    if ( style & wxBU_AUTODRAW )
    {
        m_marginX = wxDEFAULT_BUTTON_MARGIN;
        m_marginY = wxDEFAULT_BUTTON_MARGIN;
    }

    if (id == wxID_ANY)
        m_windowId = NewControlId();
    else
        m_windowId = id;

    long msStyle = WS_VISIBLE | WS_TABSTOP | WS_CHILD | BS_OWNERDRAW ;

    if ( m_windowStyle & wxCLIP_SIBLINGS )
        msStyle |= WS_CLIPSIBLINGS;

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

    m_hWnd = (WXHWND) CreateWindowEx(
                    0,
                    wxT("BUTTON"),
                    wxEmptyString,
                    msStyle,
                    0, 0, 0, 0,
                    GetWinHwnd(parent),
                    (HMENU)m_windowId,
                    wxGetInstance(),
                    NULL
                   );

    // Subclass again for purposes of dialog editing mode
    SubclassWin(m_hWnd);

    SetPosition(pos);
    SetBestSize(size);

    return true;
}

bool wxBitmapButton::SetBackgroundColour(const wxColour& colour)
{
    if ( !wxBitmapButtonBase::SetBackgroundColour(colour) )
    {
        // didn't change
        return false;
    }

    // invalidate the brush, it will be recreated the next time it's needed
    m_brushDisabled = wxNullBrush;

    return true;
}

void wxBitmapButton::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    m_brushDisabled = wxNullBrush;

    if ( !IsEnabled() )
    {
        // this change affects our current state
        Refresh();
    }

    event.Skip();
}

// VZ: should be at the very least less than wxDEFAULT_BUTTON_MARGIN
#define FOCUS_MARGIN 3

bool wxBitmapButton::MSWOnDraw(WXDRAWITEMSTRUCT *item)
{
#ifndef __WXWINCE__
    long style = GetWindowLong((HWND) GetHWND(), GWL_STYLE);
    if (style & BS_BITMAP)
    {
        // Let default procedure draw the bitmap, which is defined
        // in the Windows resource.
        return false;
    }
#endif

    LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT) item;
    HDC hDC                = lpDIS->hDC;
    UINT state             = lpDIS->itemState;
    bool isSelected        = (state & ODS_SELECTED) != 0;
    bool autoDraw          = (GetWindowStyleFlag() & wxBU_AUTODRAW) != 0;


    // choose the bitmap to use depending on the button state
    wxBitmap* bitmap;

    if ( isSelected && m_bmpSelected.Ok() )
        bitmap = &m_bmpSelected;
    else if ((state & ODS_FOCUS) && m_bmpFocus.Ok())
        bitmap = &m_bmpFocus;
    else if ((state & ODS_DISABLED) && m_bmpDisabled.Ok())
        bitmap = &m_bmpDisabled;
    else
        bitmap = &m_bmpNormal;

    if ( !bitmap->Ok() )
        return false;

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
    dst.SetHDC((WXHDC) hDC, false);
    dst.DrawBitmap(*bitmap, x1, y1, true);

    // draw focus / disabled state, if auto-drawing
    if ( (state & ODS_DISABLED) && autoDraw )
    {
        DrawButtonDisable((WXHDC) hDC,
                          lpDIS->rcItem.left, lpDIS->rcItem.top,
                          lpDIS->rcItem.right, lpDIS->rcItem.bottom,
                          true);
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

    return true;
}

// GRG Feb/2000, support for bmp buttons with Win95/98 standard LNF

#if defined(__WIN95__)

void wxBitmapButton::DrawFace( WXHDC dc, int left, int top,
    int right, int bottom, bool sel )
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
    // brushFace   = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
    // Taking the background colour fits in better with
    // Windows XP themes.
    brushFace   = CreateSolidBrush(m_backgroundColour.m_pixel);

    // draw the rectangle
    RECT rect;
    rect.left   = left;
    rect.right  = right;
    rect.top    = top;
    rect.bottom = bottom;
    FillRect((HDC) dc, &rect, brushFace);

    // draw the border
    oldp = (HPEN) SelectObject( (HDC) dc, sel? penDkShadow : penHiLight);

    wxDrawLine((HDC) dc, left, top, right-1, top);
    wxDrawLine((HDC) dc, left, top+1, left, bottom-1);

    SelectObject( (HDC) dc, sel? penShadow : penLight);
    wxDrawLine((HDC) dc, left+1, top+1, right-2, top+1);
    wxDrawLine((HDC) dc, left+1, top+2, left+1, bottom-2);

    SelectObject( (HDC) dc, sel? penLight : penShadow);
    wxDrawLine((HDC) dc, left+1, bottom-2, right-1, bottom-2);
    wxDrawLine((HDC) dc, right-2, bottom-3, right-2, top);

    SelectObject( (HDC) dc, sel? penHiLight : penDkShadow);
    wxDrawLine((HDC) dc, left, bottom-1, right+2, bottom-1);
    wxDrawLine((HDC) dc, right-1, bottom-2, right-1, top-1);

    // delete allocated resources
    SelectObject((HDC) dc,oldp);
    DeleteObject(penHiLight);
    DeleteObject(penLight);
    DeleteObject(penShadow);
    DeleteObject(penDkShadow);
    DeleteObject(brushFace);
}

#else

void wxBitmapButton::DrawFace( WXHDC dc, int left, int top,
    int right, int bottom, bool sel )
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


void wxBitmapButton::DrawButtonFocus( WXHDC dc, int left, int top, int right,
    int bottom, bool WXUNUSED(sel) )
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

void
wxBitmapButton::DrawButtonDisable( WXHDC dc,
                                   int left, int top, int right, int bottom,
                                   bool with_marg )
{
    if ( !m_brushDisabled.Ok() )
    {
        // draw a bitmap with two black and two background colour pixels
        wxBitmap bmp(2, 2);
        wxMemoryDC dc;
        dc.SelectObject(bmp);
        dc.SetPen(*wxBLACK_PEN);
        dc.DrawPoint(0, 0);
        dc.DrawPoint(1, 1);
        dc.SetPen(GetBackgroundColour());
        dc.DrawPoint(0, 1);
        dc.DrawPoint(1, 0);

        m_brushDisabled = wxBrush(bmp);
    }

    SelectInHDC selectBrush((HDC)dc, GetHbrushOf(m_brushDisabled));

    // ROP for "dest |= pattern" operation -- as it doesn't have a standard
    // name, give it our own
    static const DWORD PATTERNPAINT = 0xFA0089UL;

    if ( with_marg )
    {
        left += m_marginX;
        top += m_marginY;
        right -= 2 * m_marginX;
        bottom -= 2 * m_marginY;
    }

    ::PatBlt( (HDC) dc, left, top, right, bottom, PATTERNPAINT);
}

void wxBitmapButton::SetDefault()
{
    wxButton::SetDefault();
}

wxSize wxBitmapButton::DoGetBestSize() const
{
    wxSize best;
    if (m_bmpNormal.Ok())
    {
        best.x = m_bmpNormal.GetWidth() + 2*m_marginX;
        best.y = m_bmpNormal.GetHeight() + 2*m_marginY;
    }

    // all buttons have at least the standard size unless the user explicitly
    // wants them to be of smaller size and used wxBU_EXACTFIT style when
    // creating the button
    if ( !HasFlag(wxBU_EXACTFIT) )
    {
        wxSize sz = GetDefaultSize();
        if ( best.x < sz.x )
            best.x = sz.x;
        if ( best.y < sz.y )
            best.y = sz.y;
    }

    return best;
}

#endif // wxUSE_BMPBUTTON

