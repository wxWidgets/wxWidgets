///////////////////////////////////////////////////////////////////////////////
// Name:        generic/caret.cpp
// Purpose:     generic wxCaret class implementation
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

#ifdef __GNUG__
    #pragma implementation "caret.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_CARET

#ifndef WX_PRECOMP
    #include "wx/window.h"
    #include "wx/dcclient.h"
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
// timer stuff
// ----------------------------------------------------------------------------

wxCaretTimer::wxCaretTimer(wxCaret *caret) 
{ 
    m_caret = caret; 
}

void wxCaretTimer::Notify() 
{ 
    m_caret->OnTimer(); 
}

void wxCaret::OnTimer()
{
    // don't blink the caret when we don't have the focus
    if ( m_hasFocus )
        Blink();
}

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
    m_hasFocus = TRUE;
    m_blinkedOut = FALSE;
}

wxCaret::~wxCaret()
{
    if ( IsVisible() )
    {
        // stop blinking
        if ( m_timer.IsRunning() )
            m_timer.Stop();
    }
}

// ----------------------------------------------------------------------------
// showing/hiding/moving the caret (base class interface)
// ----------------------------------------------------------------------------

void wxCaret::DoShow()
{
    int blinkTime = GetBlinkTime();
    if ( blinkTime )
        m_timer.Start(blinkTime);

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
    if ( IsVisible() )
    {
        if ( !m_blinkedOut )
        {
            // hide it right now and it will be shown the next time it blinks
            // unless it should be shown all the time in which case just show
            // it at the new position
            if ( m_timer.IsRunning() )
                Blink();
            else
                Refresh();
        }
    }
    //else: will be shown at the correct location when it is shown
}

// ----------------------------------------------------------------------------
// handling the focus
// ----------------------------------------------------------------------------

void wxCaret::OnSetFocus()
{
    m_hasFocus = TRUE;

    Refresh();
}

void wxCaret::OnKillFocus()
{
    m_hasFocus = FALSE;

    if ( IsVisible() )
    {
        // the caret must be shown - otherwise, if it is hidden now, it will
        // stay so until the focus doesn't return because it won't blink any
        // more
        m_blinkedOut = FALSE;
    }

    Refresh();
}

// ----------------------------------------------------------------------------
// drawing the caret
// ----------------------------------------------------------------------------

void wxCaret::Blink()
{
    m_blinkedOut = !m_blinkedOut;

    Refresh();
}

void wxCaret::Refresh()
{
    if ( !m_blinkedOut )
    {
        wxClientDC dc(GetWindow());
        DoDraw(&dc);
    }
    else
    {
        // FIXME can't be less efficient than this... we probably should use
        //       backing store for the caret instead of leaving all the burden
        //       of correct refresh logic handling to the user code

        // NB: +1 is needed!
        wxRect rect(m_x, m_y, m_width + 1, m_height + 1);
        GetWindow()->Refresh(FALSE, &rect);
    }
}

void wxCaret::DoDraw(wxDC *dc)
{
    dc->SetPen( *wxBLACK_PEN );

    // VZ: Robert's code for I-shaped caret - this is nice but doesn't look
    //     at all the same as the MSW version and I don't know how to indicate
    //     that the window has focus or not with such caret
#if 0
    dc->DrawLine( m_x, m_y, m_x+m_width, m_y );
    dc->DrawLine( m_x, m_y+m_height, m_x+m_width, m_y+m_height );
    dc->DrawLine( m_x+(m_width/2), m_y, m_x+(m_width/2), m_y+m_height );
//  dc->DrawLine( m_x+(m_width/2)+1, m_y, m_x+(m_width/2)+1, m_y+m_height );
#else // 1
    if ( m_hasFocus )
        dc->SetBrush( *wxBLACK_BRUSH );
    dc->DrawRectangle( m_x, m_y, m_width, m_height );
#endif // 0/1
}

#endif // wxUSE_CARET
