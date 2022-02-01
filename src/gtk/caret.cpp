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

namespace wxPrivate
{
static bool IsWayland()
{
#ifdef GDK_WINDOWING_WAYLAND
    GdkDisplay* display = gdk_display_get_default();
    const char* displayTypeName = g_type_name(G_TYPE_FROM_INSTANCE(display));

    return strcmp("GdkWaylandDisplay", displayTypeName) == 0;
#else
    return false;
#endif // GDK_WINDOWING_WAYLAND
}
} // namespace wxPrivate

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

    return g_value_get_long(&blinkTime);
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
        g_value_set_long(&blinkTime, milliseconds);
        g_object_set_property(G_OBJECT(settings), "gtk-cursor-blink-time", &blinkTime);
    }

    g_object_set_property(G_OBJECT(settings), "gtk-cursor-blink", &blink);
}

void wxCaret::OnTimer()
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
    if ( m_blinkedOut )
        Blink();

    int blinkTime = GetBlinkTime();
    if ( blinkTime )
        m_timer.Start(blinkTime);
}

void wxCaret::DoHide()
{
    m_timer.Stop();

    if ( !m_blinkedOut )
        Blink();
}

// ---------------------------------------------------------------------------
// moving the caret
// ---------------------------------------------------------------------------

void wxCaret::DoMove()
{
    if ( IsVisible() )
    {
        BlinkOld();
        Blink();
    }

    m_xx = m_x;
    m_yy = m_y;
}

// ---------------------------------------------------------------------------
// blink/draw the caret
// ---------------------------------------------------------------------------

void wxCaret::DoBlink(int x, int y)
{
    m_blinkedOut = !m_blinkedOut;

    Draw(x, y);
}

void wxCaret::Draw(int x, int y)
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

    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_set_operator(cr, CAIRO_OPERATOR_DIFFERENCE);
    cairo_rectangle(cr, x, y, m_width , m_height);
    cairo_clip_preserve(cr);

    if ( m_hasFocus )
        cairo_fill(cr);
    else
        cairo_stroke(cr);

    cairo_destroy(cr);

    static bool s_isWayland = wxPrivate::IsWayland();
    if ( s_isWayland )
    {
        win->RefreshRect(wxRect(x, y, m_width , m_height));
    }
}

#endif // wxUSE_CARET && defined(__WXGTK3__)
