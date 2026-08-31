///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/headerctrlg.cpp
// Purpose:     generic wxHeaderCtrl implementation
// Author:      Vadim Zeitlin
// Created:     2008-12-03
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


#if wxUSE_HEADERCTRL

#include "wx/headerctrl.h"

#ifdef wxHAS_GENERIC_HEADERCTRL

#include "wx/dcbuffer.h"
#include "wx/renderer.h"
#include "wx/weakref.h"

#include <cstdint>
#include <limits>
#include <unordered_map>

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

namespace
{

const unsigned COL_NONE = (unsigned)-1;

// Keep mutation generations outside wxHeaderCtrl: this class is exported and
// adding a data member to it would change the ABI of all generic ports.
using HeaderRevisions =
    std::unordered_map<const wxHeaderCtrl*, std::uint64_t>;

HeaderRevisions& GetHeaderRevisions()
{
    static HeaderRevisions revisions;
    return revisions;
}

void RegisterHeader(const wxHeaderCtrl* header)
{
    const bool inserted = GetHeaderRevisions().emplace(header, 0).second;
    wxUnusedVar(inserted);
    wxASSERT_MSG( inserted, "header registered twice" );
}

void UnregisterHeader(const wxHeaderCtrl* header)
{
    const size_t erased = GetHeaderRevisions().erase(header);
    wxUnusedVar(erased);
    wxASSERT_MSG( erased == 1, "unregistered header" );
}

std::uint64_t GetHeaderRevision(const wxHeaderCtrl* header)
{
    const HeaderRevisions& revisions = GetHeaderRevisions();
    const HeaderRevisions::const_iterator it = revisions.find(header);
    wxASSERT_MSG( it != revisions.end(), "unregistered header" );
    return it == revisions.end() ? 0 : it->second;
}

void BumpHeaderRevision(const wxHeaderCtrl* header)
{
    HeaderRevisions& revisions = GetHeaderRevisions();
    const HeaderRevisions::iterator it = revisions.find(header);
    wxASSERT_MSG( it != revisions.end(), "unregistered header" );
    if ( it != revisions.end() )
        ++it->second;
}

int AddColumnWidth(int pos, int width)
{
    wxASSERT_MSG( width >= 0, "column width must be non-negative" );

    const int max = std::numeric_limits<int>::max();
    return pos > max - width ? max : pos + width;
}

int AddOffset(int pos, int offset)
{
    const long long sum =
        static_cast<long long>(pos) + offset;
    if ( sum > std::numeric_limits<int>::max() )
        return std::numeric_limits<int>::max();
    if ( sum < std::numeric_limits<int>::min() )
        return std::numeric_limits<int>::min();

    return static_cast<int>(sum);
}

int SubtractOffset(int pos, int offset)
{
    const long long difference =
        static_cast<long long>(pos) - offset;
    if ( difference > std::numeric_limits<int>::max() )
        return std::numeric_limits<int>::max();
    if ( difference < std::numeric_limits<int>::min() )
        return std::numeric_limits<int>::min();

    return static_cast<int>(difference);
}

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
    m_colBeingResized =
    m_colBeingReordered = COL_NONE;
    m_dragOffset = 0;
    m_scrollOffset = 0;
    m_wasSeparatorDClick = false;

    RegisterHeader(this);
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
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    return true;
}

wxHeaderCtrl::~wxHeaderCtrl()
{
    UnregisterHeader(this);
}

// ----------------------------------------------------------------------------
// wxHeaderCtrl columns manipulation
// ----------------------------------------------------------------------------

void wxHeaderCtrl::DoSetCount(unsigned int count)
{
    const unsigned int countOld = m_numColumns;
    const unsigned int cancelled = AbortDraggingForColumnMutation();

    // update the column indices order array before changing m_numColumns
    DoResizeColumnIndices(m_colIndices, count);

    m_numColumns = count;
    BumpHeaderRevision(this);

    // don't leave the column index invalid, this would cause a crash later if
    // it is used from OnMouse()
    if ( count != countOld || m_hover >= count )
        m_hover = COL_NONE;

    InvalidateBestSize();
    Refresh();

    if ( cancelled != COL_NONE )
        NotifyDraggingCancelled(cancelled);
}

