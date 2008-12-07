///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/headerctrlg.cpp
// Purpose:     generic wxHeaderCtrl implementation
// Author:      Vadim Zeitlin
// Created:     2008-12-03
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/headerctrl.h"

#ifdef wxHAS_GENERIC_HEADERCTRL

#include "wx/dcbuffer.h"
#include "wx/renderer.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

namespace
{

const unsigned NO_SORT = (unsigned)-1;

const unsigned COL_NONE = (unsigned)-1;

} // anonymous namespace

// ============================================================================
// wxHeaderCtrl implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxHeaderCtrl creation
// ----------------------------------------------------------------------------

void wxHeaderCtrl::Init()
{
    m_hover = COL_NONE;
    m_scrollOffset = 0;
}

bool wxHeaderCtrl::Create(wxWindow *parent,
                          wxWindowID id,
                          const wxPoint& pos,
                          const wxSize& size,
                          long style,
                          const wxString& name)
{
    if ( !wxHeaderCtrlBase::Create(parent, id, pos, size,
                                   style, wxDefaultValidator, name) )
        return false;

    // tell the system to not paint the background at all to avoid flicker as
    // we paint the entire window area in our OnPaint()
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);

    return true;
}

wxHeaderCtrl::~wxHeaderCtrl()
{
}

// ----------------------------------------------------------------------------
// wxHeaderCtrl columns manipulation
// ----------------------------------------------------------------------------

unsigned int wxHeaderCtrl::DoGetCount() const
{
    return m_cols.size();
}

// ----------------------------------------------------------------------------
// wxHeaderCtrl scrolling
// ----------------------------------------------------------------------------

void wxHeaderCtrl::DoScrollHorz(int dx)
{
    m_scrollOffset += dx;

    // don't call our own version which calls this function!
    wxControl::ScrollWindow(dx, 0);
}

// ----------------------------------------------------------------------------
// wxHeaderCtrl geometry
// ----------------------------------------------------------------------------

wxSize wxHeaderCtrl::DoGetBestSize() const
{
    // the vertical size is rather arbitrary but it looks better if we leave
    // some space around the text
    return wxSize(GetColStart(GetColumnCount()), (7*GetCharHeight())/4);
}

int wxHeaderCtrl::GetColStart(unsigned int idx) const
{
    int pos = 0;
    for ( unsigned n = 0; n < idx; n++ )
    {
        const wxHeaderColumn& col = m_cols[n];
        if ( col.IsShown() )
            pos += col.GetWidth();
    }

    return pos;
}

// ----------------------------------------------------------------------------
// wxHeaderCtrl repainting
// ----------------------------------------------------------------------------

void wxHeaderCtrl::RefreshCol(unsigned int idx)
{
    wxRect rect = GetClientRect();
    rect.x += GetColStart(idx);
    rect.width = m_cols[idx].GetWidth();

    RefreshRect(rect);
}

void wxHeaderCtrl::RefreshColsAfter(unsigned int idx)
{
    wxRect rect = GetClientRect();
    const int ofs = GetColStart(idx);
    rect.x += ofs;
    rect.width -= ofs;

    RefreshRect(rect);
}

// ----------------------------------------------------------------------------
// wxHeaderCtrl event handlers
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxHeaderCtrl, wxControl)
    EVT_PAINT(wxHeaderCtrl::OnPaint)

    EVT_MOUSE_EVENTS(wxHeaderCtrl::OnMouse)
END_EVENT_TABLE()

void wxHeaderCtrl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    int w, h;
    GetClientSize(&w, &h);

    wxAutoBufferedPaintDC dc(this);

    dc.SetBackground(GetBackgroundColour());
    dc.Clear();

    // account for the horizontal scrollbar offset in the parent window
    dc.SetDeviceOrigin(m_scrollOffset, 0);

    const unsigned int count = m_cols.size();
    int xpos = 0;
    for ( unsigned int i = 0; i < count; i++ )
    {
        const wxHeaderColumn& col = m_cols[i];
        if ( col.IsHidden() )
            continue;

        const int colWidth = col.GetWidth();

        wxHeaderSortIconType sortArrow;
        switch ( m_sortOrders[i] )
        {
            default:
                wxFAIL_MSG( "wrong sort order value" );
                // fall through

            case -1:
                sortArrow = wxHDR_SORT_ICON_NONE;
                break;

            case 0:
                sortArrow = wxHDR_SORT_ICON_DOWN;
                break;

            case 1:
                sortArrow = wxHDR_SORT_ICON_UP;
                break;
        }

        int state = 0;
        if ( IsEnabled() )
        {
            if ( i == m_hover )
                state = wxCONTROL_CURRENT;
        }
        else // disabled
        {
            state = wxCONTROL_DISABLED;
        }

        wxHeaderButtonParams params;
        params.m_labelText = col.GetTitle();
        params.m_labelBitmap = col.GetBitmap();
        params.m_labelAlignment = col.GetAlignment();

        wxRendererNative::Get().DrawHeaderButton
                                (
                                    this,
                                    dc,
                                    wxRect(xpos, 0, colWidth, h),
                                    state,
                                    sortArrow,
                                    &params
                                );

        xpos += colWidth;
    }
}

void wxHeaderCtrl::OnMouse(wxMouseEvent& event)
{
    event.Skip();
}

#endif // wxHAS_GENERIC_HEADERCTRL
