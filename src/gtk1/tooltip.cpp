/////////////////////////////////////////////////////////////////////////////
// Name:        tooltip.cpp
// Purpose:     wxToolTip implementation
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "tooltip.h"
#endif

#include "wx/setup.h"

#if wxUSE_TOOLTIPS

#include "wx/window.h"
#include "wx/tooltip.h"

#include "gtk/gtk.h"
#include "gdk/gdk.h"

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

static GtkTooltips *ss_tooltips = (GtkTooltips*) NULL;
static GdkColor     ss_bg;
static GdkColor     ss_fg;

//-----------------------------------------------------------------------------
// wxToolTip
//-----------------------------------------------------------------------------

wxToolTip::wxToolTip( const wxString &tip )
{
    m_text = tip;
    m_window = (wxWindow*) NULL;
}

void wxToolTip::SetTip( const wxString &tip )
{
    m_text = tip;
    Apply( m_window );
}

void wxToolTip::Apply( wxWindow *win )
{
    if (!win) return;

    if (!ss_tooltips)
    {
        ss_tooltips = gtk_tooltips_new();

        ss_fg.red = 0;
        ss_fg.green = 0;
        ss_fg.blue = 0;
        gdk_color_alloc( gtk_widget_get_default_colormap(), &ss_fg );

        ss_bg.red = 65535;
        ss_bg.green = 65535;
        ss_bg.blue = 50000;
        gdk_color_alloc( gtk_widget_get_default_colormap(), &ss_bg );

#if (GTK_MINOR_VERSION > 0)
        gtk_tooltips_force_window( ss_tooltips );
	
        GtkStyle *g_style = 
          gtk_style_copy(
            gtk_widget_get_style( ss_tooltips->tip_window ) );
	    
        g_style->fg[GTK_STATE_NORMAL] = ss_fg;
        g_style->bg[GTK_STATE_NORMAL] = ss_bg;
	
        gtk_widget_set_style( ss_tooltips->tip_window, g_style );
#else
        gtk_tooltips_set_colors( ss_tooltips, &ss_bg, &ss_fg );
#endif
    }

    m_window = win;

    if (m_text.IsEmpty())
        m_window->ApplyToolTip( ss_tooltips, (wxChar*) NULL );
    else
        m_window->ApplyToolTip( ss_tooltips, m_text );
}

void wxToolTip::Enable( bool flag )
{
    if (!ss_tooltips) return;

    if (flag)
        gtk_tooltips_enable( ss_tooltips );
    else
        gtk_tooltips_disable( ss_tooltips );
}

void wxToolTip::SetDelay( long msecs )
{
    if (!ss_tooltips)
        return;

    gtk_tooltips_set_delay( ss_tooltips, (int)msecs );
}

#endif