unsigned int wxHeaderCtrl::DoGetCount() const
{
    return m_numColumns;
}

void wxHeaderCtrl::DoUpdate(unsigned int idx)
{
    const unsigned int cancelled = AbortDraggingForColumnMutation();
    BumpHeaderRevision(this);

    InvalidateBestSize();

    // we need to refresh not only this column but also the ones after it in
    // case it was shown or hidden or its width changed -- it would be nice to
    // avoid doing this unnecessary by storing the old column width (TODO)
    RefreshColsAfter(idx);

    if ( cancelled != COL_NONE )
        NotifyDraggingCancelled(cancelled);
}

// ----------------------------------------------------------------------------
// wxHeaderCtrl scrolling
// ----------------------------------------------------------------------------

void wxHeaderCtrl::DoScrollHorz(int dx)
{
    if ( !dx )
        return;

    const unsigned int cancelled = AbortDraggingForColumnMutation();
    BumpHeaderRevision(this);
    const int scrollOffsetOld = m_scrollOffset;
    m_scrollOffset = AddOffset(m_scrollOffset, dx);
    const int dxEffective =
        SubtractOffset(m_scrollOffset, scrollOffsetOld);

    // don't call our own version which calls this function!
    wxControl::ScrollWindow(dxEffective, 0);

    if ( cancelled != COL_NONE )
        NotifyDraggingCancelled(cancelled);
}

// ----------------------------------------------------------------------------
// wxHeaderCtrl geometry
// ----------------------------------------------------------------------------

wxSize wxHeaderCtrl::DoGetBestSize() const
{
    wxWindow *win = GetParent();
    int height = wxRendererNative::Get().GetHeaderButtonHeight( win );

    int width = 0;
    const unsigned int count = GetColumnCount();
    for ( unsigned int pos = 0; pos < count; ++pos )
    {
        const wxHeaderColumn& col = GetColumn(m_colIndices[pos]);
        if ( col.IsShown() )
            width = AddColumnWidth(width, GetEffectiveColumnWidth(col));
    }

    // the vertical size is rather arbitrary but it looks better if we leave
    // some space around the text
    return wxSize(IsEmpty() ? wxHeaderCtrlBase::DoGetBestSize().x : width,
                  height); // (7*GetCharHeight())/4);
}

int wxHeaderCtrl::GetEffectiveColumnWidth(const wxHeaderColumn& col) const
{
    int width = col.GetWidth();
    if ( width < 0 )
    {
        width =
            const_cast<wxHeaderCtrl*>(this)->GetColumnTitleWidth(col);
    }

    return wxMax(width, 0);
}

int wxHeaderCtrl::GetColStart(unsigned int idx) const
{
    int pos = m_scrollOffset;
    for ( unsigned n = 0; ; n++ )
    {
        const unsigned i = m_colIndices[n];
        if ( i == idx )
            break;

        const wxHeaderColumn& col = GetColumn(i);
        if ( col.IsShown() )
            pos = AddColumnWidth(pos, GetEffectiveColumnWidth(col));
    }

    return pos;
}

int wxHeaderCtrl::GetColEnd(unsigned int idx) const
{
    int x = GetColStart(idx);

    return AddColumnWidth(x, GetEffectiveColumnWidth(GetColumn(idx)));
}

unsigned int wxHeaderCtrl::FindColumnAtPoint(int xPhysical, bool *onSeparator) const
{
    int pos = 0;
    const long long xLogical =
        static_cast<long long>(xPhysical) - m_scrollOffset;
    const unsigned count = GetColumnCount();
    for ( unsigned n = 0; n < count; n++ )
    {
        const unsigned idx = m_colIndices[n];
        const wxHeaderColumn& col = GetColumn(idx);
        if ( col.IsHidden() )
            continue;

        pos = AddColumnWidth(pos, GetEffectiveColumnWidth(col));

        // TODO: don't hardcode sensitivity
        const int separatorClickMargin = FromDIP(8);

        // if the column is resizable, check if we're approximatively over the
        // line separating it from the next column
        const long long distance = xLogical - pos;
        if ( col.IsResizeable() &&
                distance > -separatorClickMargin &&
                distance < separatorClickMargin )
        {
            if ( onSeparator )
                *onSeparator = true;
            return idx;
        }

        // inside this column?
        if ( xLogical < pos )
        {
            if ( onSeparator )
                *onSeparator = false;
            return idx;
        }
    }

    if ( onSeparator )
        *onSeparator = false;
    return COL_NONE;
}

