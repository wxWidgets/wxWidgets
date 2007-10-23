/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/filedlg.cpp
// Purpose:     native implementation of wxFileDialog
// Author:      Robert Roebling, Zbigniew Zagorski, Mart Raudsepp
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, 2004 Zbigniew Zagorski, 2005 Mart Raudsepp
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_FILEDLG && defined(__WXGTK24__)

#include "wx/filedlg.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/msgdlg.h"
#endif

#include <gtk/gtk.h>
#include "wx/gtk/private.h"

#include <unistd.h> // chdir

#include "wx/filename.h" // wxFilename
#include "wx/tokenzr.h" // wxStringTokenizer
#include "wx/filefn.h" // ::wxGetCwd

//-----------------------------------------------------------------------------
// "clicked" for OK-button
//-----------------------------------------------------------------------------

extern "C" {
static void gtk_filedialog_ok_callback(GtkWidget *widget, wxFileDialog *dialog)
{
    int style = dialog->GetWindowStyle();
    wxGtkString filename(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget)));

    // gtk version numbers must be identical with the one in ctor (that calls set_do_overwrite_confirmation)
#if GTK_CHECK_VERSION(2,7,3)
    if(gtk_check_version(2,7,3) != NULL)
#endif
    if ((style & wxFD_SAVE) && (style & wxFD_OVERWRITE_PROMPT))
    {
        if ( g_file_test(filename, G_FILE_TEST_EXISTS) )
        {
            wxString msg;

            msg.Printf(
                _("File '%s' already exists, do you really want to overwrite it?"),
                wxString(filename, *wxConvFileName));

            wxMessageDialog dlg(dialog, msg, _("Confirm"),
                               wxYES_NO | wxICON_QUESTION);
            if (dlg.ShowModal() != wxID_YES)
                return;
        }
    }

    if (style & wxFD_FILE_MUST_EXIST)
    {
        if ( !g_file_test(filename, G_FILE_TEST_EXISTS) )
        {
            wxMessageDialog dlg( dialog, _("Please choose an existing file."),
                                 _("Error"), wxOK| wxICON_ERROR);
            dlg.ShowModal();
            return;
        }
    }

    // change to the directory where the user went if asked
    if (style & wxFD_CHANGE_DIR)
    {
        // Use chdir to not care about filename encodings
        wxGtkString folder(g_path_get_dirname(filename));
        chdir(folder);
    }

    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, wxID_OK);
    event.SetEventObject(dialog);
    dialog->GetEventHandler()->ProcessEvent(event);
}
}

//-----------------------------------------------------------------------------
// "clicked" for Cancel-button
//-----------------------------------------------------------------------------

extern "C"
{

static void
gtk_filedialog_cancel_callback(GtkWidget * WXUNUSED(w), wxFileDialog *dialog)
{
    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, wxID_CANCEL);
    event.SetEventObject(dialog);
    dialog->GetEventHandler()->ProcessEvent(event);
}

static void gtk_filedialog_response_callback(GtkWidget *w,
                                             gint response,
                                             wxFileDialog *dialog)
{
    if (response == GTK_RESPONSE_ACCEPT)
        gtk_filedialog_ok_callback(w, dialog);
    else    // GTK_RESPONSE_CANCEL or GTK_RESPONSE_NONE
        gtk_filedialog_cancel_callback(w, dialog);
}

static void gtk_filedialog_update_preview_callback(GtkFileChooser *chooser,
                                                   gpointer user_data)
{
#if GTK_CHECK_VERSION(2,4,0)
    GtkWidget *preview = GTK_WIDGET(user_data);

    wxGtkString filename(gtk_file_chooser_get_preview_filename(chooser));

    if ( !filename )
        return;

    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_size(filename, 128, 128, NULL);
    gboolean have_preview = pixbuf != NULL;

    gtk_image_set_from_pixbuf(GTK_IMAGE(preview), pixbuf);
    if ( pixbuf )
        g_object_unref (pixbuf);

    gtk_file_chooser_set_preview_widget_active(chooser, have_preview);
#else
    wxUnusedVar(chooser);
    wxUnusedVar(user_data);
#endif // GTK+ 2.4+
}

} // extern "C"


//-----------------------------------------------------------------------------
// wxFileDialog
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxFileDialog,wxGenericFileDialog)

BEGIN_EVENT_TABLE(wxFileDialog,wxGenericFileDialog)
    EVT_BUTTON(wxID_OK, wxFileDialog::OnFakeOk)
