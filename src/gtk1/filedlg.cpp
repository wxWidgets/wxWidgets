/////////////////////////////////////////////////////////////////////////////
// Name:        filedlg.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "filedlg.h"
#endif

#include "wx/filedlg.h"
#include "wx/utils.h"
#include "wx/intl.h"
#include "wx/generic/msgdlgg.h"

//-----------------------------------------------------------------------------
// wxFileDialog
//-----------------------------------------------------------------------------

void gtk_filedialog_ok_callback( GtkWidget *WXUNUSED(widget), gpointer data )
{
  wxFileDialog *dialog = (wxFileDialog*)data;
  wxCommandEvent event(wxEVT_NULL);
  int style;

  style=dialog->GetStyle();

  if((style&wxSAVE)&&(style&wxOVERWRITE_PROMPT))
	if(wxFileExists(gtk_file_selection_get_filename(GTK_FILE_SELECTION(dialog->m_widget) ))) {
	  if(wxMessageBox("File exists. Overwrite?","Confirm",wxYES_NO)!=wxYES)
		return;
	}

  dialog->OnOk( event );
};

void gtk_filedialog_cancel_callback( GtkWidget *WXUNUSED(widget), gpointer data )
{
  wxFileDialog *dialog = (wxFileDialog*)data;
  wxCommandEvent event(wxEVT_NULL);
  dialog->OnCancel( event );
};

IMPLEMENT_DYNAMIC_CLASS(wxFileDialog,wxDialog)

wxFileDialog::wxFileDialog(wxWindow *parent, const wxString& message,
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
  
  m_widget = gtk_file_selection_new( "File selection" );
  
  int x = (gdk_screen_width () - 400) / 2;
  int y = (gdk_screen_height () - 400) / 2;
  gtk_widget_set_uposition( m_widget, x, y );
  
  GtkFileSelection *sel = GTK_FILE_SELECTION(m_widget);

  m_path.Append(m_dir);
  if(! m_path.IsEmpty() && m_path.Last()!='/') m_path.Append('/');
  m_path.Append(m_fileName);

  if(m_path.Length()>1) gtk_file_selection_set_filename(sel,m_path);
  
  gtk_signal_connect( GTK_OBJECT(sel->ok_button), "clicked", 
    GTK_SIGNAL_FUNC(gtk_filedialog_ok_callback), (gpointer*)this );

  gtk_signal_connect( GTK_OBJECT(sel->cancel_button), "clicked", 
    GTK_SIGNAL_FUNC(gtk_filedialog_cancel_callback), (gpointer*)this );
};

int wxFileDialog::ShowModal(void)
{
  int ret = wxDialog::ShowModal();

  if (ret == wxID_OK)
  {
    m_fileName = gtk_file_selection_get_filename( GTK_FILE_SELECTION(m_widget) );
    m_path = gtk_file_selection_get_filename( GTK_FILE_SELECTION(m_widget) );
  };
  return ret;
};
    

char *wxFileSelector(const char *title,
                     const char *defaultDir, const char *defaultFileName,
                     const char *defaultExtension, const char *filter, int flags,
                     wxWindow *parent, int x, int y)
{
	wxString filter2("");
	if ( defaultExtension && !filter )
		filter2 = wxString("*.") + wxString(defaultExtension) ;
	else if ( filter )
		filter2 = filter;

	wxString defaultDirString;
	if (defaultDir)
		defaultDirString = defaultDir;
	else
		defaultDirString = "";

	wxString defaultFilenameString;
	if (defaultFileName)
		defaultFilenameString = defaultFileName;
	else
		defaultFilenameString = "";

	wxFileDialog fileDialog(parent, title, defaultDirString, defaultFilenameString, 
	  filter2, flags, wxPoint(x, y));

	if ( fileDialog.ShowModal() == wxID_OK )
	{
		strcpy(wxBuffer, (const char *)fileDialog.GetPath());
		return wxBuffer;
	}
	else
		return NULL;
};

char* wxLoadFileSelector(const char *what, const char *extension, const char *default_name, 
         wxWindow *parent )
{
  char *ext = (char *)extension;
  
  char prompt[50];
  wxString str = _("Load %s file");
  sprintf(prompt, str, what);

  if (*ext == '.') ext++;
  char wild[60];
  sprintf(wild, "*.%s", ext);

  return wxFileSelector (prompt, NULL, default_name, ext, wild, 0, parent);
};

char* wxSaveFileSelector(const char *what, const char *extension, const char *default_name, 
         wxWindow *parent )
{
  char *ext = (char *)extension;
  
  char prompt[50];
  wxString str = _("Save %s file");
  sprintf(prompt, str, what);

  if (*ext == '.') ext++;
  char wild[60];
  sprintf(wild, "*.%s", ext);

  return wxFileSelector (prompt, NULL, default_name, ext, wild, 0, parent);
};