unsigned int wxHeaderCtrl::FindColumnClosestToPoint(int xPhysical) const
{
    const unsigned int colIndexAtPoint = FindColumnAtPoint(xPhysical);

    // valid column found?
    if ( colIndexAtPoint != COL_NONE )
        return colIndexAtPoint;

    // if not, xPhysical must be beyond the rightmost column, so return its
    // index instead -- if we have it
    const unsigned int count = GetColumnCount();
    if ( !count )
        return COL_NONE;

    return m_colIndices[count - 1];
}

// ----------------------------------------------------------------------------
// wxHeaderCtrl repainting
// ----------------------------------------------------------------------------

void wxHeaderCtrl::RefreshCol(unsigned int idx)
{
    wxRect rect = GetClientRect();
    rect.x = AddOffset(rect.x, GetColStart(idx));
    rect.width = GetEffectiveColumnWidth(GetColumn(idx));

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
    if ( ofs >= rect.width )
        return;
    rect.x = AddOffset(rect.x, ofs);
    rect.width = SubtractOffset(rect.width, ofs);

    RefreshRect(rect);
}

// ----------------------------------------------------------------------------
// wxHeaderCtrl dragging/resizing/reordering
// ----------------------------------------------------------------------------

bool wxHeaderCtrl::IsResizing() const
{
    return m_colBeingResized != COL_NONE;
}

bool wxHeaderCtrl::IsReordering() const
{
    return m_colBeingReordered != COL_NONE;
}

void wxHeaderCtrl::ClearMarkers()
{
    wxOverlayDC dc(m_overlay, this);
    dc.Clear();
}

void wxHeaderCtrl::EndDragging()
{
    // We currently only use markers for reordering, not for resizing
    if (IsReordering())
    {
        ClearMarkers();
        m_overlay.Reset();
    }

    // don't use the special dragging cursor any more
    SetCursor(wxNullCursor);
}

void wxHeaderCtrl::CancelDragging()
{
    wxASSERT_MSG( IsDragging(),
                  "shouldn't be called if we're not dragging anything" );

    const unsigned int col = AbortDraggingForColumnMutation();
    if ( col != COL_NONE )
        NotifyDraggingCancelled(col);
}

unsigned int wxHeaderCtrl::AbortDraggingForColumnMutation()
{
    if ( !IsDragging() )
        return COL_NONE;

    const unsigned int col =
        IsResizing() ? m_colBeingResized : m_colBeingReordered;

    // Clear the visual state while IsReordering() still identifies it, but
    // clear both logical states before releasing capture: capture loss can be
    // delivered synchronously and must not start a second cancellation.
    EndDragging();
    m_colBeingResized = COL_NONE;
    m_colBeingReordered = COL_NONE;

    if ( HasCapture() )
        ReleaseMouse();

    return col;
}

void wxHeaderCtrl::NotifyDraggingCancelled(unsigned int col)
{
    wxHeaderCtrlEvent event(wxEVT_HEADER_DRAGGING_CANCELLED, GetId());
    event.SetEventObject(this);
    event.SetColumn(col);

    // This callback is deliberately the last operation of every caller: the
    // event handler is allowed to destroy this control.
    GetEventHandler()->ProcessEvent(event);
}

int wxHeaderCtrl::ConstrainByMinWidth(unsigned int col, int& xPhysical)
{
    const int xStart = GetColStart(col);

    // notice that GetMinWidth() returns 0 if there is no minimal width so it
    // still makes sense to use it even in this case
    const int xMinEnd =
        AddColumnWidth(xStart, wxMax(GetColumn(col).GetMinWidth(), 0));

    if ( xPhysical < xMinEnd )
        xPhysical = xMinEnd;

    const long long width =
        static_cast<long long>(xPhysical) - xStart;
    return width > std::numeric_limits<int>::max()
               ? std::numeric_limits<int>::max()
               : static_cast<int>(wxMax(width, 0LL));
}

