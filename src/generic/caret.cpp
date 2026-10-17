///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/caret.cpp
// Purpose:     generic wxCaret class implementation
// Author:      Vadim Zeitlin (original code by Robert Roebling)
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
#include "wx/settings.h"

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
    // Convert postion from client to logical coordinates. We will convert
    // back to client upon every use of the position so that a change in
    // position due to scrolling is taken into account.
    wxPoint viewStart = GetWindow()->GetViewStartPixels();
    m_x += viewStart.x;
    m_y += viewStart.y;

    if (m_overlay.IsNative())
    {
        m_overlay.Reset();

        // Resetting a native overlay clears its contents, so redraw the caret
        // immediately at its new position if it is currently visible.
        if ( IsVisible() && !m_blinkedOut )
            Refresh();

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

void wxCaret::DoGetPosition(int* x, int* y) const
{
    wxPoint pt = LogicalToClient();
    if ( x ) *x = pt.x;
    if ( y ) *y = pt.y;
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
    wxPoint pt = LogicalToClient();

    wxClientDC dcWin(GetWindow());

    if (m_overlay.IsNative())
    {
        if (m_blinkedOut)
            m_overlay.Reset();
        else
        {
            wxDCOverlay dcOverlay(m_overlay, &dcWin, pt.x, pt.y, m_width, m_height);
            DoDraw(&dcWin, GetWindow());
        }

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
                       &dcWin, pt.x, pt.y);

            m_xOld = pt.x;
            m_yOld = pt.y;
        }
        //else: we already saved the image below the caret, don't do it any
        //      more

        // and draw the caret there
        DoDraw(&dcWin, GetWindow());
    }
}

void wxCaret::DoDraw(wxDC *dc, wxWindow* win)
{
    // Set caret colour black or white depending on background colour.
    wxPen pen(*wxBLACK_PEN);
    wxBrush brush(*wxBLACK_BRUSH);
    if ( win && win->UseBackgroundColour() )
    {
        if ( win->GetBackgroundColour().GetLuminance() < 0.5 )
        {
            // Custom background is dark, so use white.
            pen = *wxWHITE_PEN;
            brush = *wxWHITE_BRUSH;
        }
    }
    else if ( wxSystemSettings::GetAppearance().IsDark() )
    {
        // Background is default colour and default is dark, so use white.
        pen = *wxWHITE_PEN;
        brush = *wxWHITE_BRUSH;
    }

    wxPoint pt = LogicalToClient();

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
            gc->DrawRectangle(pt.x + 0.5, pt.y + 0.5, m_width - 1, m_height - 1);
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

    dc->DrawRectangle(pt.x, pt.y, m_width, m_height);
}

wxPoint wxCaret::LogicalToClient() const
{
    wxPoint viewStart = GetWindow()->GetViewStartPixels();
    return wxPoint(m_x - viewStart.x, m_y - viewStart.y);
}

#endif // wxUSE_CARET
