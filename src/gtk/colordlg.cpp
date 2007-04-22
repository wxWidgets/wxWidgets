/////////////////////////////////////////////////////////////////////////////
// Name:        gtk/colordlg.cpp
// Purpose:     Native wxColourDialog for GTK+
// Author:      Vaclav Slavik
// Modified by:
// Created:     2004/06/04
// RCS-ID:      $Id$
// Copyright:   (c) Vaclav Slavik, 2004
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_COLOURDLG && defined(__WXGTK20__)

#include "wx/colordlg.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
#endif

#include "wx/gtk/private.h"

IMPLEMENT_DYNAMIC_CLASS(wxColourDialog, wxDialog)

wxColourDialog::wxColourDialog(wxWindow *parent, wxColourData *data)
{
    Create(parent, data);
}

bool wxColourDialog::Create(wxWindow *parent, wxColourData *data)
{
    if (data)
        m_data = *data;

    wxString title(_("Choose colour"));
    m_widget = gtk_color_selection_dialog_new(wxGTK_CONV(title));

    m_parent = GetParentForModalDialog(parent);
    if ( m_parent )
    {
        GtkWindow* gtk_parent = GTK_WINDOW( gtk_widget_get_toplevel(m_parent->m_widget) );
        gtk_window_set_transient_for(GTK_WINDOW(m_widget),
                                     gtk_parent);
    }

    GtkColorSelection *sel =
        GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(m_widget)->colorsel);
    gtk_color_selection_set_has_palette(sel, true);

    return true;
}

int wxColourDialog::ShowModal()
{
    ColourDataToDialog();

    gint result = gtk_dialog_run(GTK_DIALOG(m_widget));
    gtk_widget_hide(m_widget);

    switch (result)
    {
        default:
            wxFAIL_MSG(_T("unexpected GtkColorSelectionDialog return code"));
            // fall through

        case GTK_RESPONSE_CANCEL:
        case GTK_RESPONSE_DELETE_EVENT:
        case GTK_RESPONSE_CLOSE:
            return wxID_CANCEL;

        case GTK_RESPONSE_OK:
            DialogToColourData();
            return wxID_OK;
    }
}

void wxColourDialog::ColourDataToDialog()
{
    GtkColorSelection *sel =
        GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(m_widget)->colorsel);

    if (m_data.GetColour().Ok())
    {
        gtk_color_selection_set_current_color(sel,
                                              m_data.GetColour().GetColor());
    }

    // setup the palette:

    GdkColor colors[16];
    gint n_colors = 0;
    for (unsigned i = 0; i < 16; i++)
    {
        wxColour c = m_data.GetCustomColour(i);
        if (c.Ok())
        {
            colors[n_colors] = *c.GetColor();
            n_colors++;
        }
    }

    wxGtkString pal(gtk_color_selection_palette_to_string(colors, n_colors));

    GtkSettings *settings = gtk_widget_get_settings(GTK_WIDGET(sel));
    g_object_set(settings, "gtk-color-palette", pal.c_str(), NULL);
}

void wxColourDialog::DialogToColourData()
{
    GtkColorSelection *sel =
        GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(m_widget)->colorsel);

    GdkColor clr;
    gtk_color_selection_get_current_color(sel, &clr);
    m_data.SetColour(wxColour(clr));

    // Extract custom palette:

    GtkSettings *settings = gtk_widget_get_settings(GTK_WIDGET(sel));
    gchar *pal;
    g_object_get(settings, "gtk-color-palette", &pal, NULL);

    GdkColor *colors;
    gint n_colors;
    if (gtk_color_selection_palette_from_string(pal, &colors, &n_colors))
    {
        for (int i = 0; i < wxMin(n_colors, 16); i++)
        {
            m_data.SetCustomColour(i, wxColour(colors[i]));
        }
        g_free(colors);
    }

    g_free(pal);
}

#endif // wxUSE_COLOURDLG && defined(__WXGTK20__)

