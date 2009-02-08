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
    #include "wx/utils.h"       // for wxLaunchDefaultBrowser()
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
    // Create empty GtkArray
    GtkArray() : m_strings(0), m_count(0)
    {
    }

    // Create GtkArray from wxArrayString. Note that the created object is
    // only valid as long as 'a' is!
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

extern "C" void
wxGtkAboutDialogOnClose(GtkAboutDialog *about)
{
    gtk_widget_destroy(GTK_WIDGET(about));
}

extern "C" void
wxGtkAboutDialogOnLink(GtkAboutDialog * WXUNUSED(about),
                       const gchar *link,
                       gpointer WXUNUSED(data))
{
    wxLaunchDefaultBrowser(wxGTK_CONV_BACK(link));
}

void wxAboutBox(const wxAboutDialogInfo& info)
{
    if ( !gtk_check_version(2,6,0) )
    {
        GtkAboutDialog * const dlg = GTK_ABOUT_DIALOG(gtk_about_dialog_new());
        gtk_about_dialog_set_name(dlg, GtkStr(info.GetName()));
        if ( info.HasVersion() )
            gtk_about_dialog_set_version(dlg, GtkStr(info.GetVersion()));
        if ( info.HasCopyright() )
            gtk_about_dialog_set_copyright(dlg, GtkStr(info.GetCopyrightToDisplay()));
        else
            gtk_about_dialog_set_copyright(dlg, NULL);
        if ( info.HasDescription() )
            gtk_about_dialog_set_comments(dlg, GtkStr(info.GetDescription()));
        if ( info.HasLicence() )
            gtk_about_dialog_set_license(dlg, GtkStr(info.GetLicence()));

        wxIcon icon = info.GetIcon();
        if ( icon.Ok() )
            gtk_about_dialog_set_logo(dlg, info.GetIcon().GetPixbuf());

        if ( info.HasWebSite() )
        {
            // NB: must be called before gtk_about_dialog_set_website() as
            //     otherwise it has no effect (although GTK+ docs don't mention
            //     this...)
            gtk_about_dialog_set_url_hook(wxGtkAboutDialogOnLink, NULL, NULL);

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

        wxString transCredits;
        if ( info.HasTranslators() )
        {
            const wxArrayString& translators = info.GetTranslators();
            const size_t count = translators.size();
            for ( size_t n = 0; n < count; n++ )
            {
                transCredits << translators[n] << _T('\n');
            }
        }
        else // no translators explicitely specified
        {
            // maybe we have translator credits in the message catalog?
            wxString translator = _("translator-credits");

            // gtk_about_dialog_set_translator_credits() is smart enough to
            // detect if "translator-credits" is untranslated and hide the
            // translators tab in that case, however it will still show the
            // "credits" button, (at least GTK 2.10.6) even if there are no
            // credits informations at all, so we still need to do the check
            // ourselves
            if ( translator != wxT("translator-credits") ) // untranslated!
                transCredits = translator;
        }

        if ( !transCredits.empty() )
            gtk_about_dialog_set_translator_credits(dlg, GtkStr(transCredits));

        g_signal_connect(dlg, "response",
                            G_CALLBACK(wxGtkAboutDialogOnClose), NULL);

        gtk_widget_show(GTK_WIDGET(dlg));
        return;
    }

    // native about dialog not available, fall back to the generic one
    wxGenericAboutBox(info);
}

#endif // wxUSE_ABOUTDLG && GTK+ 2.6+
