/////////////////////////////////////////////////////////////////////////////
// Name:        msw/statbox.cpp
// Purpose:     wxStaticBox
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "statbox.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_STATBOX

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/dcclient.h"
#endif

#include "wx/statbox.h"
#include "wx/notebook.h"
#include "wx/sysopt.h"
#include "wx/image.h"
#include "wx/dcmemory.h"

#include "wx/msw/private.h"

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

#if wxUSE_EXTENDED_RTTI
WX_DEFINE_FLAGS( wxStaticBoxStyle )

wxBEGIN_FLAGS( wxStaticBoxStyle )
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

wxEND_FLAGS( wxStaticBoxStyle )

IMPLEMENT_DYNAMIC_CLASS_XTI(wxStaticBox, wxControl,"wx/statbox.h")

wxBEGIN_PROPERTIES_TABLE(wxStaticBox)
    wxPROPERTY( Label,wxString, SetLabel, GetLabel, wxString() , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY_FLAGS( WindowStyle , wxStaticBoxStyle , long , SetWindowStyleFlag , GetWindowStyleFlag , EMPTY_MACROVALUE, 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // style
/*
    TODO PROPERTIES :
        label
*/
wxEND_PROPERTIES_TABLE()

wxBEGIN_HANDLERS_TABLE(wxStaticBox)
wxEND_HANDLERS_TABLE()

wxCONSTRUCTOR_6( wxStaticBox , wxWindow* , Parent , wxWindowID , Id , wxString , Label , wxPoint , Position , wxSize , Size , long , WindowStyle )
#else
IMPLEMENT_DYNAMIC_CLASS(wxStaticBox, wxControl)
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxStaticBox
// ----------------------------------------------------------------------------

bool wxStaticBox::Create(wxWindow *parent,
                         wxWindowID id,
                         const wxString& label,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxString& name)
{
    if ( !CreateControl(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    if ( !MSWCreateControl(wxT("BUTTON"), label, pos, size) )
        return false;

#ifndef __WXWINCE__
    Connect(wxEVT_PAINT, wxPaintEventHandler(wxStaticBox::OnPaint));
#endif

    return true;
}

wxBorder wxStaticBox::GetDefaultBorder() const
{
    return wxBORDER_NONE;
}

WXDWORD wxStaticBox::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    long styleWin = wxStaticBoxBase::MSWGetStyle(style, exstyle);

    // no need for it anymore, must be removed for wxRadioBox child
    // buttons to be able to repaint themselves
    styleWin &= ~WS_CLIPCHILDREN;

    if ( exstyle )
        *exstyle = 0;

    return styleWin | BS_GROUPBOX;
}

wxSize wxStaticBox::DoGetBestSize() const
{
    int cx, cy;
    wxGetCharSize(GetHWND(), &cx, &cy, GetFont());

    int wBox;
    GetTextExtent(wxGetWindowText(m_hWnd), &wBox, &cy);

    wBox += 3*cx;
    int hBox = EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy);

    return wxSize(wBox, hBox);
}

WXLRESULT wxStaticBox::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
#ifndef __WXWINCE__
    if ( nMsg == WM_NCHITTEST )
    {
        // This code breaks some other processing such as enter/leave tracking
        // so it's off by default.

        static int s_useHTClient = -1;
        if (s_useHTClient == -1)
            s_useHTClient = wxSystemOptions::GetOptionInt(wxT("msw.staticbox.htclient"));
        if (s_useHTClient == 1)
        {
            int xPos = LOWORD(lParam);  // horizontal position of cursor
            int yPos = HIWORD(lParam);  // vertical position of cursor

            ScreenToClient(&xPos, &yPos);

            // Make sure you can drag by the top of the groupbox, but let
            // other (enclosed) controls get mouse events also
            if ( yPos < 10 )
                return (long)HTCLIENT;
        }
    }
#endif // !__WXWINCE__

    return wxControl::MSWWindowProc(nMsg, wParam, lParam);
}

void wxStaticBox::GetBordersForSizer(int *borderTop, int *borderOther) const
{
    wxStaticBoxBase::GetBordersForSizer(borderTop, borderOther);

    // need extra space, don't know how much but this seems to be enough
    *borderTop += GetCharHeight()/3;
}

// MSWGetRegionWithoutSelf helper: removes the given rectangle from region
static inline void
SubtractRectFromRgn(HRGN hrgn, int left, int top, int right, int bottom)
{
    AutoHRGN hrgnRect(::CreateRectRgn(left, top, right, bottom));
    if ( !hrgnRect )
    {
        wxLogLastError(_T("CreateRectRgn()"));
        return;
    }

    ::CombineRgn(hrgn, hrgn, hrgnRect, RGN_DIFF);
}

void wxStaticBox::MSWGetRegionWithoutSelf(WXHRGN hRgn, int w, int h)
{
    HRGN hrgn = (HRGN)hRgn;

    // remove the area occupied by the static box borders from the region
    int borderTop, border;
    GetBordersForSizer(&borderTop, &border);

    // top
    SubtractRectFromRgn(hrgn, 0, 0, w, borderTop);

    // bottom
    SubtractRectFromRgn(hrgn, 0, h - border, w, h);

    // left
    SubtractRectFromRgn(hrgn, 0, 0, border, h);

    // right
    SubtractRectFromRgn(hrgn, w - border, 0, w, h);
}

WXHRGN wxStaticBox::MSWGetRegionWithoutChildren()
{
    RECT rc;
    ::GetWindowRect(GetHwnd(), &rc);
    HRGN hrgn = ::CreateRectRgn(rc.left, rc.top, rc.right + 1, rc.bottom + 1);

    // iterate over all child windows (not just wxWindows but all windows)
    for ( HWND child = ::GetWindow(GetHwndOf(GetParent()), GW_CHILD);
          child;
          child = ::GetWindow(child, GW_HWNDNEXT) )
    {
        wxWindow *childWindow = wxGetWindowFromHWND((WXHWND) child);

        // can't just test for (this != child) here since if a wxStaticBox
        // overlaps another wxStaticBox then neither are drawn. The overlapping
        // region will flicker but we shouldn't have overlapping windows anyway.
        if ( !childWindow || !wxDynamicCast(childWindow, wxStaticBox) )
        {
            ::GetWindowRect(child, &rc);
            if ( ::RectInRegion(hrgn, &rc) )
            {
                // need to remove WS_CLIPSIBLINGS from all sibling windows
                // that are within this staticbox if set
                LONG style = ::GetWindowLong(child, GWL_STYLE);
                if ( style & WS_CLIPSIBLINGS )
                {
                    style &= ~WS_CLIPSIBLINGS;
                    ::SetWindowLong(child, GWL_STYLE, style);

                    // MSDN: "If you have changed certain window data using
                    // SetWindowLong, you must call SetWindowPos to have the
                    // changes take effect."
                    ::SetWindowPos(child, NULL, 0, 0, 0, 0,
                                   SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                                   SWP_FRAMECHANGED);
                }

                AutoHRGN hrgnChild(::CreateRectRgnIndirect(&rc));
                ::CombineRgn(hrgn, hrgn, hrgnChild, RGN_DIFF);
            }
        }
    }

    return (WXHRGN)hrgn;
}

// helper for OnPaint()
void wxStaticBox::PaintBackground(wxDC& dc, const RECT& rc)
{
    // note that static box should be transparent, so it should show its
    // parents colour, not its own
    wxWindow * const parent = GetParent();

    HBRUSH hbr = (HBRUSH)parent->MSWGetBgBrush(dc.GetHDC());
    if ( !hbr )
    {
        wxBrush *brush =
            wxTheBrushList->FindOrCreateBrush(parent->GetBackgroundColour());
        if ( brush )
            hbr = GetHbrushOf(*brush);
    }

    if ( hbr )
        ::FillRect(GetHdcOf(dc), &rc, hbr);
}

void wxStaticBox::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    RECT rc;
    ::GetClientRect(GetHwnd(), &rc);

    // draw the entire box in a memory DC, but only blit the bits not redrawn
    // either by our children windows nor by FillRect() painting the background
    // below
    wxMemoryDC memdc;
    wxBitmap bitmap(rc.right, rc.bottom);
    memdc.SelectObject(bitmap);

    PaintBackground(memdc, rc);
    MSWDefWindowProc(WM_PAINT, (WPARAM)GetHdcOf(memdc), 0);

    int borderTop, border;
    GetBordersForSizer(&borderTop, &border);

    // top
    dc.Blit(border, 0, rc.right - border, borderTop,
            &memdc, border, 0);
    // bottom
    dc.Blit(border, rc.bottom - border, rc.right - border, rc.bottom,
            &memdc, border, rc.bottom - border);
    // left
    dc.Blit(0, 0, border, rc.bottom,
            &memdc, 0, 0);
    // right
    dc.Blit(rc.right - border, 0, rc.right, rc.bottom,
            &memdc, rc.right - border, 0);

    AutoHRGN hrgn((HRGN)MSWGetRegionWithoutChildren());
    RECT rcWin;
    ::GetWindowRect(GetHwnd(), &rcWin);
    ::OffsetRgn(hrgn, -rcWin.left, -rcWin.top);


    // now remove the box itself
    MSWGetRegionWithoutSelf((WXHRGN) hrgn, rc.right, rc.bottom);

    // and paint the inside of the box (excluding child controls)
    ::SelectClipRgn(GetHdcOf(dc), hrgn);
    PaintBackground(dc, rc);
    ::SelectClipRgn(GetHdcOf(dc), NULL);
}

#endif // wxUSE_STATBOX

