/////////////////////////////////////////////////////////////////////////////
// Name:        gtk/filedlg.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "filedlg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/filedlg.h"
#include "wx/utils.h"
#include "wx/intl.h"
#include "wx/generic/msgdlgg.h"

#include <gtk/gtk.h>

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

//-----------------------------------------------------------------------------
// "delete_event"
//-----------------------------------------------------------------------------

static
bool gtk_filedialog_delete_callback( GtkWidget *WXUNUSED(widget), GdkEvent *WXUNUSED(event), wxDialog *win )
{
    if (g_isIdle) wxapp_install_idle_handler();

/*
    printf( "OnDelete from " );
    if (win->GetClassInfo() && win->GetClassInfo()->GetClassName())
        printf( win->GetClassInfo()->GetClassName() );
    printf( ".\n" );
*/

    win->Close();

    return TRUE;
}

//-----------------------------------------------------------------------------
// "clicked" for OK-button
//-----------------------------------------------------------------------------

static
void gtk_filedialog_ok_callback( GtkWidget *WXUNUSED(widget), wxFileDialog *dialog )
{
    if (g_isIdle) wxapp_install_idle_handler();

    int style = dialog->GetStyle();

    GtkFileSelection *filedlg = GTK_FILE_SELECTION(dialog->m_widget);
    char *filename = gtk_file_selection_get_filename(filedlg);

    if ( (style & wxSAVE) && ( style & wxOVERWRITE_PROMPT ) )
    {
        if (wxFileExists( filename ))
        {
            wxString msg;
            msg.Printf( _("File '%s' already exists, do you really want to "
                         "overwrite it?"), filename);

            if (wxMessageBox(msg, _("Confirm"), wxYES_NO) != wxYES)
                return;
        }
    }
    else if ( (style & wxOPEN) && ( style & wxFILE_MUST_EXIST) )
    {
        if ( !wxFileExists( filename ) )
        {
            wxMessageBox(_("Please choose an existing file."), _("Error"), wxOK);

            return;
        }
    }

    // change to the directory where the user went if asked
    if ( style & wxCHANGE_DIR )
    {
        wxString cwd;
        wxSplitPath(filename, &cwd, NULL, NULL);

        if ( cwd != wxGetCwd() )
        {
            wxSetWorkingDirectory(cwd);
        }
    }

    dialog->SetPath( filename );

    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, wxID_OK);
    event.SetEventObject( dialog );
    dialog->GetEventHandler()->ProcessEvent( event );
}

//-----------------------------------------------------------------------------
// "clicked" for Cancel-button
//-----------------------------------------------------------------------------

static
void gtk_filedialog_cancel_callback( GtkWidget *WXUNUSED(w), wxFileDialog *dialog )
{
    if (g_isIdle) wxapp_install_idle_handler();

    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, wxID_CANCEL);
    event.SetEventObject( dialog );
    dialog->GetEventHandler()->ProcessEvent( event );
}

//-----------------------------------------------------------------------------
// wxFileDialog
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxFileDialog,wxFileDialogBase)

wxFileDialog::wxFileDialog( wxWindow *parent, const wxString& message,
                            const wxString& defaultDir, const wxString& defaultFileName,
                            const wxString& wildCard,
                            long style, const wxPoint& pos )
             :wxFileDialogBase(parent, message, defaultDir, defaultFileName, wildCard, style, pos)
{
    m_needParent = FALSE;

    if (!PreCreation( parent, pos, wxDefaultSize ) ||
        !CreateBase( parent, wxID_ANY, pos, wxDefaultSize, style, wxDefaultValidator, wxT("filedialog") ))
    {
        wxFAIL_MSG( wxT("wxXX creation failed") );
        return;
    }

    m_widget = gtk_file_selection_new( m_message.mbc_str() );

    int x = (gdk_screen_width () - 400) / 2;
    int y = (gdk_screen_height () - 400) / 2;
    gtk_widget_set_uposition( m_widget, x, y );

    GtkFileSelection *sel = GTK_FILE_SELECTION(m_widget);
    gtk_file_selection_hide_fileop_buttons( sel ); // they don't work anyway

    m_path.Append(m_dir);
    if( ! m_path.IsEmpty() && m_path.Last()!=wxT('/') )
        m_path.Append('/');
    m_path.Append(m_fileName);

    if(m_path.Length()>1) gtk_file_selection_set_filename(sel,m_path.mbc_str());

    gtk_signal_connect( GTK_OBJECT(sel->ok_button), "clicked",
      GTK_SIGNAL_FUNC(gtk_filedialog_ok_callback), (gpointer*)this );

    // strange way to internationalize
    gtk_label_set( GTK_LABEL( GTK_BUTTON(sel->ok_button)->child ), wxConvCurrent->cWX2MB(_("OK")) );

    gtk_signal_connect( GTK_OBJECT(sel->cancel_button), "clicked",
      GTK_SIGNAL_FUNC(gtk_filedialog_cancel_callback), (gpointer*)this );

    // strange way to internationalize
    gtk_label_set( GTK_LABEL( GTK_BUTTON(sel->cancel_button)->child ), wxConvCurrent->cWX2MB(_("Cancel")) );

    gtk_signal_connect( GTK_OBJECT(m_widget), "delete_event",
        GTK_SIGNAL_FUNC(gtk_filedialog_delete_callback), (gpointer)this );
}

void wxFileDialog::SetPath(const wxString& path)
{
    // not only set the full path but also update filename and dir
    m_path = path;
    if ( !!path )
    {
        wxString ext;
        wxSplitPath(path, &m_dir, &m_fileName, &ext);
        if (!ext.IsEmpty())
        {
            m_fileName += wxT(".");
            m_fileName += ext;
        }
    }
}

