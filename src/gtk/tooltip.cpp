/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/tooltip.cpp
// Purpose:     wxToolTip implementation
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_TOOLTIPS

#include "wx/tooltip.h"

#ifndef WX_PRECOMP
    #include "wx/window.h"
#endif

#include "wx/gtk/private.h"

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

#if !GTK_CHECK_VERSION(3,0,0) && !defined(GTK_DISABLE_DEPRECATED)
static GtkTooltips *gs_tooltips = nullptr;
#endif

//-----------------------------------------------------------------------------
// wxToolTip
//-----------------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxToolTip, wxObject);

wxToolTip::wxToolTip( const wxString &tip )
    : m_text(tip)
{
    m_window = nullptr;
}

void wxToolTip::SetTip( const wxString &tip )
{
    m_text = tip;
    if (m_window)
        m_window->GTKApplyToolTip(m_text.utf8_str());
}

void wxToolTip::GTKSetWindow(wxWindow* win)
{
    wxASSERT(win);
    m_window = win;
    m_window->GTKApplyToolTip(m_text.utf8_str());
}

/* static */
void wxToolTip::GTKApply(GtkWidget* widget, const char* tip)
{
#if GTK_CHECK_VERSION(2, 12, 0)
    if (wx_is_at_least_gtk2(12))
        gtk_widget_set_tooltip_text(widget, tip);
    else
#endif
    {
#if !GTK_CHECK_VERSION(3,0,0) && !defined(GTK_DISABLE_DEPRECATED)
        if ( !gs_tooltips )
            gs_tooltips = gtk_tooltips_new();

        gtk_tooltips_set_tip(gs_tooltips, widget, tip, nullptr);
#endif
    }
}

void wxToolTip::Enable( bool flag )
{
#ifdef __WXGTK4__
    wxUnusedVar(flag);
#else
#if GTK_CHECK_VERSION(2, 12, 0)
    if (wx_is_at_least_gtk2(12))
    {
        GtkSettings* settings = gtk_settings_get_default();
        wxGCC_WARNING_SUPPRESS(deprecated-declarations)
        if (settings)
            gtk_settings_set_long_property(settings, "gtk-enable-tooltips", flag, nullptr);
        wxGCC_WARNING_RESTORE()
    }
    else
#endif
    {
#if !GTK_CHECK_VERSION(3,0,0) && !defined(GTK_DISABLE_DEPRECATED)
        if (!gs_tooltips)
            gs_tooltips = gtk_tooltips_new();

        if (flag)
            gtk_tooltips_enable( gs_tooltips );
        else
            gtk_tooltips_disable( gs_tooltips );
#endif
    }
#endif // !__WXGTK4__
}

void wxToolTip::SetDelay( long msecs )
{
#ifdef __WXGTK4__
    wxUnusedVar(msecs);
#else
#if GTK_CHECK_VERSION(2, 12, 0)
    if (wx_is_at_least_gtk2(12))
    {
        GtkSettings* settings = gtk_settings_get_default();
        wxGCC_WARNING_SUPPRESS(deprecated-declarations)
        if (settings)
            gtk_settings_set_long_property(settings, "gtk-tooltip-timeout", msecs, nullptr);
        wxGCC_WARNING_RESTORE()
    }
    else
#endif
    {
#if !GTK_CHECK_VERSION(3,0,0) && !defined(GTK_DISABLE_DEPRECATED)
        if (!gs_tooltips)
            gs_tooltips = gtk_tooltips_new();

        gtk_tooltips_set_delay( gs_tooltips, (int)msecs );
#endif
    }
#endif // !__WXGTK4__
}

void wxToolTip::SetAutoPop( long WXUNUSED(msecs) )
{
}

void wxToolTip::SetReshow( long WXUNUSED(msecs) )
{
}

#endif // wxUSE_TOOLTIPS
