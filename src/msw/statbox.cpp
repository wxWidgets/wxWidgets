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

BEGIN_EVENT_TABLE(wxStaticBox, wxControl)
    EVT_PAINT(wxStaticBox::OnPaint)
END_EVENT_TABLE()

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

    return true;
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

    // if not using correct (but backwards cojmpatible) text metrics
    // calculations, we need to add some extra margin or otherwise static box
    // title is clipped
#if !wxDIALOG_UNIT_COMPATIBILITY
    if ( !GetLabel().empty() )
        *borderTop += GetCharHeight()/3;
#endif // !wxDIALOG_UNIT_COMPATIBILITY
}

// rc must be in client coords!
void wxStaticBox::MSWClipBoxRegion(HRGN hrgn, const RECT *rc)
{
    HRGN hrgnchild;

    // top
    hrgnchild = ::CreateRectRgn(0, 0, rc->right, 14);
    ::CombineRgn(hrgn, hrgn, hrgnchild, RGN_DIFF);
    ::DeleteObject(hrgnchild);

    // bottom
    hrgnchild = ::CreateRectRgn(0, rc->bottom - 7, rc->right, rc->bottom);
    ::CombineRgn(hrgn, hrgn, hrgnchild, RGN_DIFF);
    ::DeleteObject(hrgnchild);

    // left
    hrgnchild = ::CreateRectRgn(0, 0, 7, rc->bottom);
    ::CombineRgn(hrgn, hrgn, hrgnchild, RGN_DIFF);
    ::DeleteObject(hrgnchild);

    // right
    hrgnchild = ::CreateRectRgn(rc->right - 7, 0, rc->right, rc->bottom);
    ::CombineRgn(hrgn, hrgn, hrgnchild, RGN_DIFF);
    ::DeleteObject(hrgnchild);
}

WXHRGN wxStaticBox::MSWCalculateClippingRegion()
{
    RECT rc;
    ::GetWindowRect(GetHwnd(), &rc);
    HRGN hrgn = ::CreateRectRgn(rc.left, rc.top, rc.right + 1, rc.bottom + 1);

    wxWindowList::compatibility_iterator node = GetParent()->GetChildren().GetFirst();
    while ( node )
    {
        wxWindow *child = node->GetData();

        // can't just test for (this != child) here since if a wxStaticBox
        // overlaps another wxStaticBox then neither are drawn. The overlapping
        // region will flicker but we shouldn't have overlapping windows anyway.
        if ( !child->IsKindOf(CLASSINFO(wxStaticBox)) )
        {
            ::GetWindowRect(GetHwndOf(child), &rc);
            if ( RectInRegion(hrgn, &rc) )
            {
                // need to remove WS_CLIPSIBLINGS from all sibling windows
                // that are within this staticbox if set
                LONG style = ::GetWindowLong(GetHwndOf(child), GWL_STYLE);
                if ( style & WS_CLIPSIBLINGS )
                {
                    style &= ~WS_CLIPSIBLINGS;
                    ::SetWindowLong(GetHwndOf(child), GWL_STYLE, style);

                    // MSDN: "If you have changed certain window data using
                    // SetWindowLong, you must call SetWindowPos to have the
                    // changes take effect."
                    ::SetWindowPos(GetHwndOf(child), NULL, 0, 0, 0, 0,
                                   SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                                   SWP_FRAMECHANGED);
                }

                HRGN hrgnchild = ::CreateRectRgnIndirect(&rc);
                ::CombineRgn(hrgn, hrgn, hrgnchild, RGN_DIFF);
                ::DeleteObject(hrgnchild);
            }
        }

        node = node->GetNext();
    }
    ::GetWindowRect(GetHwnd(), &rc);
    ::OffsetRgn(hrgn, -rc.left, -rc.top);

    return hrgn;
}

void wxStaticBox::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    RECT rc;
    ::GetClientRect(GetHwnd(), &rc);

    // paint the actual box
    wxMemoryDC memdc;
    wxBitmap bitmap(rc.right, rc.bottom);
    memdc.SelectObject(bitmap);

    // get bg brush
    WXHBRUSH hbr = DoMSWControlColor(GetHdcOf(memdc), wxNullColour);
    if ( !hbr )
    {
        wxBrush *brush = wxTheBrushList->FindOrCreateBrush(GetBackgroundColour(), wxSOLID);
        hbr = (WXHBRUSH)brush->GetResourceHandle();
    }

    // draw solid box, but only blit the good bits
    ::FillRect(GetHdcOf(memdc), &rc, (HBRUSH)hbr);
    MSWDefWindowProc(WM_PAINT, (WPARAM)GetHdcOf(memdc), 0);

    // top
    dc.Blit(7, 0, rc.right - 7, 14, &memdc, 7, 0);
    // bottom
    dc.Blit(7, rc.bottom - 7, rc.right - 7, rc.bottom, &memdc, 7, rc.bottom - 7);
    // left
    dc.Blit(0, 0, 7, rc.bottom, &memdc, 0, 0);
    // right
    dc.Blit(rc.right - 7, 0, rc.right, rc.bottom, &memdc, rc.right - 7, 0);

    // paint the inner
    HRGN hrgn = (HRGN)MSWCalculateClippingRegion();
    // now remove the box itself
    MSWClipBoxRegion(hrgn, &rc);

    hbr = DoMSWControlColor(GetHdcOf(dc), wxNullColour);
    if ( !hbr )
    {
        wxBrush *brush = wxTheBrushList->FindOrCreateBrush(GetBackgroundColour(), wxSOLID);
        hbr = (WXHBRUSH)brush->GetResourceHandle();
    }

    ::SelectClipRgn(GetHdcOf(dc), hrgn);
    ::FillRect(GetHdcOf(dc), &rc, (HBRUSH)hbr);
    ::SelectClipRgn(GetHdcOf(dc), NULL);
    ::DeleteObject(hrgn);
}

#endif // wxUSE_STATBOX

