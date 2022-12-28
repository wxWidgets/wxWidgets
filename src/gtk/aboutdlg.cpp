///////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/aboutdlg.cpp
// Purpose:     native GTK+ wxAboutBox() implementation
// Author:      Vadim Zeitlin
// Created:     2006-10-08
// Copyright:   (c) 2006 Vadim Zeitlin <vadim@wxwidgets.org>
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

#if wxUSE_ABOUTDLG

#include "wx/aboutdlg.h"

#ifndef WX_PRECOMP
    #include "wx/window.h"
#endif //WX_PRECOMP

#include "wx/gtk/private.h"

// ----------------------------------------------------------------------------
// GtkArray: temporary array of GTK strings
// ----------------------------------------------------------------------------

namespace
{

class GtkArray
{
public:
    // Create empty GtkArray
    GtkArray() : m_strings(nullptr), m_count(0)
    {
    }

    // Create GtkArray from wxArrayString. Note that the created object is
    // only valid as long as 'a' is!
    GtkArray(const wxArrayString& a)
    {
        m_count = a.size();
        m_strings = new const gchar *[m_count + 1];

        for ( size_t n = 0; n < m_count; n++ )
        {
            // notice that there is no need to copy the string pointer here
            // because this class is used only as a temporary and during its
            // existence the pointer persists in wxString which uses it either
            // for internal representation (in wxUSE_UNICODE_UTF8 case) or as
            // cached m_convertedToChar (in wxUSE_UNICODE_WCHAR case)
            m_strings[n] = a[n].utf8_str();
        }

        // array must be null-terminated
        m_strings[m_count] = nullptr;
    }

    operator const gchar **() const { return m_strings; }

    ~GtkArray()
    {
        delete [] m_strings;
    }

private:
    const gchar **m_strings;
    size_t m_count;

    wxDECLARE_NO_COPY_CLASS(GtkArray);
};

} // anonymous namespace

// ============================================================================
// implementation
// ============================================================================

// GTK+ about dialog is modeless, keep track of it in this variable
static GtkAboutDialog *gs_aboutDialog = nullptr;

extern "C" {
static void wxGtkAboutDialogOnClose(GtkAboutDialog *about)
{
    gtk_widget_destroy(GTK_WIDGET(about));
    if ( about == gs_aboutDialog )
        gs_aboutDialog = nullptr;
}
}

#ifdef __WXGTK3__
extern "C" {
static gboolean activate_link(GtkAboutDialog*, const char* link, void* dontIgnore)
{
    if (dontIgnore)
    {
        wxLaunchDefaultBrowser(wxString::FromUTF8(link));
        return true;
    }
    return false;
}
}
#else
extern "C" {
static void wxGtkAboutDialogOnLink(GtkAboutDialog*, const char* link, void*)
{
    wxLaunchDefaultBrowser(wxString::FromUTF8(link));
}
}
#endif

void wxAboutBox(const wxAboutDialogInfo& info, wxWindow* parent)
{
    // don't create another dialog if one is already present
    if ( !gs_aboutDialog )
        gs_aboutDialog = GTK_ABOUT_DIALOG(gtk_about_dialog_new());

    GtkAboutDialog * const dlg = gs_aboutDialog;
    gtk_about_dialog_set_program_name(dlg, info.GetName().utf8_str());
    if ( info.HasVersion() )
        gtk_about_dialog_set_version(dlg, info.GetVersion().utf8_str());
    else
        gtk_about_dialog_set_version(dlg, nullptr);
    if ( info.HasCopyright() )
        gtk_about_dialog_set_copyright(dlg, info.GetCopyrightToDisplay().utf8_str());
    else
        gtk_about_dialog_set_copyright(dlg, nullptr);
    if ( info.HasDescription() )
        gtk_about_dialog_set_comments(dlg, info.GetDescription().utf8_str());
    else
        gtk_about_dialog_set_comments(dlg, nullptr);
    if ( info.HasLicence() )
        gtk_about_dialog_set_license(dlg, info.GetLicence().utf8_str());
    else
        gtk_about_dialog_set_license(dlg, nullptr);

    wxIcon icon = info.GetIcon();
    if ( icon.IsOk() )
        gtk_about_dialog_set_logo(dlg, info.GetIcon().GetPixbuf());

    if ( info.HasWebSite() )
    {
#ifdef __WXGTK3__
        g_signal_connect(dlg, "activate-link", G_CALLBACK(activate_link), dlg);
#else
        // NB: must be called before gtk_about_dialog_set_website() as
        //     otherwise it has no effect (although GTK+ docs don't mention
        //     this...)
        gtk_about_dialog_set_url_hook(wxGtkAboutDialogOnLink, nullptr, nullptr);
#endif

        gtk_about_dialog_set_website(dlg, info.GetWebSiteURL().utf8_str());
        gtk_about_dialog_set_website_label
        (
            dlg,
            info.GetWebSiteDescription().utf8_str()
        );
    }
    else
    {
        gtk_about_dialog_set_website(dlg, nullptr);
        gtk_about_dialog_set_website_label(dlg, nullptr);
#ifdef __WXGTK3__
        g_signal_connect(dlg, "activate-link", G_CALLBACK(activate_link), nullptr);
#else
        gtk_about_dialog_set_url_hook(nullptr, nullptr, nullptr);
#endif
    }

    if ( info.HasDevelopers() )
        gtk_about_dialog_set_authors(dlg, GtkArray(info.GetDevelopers()));
    else
        gtk_about_dialog_set_authors(dlg, GtkArray());
    if ( info.HasDocWriters() )
        gtk_about_dialog_set_documenters(dlg, GtkArray(info.GetDocWriters()));
    else
        gtk_about_dialog_set_documenters(dlg, GtkArray());
    if ( info.HasArtists() )
        gtk_about_dialog_set_artists(dlg, GtkArray(info.GetArtists()));
    else
        gtk_about_dialog_set_artists(dlg, GtkArray());

    wxString transCredits;
    if ( info.HasTranslators() )
    {
        const wxArrayString& translators = info.GetTranslators();
        const size_t count = translators.size();
        for ( size_t n = 0; n < count; n++ )
        {
            transCredits << translators[n] << wxT('\n');
        }
    }
    else // no translators explicitly specified
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
        gtk_about_dialog_set_translator_credits(dlg, transCredits.utf8_str());
    else
        gtk_about_dialog_set_translator_credits(dlg, nullptr);

    g_signal_connect(dlg, "response",
                        G_CALLBACK(wxGtkAboutDialogOnClose), nullptr);

    GtkWindow* gtkParent = nullptr;
    if (parent && parent->m_widget)
        gtkParent = (GtkWindow*)gtk_widget_get_ancestor(parent->m_widget, GTK_TYPE_WINDOW);
    gtk_window_set_transient_for(GTK_WINDOW(dlg), gtkParent);

    gtk_window_present(GTK_WINDOW(dlg));
}

#endif // wxUSE_ABOUTDLG
