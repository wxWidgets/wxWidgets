/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/dirdlg.cpp
// Purpose:     native implementation of wxDirDialogGTK
// Author:      Robert Roebling, Zbigniew Zagorski, Mart Raudsepp, Francesco Montorsi
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, 2004 Zbigniew Zagorski, 2005 Mart Raudsepp
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



/*
  NOTE: the GtkFileChooser interface can be used both for wxFileDialog and for wxDirDialogGTK.
        Thus following code is very similar (even if not identic) to src/gtk/filedlg.cpp
        If you find a problem in this code, remember to check also that file !
*/



#if wxUSE_DIRDLG

#include "wx/dirdlg.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/filedlg.h"
#endif

#ifdef __WXGTK24__      // only for GTK+ > 2.4 there is GtkFileChooserDialog

#include <gtk/gtk.h>
#include "wx/gtk/private.h"

#include <unistd.h> // chdir


//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();

//-----------------------------------------------------------------------------
// "clicked" for OK-button
//-----------------------------------------------------------------------------

extern "C" {
static void gtk_filedialog_ok_callback(GtkWidget *widget, wxDirDialogGTK *dialog)
{
    gchar* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget));

    // change to the directory where the user went if asked
    if (dialog->HasFlag(wxDD_CHANGE_DIR))
        chdir(filename);

    g_free(filename);

    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, wxID_OK);
    event.SetEventObject(dialog);
    dialog->GetEventHandler()->ProcessEvent(event);
}
}

//-----------------------------------------------------------------------------
// "clicked" for Cancel-button
//-----------------------------------------------------------------------------

extern "C" {
static void gtk_filedialog_cancel_callback(GtkWidget *WXUNUSED(w),
                                           wxDirDialogGTK *dialog)
{
    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, wxID_CANCEL);
    event.SetEventObject(dialog);
    dialog->GetEventHandler()->ProcessEvent(event);
}
}

extern "C" {
static void gtk_filedialog_response_callback(GtkWidget *w,
                                             gint response,
                                             wxDirDialogGTK *dialog)
{
    wxapp_install_idle_handler();

    if (response == GTK_RESPONSE_ACCEPT)
        gtk_filedialog_ok_callback(w, dialog);
    else if (response == GTK_RESPONSE_CANCEL)
        gtk_filedialog_cancel_callback(w, dialog);
    else // "delete"
    {
        gtk_filedialog_cancel_callback(w, dialog);
        dialog->m_destroyed_by_delete = true;
    }
}
}

#endif // __WXGTK24__

//-----------------------------------------------------------------------------
// wxDirDialogGTK
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxDirDialogGTK,wxGenericDirDialog)

BEGIN_EVENT_TABLE(wxDirDialogGTK,wxGenericDirDialog)
    EVT_BUTTON(wxID_OK, wxDirDialogGTK::OnFakeOk)
END_EVENT_TABLE()

wxDirDialogGTK::wxDirDialogGTK(wxWindow* parent, const wxString& title,
                        const wxString& defaultPath, long style,
                        const wxPoint& pos, const wxSize& sz,
                        const wxString& name)
{
#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
    {
        m_message = title;
        m_needParent = false;
        m_destroyed_by_delete = false;

        if (!PreCreation(parent, pos, wxDefaultSize) ||
            !CreateBase(parent, wxID_ANY, pos, wxDefaultSize, style,
                    wxDefaultValidator, wxT("filedialog")))
        {
            wxFAIL_MSG( wxT("wxDirDialogGTK creation failed") );
            return;
        }

        GtkFileChooserAction gtk_action;
        GtkWindow* gtk_parent = NULL;
        if (parent)
            gtk_parent = GTK_WINDOW( gtk_widget_get_toplevel(parent->m_widget) );

        gtk_action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
        if (style & wxDD_NEW_DIR_BUTTON)
            gtk_action = GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER;

        m_widget = gtk_file_chooser_dialog_new(
                       wxGTK_CONV(m_message),
                       gtk_parent,
                       gtk_action,
                       GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                       GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                       NULL);

        gtk_dialog_set_default_response(GTK_DIALOG(m_widget), GTK_RESPONSE_ACCEPT);

        // local-only property could be set to false to allow non-local files to be loaded.
        // In that case get/set_uri(s) should be used instead of get/set_filename(s) everywhere
        // and the GtkFileChooserDialog should probably also be created with a backend,
        // e.g "gnome-vfs", "default", ... (gtk_file_chooser_dialog_new_with_backend).
        // Currently local-only is kept as the default - true:
        // gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER(m_widget), true);

        g_signal_connect (m_widget, "response",
            G_CALLBACK (gtk_filedialog_response_callback), this);

        if ( !defaultPath.empty() )
            gtk_file_chooser_set_current_folder( GTK_FILE_CHOOSER(m_widget),
                    wxConvFileName->cWX2MB(defaultPath) );
    }
    else
#endif
        wxGenericDirDialog::Create(parent, title, defaultPath, style, pos, sz, name);
}

wxDirDialogGTK::~wxDirDialogGTK()
{
#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
    {
        if (m_destroyed_by_delete)
            m_widget = NULL;
    }
#endif
}

void wxDirDialogGTK::OnFakeOk( wxCommandEvent &event )
{
#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
        wxDialog::OnOK( event );
    else
#endif
        wxGenericDirDialog::OnOK( event );
}

int wxDirDialogGTK::ShowModal()
{
#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
        return wxDialog::ShowModal();
    else
#endif
        return wxGenericDirDialog::ShowModal();
}

bool wxDirDialogGTK::Show( bool show )
{
#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
        return wxDialog::Show( show );
    else
#endif
        return wxGenericDirDialog::Show( show );
}

void wxDirDialogGTK::DoSetSize(int x, int y, int width, int height, int sizeFlags )
{
    if (!m_wxwindow)
        return;
    else
        wxGenericDirDialog::DoSetSize( x, y, width, height, sizeFlags );
}

void wxDirDialogGTK::SetPath(const wxString& dir)
{
#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
    {
        if (wxDirExists(dir))
        {
            gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(m_widget), wxConvFileName->cWX2MB(dir));
        }
    }
    else
#endif
        wxGenericDirDialog::SetPath( dir );
}

wxString wxDirDialogGTK::GetPath() const
{
#ifdef __WXGTK24__
    if (!gtk_check_version(2,4,0))
        return wxConvFileName->cMB2WX( gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(m_widget) ) );
    else
#endif
        return wxGenericDirDialog::GetPath();
}

#endif // wxUSE_DIRDLG