void wxHeaderCtrl::StartOrContinueResizing(unsigned int col, int xPhysical)
{
    wxHeaderCtrlEvent event(IsResizing() ? wxEVT_HEADER_RESIZING
                                         : wxEVT_HEADER_BEGIN_RESIZE,
                            GetId());
    event.SetEventObject(this);
    event.SetColumn(col);

    event.SetWidth(ConstrainByMinWidth(col, xPhysical));

    const wxWeakRef<wxWindow> weakThis(this);
    const std::uint64_t revision = GetHeaderRevision(this);
    const bool processed = GetEventHandler()->ProcessEvent(event);
    if ( weakThis.get() != this )
        return;

    if ( revision != GetHeaderRevision(this) )
    {
        if ( IsDragging() )
            CancelDragging();
        return;
    }

    if ( processed && !event.IsAllowed() )
    {
        if ( IsResizing() )
        {
            CancelDragging();
        }
        //else: nothing to do -- we just don't start to resize
        return;
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

    }
    InvalidateBestSize();
    RefreshColsAfter(col);
}

void wxHeaderCtrl::EndResizing(int xPhysical)
{
    wxASSERT_MSG( IsResizing(), "shouldn't be called if we're not resizing" );

    const unsigned int col = m_colBeingResized;
    const int width = ConstrainByMinWidth(col, xPhysical);

    EndDragging();
    m_colBeingResized = COL_NONE;

    if ( HasCapture() )
        ReleaseMouse();

    wxHeaderCtrlEvent event(wxEVT_HEADER_END_RESIZE, GetId());
    event.SetEventObject(this);
    event.SetColumn(col);
    event.SetWidth(width);

    GetEventHandler()->ProcessEvent(event);
}

void wxHeaderCtrl::UpdateReorderingMarker(int xPhysical)
{
    wxOverlayDC dc(m_overlay, this);
    dc.Clear();

    dc.SetPen(*wxBLUE);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    // draw the phantom position of the column being dragged
    const int x = SubtractOffset(xPhysical, m_dragOffset);
    int y = GetClientSize().y;
    dc.DrawRectangle(x, 0,
                     GetEffectiveColumnWidth(
                         GetColumn(m_colBeingReordered)),
                     y);

    // and also a hint indicating where it is going to be inserted if it's
    // dropped now
    unsigned int col = FindColumnClosestToPoint(xPhysical);
    if ( col != COL_NONE )
    {
        static const int DROP_MARKER_WIDTH = 4;

        dc.SetBrush(*wxBLUE);
        dc.DrawRectangle(GetColEnd(col) - DROP_MARKER_WIDTH/2, 0,
                         DROP_MARKER_WIDTH, y);
    }
}

void wxHeaderCtrl::StartReordering(unsigned int col, int xPhysical)
{
    wxHeaderCtrlEvent event(wxEVT_HEADER_BEGIN_REORDER, GetId());
    event.SetEventObject(this);
    event.SetColumn(col);

    const wxWeakRef<wxWindow> weakThis(this);
    const std::uint64_t revision = GetHeaderRevision(this);
    const bool processed = GetEventHandler()->ProcessEvent(event);
    if ( weakThis.get() != this )
        return;

    if ( revision != GetHeaderRevision(this) )
        return;

    if ( processed && !event.IsAllowed() )
    {
        // don't start dragging it, nothing to do otherwise
        return;
    }

    m_dragOffset = SubtractOffset(xPhysical, GetColStart(col));

    m_colBeingReordered = col;
    SetCursor(wxCursor(wxCURSOR_HAND));
    CaptureMouse();

    // do not call UpdateReorderingMarker() here: we don't want to give
    // feedback for reordering until the user starts to really move the mouse
    // as he might want to just click on the column and not move it at all
}

