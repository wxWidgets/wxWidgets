/////////////////////////////////////////////////////////////////////////////
// Name:        tooltip.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "tooltip.h"
#endif

#include "wx/tooltip.h"

#include "gtk/gtk.h"
#include "gdk/gdk.h"

//-----------------------------------------------------------------------------
// wxToolTip
//-----------------------------------------------------------------------------

wxToolTip::wxToolTip( const wxString &tip )
{
    m_text = tip;
    
    m_tooltips = (GtkTooltips*) NULL;
    
    m_fg = new GdkColor;
    m_fg->red = 0; 
    m_fg->green = 0;
    m_fg->blue = 0;
	
    m_bg = new GdkColor;
    m_bg->red = 65535;
    m_bg->green = 65535;
    m_bg->blue = 50000;
}

wxToolTip::~wxToolTip()
{
    gtk_object_unref( GTK_OBJECT(m_tooltips) );
    
    delete m_fg;
    delete m_bg;
}

bool wxToolTip::Ok()
{
    return (m_tooltips);
}

void wxToolTip::Create( GtkWidget *tool )
{
    if (!m_tooltips)
    {
        m_tooltips = gtk_tooltips_new();
	
        gdk_color_alloc( gtk_widget_get_colormap( tool ), m_fg );
        gdk_color_alloc( gtk_widget_get_colormap( tool ), m_bg );
	
	gtk_tooltips_set_colors( m_tooltips, m_bg, m_fg );
    }
    
    gtk_tooltips_set_tip( m_tooltips, tool, m_text, (gchar*) NULL );
}

void wxToolTip::Enable( bool flag )
{
    if (!Ok()) return;
    
    if (flag)
        gtk_tooltips_enable( m_tooltips );
    else
        gtk_tooltips_disable( m_tooltips );
}

void wxToolTip::SetDelay( long msecs )
{
    if (!Ok()) return;
    
    gtk_tooltips_set_delay( m_tooltips, msecs );
}

