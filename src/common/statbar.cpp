///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/statbar.cpp
// Purpose:     wxStatusBarBase implementation
// Author:      Vadim Zeitlin
// Modified by: Francesco Montorsi
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

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_STATUSBAR

#include "wx/statusbr.h"

#ifndef WX_PRECOMP
    #include "wx/frame.h"
#endif //WX_PRECOMP

const char wxStatusBarNameStr[] = "statusBar";


// ============================================================================
// wxStatusBarBase implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxStatusBar, wxWindow)

#include <wx/arrimpl.cpp> // This is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(wxStatusBarPaneArray);


// ----------------------------------------------------------------------------
// ctor/dtor
// ----------------------------------------------------------------------------

wxStatusBarBase::wxStatusBarBase()
{
    m_bSameWidthForAllPanes = true;
}

wxStatusBarBase::~wxStatusBarBase()
{
    // notify the frame that it doesn't have a status bar any longer to avoid
    // dangling pointers
    wxFrame *frame = dynamic_cast<wxFrame*>(GetParent());
    if ( frame && frame->GetStatusBar() == this )
        frame->SetStatusBar(NULL);
}

// ----------------------------------------------------------------------------
// field widths
// ----------------------------------------------------------------------------

void wxStatusBarBase::SetFieldsCount(int number, const int *widths)
{
    wxCHECK_RET( number > 0, _T("invalid field number in SetFieldsCount") );

    if ( (size_t)number > m_panes.GetCount() )
    {
        wxStatusBarPane newPane;

        // add more entries with the default style and zero width
        // (this will be set later)
        for (size_t i = m_panes.GetCount(); i < (size_t)number; ++i)
            m_panes.Add(newPane);
    }
    else if ( (size_t)number < m_panes.GetCount() )
    {
        // remove entries in excess
        m_panes.RemoveAt(number, m_panes.GetCount()-number);
    }

    // SetStatusWidths will automatically refresh
    SetStatusWidths(number, widths);
}

void wxStatusBarBase::SetStatusWidths(int WXUNUSED_UNLESS_DEBUG(n),
                                    const int widths[])
{
    wxASSERT_MSG( (size_t)n == m_panes.GetCount(), _T("field number mismatch") );

    if (widths == NULL)
    {
        // special value meaning: override explicit pane widths and make them all
        // of the same size
        m_bSameWidthForAllPanes = true;
    }
    else
    {
        for ( size_t i = 0; i < m_panes.GetCount(); i++ )
            m_panes[i].nWidth = widths[i];

        m_bSameWidthForAllPanes = false;
    }

    // update the display after the widths changed
    Refresh();
}

void wxStatusBarBase::SetStatusStyles(int WXUNUSED_UNLESS_DEBUG(n),
                                    const int styles[])
{
    wxCHECK_RET( styles, _T("NULL pointer in SetStatusStyles") );

    wxASSERT_MSG( (size_t)n == m_panes.GetCount(), _T("field number mismatch") );

    for ( size_t i = 0; i < m_panes.GetCount(); i++ )
        m_panes[i].nStyle = styles[i];

    // update the display after the widths changed
    Refresh();
}

wxArrayInt wxStatusBarBase::CalculateAbsWidths(wxCoord widthTotal) const
{
    wxArrayInt widths;

    if ( m_bSameWidthForAllPanes )
    {
        // Default: all fields have the same width. This is not always
        // possible to do exactly (if widthTotal is not divisible by
        // m_panes.GetCount()) - if that happens, we distribute the extra
        // pixels among all fields:
        int widthToUse = widthTotal;

        for ( size_t i = m_panes.GetCount(); i > 0; i-- )
        {
            // divide the unassigned width evently between the
            // not yet processed fields:
            int w = widthToUse / i;
            widths.Add(w);
            widthToUse -= w;
        }
    }
    else // do not override explicit pane widths
    {
        // calculate the total width of all the fixed width fields and the
        // total number of var field widths counting with multiplicity
        size_t nTotalWidth = 0,
            nVarCount = 0,
            i;

        for ( i = 0; i < m_panes.GetCount(); i++ )
        {
            if ( m_panes[i].nWidth >= 0 )
                nTotalWidth += m_panes[i].nWidth;
            else
                nVarCount += -m_panes[i].nWidth;
        }

        // the amount of extra width we have per each var width field
        int widthExtra = widthTotal - nTotalWidth;

        // do fill the array
        for ( i = 0; i < m_panes.GetCount(); i++ )
        {
            if ( m_panes[i].nWidth >= 0 )
                widths.Add(m_panes[i].nWidth);
            else
            {
                int nVarWidth = widthExtra > 0 ? (widthExtra * (-m_panes[i].nWidth)) / nVarCount : 0;
                nVarCount += m_panes[i].nWidth;
                widthExtra -= nVarWidth;
                widths.Add(nVarWidth);
            }
        }
    }

    return widths;
}

// ----------------------------------------------------------------------------
// status text stacks
// ----------------------------------------------------------------------------

void wxStatusBarBase::PushStatusText(const wxString& text, int number)
{
    // save current status text in the stack
    m_panes[number].arrStack.push_back(GetStatusText(number));

    // update current status text
    SetStatusText(text, number);
}

void wxStatusBarBase::PopStatusText(int number)
{
    wxString text = m_panes[number].arrStack.back();
    m_panes[number].arrStack.pop_back();  // also remove it from the stack

    // restore the popped status text in the pane
    SetStatusText(text, number);
}

#endif // wxUSE_STATUSBAR
