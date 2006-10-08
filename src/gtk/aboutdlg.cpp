///////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/aboutdlg.cpp
// Purpose:     native GTK+ wxAboutBox() implementation
// Author:      Vadim Zeitlin
// Created:     2006-10-08
// RCS-ID:      $Id$
// Copyright:   (c) 2006 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_ABOUTDLG && defined(__WXGTK26__)

#ifndef WX_PRECOMP
#endif //WX_PRECOMP

#include "wx/aboutdlg.h"
#include "wx/generic/aboutdlgg.h"

#include "wx/gtk/private.h"

// ----------------------------------------------------------------------------
// GtkStr: temporary GTK string
// ----------------------------------------------------------------------------

class GtkStr : public wxGtkString
{
public:
    GtkStr(const wxString& s)
        : wxGtkString(wx_const_cast(char *, wxGTK_CONV_SYS(s).release()))
    {
    }
};

// ----------------------------------------------------------------------------
// GtkArray: temporary array of GTK strings
// ----------------------------------------------------------------------------

class GtkArray
{
public:
    GtkArray(const wxArrayString& a)
    {
        m_count = a.size();
        m_strings = new const gchar *[m_count + 1];
        for ( size_t n = 0; n < m_count; n++ )
            m_strings[n] = wxGTK_CONV_SYS(a[n]).release();

        // array must be NULL-terminated
        m_strings[m_count] = NULL;
    }

    operator const gchar **() const { return m_strings; }

    ~GtkArray()
    {
        for ( size_t n = 0; n < m_count; n++ )
            free(wx_const_cast(gchar *, m_strings[n]));

        delete [] m_strings;
    }

private:
    const gchar **m_strings;
    size_t m_count;

    DECLARE_NO_COPY_CLASS(GtkArray)
};

// ============================================================================
// implementation
// ============================================================================

void wxAboutBox(const wxAboutDialogInfo& info)
{
    if ( !gtk_check_version(2,6,0) )
    {
        GtkAboutDialog * const dlg = GTK_ABOUT_DIALOG(gtk_about_dialog_new());
        gtk_about_dialog_set_name(dlg, GtkStr(info.GetName()));
        if ( info.HasVersion() )
            gtk_about_dialog_set_version(dlg, GtkStr(info.GetVersion()));
        if ( info.HasCopyright() )
            gtk_about_dialog_set_copyright(dlg, GtkStr(info.GetCopyright()));
        if ( info.HasDescription() )
            gtk_about_dialog_set_comments(dlg, GtkStr(info.GetDescription()));
        if ( info.HasLicence() )
            gtk_about_dialog_set_license(dlg, GtkStr(info.GetLicence()));

        wxIcon icon = info.GetIcon();
        if ( icon.Ok() )
            gtk_about_dialog_set_logo(dlg, info.GetIcon().GetPixbuf());

        if ( info.HasWebSite() )
        {
            gtk_about_dialog_set_website(dlg, GtkStr(info.GetWebSiteURL()));
            gtk_about_dialog_set_website_label
            (
                dlg,
                GtkStr(info.GetWebSiteDescription())
            );
        }

        if ( info.HasDevelopers() )
            gtk_about_dialog_set_authors(dlg, GtkArray(info.GetDevelopers()));
        if ( info.HasDocWriters() )
            gtk_about_dialog_set_documenters(dlg, GtkArray(info.GetDocWriters()));
        if ( info.HasArtists() )
            gtk_about_dialog_set_artists(dlg, GtkArray(info.GetArtists()));
        if ( info.HasTranslators() )
        {
            gtk_about_dialog_set_translator_credits
            (
                dlg,
                GtkStr(_("translator-credits"))
            );
        }

        gtk_widget_show(GTK_WIDGET(dlg));
        return;
    }

    // native about dialog not available, fall back to the generic one
    wxGenericAboutBox(info);
}

#endif // wxUSE_ABOUTDLG && GTK+ 2.6+