bool wxHeaderCtrl::EndReordering(int xPhysical)
{
    wxASSERT_MSG( IsReordering(), "shouldn't be called if we're not reordering" );

    const unsigned int colOld = m_colBeingReordered;
    const unsigned int colNew = FindColumnClosestToPoint(xPhysical);
    const bool wasDragged =
        SubtractOffset(xPhysical, GetColStart(colOld)) != m_dragOffset;

    EndDragging();
    m_colBeingReordered = COL_NONE;

    if ( HasCapture() )
        ReleaseMouse();

    // mouse drag must be longer than min distance m_dragOffset
    if ( !wasDragged )
        return false;

    // cannot proceed without a valid column index
    if ( colNew == COL_NONE )
        return false;

    if ( colNew != colOld )
    {
        wxHeaderCtrlEvent event(wxEVT_HEADER_END_REORDER, GetId());
        event.SetEventObject(this);
        event.SetColumn(colOld);

        const unsigned pos = GetColumnPos(colNew);
        event.SetNewOrder(pos);

        const wxWeakRef<wxWindow> weakThis(this);
        const std::uint64_t revision = GetHeaderRevision(this);
        const bool processed = GetEventHandler()->ProcessEvent(event);
        if ( weakThis.get() != this ||
                revision != GetHeaderRevision(this) )
            return true;

        if ( !processed )
        {
            // get the reordered columns
            wxArrayInt order = GetColumnsOrder();
            MoveColumnInOrderArray(order, colOld, pos);

            // As the event wasn't processed, call the virtual function
            // callback.
            UpdateColumnsOrder(order);
            if ( weakThis.get() != this ||
                    revision != GetHeaderRevision(this) )
                return true;

            // update columns order
            SetColumnsOrder(order);
        }
        else if ( event.IsAllowed() )
        {
            // do reorder the columns
            DoMoveCol(colOld, pos);
        }
    }

    // whether we moved the column or not, the user did move the mouse and so
    // did try to do it so return true
    return true;
}

// ----------------------------------------------------------------------------
// wxHeaderCtrl column reordering
// ----------------------------------------------------------------------------

void wxHeaderCtrl::DoSetColumnsOrder(const wxArrayInt& order)
{
    if ( m_colIndices == order )
    {
        Refresh();
        return;
    }

    const unsigned int cancelled = AbortDraggingForColumnMutation();
    m_colIndices = order;
    m_hover = COL_NONE;
    BumpHeaderRevision(this);
    Refresh();

    if ( cancelled != COL_NONE )
        NotifyDraggingCancelled(cancelled);
}

wxArrayInt wxHeaderCtrl::DoGetColumnsOrder() const
{
    return m_colIndices;
}

void wxHeaderCtrl::DoMoveCol(unsigned int idx, unsigned int pos)
{
    const int posOld = m_colIndices.Index(idx);
    if ( posOld == static_cast<int>(pos) )
    {
        Refresh();
        return;
    }

    const unsigned int cancelled = AbortDraggingForColumnMutation();
    MoveColumnInOrderArray(m_colIndices, idx, pos);
    m_hover = COL_NONE;
    BumpHeaderRevision(this);
    Refresh();

    if ( cancelled != COL_NONE )
        NotifyDraggingCancelled(cancelled);
}

// ----------------------------------------------------------------------------
// wxHeaderCtrl event handlers
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(wxHeaderCtrl, wxHeaderCtrlBase)
    EVT_PAINT(wxHeaderCtrl::OnPaint)

    EVT_MOUSE_EVENTS(wxHeaderCtrl::OnMouse)

    EVT_MOUSE_CAPTURE_LOST(wxHeaderCtrl::OnCaptureLost)

    EVT_KEY_DOWN(wxHeaderCtrl::OnKeyDown)
wxEND_EVENT_TABLE()

