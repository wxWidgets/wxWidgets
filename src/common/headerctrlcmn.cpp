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

// ============================================================================
// wxHeaderCtrlSimple implementation
// ============================================================================

void wxHeaderCtrlSimple::Init()
{
    m_sortKey = wxNO_COLUMN;
}

wxHeaderColumnBase& wxHeaderCtrlSimple::GetColumn(unsigned int idx)
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

