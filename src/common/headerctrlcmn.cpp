///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/headerctrlcmn.cpp
// Purpose:     implementation of wxHeaderCtrlBase
// Author:      Vadim Zeitlin
// Created:     2008-12-02
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

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

namespace
{

const unsigned int wxNO_COLUMN = static_cast<unsigned>(-1);

} // anonymous namespace

// ============================================================================
// wxHeaderCtrlBase implementation
// ============================================================================

extern WXDLLIMPEXP_DATA_CORE(const char) wxHeaderCtrlNameStr[] = "wxHeaderCtrl";

BEGIN_EVENT_TABLE(wxHeaderCtrlBase, wxControl)
    EVT_HEADER_SEPARATOR_DCLICK(wxID_ANY, wxHeaderCtrlBase::OnSeparatorDClick)
END_EVENT_TABLE()

void wxHeaderCtrlBase::ScrollWindow(int dx,
                                    int WXUNUSED_UNLESS_DEBUG(dy),
                                    const wxRect * WXUNUSED_UNLESS_DEBUG(rect))

{
    // this doesn't make sense at all
    wxASSERT_MSG( !dy, "header window can't be scrolled vertically" );

    // this would actually be nice to support for "frozen" headers but it isn't
    // supported currently
    wxASSERT_MSG( !rect, "header window can't be scrolled partially" );

    DoScrollHorz(dx);
}

void wxHeaderCtrlBase::SetColumnCount(unsigned int count)
{
    if ( count == GetColumnCount() )
        return;

    OnColumnCountChanging(count);

    DoSetCount(count);
}

// ----------------------------------------------------------------------------
// wxHeaderCtrlBase event handling
// ----------------------------------------------------------------------------

void wxHeaderCtrlBase::OnSeparatorDClick(wxHeaderCtrlEvent& event)
{
    const unsigned col = event.GetColumn();

    int w = wxWindowBase::GetTextExtent(GetColumn(col).GetTitle()).x;
    w += 4*GetCharWidth(); // add some arbitrary margins around text

    if ( !UpdateColumnWidthToFit(col, w) )
        event.Skip();
    else
        UpdateColumn(col);
}

// ----------------------------------------------------------------------------
// wxHeaderCtrlBase column reordering
// ----------------------------------------------------------------------------

void wxHeaderCtrlBase::SetColumnsOrder(const wxArrayInt& order)
{
    const unsigned count = GetColumnCount();
    wxCHECK_RET( order.size() == count, "wrong number of columns" );

    // check the array validity
    wxArrayInt seen(count, 0);
    for ( unsigned n = 0; n < count; n++ )
    {
        const unsigned idx = order[n];
        wxCHECK_RET( idx < count, "invalid column index" );
        wxCHECK_RET( !seen[idx], "duplicate column index" );

        seen[idx] = 1;
    }

    DoSetColumnsOrder(order);

    // TODO-RTL: do we need to reverse the array?
}

void wxHeaderCtrlBase::ResetColumnsOrder()
{
    const unsigned count = GetColumnCount();
    wxArrayInt order(count);
    for ( unsigned n = 0; n < count; n++ )
        order[n] = n;

    DoSetColumnsOrder(order);
}

wxArrayInt wxHeaderCtrlBase::GetColumnsOrder() const
{
    const wxArrayInt order = DoGetColumnsOrder();

    wxASSERT_MSG( order.size() == GetColumnCount(), "invalid order array" );

    return order;
}

unsigned int wxHeaderCtrlBase::GetColumnAt(unsigned int pos) const
{
    wxCHECK_MSG( pos < GetColumnCount(), wxNO_COLUMN, "invalid position" );

    return GetColumnsOrder()[pos];
}

unsigned int wxHeaderCtrlBase::GetColumnPos(unsigned int idx) const
{
    const unsigned count = GetColumnCount();

    wxCHECK_MSG( idx < count, wxNO_COLUMN, "invalid index" );

    const wxArrayInt order = GetColumnsOrder();
    for ( unsigned n = 0; n < count; n++ )
    {
        if ( (unsigned)order[n] == idx )
            return n;
    }

    wxFAIL_MSG( "column unexpectedly not displayed at all" );

    return wxNO_COLUMN;
}

/* static */
void wxHeaderCtrlBase::MoveColumnInOrderArray(wxArrayInt& order,
                                              unsigned int idx,
                                              unsigned int pos)
{
    const unsigned count = order.size();

    wxArrayInt orderNew;
    orderNew.reserve(count);
    for ( unsigned n = 0; ; n++ )
    {
        // NB: order of checks is important for this to work when the new
        //     column position is the same as the old one

        // insert the column at its new position
        if ( orderNew.size() == pos )
            orderNew.push_back(idx);

        if ( n == count )
            break;

        // delete the column from its old position
        const unsigned idxOld = order[n];
        if ( idxOld == idx )
            continue;

        orderNew.push_back(idxOld);
    }

    order.swap(orderNew);
}

