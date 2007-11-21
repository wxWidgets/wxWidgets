/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/tooltip.cpp
// Purpose:     wxToolTip implementation
// Author:      Robert Roebling
// Id:          $Id$
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

static GtkTooltips *gs_tooltips = (GtkTooltips*) NULL;

//-----------------------------------------------------------------------------
// wxToolTip
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxToolTip, wxObject)

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
    if (!win)
        return;

    if ( !gs_tooltips )
        gs_tooltips = gtk_tooltips_new();

    m_window = win;

    if (m_text.empty())
        m_window->ApplyToolTip( gs_tooltips, (wxChar*) NULL );
    else
        m_window->ApplyToolTip( gs_tooltips, m_text );
}

/* static */
void wxToolTip::Apply(GtkWidget *w, const wxCharBuffer& tip)
{
    if ( !gs_tooltips )
        gs_tooltips = gtk_tooltips_new();

    gtk_tooltips_set_tip(gs_tooltips, w, tip, NULL);
}

void wxToolTip::Enable( bool flag )
{
    if (!gs_tooltips)
        return;

    if (flag)
        gtk_tooltips_enable( gs_tooltips );
    else
        gtk_tooltips_disable( gs_tooltips );
}

G_BEGIN_DECLS
void gtk_tooltips_set_delay (GtkTooltips *tooltips,
                             guint delay);
G_END_DECLS

void wxToolTip::SetDelay( long msecs )
{
    if (!gs_tooltips)
        return;

    // FIXME: This is a deprecated function and might not even have an effect.
    // Try to not use it, after which remove the prototype above.
    gtk_tooltips_set_delay( gs_tooltips, (int)msecs );
}

#endif // wxUSE_TOOLTIPS
