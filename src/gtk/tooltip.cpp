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
// global data
//-----------------------------------------------------------------------------

GtkTooltips *gs_tooltips = (GtkTooltips*) NULL;
GdkColor gs_tooltip_bg;
GdkColor gs_tooltip_fg;

//-----------------------------------------------------------------------------
// wxToolTip
//-----------------------------------------------------------------------------

void wxToolTip::Add( wxWindow *tool, const wxString &tip )
{
    if (!gs_tooltips)
    {
        gs_tooltips = gtk_tooltips_new();
	
	gs_tooltip_fg.red = 0;
	gs_tooltip_fg.green = 0;
	gs_tooltip_fg.blue = 0;
        gdk_color_alloc( gtk_widget_get_colormap( tool->GetConnectWidget() ), &gs_tooltip_fg );
	
	gs_tooltip_bg.red = 65535;
	gs_tooltip_bg.green = 65535;
	gs_tooltip_bg.blue = 50000;
        gdk_color_alloc( gtk_widget_get_colormap( tool->GetConnectWidget() ), &gs_tooltip_bg );
	
	gtk_tooltips_set_colors( gs_tooltips, &gs_tooltip_bg, &gs_tooltip_fg );
    }
    
    gtk_tooltips_set_tip( gs_tooltips, tool->GetConnectWidget(), tip, (gchar*) NULL );
}

void wxToolTip::Enable( bool flag )
{
    if (!gs_tooltips) gs_tooltips = gtk_tooltips_new();
    
    if (flag)
        gtk_tooltips_enable( gs_tooltips );
    else
        gtk_tooltips_disable( gs_tooltips );
}

void wxToolTip::SetDelay( long msecs )
{
    if (!gs_tooltips) gs_tooltips = gtk_tooltips_new();
    
    gtk_tooltips_set_delay( gs_tooltips, msecs );
}

