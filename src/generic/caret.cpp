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
#include "wx/graphics.h"

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
        gtk_settings_set_long_property(settings, "gtk-cursor-blink", gtk_false, nullptr);
    }
    else
    {
        gtk_settings_set_long_property(settings, "gtk-cursor-blink", gtk_true, nullptr);
        gtk_settings_set_long_property(settings, "gtk-cursor-time", milliseconds, nullptr);
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
    m_xOld =
    m_yOld = -1;
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
    if (m_overlay.IsNative())
    {
        m_overlay.Reset();
        return;
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

    if (m_overlay.IsNative())
        m_overlay.Reset();
    else
    {
        // Change bitmap size
        m_bmpUnderCaret.UnRef();
    }

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
    wxClientDC dcWin(GetWindow());
    if (m_overlay.IsNative())
    {
        wxDCOverlay dcOverlay(m_overlay, &dcWin, m_x, m_y, m_width, m_height);
        if (m_blinkedOut)
            dcOverlay.Clear();
        else
            DoDraw(&dcWin, GetWindow());

        return;
    }

    if ( m_blinkedOut )
    {
        // restore the old image
        dcWin.DrawBitmap(m_bmpUnderCaret, m_xOld, m_yOld);
        m_xOld =
        m_yOld = -1;
    }
    else
    {
        if ( m_xOld == -1 && m_yOld == -1 )
        {
            if (!m_bmpUnderCaret.IsOk())
                m_bmpUnderCaret.Create(m_width, m_height, dcWin);
            wxMemoryDC dcMem(m_bmpUnderCaret);
            // save the part we're going to overdraw
            dcMem.Blit(0, 0, m_width, m_height,
                       &dcWin, m_x, m_y);

            m_xOld = m_x;
            m_yOld = m_y;
        }
        //else: we already saved the image below the caret, don't do it any
        //      more

        // and draw the caret there
        DoDraw(&dcWin, GetWindow());
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
#if wxUSE_GRAPHICS_CONTEXT
    wxGraphicsContext* gc = dc->GetGraphicsContext();
#endif
    if (m_hasFocus)
    {
        dc->SetPen(*wxTRANSPARENT_PEN);
        dc->SetBrush(brush);
    }
    else
    {
        pen.SetJoin(wxJOIN_MITER);
        dc->SetPen(pen);
        dc->SetBrush(*wxTRANSPARENT_BRUSH);
#if wxUSE_GRAPHICS_CONTEXT
        if (gc)
        {
            // Draw outline rect so that its outside edges correspond with
            // those of the solid rect. This is necessary to avoid drawing
            // outside the solid rect bounds when window content is scaled.
            gc->EnableOffset(false);
            gc->DrawRectangle(m_x + 0.5, m_y + 0.5, m_width - 1, m_height - 1);
            return;
        }
#endif
    }

#if wxUSE_GRAPHICS_CONTEXT
    if (gc == nullptr)
#endif
    {
        dc->SetLogicalFunction(wxINVERT);
    }

    dc->DrawRectangle(m_x, m_y, m_width, m_height);
}

#endif // wxUSE_CARET