void
wxHeaderCtrlBase::DoResizeColumnIndices(wxArrayInt& colIndices, unsigned int count)
{
    // update the column indices array if necessary
    const unsigned countOld = colIndices.size();
    if ( count > countOld )
    {
        // all new columns have default positions equal to their indices
        for ( unsigned n = countOld; n < count; n++ )
            colIndices.push_back(n);
    }
    else if ( count < countOld )
    {
        // filter out all the positions which are invalid now while keeping the
        // order of the remaining ones
        wxArrayInt colIndicesNew;
        colIndicesNew.reserve(count);
        for ( unsigned n = 0; n < countOld; n++ )
        {
            const unsigned idx = colIndices[n];
            if ( idx < count )
                colIndicesNew.push_back(idx);
        }

        colIndices.swap(colIndicesNew);
    }
    else // count didn't really change, we shouldn't even be called
    {
        wxFAIL_MSG( "useless call to DoResizeColumnIndices()" );
    }

    wxASSERT_MSG( colIndices.size() == count, "logic error" );
}

// ============================================================================
// wxHeaderCtrlSimple implementation
// ============================================================================

void wxHeaderCtrlSimple::Init()
{
    m_sortKey = wxNO_COLUMN;
}

wxHeaderColumn& wxHeaderCtrlSimple::GetColumn(unsigned int idx)
{
    return m_cols[idx];
}

void wxHeaderCtrlSimple::DoInsert(const wxHeaderColumnSimple& col, unsigned int idx)
{
    m_cols.insert(m_cols.begin() + idx, col);

    UpdateColumnCount();
}

void wxHeaderCtrlSimple::DoDelete(unsigned int idx)
{
    m_cols.erase(m_cols.begin() + idx);
    if ( idx == m_sortKey )
        m_sortKey = wxNO_COLUMN;

    UpdateColumnCount();
}

void wxHeaderCtrlSimple::DeleteAllColumns()
{
    m_cols.clear();
    m_sortKey = wxNO_COLUMN;

    UpdateColumnCount();
}


void wxHeaderCtrlSimple::DoShowColumn(unsigned int idx, bool show)
{
    if ( show != m_cols[idx].IsShown() )
    {
        m_cols[idx].SetHidden(!show);

        UpdateColumn(idx);
    }
}

void wxHeaderCtrlSimple::DoShowSortIndicator(unsigned int idx, bool ascending)
{
    RemoveSortIndicator();

    m_cols[idx].SetAsSortKey(ascending);
    m_sortKey = idx;

    UpdateColumn(idx);
}

void wxHeaderCtrlSimple::RemoveSortIndicator()
{
    if ( m_sortKey != wxNO_COLUMN )
    {
        const unsigned sortOld = m_sortKey;
        m_sortKey = wxNO_COLUMN;

        m_cols[sortOld].UnsetAsSortKey();

        UpdateColumn(sortOld);
    }
}

bool
wxHeaderCtrlSimple::UpdateColumnWidthToFit(unsigned int idx, int widthTitle)
{
    const int widthContents = GetBestFittingWidth(idx);
    if ( widthContents == -1 )
        return false;

    m_cols[idx].SetWidth(wxMax(widthContents, widthTitle));

    return true;
}

// ============================================================================
// wxHeaderCtrlEvent implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxHeaderCtrlEvent, wxNotifyEvent)

const wxEventType wxEVT_COMMAND_HEADER_CLICK = wxNewEventType();
const wxEventType wxEVT_COMMAND_HEADER_RIGHT_CLICK = wxNewEventType();
const wxEventType wxEVT_COMMAND_HEADER_MIDDLE_CLICK = wxNewEventType();

const wxEventType wxEVT_COMMAND_HEADER_DCLICK = wxNewEventType();
const wxEventType wxEVT_COMMAND_HEADER_RIGHT_DCLICK = wxNewEventType();
const wxEventType wxEVT_COMMAND_HEADER_MIDDLE_DCLICK = wxNewEventType();

const wxEventType wxEVT_COMMAND_HEADER_SEPARATOR_DCLICK = wxNewEventType();

const wxEventType wxEVT_COMMAND_HEADER_BEGIN_RESIZE = wxNewEventType();
const wxEventType wxEVT_COMMAND_HEADER_RESIZING = wxNewEventType();
const wxEventType wxEVT_COMMAND_HEADER_END_RESIZE = wxNewEventType();

const wxEventType wxEVT_COMMAND_HEADER_BEGIN_REORDER = wxNewEventType();
const wxEventType wxEVT_COMMAND_HEADER_END_REORDER = wxNewEventType();

const wxEventType wxEVT_COMMAND_HEADER_DRAGGING_CANCELLED = wxNewEventType();
