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
    m_hover =
    m_colBeingResized = COL_NONE;
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
    // update the column indices array if necessary
    if ( count > m_numColumns )
    {
        // all new columns have default positions equal to their indices
        for ( unsigned n = m_numColumns; n < count; n++ )
            m_colIndices.push_back(n);
    }
    else if ( count < m_numColumns )
    {
        // filter out all the positions which are invalid now while keeping the
        // order of the remaining ones
        wxArrayInt colIndices;
        for ( unsigned n = 0; n < m_numColumns; n++ )
        {
            const unsigned idx = m_colIndices[n];
            if ( idx < count )
                colIndices.push_back(idx);
        }

        wxASSERT_MSG( colIndices.size() == count, "logic error" );

        m_colIndices = colIndices;
    }

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
        const wxHeaderColumnBase& col = self->GetColumn(m_colIndices[n]);
        if ( col.IsShown() )
            pos += col.GetWidth();
    }

    return pos;
}

int wxHeaderCtrl::FindColumnAtPoint(int x, bool& onSeparator) const
{
    wxHeaderCtrl * const self = const_cast<wxHeaderCtrl *>(this);

    int pos = 0;
    const unsigned count = GetColumnCount();
    for ( unsigned n = 0; n < count; n++ )
    {
        const wxHeaderColumnBase& col = self->GetColumn(m_colIndices[n]);
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
            return GetColumnAt(n);
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
// wxHeaderCtrl dragging
// ----------------------------------------------------------------------------

bool wxHeaderCtrl::IsResizing() const
{
    return m_colBeingResized != COL_NONE;
}

void wxHeaderCtrl::UpdateResizingMarker(int xPhysical)
{
    // unfortunately drawing the marker over the parent window doesn't work as
    // it's usually covered by another window (the main control view) so just
    // draw the marker over the header itself, even if it makes it not very
    // useful
    wxClientDC dc(this);

    wxDCOverlay dcover(m_overlay, &dc);
    dcover.Clear();

    if ( xPhysical != -1 )
    {
        dc.SetPen(*wxLIGHT_GREY_PEN);
        dc.DrawLine(xPhysical, 0, xPhysical, GetClientSize().y);
    }
}

void wxHeaderCtrl::EndDragging()
{
    UpdateResizingMarker(-1);

    m_overlay.Reset();

    // don't use the special dragging cursor any more
    SetCursor(wxNullCursor);
}

int wxHeaderCtrl::ConstrainByMinWidth(unsigned int col, int& xPhysical)
{
    const int xStart = GetColStart(col);

    // notice that GetMinWidth() returns 0 if there is no minimal width so it
    // still makes sense to use it even in this case
    const int xMinEnd = xStart + GetColumn(col).GetMinWidth();

    if ( xPhysical < xMinEnd )
        xPhysical = xMinEnd;

    return xPhysical - xStart;
}

void wxHeaderCtrl::StartOrContinueResizing(unsigned int col, int xPhysical)
{
    wxHeaderCtrlEvent event(IsResizing() ? wxEVT_COMMAND_HEADER_RESIZING
                                         : wxEVT_COMMAND_HEADER_BEGIN_RESIZE,
                            GetId());
    event.SetEventObject(this);
    event.SetColumn(col);

    event.SetWidth(ConstrainByMinWidth(col, xPhysical));

    if ( GetEventHandler()->ProcessEvent(event) && !event.IsAllowed() )
    {
        if ( IsResizing() )
        {
            ReleaseMouse();
            EndResizing(-1);
        }
        //else: nothing to do -- we just don't start to resize
    }
    else // go ahead with resizing
    {
        if ( !IsResizing() )
        {
            m_colBeingResized = col;
            SetCursor(wxCursor(wxCURSOR_SIZEWE));
            CaptureMouse();
        }
        //else: we had already done the above when we started

        UpdateResizingMarker(xPhysical);
    }
}

void wxHeaderCtrl::EndResizing(int xPhysical)
{
    wxASSERT_MSG( IsResizing(), "shouldn't be called if we're not resizing" );

    EndDragging();

    const bool cancelled = xPhysical == -1;

    // if dragging was cancelled we must have already lost the mouse capture so
    // don't try to release it
    if ( !cancelled )
        ReleaseMouse();

    wxHeaderCtrlEvent event(cancelled ? wxEVT_COMMAND_HEADER_DRAGGING_CANCELLED
                                      : wxEVT_COMMAND_HEADER_END_RESIZE,
                            GetId());
    event.SetEventObject(this);
    event.SetColumn(m_colBeingResized);
    if ( !cancelled )
        event.SetWidth(ConstrainByMinWidth(m_colBeingResized, xPhysical));

    GetEventHandler()->ProcessEvent(event);

    m_colBeingResized = COL_NONE;
}

// ----------------------------------------------------------------------------
// wxHeaderCtrl column reordering
// ----------------------------------------------------------------------------

void wxHeaderCtrl::DoSetColumnsOrder(const wxArrayInt& order)
{
    m_colIndices = order;
    Refresh();
}

wxArrayInt wxHeaderCtrl::DoGetColumnsOrder() const
{
    return m_colIndices;
}

// ----------------------------------------------------------------------------
// wxHeaderCtrl event handlers
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxHeaderCtrl, wxHeaderCtrlBase)
    EVT_PAINT(wxHeaderCtrl::OnPaint)

    EVT_MOUSE_EVENTS(wxHeaderCtrl::OnMouse)

    EVT_MOUSE_CAPTURE_LOST(wxHeaderCtrl::OnCaptureLost)

    EVT_KEY_DOWN(wxHeaderCtrl::OnKeyDown)
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
        const wxHeaderColumnBase& col = GetColumn(m_colIndices[i]);
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

void wxHeaderCtrl::OnCaptureLost(wxMouseCaptureLostEvent& WXUNUSED(event))
{
    if ( IsResizing() )
        EndResizing(-1);
}

void wxHeaderCtrl::OnKeyDown(wxKeyEvent& event)
{
    if ( IsResizing() && event.GetKeyCode() == WXK_ESCAPE )
    {
        ReleaseMouse();
        EndResizing(-1);
    }
    else
    {
        event.Skip();
    }
}

void wxHeaderCtrl::OnMouse(wxMouseEvent& mevent)
{
    // do this in advance to allow simply returning if we're not interested,
    // we'll undo it if we do handle the event below
    mevent.Skip();


    // account for the control displacement
    const int xPhysical = mevent.GetX();
    const int xLogical = xPhysical - m_scrollOffset;

    // first deal with the [continuation of any] dragging operations in
    // progress
    if ( IsResizing() )
    {
        if ( mevent.LeftUp() )
            EndResizing(xPhysical);
        else // update the live separator position
            StartOrContinueResizing(m_colBeingResized, xPhysical);

        return;
    }


    // find if the event is over a column at all
    bool onSeparator;
    const unsigned col = mevent.Leaving()
                            ? (onSeparator = false, COL_NONE)
                            : FindColumnAtPoint(xLogical, onSeparator);


    // update the highlighted column if it changed
    if ( col != m_hover )
    {
        const unsigned hoverOld = m_hover;
        m_hover = col;

        RefreshColIfNotNone(hoverOld);
        RefreshColIfNotNone(m_hover);
    }

    // update mouse cursor as it moves around
    if ( mevent.Moving() )
    {
        SetCursor(onSeparator ? wxCursor(wxCURSOR_SIZEWE) : wxNullCursor);
        return;
    }

    // all the other events only make sense when they happen over a column
    if ( col == COL_NONE )
        return;


    // enter various dragging modes on left mouse press
    if ( mevent.LeftDown() )
    {
        if ( onSeparator )
        {
            // start resizing the column
            wxASSERT_MSG( !IsResizing(), "reentering resize mode?" );
            StartOrContinueResizing(col, xPhysical);
        }
        else // on column itself
        {
            // TODO: drag column
            ;
        }

        return;
    }

    // determine the type of header event corresponding to click events
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