void wxHeaderCtrl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    int w, h;
    GetClientSize(&w, &h);

    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    int xpos = m_scrollOffset;
    for ( unsigned int i = 0; i < m_numColumns; i++ )
    {
        const unsigned idx = m_colIndices[i];
        const wxHeaderColumn& col = GetColumn(idx);
        if ( col.IsHidden() )
            continue;

        const int colWidth = GetEffectiveColumnWidth(col);
        const int colEnd = AddColumnWidth(xpos, colWidth);
        if ( colEnd < 0 )
        {
            // This column is not shown on screen because it is to the left of
            // the shown area, don't bother drawing it.
            xpos = colEnd;
            continue;
        }

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
            if ( idx == m_hover )
                state = wxCONTROL_CURRENT;
        }
        else // disabled
        {
            state = wxCONTROL_DISABLED;
        }

        if (i == 0)
           state |= wxCONTROL_SPECIAL;

        wxHeaderButtonParams params;
        params.m_labelText = col.GetTitle();
        params.m_labelBitmap = col.GetBitmapBundle().GetBitmapFor(this);
        params.m_labelAlignment = col.GetAlignment();

#ifdef __WXGTK__
        if (i == m_numColumns - 1 && colEnd >= w)
        {
            state |= wxCONTROL_DIRTY;
        }
#endif

        wxRendererNative::Get().DrawHeaderButton
                                (
                                    this,
                                    dc,
                                    wxRect(xpos, 0, colWidth, h),
                                    state,
                                    sortArrow,
                                    &params
                                );

        xpos = colEnd;
        if ( xpos > w )
        {
            // Next column and all the others are beyond the right border of
            // the window, no need to continue.
            break;
        }
    }
    if (xpos < w)
    {
        int state = wxCONTROL_DIRTY;
        if (!IsEnabled())
            state |= wxCONTROL_DISABLED;
        wxRendererNative::Get().DrawHeaderButton(
            this, dc, wxRect(xpos, 0, w - xpos, h), state);
    }
}

void wxHeaderCtrl::OnCaptureLost(wxMouseCaptureLostEvent& WXUNUSED(event))
{
    if ( IsDragging() )
        CancelDragging();
}

void wxHeaderCtrl::OnKeyDown(wxKeyEvent& event)
{
    if ( event.GetKeyCode() == WXK_ESCAPE )
    {
        if ( IsDragging() )
        {
            CancelDragging();

            return;
        }
    }

    event.Skip();
}

void wxHeaderCtrl::OnMouse(wxMouseEvent& mevent)
{
    const bool wasSeparatorDClick = m_wasSeparatorDClick;
    m_wasSeparatorDClick = false;

    // do this in advance to allow simply returning if we're not interested,
    // we'll undo it if we do handle the event below
    mevent.Skip();


    // account for the control displacement
    const int xPhysical = mevent.GetX();

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

    if ( IsReordering() )
    {
        if ( !mevent.LeftUp() )
        {
            // update the column position
            UpdateReorderingMarker(xPhysical);

            return;
        }

        // finish reordering and continue to generate a click event below if we
        // didn't really reorder anything
        if ( EndReordering(xPhysical) )
            return;
    }


    // find if the event is over a column at all
    bool onSeparator;
    const unsigned col = mevent.Leaving()
                            ? ((void)(onSeparator = false), COL_NONE)
                            : FindColumnAtPoint(xPhysical, &onSeparator);


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
            wxASSERT_MSG( !IsResizing(), "reentering column resize mode?" );
            StartOrContinueResizing(col, xPhysical);
        }
        // on column itself - both header and column must have the appropriate
        // flags to allow dragging the column
        else if ( HasFlag(wxHD_ALLOW_REORDER) && GetColumn(col).IsReorderable() )
        {

            // start dragging the column
            wxASSERT_MSG( !IsReordering(), "reentering column move mode?" );

            StartReordering(col, xPhysical);
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
                    evtType = wxEVT_HEADER_SEPARATOR_DCLICK;
                    m_wasSeparatorDClick = true;
                }
                else if (!wasSeparatorDClick)
                {
                    evtType = click ? wxEVT_HEADER_CLICK
                                    : wxEVT_HEADER_DCLICK;
                }
                break;

            case wxMOUSE_BTN_RIGHT:
                evtType = click ? wxEVT_HEADER_RIGHT_CLICK
                                : wxEVT_HEADER_RIGHT_DCLICK;
                break;

            case wxMOUSE_BTN_MIDDLE:
                evtType = click ? wxEVT_HEADER_MIDDLE_CLICK
                                : wxEVT_HEADER_MIDDLE_DCLICK;
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

#endif // wxUSE_HEADERCTRL
