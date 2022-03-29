///////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/caret.cpp
// Purpose:     GTK3 implementation of wxCaret
// Author:      Kettab Ali
// Created:     2022-01-29
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_CARET && defined(__WXGTK3__)

#ifndef WX_PRECOMP
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "wx/caret.h"
#include "wx/gtk/private/wrapgtk.h"

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// blink time
// ---------------------------------------------------------------------------

//static
int wxCaretBase::GetBlinkTime()
{
    GtkSettings* settings = gtk_settings_get_default();

    GValue blink = G_VALUE_INIT;
    g_value_init(&blink, G_TYPE_BOOLEAN);
    g_object_get_property(G_OBJECT(settings), "gtk-cursor-blink", &blink);

    if ( !g_value_get_boolean(&blink) )
        return 0;

    GValue blinkTime = G_VALUE_INIT;
    g_value_init(&blinkTime, G_TYPE_LONG);
    g_object_get_property(G_OBJECT(settings), "gtk-cursor-blink-time", &blinkTime);

    // Quoting gtk-cursor-blink-time documentation:
    //
    //   "Length of the cursor blink cycle, in milliseconds.
    //    Default value: 1200"
    //
    // blinkTime (blink cycle) = time interval between 2 consecutive blinks.
    //
    return g_value_get_long(&blinkTime) / 2;
}

//static
void wxCaretBase::SetBlinkTime(int milliseconds)
{
    GtkSettings* settings = gtk_settings_get_default();

    GValue blink = G_VALUE_INIT;
    g_value_init(&blink, G_TYPE_BOOLEAN);

    if ( milliseconds == 0 )
    {
        g_value_set_boolean(&blink, FALSE);
    }
    else
    {
        g_value_set_boolean(&blink, TRUE);

        GValue blinkTime = G_VALUE_INIT;
        g_value_init(&blinkTime, G_TYPE_LONG);
        g_value_set_long(&blinkTime, 2 * milliseconds);
        g_object_set_property(G_OBJECT(settings), "gtk-cursor-blink-time", &blinkTime);
    }

    g_object_set_property(G_OBJECT(settings), "gtk-cursor-blink", &blink);
}

// ---------------------------------------------------------------------------
// wxCaret implementation
// ---------------------------------------------------------------------------

wxCaret::~wxCaret()
{
    wxWindow* const win = GetWindow();
    if (win)
        win->Unbind(wxEVT_TIMER, &wxCaret::OnTimer, this);
}

void wxCaret::Init()
{
    m_hasFocus = true;
    m_blinkedOut = true;

    m_blinkTime = GetBlinkTime();
}

void wxCaret::SetupTimer()
{
    wxWindow* const win = GetWindow();
    m_timer.SetOwner(win);

    win->Bind(wxEVT_TIMER, &wxCaret::OnTimer, this);
}

void wxCaret::OnTimer(wxTimerEvent& WXUNUSED(event))
{
    // don't blink the caret when we don't have the focus
    if ( m_hasFocus )
        Blink();
}

// ---------------------------------------------------------------------------
// focus the caret
// ---------------------------------------------------------------------------
void wxCaret::OnFocus(bool hasFocus)
{
    if ( IsVisible() )
    {
        // hide it first if it isn't hidden ...
        if ( !m_blinkedOut )
            Blink();

        m_hasFocus = hasFocus;

        Blink();
    }
}

void wxCaret::OnSetFocus()
{
    OnFocus(true);
}

void wxCaret::OnKillFocus()
{
    OnFocus(false);
}

// ---------------------------------------------------------------------------
// showing/hiding the caret
// ---------------------------------------------------------------------------

void wxCaret::DoShow()
{
    if ( !m_blinkedOut )
        Draw();

    if ( m_blinkTime )
        m_timer.Start(m_blinkTime);
}

void wxCaret::DoHide()
{
    m_timer.Stop();

    if ( m_blinkedOut )
        Draw();
}

// ---------------------------------------------------------------------------
// moving the caret
// ---------------------------------------------------------------------------

void wxCaret::DoMove()
{
    if ( IsVisible() )
        Blink();
}

void wxCaret::SetPosition(int x, int y)
{
    // blink out the caret at the old position first.
    if ( !m_blinkedOut )
        Blink();

    wxCaretBase::SetPosition(x, y);
}

// ---------------------------------------------------------------------------
// blink/draw the caret
// ---------------------------------------------------------------------------

void wxCaret::Blink()
{
    m_blinkedOut = !m_blinkedOut;

    GetWindow()->RefreshRect(wxRect(m_x, m_y, m_width , m_height), false);
}

void wxCaret::Draw()
{
    wxWindow* const win = GetWindow();

    if ( !win->IsShownOnScreen() )
        return;

    cairo_t* cr = gdk_cairo_create(gtk_widget_get_window(win->GetHandle()));

    if ( win->GetLayoutDirection() == wxLayout_RightToLeft )
    {
        cairo_translate(cr, win->GetClientSize().x, 0);
        cairo_scale(cr, -1, 1);
    }

    // We want to use the foreground colour as the caret colour.

    const wxColour fgCol = win->GetForegroundColour();
    const wxColour bgCol = win->GetBackgroundColour();

    double red     = (bgCol.Red()   + fgCol.Red())   / 255.;
    double green   = (bgCol.Green() + fgCol.Green()) / 255.;
    double blue    = (bgCol.Blue()  + fgCol.Blue())  / 255.;

    cairo_set_source_rgb(cr, red, green, blue);
    cairo_set_operator(cr, CAIRO_OPERATOR_DIFFERENCE);
    cairo_rectangle(cr, m_x, m_y, m_width , m_height);
    cairo_clip_preserve(cr);

    m_hasFocus ? cairo_fill(cr) : cairo_stroke(cr);

    // The CAIRO_OPERATOR_DIFFERENCE takes the difference of the
    // destination (xB) and source (xA) colors by performing abs(xB−xA):
    //
    // If xB+xA <= 1, the operator is lossless and will alternate between
    // xB and xA on each execution.
    //
    // If xB+xA > 1, the following calculations are needed to compensate
    // for the loss of the original colour.

    red   = wxMax(red - 1.0,   0.0);
    green = wxMax(green - 1.0, 0.0);
    blue  = wxMax(blue - 1.0,  0.0);

    if ( red != 0.0 || green != 0.0 || blue != 0.0 )
    {
        cairo_set_source_rgb(cr, red, green, blue);
        cairo_set_operator(cr, CAIRO_OPERATOR_ADD);
        cairo_rectangle(cr, m_x, m_y, m_width , m_height);

        m_hasFocus ? cairo_fill(cr) : cairo_stroke(cr);
    }

    cairo_destroy(cr);
}

#endif // wxUSE_CARET && defined(__WXGTK3__)
