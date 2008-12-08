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
    m_numColumns = 0;
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

void wxHeaderCtrl::DoSetCount(unsigned int count)
{
    m_numColumns = count;

    Refresh();
}

unsigned int wxHeaderCtrl::DoGetCount() const
{
    return m_numColumns;
}

void wxHeaderCtrl::DoUpdate(unsigned int idx)
{
    // we need to refresh not only this column but also the ones after it in
    // case it was shown or hidden or its width changed -- it would be nice to
    // avoid doing this unnecessary by storing the old column width (TODO)
    RefreshColsAfter(idx);
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
    wxHeaderCtrl * const self = const_cast<wxHeaderCtrl *>(this);

    int pos = m_scrollOffset;
    for ( unsigned n = 0; n < idx; n++ )
    {
        const wxHeaderColumnBase& col = self->GetColumn(n);
        if ( col.IsShown() )
            pos += col.GetWidth();
    }

    return pos;
}

int wxHeaderCtrl::FindColumnAtPos(int x, bool& onSeparator) const
{
    wxHeaderCtrl * const self = const_cast<wxHeaderCtrl *>(this);

    int pos = 0;
    const unsigned count = GetColumnCount();
    for ( unsigned n = 0; n < count; n++ )
    {
        const wxHeaderColumnBase& col = self->GetColumn(n);
        if ( col.IsHidden() )
            continue;

        pos += col.GetWidth();

        // if the column is resizeable, check if we're approximatively over the
        // line separating it from the next column
        //
        // TODO: don't hardcode sensitivity
        if ( col.IsResizeable() && abs(x - pos) < 8 )
        {
            onSeparator = true;
            return n;
        }

        // inside this column?
        if ( x < pos )
        {
            onSeparator = false;
            return n;
        }
    }

    return COL_NONE;
}

// ----------------------------------------------------------------------------
// wxHeaderCtrl repainting
// ----------------------------------------------------------------------------

void wxHeaderCtrl::RefreshCol(unsigned int idx)
{
    wxRect rect = GetClientRect();
    rect.x += GetColStart(idx);
    rect.width = GetColumn(idx).GetWidth();

    RefreshRect(rect);
}

void wxHeaderCtrl::RefreshColIfNotNone(unsigned int idx)
{
    if ( idx != COL_NONE )
        RefreshCol(idx);
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

BEGIN_EVENT_TABLE(wxHeaderCtrl, wxHeaderCtrlBase)
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

    const unsigned int count = m_numColumns;
    int xpos = 0;
    for ( unsigned int i = 0; i < count; i++ )
    {
        const wxHeaderColumnBase& col = GetColumn(i);
        if ( col.IsHidden() )
            continue;

        const int colWidth = col.GetWidth();

        wxHeaderSortIconType sortArrow;
        if ( col.IsSortKey() )
        {
            sortArrow = col.IsSortOrderAscending() ? wxHDR_SORT_ICON_UP
                                                   : wxHDR_SORT_ICON_DOWN;
        }
        else // not sorting by this column
        {
            sortArrow = wxHDR_SORT_ICON_NONE;
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

void wxHeaderCtrl::OnMouse(wxMouseEvent& mevent)
{
    mevent.Skip();

    // account for the control displacement
    const int x = mevent.GetX() - m_scrollOffset;

    // find if the event is over a column at all
    bool onSeparator;
    const unsigned col = mevent.Leaving()
                            ? (onSeparator = false, COL_NONE)
                            : FindColumnAtPos(x, onSeparator);

    // update the highlighted column if it changed
    if ( col != m_hover )
    {
        const unsigned hoverOld = m_hover;
        m_hover = col;

        RefreshColIfNotNone(hoverOld);
        RefreshColIfNotNone(m_hover);
    }

    if ( col == COL_NONE )
        return;

    // update mouse cursor as it moves around
    if ( mevent.Moving() )
    {
        SetCursor(onSeparator ? wxCursor(wxCURSOR_SIZEWE) : wxNullCursor);
        return;
    }

    if ( mevent.LeftDown() )
    {
        // TODO
        if ( onSeparator )
            // resize column
            ;
        else
            // drag column
            ;

        return;
    }

    // determine the type of header event corresponding to this mouse event
    wxEventType evtType = wxEVT_NULL;
    const bool click = mevent.ButtonUp(),
               dblclk = mevent.ButtonDClick();
    if ( click || dblclk )
    {
        switch ( mevent.GetButton() )
        {
            case wxMOUSE_BTN_LEFT:
                // treat left double clicks on separator specially
                if ( onSeparator && dblclk )
                {
                    evtType = wxEVT_COMMAND_HEADER_SEPARATOR_DCLICK;
                }
                else // not double click on separator
                {
                    evtType = click ? wxEVT_COMMAND_HEADER_CLICK
                                    : wxEVT_COMMAND_HEADER_DCLICK;
                }
                break;

            case wxMOUSE_BTN_RIGHT:
                evtType = click ? wxEVT_COMMAND_HEADER_RIGHT_CLICK
                                : wxEVT_COMMAND_HEADER_RIGHT_DCLICK;
                break;

            case wxMOUSE_BTN_MIDDLE:
                evtType = click ? wxEVT_COMMAND_HEADER_MIDDLE_CLICK
                                : wxEVT_COMMAND_HEADER_MIDDLE_DCLICK;
                break;

            default:
                // ignore clicks from other mouse buttons
                ;
        }
    }

    if ( evtType == wxEVT_NULL )
        return;

    wxHeaderCtrlEvent event(evtType, GetId());
    event.SetEventObject(this);
    event.SetColumn(col);

    if ( GetEventHandler()->ProcessEvent(event) )
        mevent.Skip(false);
}

#endif // wxHAS_GENERIC_HEADERCTRL
