/////////////////////////////////////////////////////////////////////////////
// Name:        filedlg.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "filedlg.h"
#endif

#include "wx/filedlg.h"
#include "wx/utils.h"
#include "wx/intl.h"
#include "wx/generic/msgdlgg.h"

#include "gtk/gtk.h"

//-----------------------------------------------------------------------------
// "delete_event"
//-----------------------------------------------------------------------------

static
bool gtk_filedialog_delete_callback( GtkWidget *WXUNUSED(widget), GdkEvent *WXUNUSED(event), wxDialog *win )
{
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
    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, wxID_CANCEL);
    event.SetEventObject( dialog );
    dialog->GetEventHandler()->ProcessEvent( event );
}

//-----------------------------------------------------------------------------
// wxFileDialog
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxFileDialog,wxDialog)

wxFileDialog::wxFileDialog( wxWindow *parent, const wxString& message,
                            const wxString& defaultDir, const wxString& defaultFileName,
                            const wxString& wildCard,
                            long style, const wxPoint& pos )
{
    m_needParent = FALSE;

    PreCreation( parent, -1, pos, wxDefaultSize, style | wxDIALOG_MODAL, "filedialog" );
    m_message = message;
    m_path = "";
    m_fileName = defaultFileName;
    m_dir = defaultDir;
    m_wildCard = wildCard;
    m_dialogStyle = style;
    m_filterIndex = 1;

    m_widget = gtk_file_selection_new( m_message );

    int x = (gdk_screen_width () - 400) / 2;
    int y = (gdk_screen_height () - 400) / 2;
    gtk_widget_set_uposition( m_widget, x, y );

    GtkFileSelection *sel = GTK_FILE_SELECTION(m_widget);
    gtk_file_selection_hide_fileop_buttons( sel ); // they don't work anyway

    m_path.Append(m_dir);
    if( ! m_path.IsEmpty() && m_path.Last()!='/' )
        m_path.Append('/');
    m_path.Append(m_fileName);

    if(m_path.Length()>1) gtk_file_selection_set_filename(sel,m_path);

    gtk_signal_connect( GTK_OBJECT(sel->ok_button), "clicked",
      GTK_SIGNAL_FUNC(gtk_filedialog_ok_callback), (gpointer*)this );

    // strange way to internationalize
    gtk_label_set( GTK_LABEL( GTK_BUTTON(sel->ok_button)->child ), _("OK") );

    gtk_signal_connect( GTK_OBJECT(sel->cancel_button), "clicked",
      GTK_SIGNAL_FUNC(gtk_filedialog_cancel_callback), (gpointer*)this );
      
    // strange way to internationalize
    gtk_label_set( GTK_LABEL( GTK_BUTTON(sel->cancel_button)->child ), _("Cancel") );
    
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
        m_fileName += ext;
    }
}

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

wxString wxFileSelector( const char *title,
                      const char *defaultDir, const char *defaultFileName,
                      const char *defaultExtension, const char *filter, int flags,
                      wxWindow *parent, int x, int y )
{
    wxString filter2;
    if ( defaultExtension && !filter )
        filter2 = wxString("*.") + wxString(defaultExtension) ;
    else if ( filter )
        filter2 = filter;

    wxString defaultDirString;
    if (defaultDir)
        defaultDirString = defaultDir;

    wxString defaultFilenameString;
    if (defaultFileName)
        defaultFilenameString = defaultFileName;

    wxFileDialog fileDialog( parent, title, defaultDirString, defaultFilenameString, filter2, flags, wxPoint(x, y) );

    if ( fileDialog.ShowModal() == wxID_OK )
    {
        return fileDialog.GetPath();
    }
    else
    {
        return wxEmptyString;
    }
}

wxString wxLoadFileSelector( const char *what, const char *extension, const char *default_name, wxWindow *parent )
{
    char *ext = (char *)extension;

    char prompt[50];
    wxString str = _("Load %s file");
    sprintf(prompt, str, what);

    if (*ext == '.') ext++;
    char wild[60];
    sprintf(wild, "*.%s", ext);

    return wxFileSelector (prompt, (const char *) NULL, default_name, ext, wild, 0, parent);
}

wxString wxSaveFileSelector(const char *what, const char *extension, const char *default_name,
         wxWindow *parent )
{
    char *ext = (char *)extension;

    char prompt[50];
    wxString str = _("Save %s file");
    sprintf(prompt, str, what);

    if (*ext == '.') ext++;
    char wild[60];
    sprintf(wild, "*.%s", ext);

    return wxFileSelector (prompt, (const char *) NULL, default_name, ext, wild, 0, parent);
}

