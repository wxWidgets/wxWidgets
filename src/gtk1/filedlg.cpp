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

IMPLEMENT_DYNAMIC_CLASS(wxFileDialog,wxDialog)

wxFileDialog::wxFileDialog( wxWindow *parent, const wxString& message,
                            const wxString& defaultDir, const wxString& defaultFileName,
                            const wxString& wildCard,
                            long style, const wxPoint& pos )
{
    m_needParent = FALSE;

    if (!PreCreation( parent, pos, wxDefaultSize ) ||
        !CreateBase( parent, -1, pos, wxDefaultSize, style | wxDIALOG_MODAL, wxDefaultValidator, wxT("filedialog") ))
    {
        wxFAIL_MSG( wxT("wxXX creation failed") );
	return;
    }
    
    m_message = message;
    m_path = wxT("");
    m_fileName = defaultFileName;
    m_dir = defaultDir;
    m_wildCard = wildCard;
    m_dialogStyle = style;
    m_filterIndex = 1;

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

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

wxString
wxFileSelectorEx(const wxChar *message,
                 const wxChar *default_path,
                 const wxChar *default_filename,
                 int *indexDefaultExtension,
                 const wxChar *wildcard,
                 int flags,
                 wxWindow *parent,
                 int x, int y)
{
    // TODO: implement this somehow
    return wxFileSelector(message, default_path, default_filename, wxT(""),
                          wildcard, flags, parent, x, y);
}

wxString wxFileSelector( const wxChar *title,
                      const wxChar *defaultDir, const wxChar *defaultFileName,
                      const wxChar *defaultExtension, const wxChar *filter, int flags,
                      wxWindow *parent, int x, int y )
{
    wxString filter2;
    if ( defaultExtension && !filter )
        filter2 = wxString(wxT("*.")) + wxString(defaultExtension) ;
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

wxString wxLoadFileSelector( const wxChar *what, const wxChar *extension, const wxChar *default_name, wxWindow *parent )
{
    wxChar *ext = (wxChar *)extension;

    wxChar prompt[50];
    wxString str = _("Load %s file");
    wxSprintf(prompt, str, what);

    if (*ext == wxT('.')) ext++;
    wxChar wild[60];
    wxSprintf(wild, wxT("*.%s"), ext);

    return wxFileSelector (prompt, (const wxChar *) NULL, default_name, ext, wild, 0, parent);
}

wxString wxSaveFileSelector(const wxChar *what, const wxChar *extension, const wxChar *default_name,
         wxWindow *parent )
{
    wxChar *ext = (wxChar *)extension;

    wxChar prompt[50];
    wxString str = _("Save %s file");
    wxSprintf(prompt, str, what);

    if (*ext == wxT('.')) ext++;
    wxChar wild[60];
    wxSprintf(wild, wxT("*.%s"), ext);

    return wxFileSelector (prompt, (const wxChar *) NULL, default_name, ext, wild, 0, parent);
}

