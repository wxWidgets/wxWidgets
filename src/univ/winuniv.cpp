///////////////////////////////////////////////////////////////////////////////
// Name:        univ/window.cpp
// Purpose:     implementation of extra wxWindow methods for wxUniv port
// Author:      Vadim Zeitlin
// Modified by:
// Created:     06.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "univwindow.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/window.h"
    #include "wx/dcclient.h"
    #include "wx/event.h"
#endif // WX_PRECOMP

#include "wx/univ/renderer.h"
#include "wx/univ/theme.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxWindow, wxWindowBase)

BEGIN_EVENT_TABLE(wxWindow, wxWindowBase)
    EVT_PAINT(wxWindow::OnPaint)
    EVT_ERASE_BACKGROUND(wxWindow::OnErase)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

void wxWindow::Init()
{
    m_scrollbarVert =
    m_scrollbarHorz = (wxScrollBar *)NULL;

    m_isCurrent = FALSE;
}

// ----------------------------------------------------------------------------
// background pixmap
// ----------------------------------------------------------------------------

void wxWindow::SetBackground(const wxBitmap& bitmap,
                              int alignment,
                              wxStretch stretch)
{
    m_bitmapBg = bitmap;
    m_alignBgBitmap = alignment;
    m_stretchBgBitmap = stretch;
}

const wxBitmap& wxWindow::GetBackgroundBitmap(int *alignment,
                                               wxStretch *stretch) const
{
    if ( m_bitmapBg.Ok() )
    {
        if ( alignment )
            *alignment = m_alignBgBitmap;
        if ( stretch )
            *stretch = m_stretchBgBitmap;
    }

    return m_bitmapBg;
}

// ----------------------------------------------------------------------------
// painting
// ----------------------------------------------------------------------------

wxRenderer *wxWindow::GetRenderer() const
{
    return wxTheme::Get()->GetRenderer();
}

// the event handler executed when the window background must be painted
void wxWindow::OnErase(wxEraseEvent& event)
{
    wxControlRenderer renderer(this, *event.GetDC(),
                               wxTheme::Get()->GetRenderer());

    if ( !DoDrawBackground(&renderer) )
    {
        // not processed
        event.Skip();
    }
}

// the event handler executed when the window must be repainted
void wxWindow::OnPaint(wxPaintEvent& event)
{
    // get the DC to use and create renderer on it
    wxPaintDC dc(this);
    wxControlRenderer renderer(this, dc, GetRenderer());

    // do draw the control!
    DoDraw(&renderer);
}

bool wxWindow::DoDrawBackground(wxControlRenderer *renderer)
{
    if ( !m_bitmapBg.Ok() )
        return FALSE;

    renderer->DrawBackgroundBitmap();

    return TRUE;
}

void wxWindow::DoDraw(wxControlRenderer *renderer)
{
    renderer->DrawBorder();
}

// ----------------------------------------------------------------------------
// state flags
// ----------------------------------------------------------------------------

bool wxWindow::IsFocused() const
{
    wxWindow *self = wxConstCast(this, wxWindow);
    return self->FindFocus() == self;
}

bool wxWindow::IsPressed() const
{
    return FALSE;
}

bool wxWindow::IsDefault() const
{
    return FALSE;
}

bool wxWindow::IsCurrent() const
{
    return m_isCurrent;
}

void wxWindow::SetCurrent(bool doit)
{
    m_isCurrent = doit;
}

int wxWindow::GetStateFlags() const
{
    int flags = 0;
    if ( !IsEnabled() )
        flags |= wxCONTROL_DISABLED;

    // the following states are only possible if our application is active - if
    // it is not, even our default/focused controls shouldn't appear as such
    if ( wxTheApp->IsActive() )
    {
        if ( IsCurrent() )
            flags |= wxCONTROL_CURRENT;
        if ( IsFocused() )
            flags |= wxCONTROL_FOCUSED;
        if ( IsPressed() )
            flags |= wxCONTROL_PRESSED;
        if ( IsDefault() )
            flags |= wxCONTROL_ISDEFAULT;
    }

    return flags;
}

// ----------------------------------------------------------------------------
// size
// ----------------------------------------------------------------------------

wxSize wxWindow::AdjustSize(const wxSize& size) const
{
    wxSize sz = size;
    wxTheme::Get()->GetRenderer()->AdjustSize(&sz, this);
    return sz;
}

// ----------------------------------------------------------------------------
// scrolling
// ----------------------------------------------------------------------------

void wxWindow::SetScrollbar(int orient,
                            int pos,
                            int thumb,
                            int range,
                            bool refresh)
{
    return wxWindowNative::SetScrollbar(orient, pos, thumb, range, refresh);
}

void wxWindow::SetScrollPos(int orient, int pos, bool refresh)
{
    return wxWindowNative::SetScrollPos(orient, pos, refresh);
}

int wxWindow::GetScrollPos(int orient) const
{
    return wxWindowNative::GetScrollPos(orient);
}

int wxWindow::GetScrollThumb(int orient) const
{
    return wxWindowNative::GetScrollThumb(orient);
}

int wxWindow::GetScrollRange(int orient) const
{
    return wxWindowNative::GetScrollRange(orient);
}