END_EVENT_TABLE()

wxFileDialog::wxFileDialog(wxWindow *parent, const wxString& message,
                           const wxString& defaultDir,
                           const wxString& defaultFileName,
                           const wxString& wildCard,
                           long style, const wxPoint& pos,
                           const wxSize& sz,
                           const wxString& name)
    : wxGenericFileDialog(parent, message, defaultDir, defaultFileName,
                          wildCard, style, pos, sz, name, true )
{
    if (gtk_check_version(2,4,0))
    {
        wxGenericFileDialog::Create( parent, message, defaultDir,
                                     defaultFileName, wildCard, style, pos );
        return;
    }

    parent = GetParentForModalDialog(parent);

    if (!PreCreation(parent, pos, wxDefaultSize) ||
        !CreateBase(parent, wxID_ANY, pos, wxDefaultSize, style,
                wxDefaultValidator, wxT("filedialog")))
    {
        wxFAIL_MSG( wxT("wxFileDialog creation failed") );
        return;
    }

    GtkFileChooserAction gtk_action;
    GtkWindow* gtk_parent = NULL;
    if (parent)
        gtk_parent = GTK_WINDOW( gtk_widget_get_toplevel(parent->m_widget) );

    const gchar* ok_btn_stock;
    if ( style & wxFD_SAVE )
    {
        gtk_action = GTK_FILE_CHOOSER_ACTION_SAVE;
        ok_btn_stock = GTK_STOCK_SAVE;
    }
    else
    {
        gtk_action = GTK_FILE_CHOOSER_ACTION_OPEN;
        ok_btn_stock = GTK_STOCK_OPEN;
    }

    m_widget = gtk_file_chooser_dialog_new(
                   wxGTK_CONV(m_message),
                   gtk_parent,
                   gtk_action,
                   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                   ok_btn_stock, GTK_RESPONSE_ACCEPT,
                   NULL);

    m_fc.SetWidget( GTK_FILE_CHOOSER(m_widget) );

    gtk_dialog_set_default_response(GTK_DIALOG(m_widget), GTK_RESPONSE_ACCEPT);

    if ( style & wxFD_MULTIPLE )
        gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(m_widget), true);

    // gtk_widget_hide_on_delete is used here to avoid that Gtk automatically
    // destroys the dialog when the user press ESC on the dialog: in that case
    // a second call to ShowModal() would result in a bunch of Gtk-CRITICAL
    // errors...
    g_signal_connect (G_OBJECT(m_widget),
                    "delete_event",
                    G_CALLBACK (gtk_widget_hide_on_delete),
                    (gpointer)this);

    // local-only property could be set to false to allow non-local files to be
    // loaded. In that case get/set_uri(s) should be used instead of
    // get/set_filename(s) everywhere and the GtkFileChooserDialog should
    // probably also be created with a backend, e.g "gnome-vfs", "default", ...
    // (gtk_file_chooser_dialog_new_with_backend). Currently local-only is kept
    // as the default - true:
    // gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER(m_widget), true);

    g_signal_connect (m_widget, "response",
        G_CALLBACK (gtk_filedialog_response_callback), this);

    SetWildcard(wildCard);

    // if defaultDir is specified it should contain the directory and
    // defaultFileName should contain the default name of the file, however if
    // directory is not given, defaultFileName contains both
    wxFileName fn;
    if ( defaultDir.empty() )
        fn.Assign(defaultFileName);
    else if ( !defaultFileName.empty() )
        fn.Assign(defaultDir, defaultFileName);
    else
        fn.AssignDir(defaultDir);

    // set the initial file name and/or directory
    fn.MakeAbsolute(); // GTK+ needs absolute path
    const wxString dir = fn.GetPath();
    if ( !dir.empty() )
    {
        gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(m_widget),
                                            dir.fn_str());
    }

    const wxString fname = fn.GetFullName();
    if ( style & wxFD_SAVE )
    {
        if ( !fname.empty() )
        {
            gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(m_widget),
                                              fname.fn_str());
        }

#if GTK_CHECK_VERSION(2,7,3)
        if ((style & wxFD_OVERWRITE_PROMPT) && !gtk_check_version(2,7,3))
            gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(m_widget), TRUE);
#endif
    }
    else // wxFD_OPEN
    {
        if ( !fname.empty() )
        {
            gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(m_widget),
                                          fn.GetFullPath().fn_str());
        }
    }

