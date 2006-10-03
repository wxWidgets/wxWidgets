/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/dirdlg.cpp
// Purpose:     native implementation of wxDirDialog
// Author:      Robert Roebling, Zbigniew Zagorski, Mart Raudsepp, Francesco Montorsi
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, 2004 Zbigniew Zagorski, 2005 Mart Raudsepp
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



/*
  NOTE: the GtkFileChooser interface can be used both for wxFileDialog and for wxDirDialog.
        Thus following code is very similar (even if not identic) to src/gtk/filedlg.cpp
        If you find a problem in this code, remember to check also that file !
*/



#if wxUSE_DIRDLG && defined( __WXGTK24__ )

#include "wx/dirdlg.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/filedlg.h"
#endif

#include "wx/gtk/private.h"

#include <unistd.h> // chdir

//-----------------------------------------------------------------------------
// "clicked" for OK-button
//-----------------------------------------------------------------------------

extern "C" {
static void gtk_dirdialog_ok_callback(GtkWidget *widget, wxDirDialog *dialog)
{
    // change to the directory where the user went if asked
    if (dialog->HasFlag(wxDD_CHANGE_DIR))
    {
        wxGtkString filename(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget)));
        chdir(filename);
    }

    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, wxID_OK);
    event.SetEventObject(dialog);
    dialog->GetEventHandler()->ProcessEvent(event);
}
}

//-----------------------------------------------------------------------------
// "clicked" for Cancel-button
//-----------------------------------------------------------------------------

extern "C" {
static void gtk_dirdialog_cancel_callback(GtkWidget *WXUNUSED(w),
                                           wxDirDialog *dialog)
{
    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, wxID_CANCEL);
    event.SetEventObject(dialog);
    dialog->GetEventHandler()->ProcessEvent(event);
}
}

extern "C" {
static void gtk_dirdialog_response_callback(GtkWidget *w,
                                             gint response,
                                             wxDirDialog *dialog)
{
    wxapp_install_idle_handler();

    if (response == GTK_RESPONSE_ACCEPT)
        gtk_dirdialog_ok_callback(w, dialog);
    else // GTK_RESPONSE_CANCEL or GTK_RESPONSE_NONE
        gtk_dirdialog_cancel_callback(w, dialog);
}
}

//-----------------------------------------------------------------------------
// wxDirDialog
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxDirDialog,wxGenericDirDialog)

BEGIN_EVENT_TABLE(wxDirDialog,wxGenericDirDialog)
    EVT_BUTTON(wxID_OK, wxDirDialog::OnFakeOk)
END_EVENT_TABLE()

wxDirDialog::wxDirDialog(wxWindow* parent, const wxString& title,
                        const wxString& defaultPath, long style,
                        const wxPoint& pos, const wxSize& sz,
                        const wxString& name)
{
    if (!gtk_check_version(2,4,0))
    {
        m_message = title;
        m_needParent = false;

        if (!PreCreation(parent, pos, wxDefaultSize) ||
            !CreateBase(parent, wxID_ANY, pos, wxDefaultSize, style,
                    wxDefaultValidator, wxT("dirdialog")))
        {
            wxFAIL_MSG( wxT("wxDirDialog creation failed") );
            return;
        }

        GtkFileChooserAction gtk_action;
        GtkWindow* gtk_parent = NULL;
        if (parent)
            gtk_parent = GTK_WINDOW( gtk_widget_get_toplevel(parent->m_widget) );

        if (HasFlag(wxDD_DIR_MUST_EXIST))
        gtk_action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
        else
            gtk_action = GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER;

        m_widget = gtk_file_chooser_dialog_new(
                       wxGTK_CONV(m_message),
                       gtk_parent,
                       gtk_action,
                       GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                       GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                       NULL);

        gtk_dialog_set_default_response(GTK_DIALOG(m_widget), GTK_RESPONSE_ACCEPT);

        // gtk_widget_hide_on_delete is used here to avoid that Gtk automatically destroys
        // the dialog when the user press ESC on the dialog: in that case a second call to
        // ShowModal() would result in a bunch of Gtk-CRITICAL errors...
        g_signal_connect (G_OBJECT(m_widget),
                        "delete_event",
                        G_CALLBACK (gtk_widget_hide_on_delete),
                        (gpointer)this);

        // local-only property could be set to false to allow non-local files to be loaded.
        // In that case get/set_uri(s) should be used instead of get/set_filename(s) everywhere
        // and the GtkFileChooserDialog should probably also be created with a backend,
        // e.g "gnome-vfs", "default", ... (gtk_file_chooser_dialog_new_with_backend).
        // Currently local-only is kept as the default - true:
        // gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER(m_widget), true);

        g_signal_connect (m_widget, "response",
            G_CALLBACK (gtk_dirdialog_response_callback), this);

        if ( !defaultPath.empty() )
            gtk_file_chooser_set_current_folder( GTK_FILE_CHOOSER(m_widget),
                    wxConvFileName->cWX2MB(defaultPath) );
    }
    else
        wxGenericDirDialog::Create(parent, title, defaultPath, style, pos, sz, name);
}

void wxDirDialog::OnFakeOk( wxCommandEvent &event )
{
    if (!gtk_check_version(2,4,0))
        EndDialog(wxID_OK);
    else
        wxGenericDirDialog::OnOK( event );
}

int wxDirDialog::ShowModal()
{
    if (!gtk_check_version(2,4,0))
        return wxDialog::ShowModal();
    else
        return wxGenericDirDialog::ShowModal();
}

bool wxDirDialog::Show( bool show )
{
    if (!gtk_check_version(2,4,0))
        return wxDialog::Show( show );
    else
        return wxGenericDirDialog::Show( show );
}

void wxDirDialog::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    if (!m_wxwindow)
        return;
    else
        wxGenericDirDialog::DoSetSize( x, y, width, height, sizeFlags );
}

void wxDirDialog::SetPath(const wxString& dir)
{
    if (!gtk_check_version(2,4,0))
    {
        if (wxDirExists(dir))
        {
            gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(m_widget), wxConvFileName->cWX2MB(dir));
        }
    }
    else
        wxGenericDirDialog::SetPath( dir );
}

wxString wxDirDialog::GetPath() const
{
    if (!gtk_check_version(2,4,0))
    {
        wxGtkString str(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(m_widget)));
        return wxConvFileName->cMB2WX(str);
    }

    return wxGenericDirDialog::GetPath();
}

#endif // wxUSE_DIRDLG
