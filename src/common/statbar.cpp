///////////////////////////////////////////////////////////////////////////////
// Name:        common/statbar.cpp
// Purpose:     wxStatusBarBase implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     14.10.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// License:     wxWindows licence
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

#include "wx/listimpl.cpp"
WX_DEFINE_LIST(wxListString);

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
    InitStacks();
}

wxStatusBarBase::~wxStatusBarBase()
{
    FreeWidths();
    FreeStacks();
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
        // copy stacks if present
        if(m_statusTextStacks)
        {
            wxListString **newStacks = new wxListString*[number];
            size_t i, j, max = wxMin(number, m_nFields);

            // copy old stacks
            for(i = 0; i < max; ++i)
                newStacks[i] = m_statusTextStacks[i];
            // free old stacks in excess
            for(j = i; j < (size_t)m_nFields; ++j)
            {
                if(m_statusTextStacks[j])
                {
                    m_statusTextStacks[j]->Clear();
                    delete m_statusTextStacks[j];
                }
            }
            // initialize new stacks to NULL
            for(j = i; j < (size_t)number; ++j)
                newStacks[j] = 0;

            m_statusTextStacks = newStacks;
        }

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

// ----------------------------------------------------------------------------
// text stacks handling
// ----------------------------------------------------------------------------

void wxStatusBarBase::InitStacks()
{
    m_statusTextStacks = NULL;
}

void wxStatusBarBase::FreeStacks()
{
    if(!m_statusTextStacks) return;
    size_t i;

    for(i = 0; i < (size_t)m_nFields; ++i)
    {
        if(m_statusTextStacks[i])
        {
            m_statusTextStacks[i]->Clear();
            delete m_statusTextStacks[i];
        }
    }

    delete[] m_statusTextStacks;
}

// ----------------------------------------------------------------------------
// text stacks
// ----------------------------------------------------------------------------

void wxStatusBarBase::PushStatusText(const wxString& text, int number)
{
    wxListString* st = GetOrCreateStatusStack(number);
    st->Insert(new wxString(GetStatusText(number)));
    SetStatusText(text, number);
}

void wxStatusBarBase::PopStatusText(int number)
{
    wxListString *st = GetStatusStack(number);
    wxCHECK_RET( st, _T("Unbalanced PushStatusText/PopStatusText") );
    wxListString::Node *top = st->GetFirst();

    SetStatusText(*top->GetData(), number);
    st->DeleteNode(top);
    if(st->GetCount() == 0)
    {
        delete st;
        m_statusTextStacks[number] = 0;
    }
}

wxListString *wxStatusBarBase::GetStatusStack(int i) const
{
    if(!m_statusTextStacks)
        return 0;
    return m_statusTextStacks[i];
}

wxListString *wxStatusBarBase::GetOrCreateStatusStack(int i)
{
    if(!m_statusTextStacks)
    {
        m_statusTextStacks = new wxListString*[m_nFields];

        size_t j;
        for(j = 0; j < (size_t)m_nFields; ++j) m_statusTextStacks[j] = 0;
    }

    if(!m_statusTextStacks[i])
    {
        m_statusTextStacks[i] = new wxListString();
        m_statusTextStacks[i]->DeleteContents(TRUE);
    }

    return m_statusTextStacks[i];
}

#endif // wxUSE_STATUSBAR