#if GTK_CHECK_VERSION(2,4,0)
    if ( style & wxFD_PREVIEW )
    {
        GtkWidget *previewImage = gtk_image_new();

        gtk_file_chooser_set_preview_widget(GTK_FILE_CHOOSER(m_widget),
                                            previewImage);
        g_signal_connect(m_widget, "update-preview",
                         G_CALLBACK(gtk_filedialog_update_preview_callback),
                         previewImage);
    }
#endif // GTK+ 2.4+
}

void wxFileDialog::OnFakeOk( wxCommandEvent &event )
{
    if (!gtk_check_version(2,4,0))
        EndDialog(wxID_OK);
    else
        wxGenericFileDialog::OnOk( event );
}

int wxFileDialog::ShowModal()
{
    if (!gtk_check_version(2,4,0))
        return wxDialog::ShowModal();
    else
        return wxGenericFileDialog::ShowModal();
}

bool wxFileDialog::Show( bool show )
{
    if (!gtk_check_version(2,4,0))
        return wxDialog::Show( show );
    else
        return wxGenericFileDialog::Show( show );
}

void wxFileDialog::DoSetSize(int x, int y, int width, int height, int sizeFlags )
{
    if (!m_wxwindow)
        return;
    else
        wxGenericFileDialog::DoSetSize( x, y, width, height, sizeFlags );
}

wxString wxFileDialog::GetPath() const
{
    if (!gtk_check_version(2,4,0))
    {
        return m_fc.GetPath();
    }

    return wxGenericFileDialog::GetPath();
}

void wxFileDialog::GetFilenames(wxArrayString& files) const
{
    if (!gtk_check_version(2,4,0))
    {
        m_fc.GetFilenames( files );
    }
    else
        wxGenericFileDialog::GetFilenames( files );
}

void wxFileDialog::GetPaths(wxArrayString& paths) const
{
    if (!gtk_check_version(2,4,0))
    {
        m_fc.GetPaths( paths );
    }
    else
        wxGenericFileDialog::GetPaths( paths );
}

void wxFileDialog::SetMessage(const wxString& message)
{
    if (!gtk_check_version(2,4,0))
    {
        m_message = message;
        SetTitle(message);
    }
    else
        wxGenericFileDialog::SetMessage( message );
}

void wxFileDialog::SetPath(const wxString& path)
{
    if (!gtk_check_version(2,4,0))
    {
        m_fc.SetPath( path );
    }
    else
        wxGenericFileDialog::SetPath( path );
}

void wxFileDialog::SetDirectory(const wxString& dir)
{
    if (!gtk_check_version(2,4,0))
    {
        m_fc.SetDirectory( dir );
    }
    else
        wxGenericFileDialog::SetDirectory( dir );
}

wxString wxFileDialog::GetDirectory() const
{
    if (!gtk_check_version(2,4,0))
    {
        m_fc.GetDirectory();
    }

    return wxGenericFileDialog::GetDirectory();
}

void wxFileDialog::SetFilename(const wxString& name)
{
    if (!gtk_check_version(2,4,0))
    {
        if (HasFdFlag(wxFD_SAVE))
            gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(m_widget), wxGTK_CONV(name));
        else
            SetPath(wxFileName(GetDirectory(), name).GetFullPath());
    }
    else
        wxGenericFileDialog::SetFilename( name );
}

wxString wxFileDialog::GetFilename() const
{
    if (!gtk_check_version(2,4,0))
        return m_fc.GetFilename();
    else
        return wxGenericFileDialog::GetFilename();
}

void wxFileDialog::SetWildcard(const wxString& wildCard)
{
    if (!gtk_check_version(2,4,0))
    {
        m_fc.SetWildcard( wildCard );
    }
    else
        wxGenericFileDialog::SetWildcard( wildCard );
}

void wxFileDialog::SetFilterIndex(int filterIndex)
{

    if (!gtk_check_version(2,4,0))
    {
       m_fc.SetFilterIndex( filterIndex);
    }
    else
        wxGenericFileDialog::SetFilterIndex( filterIndex );
}

int wxFileDialog::GetFilterIndex() const
{
    if (!gtk_check_version(2,4,0))
    {
        return m_fc.GetFilterIndex();
    }
    else
                return wxGenericFileDialog::GetFilterIndex();
}

#endif // wxUSE_FILEDLG &&  __WXGTK24__
