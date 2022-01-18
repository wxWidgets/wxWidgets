///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/caret.cpp
// Purpose:     generic wxCaret class implementation
// Author:      Vadim Zeitlin (original code by Robert Roebling)
// Modified by:
// Created:     25.05.99
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_CARET

#ifndef WX_PRECOMP
    #include "wx/window.h"
    #include "wx/dcclient.h"
    #include "wx/dcmemory.h"
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

#ifdef _WXGTK__
    GtkSettings *settings = gtk_settings_get_default();
    if (millseconds == 0)
    {
        gtk_settings_set_long_property(settings, "gtk-cursor-blink", gtk_false, NULL);
    }
    else
    {
        gtk_settings_set_long_property(settings, "gtk-cursor-blink", gtk_true, NULL);
        gtk_settings_set_long_property(settings, "gtk-cursor-time", milliseconds, NULL);
    }
#endif
}

// ----------------------------------------------------------------------------
// initialization and destruction
// ----------------------------------------------------------------------------

void wxCaret::InitGeneric()
{
    m_hasFocus = true;
    m_blinkedOut = true;

#ifndef wxHAS_NATIVE_OVERLAY
    m_overlay.UseGeneric();
#endif
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

    if ( m_blinkedOut )
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
    if ( m_overlay.IsNative() )
    {
#ifndef __WXGTK3__
        m_overlay.Reset();
#endif
    }

    if ( IsVisible() )
    {
        if ( !m_blinkedOut )
        {
            // hide it right now and it will be shown the next time it blinks
            Blink();

            // but if the caret is not blinking, we should blink it back into
            // visibility manually
            if ( !m_timer.IsRunning() )
                Blink();
        }
    }
    //else: will be shown at the correct location when it is shown
}

void wxCaret::DoSize()
{
    int countVisible = m_countVisible;
    if (countVisible > 0)
    {
        m_countVisible = 0;
        DoHide();
    }

    m_overlay.Reset();

    if (countVisible > 0)
    {
        m_countVisible = countVisible;
        DoShow();
    }
}

// ----------------------------------------------------------------------------
// handling the focus
// ----------------------------------------------------------------------------

void wxCaret::OnSetFocus()
{
    m_hasFocus = true;

    if ( IsVisible() )
        Refresh();
}

void wxCaret::OnKillFocus()
{
    m_hasFocus = false;

    if ( IsVisible() )
    {
        // the caret must be shown - otherwise, if it is hidden now, it will
        // stay so until the focus doesn't return because it won't blink any
        // more

        // hide it first if it isn't hidden ...
        if ( !m_blinkedOut )
            Blink();

        // .. and show it in the new style
        Blink();
    }
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
    wxWindow* const win = GetWindow();

    if ( !win->IsShownOnScreen() )
    {
        return;
    }

    wxDCOverlay dcOverlay( m_overlay, win, m_x, m_y, m_width , m_height );

    if ( m_blinkedOut )
    {
        dcOverlay.Clear();

        if ( !m_overlay.IsNative() )
            m_overlay.Reset();
    }
    else
    {
        wxDC& dc = dcOverlay;
        DoDraw( &dc, win );
    }
}

void wxCaret::DoDraw(wxDC *dc, wxWindow* win)
{
    wxPen pen(*wxBLACK_PEN);
    wxBrush brush(*wxBLACK_BRUSH);
    if (win)
    {
        wxColour backgroundColour(win->GetBackgroundColour());
        if (backgroundColour.Red() < 100 &&
            backgroundColour.Green() < 100 &&
            backgroundColour.Blue() < 100)
        {
            pen = *wxWHITE_PEN;
            brush = *wxWHITE_BRUSH;
        }
    }
    dc->SetPen( pen );
    dc->SetBrush(m_hasFocus ? brush : *wxTRANSPARENT_BRUSH);

    // VZ: unfortunately, the rectangle comes out a pixel smaller when this is
    //     done under wxGTK - no idea why
    //dc->SetLogicalFunction(wxINVERT);

    dc->DrawRectangle(m_x, m_y, m_width, m_height);
}

#endif // wxUSE_CARET
