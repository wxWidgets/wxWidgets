///////////////////////////////////////////////////////////////////////////////
// Name:        generic/caret.h
// Purpose:     generic wxCaret class
// Author:      Vadim Zeitlin (original code by Robert Roebling)
// Modified by:
// Created:     25.05.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if 0 //def __GNUG__
    #pragma implementation "caret.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif //WX_PRECOMP

#include "wx/caret.h"

// ----------------------------------------------------------------------------
// global variables for this module
// ----------------------------------------------------------------------------

// the blink time (common to all carets for MSW compatibility)
static int gs_blinkTime = 500;  // in milliseconds

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxCaret static functions and data
// ----------------------------------------------------------------------------


int wxCaretBase::GetBlinkTime()
{
    return gs_blinkTime;
}

void wxCaretBase::SetBlinkTime(int milliseconds)
{
    gs_blinkTime = milliseconds;
}

// ----------------------------------------------------------------------------
// initialization and destruction
// ----------------------------------------------------------------------------

void wxCaret::InitGeneric()
{
}

wxCaret::~wxCaret()
{
    if ( IsVisible() )
    {
        // stop blinking
        m_timer.Stop();
    }
}

// ----------------------------------------------------------------------------
// showing/hiding/moving the caret (base class interface)
// ----------------------------------------------------------------------------

void wxCaret::DoShow()
{
    m_timer.Start(GetBlinkTime());

    m_blinkedOut = TRUE;
    Blink();
}

void wxCaret::DoHide()
{
    m_timer.Stop();

    if ( !m_blinkedOut )
    {
        Blink();
    }
}

void wxCaret::DoMove()
{
    if ( IsVisible() && !m_blinkedOut )
    {
        Blink();
    }
    //else: will be shown at the correct location next time it blinks
}

// ----------------------------------------------------------------------------
// drawing the caret
// ----------------------------------------------------------------------------

void wxCaret::Blink()
{
    m_blinkedOut = !m_blinkedOut;

    wxClientDC dc(GetWindow());
    if ( !m_blinkedOut )
    {
        DoDraw(&dc);
    }
    else
    {
        // FIXME can't be less efficient than this... (+1 needed!)
        wxRect rect(m_x, m_y, m_width + 1, m_height + 1);
        GetWindow()->Refresh(FALSE, &rect);
    }
}

void wxCaret::DoDraw(wxDC *dc)
{
    dc->SetPen( *wxBLACK_PEN );
    dc->DrawLine( m_x, m_y, m_x+m_width, m_y );
    dc->DrawLine( m_x, m_y+m_height, m_x+m_width, m_y+m_height );
    dc->DrawLine( m_x+(m_width/2), m_y, m_x+(m_width/2), m_y+m_height );
//  dc->DrawLine( m_x+(m_width/2)+1, m_y, m_x+(m_width/2)+1, m_y+m_height );
}
