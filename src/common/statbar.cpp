///////////////////////////////////////////////////////////////////////////////
// Name:        common/statbar.cpp
// Purpose:     wxStatusBarBase implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     14.10.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// License:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "statbar.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/statusbr.h"
#endif //WX_PRECOMP

#if wxUSE_STATUSBAR

// ============================================================================
// wxStatusBarBase implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxStatusBar, wxWindow)

// ----------------------------------------------------------------------------
// ctor/dtor
// ----------------------------------------------------------------------------

wxStatusBarBase::wxStatusBarBase()
{
    m_nFields = 0;

    InitWidths();
}

wxStatusBarBase::~wxStatusBarBase()
{
    FreeWidths();
}

// ----------------------------------------------------------------------------
// widths array handling
// ----------------------------------------------------------------------------

void wxStatusBarBase::InitWidths()
{
    m_statusWidths = NULL;
}

void wxStatusBarBase::FreeWidths()
{
    delete [] m_statusWidths;
}

// ----------------------------------------------------------------------------
// field widths
// ----------------------------------------------------------------------------

void wxStatusBarBase::SetFieldsCount(int number, const int *widths)
{
    wxCHECK_RET( number > 0, _T("invalid field number in SetFieldsCount") );

    bool refresh = FALSE;

    if ( number != m_nFields )
    {
        m_nFields = number;

        ReinitWidths();

        refresh = TRUE;
    }
    //else: keep the old m_statusWidths if we had them

    if ( widths )
    {
        SetStatusWidths(number, widths);

        // already done from SetStatusWidths()
        refresh = FALSE;
    }

    if ( refresh )
        Refresh();
}

void wxStatusBarBase::SetStatusWidths(int WXUNUSED_UNLESS_DEBUG(n),
                                      const int widths[])
{
    wxCHECK_RET( widths, _T("NULL pointer in SetStatusWidths") );

    wxASSERT_MSG( n == m_nFields, _T("field number mismatch") );

    if ( !m_statusWidths )
        m_statusWidths = new int[m_nFields];

    for ( int i = 0; i < m_nFields; i++ )
    {
        m_statusWidths[i] = widths[i];
    }

    // update the display after the widths changed
    Refresh();
}

wxArrayInt wxStatusBarBase::CalculateAbsWidths(wxCoord widthTotal) const
{
    wxArrayInt widths;

    if ( m_statusWidths == NULL )
    {
        if ( m_nFields )
        {
            // default: all fields have the same width
            int nWidth = widthTotal / m_nFields;
            for ( int i = 0; i < m_nFields; i++ )
            {
                widths.Add(nWidth);
            }
        }
        //else: we're empty anyhow
    }
    else // have explicit status widths
    {
        // calculate the total width of all the fixed width fields and the
        // total number of var field widths counting with multiplicity
        int nTotalWidth = 0,
            nVarCount = 0,
            i;
        for ( i = 0; i < m_nFields; i++ )
        {
            if ( m_statusWidths[i] >= 0 )
            {
                nTotalWidth += m_statusWidths[i];
            }
            else
            {
                nVarCount += -m_statusWidths[i];
            }
        }

        // the amount of extra width we have per each var width field
        int nVarWidth;
        if ( nVarCount )
        {
            int widthExtra = widthTotal - nTotalWidth;
            nVarWidth = widthExtra > 0 ? widthExtra / nVarCount : 0;
        }
        else // no var width fields at all
        {
            nVarWidth = 0;
        }

        // do fill the array
        for ( i = 0; i < m_nFields; i++ )
        {
            if ( m_statusWidths[i] >= 0 )
            {
                widths.Add(m_statusWidths[i]);
            }
            else
            {
                widths.Add(-m_statusWidths[i]*nVarWidth);
            }
        }
    }

    return widths;
}

#endif // wxUSE_STATUSBAR

