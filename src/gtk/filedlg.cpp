/////////////////////////////////////////////////////////////////////////////
// Name:        gtk/filedlg.cpp
// Purpose:     native implementation of wxFileDialog
// Author:      Robert Roebling, Zbigniew Zagorski
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, 2004 Zbigniew Zagorski
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "filedlg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_FILEDLG

#include "wx/filedlg.h"
#include "wx/utils.h"
#include "wx/intl.h"
#include "wx/filename.h"
#include "wx/msgdlg.h"

#include <gtk/gtk.h>

#ifdef __WXGTK24__
#include "wx/gtk/private.h"

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

//-----------------------------------------------------------------------------
// "clicked" for OK-button
//-----------------------------------------------------------------------------

static void gtk_filedialog_ok_callback(GtkWidget *widget, wxFileDialog *dialog)
{
    int style = dialog->GetStyle();
    gchar* text = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget));
    wxString filename(wxGTK_CONV_BACK(text));

    if ((style & wxSAVE) && (style & wxOVERWRITE_PROMPT))
    {
        if (wxFileExists(filename))
        {
            wxString msg;
            msg.Printf(
             _("File '%s' already exists, do you really want to overwrite it?"),
             filename.c_str());

            wxMessageDialog dlg(dialog, msg, _("Confirm"),
                               wxYES_NO | wxICON_QUESTION);
            if (dlg.ShowModal() != wxID_YES)
                return;
        }
    }
    else if ((style & wxOPEN) && ( style & wxFILE_MUST_EXIST))
    {
        if (!wxFileExists( filename ))
        {
            wxMessageDialog dlg(dialog,
                                _("Please choose an existing file."),
                                _("Error"), wxOK | wxICON_ERROR);
            dlg.ShowModal();

            return;
        }
    }

    // change to the directory where the user went if asked
    if (style & wxCHANGE_DIR)
    {
        wxString cwd;
        wxSplitPath(filename, &cwd, NULL, NULL);

        if (cwd != wxGetCwd())
        {
            wxSetWorkingDirectory(cwd);
        }
    }

    dialog->SetPath(filename);
    dialog->UpdateFromDialog();
    
    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, wxID_OK);
    event.SetEventObject(dialog);
    dialog->GetEventHandler()->ProcessEvent(event);
}

//-----------------------------------------------------------------------------
// "clicked" for Cancel-button
//-----------------------------------------------------------------------------

static void gtk_filedialog_cancel_callback(GtkWidget *WXUNUSED(w),
                                           wxFileDialog *dialog)
{
    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, wxID_CANCEL);
    event.SetEventObject(dialog);
    dialog->GetEventHandler()->ProcessEvent(event);
}

static void gtk_filedialog_response_callback(GtkWidget *w,
                                             int response,
                                             wxFileDialog *dialog)
{
    wxapp_install_idle_handler();
    
    if (response == GTK_RESPONSE_ACCEPT)
        gtk_filedialog_ok_callback(w, dialog);
    else if (response == GTK_RESPONSE_CANCEL)
        gtk_filedialog_cancel_callback(w, dialog);
    else // "delete"
    {
        gtk_filedialog_cancel_callback(w, dialog);
        dialog->m_destroyed_by_delete = TRUE;
    }
}
#endif

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
                           long style, const wxPoint& pos)
    : wxGenericFileDialog(parent, message, defaultDir, defaultFileName,
                       wildCard, style, pos, true )
{
#ifdef __WXGTK24__
    m_needParent = FALSE;
    m_destroyed_by_delete = FALSE;

    if (!PreCreation(parent, pos, wxDefaultSize) ||
        !CreateBase(parent, wxID_ANY, pos, wxDefaultSize, style,
                    wxDefaultValidator, wxT("filedialog")))
    {
        wxFAIL_MSG( wxT("wxFileDialog creation failed") );
        return;
    }

    bool multiple = (style & wxMULTIPLE) == wxMULTIPLE;
    GtkFileChooserAction gtk_action;
    GtkWindow* gtk_parent = NULL;
    if (parent)
        gtk_parent = GTK_WINDOW(parent->m_widget);
    
    gchar* ok_btn_stock;
    if ((style & wxSAVE) == wxSAVE)
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

    gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(m_widget), multiple);
        
    gtk_signal_connect(GTK_OBJECT(m_widget),
                             "response", 
                             GTK_SIGNAL_FUNC(gtk_filedialog_response_callback),
                             (gpointer*)this);
    
    m_path = m_dir;
    if (!m_path.empty() && m_path.Last() != wxT('/'))
        m_path += wxT('/');
    m_path += m_fileName;
    SetPath(m_path);
    
    SetWildcard(wildCard);
    SetFilterIndex(0);
#else
    wxGenericFileDialog::Create( parent, message, defaultDir, defaultFileName, wildCard, style, pos );
#endif
}

wxFileDialog::~wxFileDialog()
{
#ifdef __WXGTK24__
    if (m_destroyed_by_delete)
        m_widget = NULL;
#endif
}

void wxFileDialog::OnFakeOk( wxCommandEvent &event )
{
#ifdef __WXGTK24__
    wxDialog::OnOK( event );
#else
    wxGenericFileDialog::OnListOk( event );
#endif
}

int wxFileDialog::ShowModal()
{
#ifdef __WXGTK24__
    return wxDialog::ShowModal();
#else
    return wxGenericFileDialog::ShowModal();
#endif
}

bool wxFileDialog::Show( bool show )
{
#ifdef __WXGTK24__
    return wxDialog::Show( show );
#else
    return wxGenericFileDialog::Show( show );
#endif
}

void wxFileDialog::GetFilenames(wxArrayString& files) const 
{
#ifdef __WXGTK24__
    GetPaths(files);
    for (size_t n = 0; n < files.GetCount(); n++ )
    {
        wxString name,ext;
        wxSplitPath(files[n], NULL, &name, &ext);
        if (!ext.IsEmpty())
        {
            name += wxT(".");
            name += ext;
        }
        files[n] = name;
    }
#else
    wxGenericFileDialog::GetFilenames( files );
#endif
}

void wxFileDialog::GetPaths(wxArrayString& paths) const 
{
#ifdef __WXGTK24__
    paths.Empty(); 
    if (GetWindowStyle() & wxMULTIPLE)
    {
        GSList *gpathsi = 
            gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(m_widget));
        GSList *gpaths = gpathsi;
        while (gpathsi)
        {
            wxString file = wxGTK_CONV_BACK((gchar*) gpathsi->data);
            paths.Add(file);
            g_free(gpathsi->data);
            gpathsi = gpathsi->next;
        }
        if (gpaths) 
            g_slist_free(gpaths);
    }
    else
    {
        paths.Add(m_fileName);
    }
#else
    wxGenericFileDialog::GetPaths( paths );
#endif
}

void wxFileDialog::SetMessage(const wxString& message)
{
#ifdef __WXGTK24__
    m_message = message;
    SetTitle(message);
#else
    wxGenericFileDialog::SetMessage( message );
#endif    
}

void wxFileDialog::SetPath(const wxString& path)
{
#ifdef __WXGTK24__
    if (path.empty()) return;

    wxFileName fn(path);
    m_path = fn.GetFullPath();
    m_dir = fn.GetPath();
    m_fileName = fn.GetFullName();
    UpdateDialog();
#else
    wxGenericFileDialog::SetPath( path );
#endif    
}

void wxFileDialog::SetDirectory(const wxString& dir)
{
#ifdef __WXGTK24__
    if (wxDirExists(dir))
    {
        m_dir = dir;
        m_path = wxFileName(m_dir, m_fileName).GetFullPath();
        UpdateDialog();
    }
#else
    wxGenericFileDialog::SetDirectory( dir );
#endif    
}

void wxFileDialog::SetFilename(const wxString& name)
{
#ifdef __WXGTK24__
    m_fileName = name;
    m_path = wxFileName(m_dir, m_fileName).GetFullPath();
    UpdateDialog();
#else
    wxGenericFileDialog::SetFilename( name );
#endif
}

void wxFileDialog::SetWildcard(const wxString& wildCard)
{
#ifdef __WXGTK24__
    m_wildCard = wildCard;
    
    GtkFileChooser* chooser = GTK_FILE_CHOOSER(m_widget);
    
    // empty current filter list:
    GSList* ifilters = gtk_file_chooser_list_filters(chooser);
    GSList* filters = ifilters;
    while (ifilters)
    {
        gtk_file_chooser_remove_filter(chooser,GTK_FILE_FILTER(ifilters->data));
        ifilters = ifilters->next;
    }
    g_slist_free(filters);
    
    // parse filters
    wxArrayString wildDescriptions, wildFilters;
    if (!wxParseCommonDialogsFilter(m_wildCard, wildDescriptions, wildFilters))
    {
        wxFAIL_MSG( wxT("Wrong file type description") );
    }
    else
    {
        // add parsed to GtkChooser
        for (size_t n = 0; n < wildFilters.GetCount(); n++)
        {
            GtkFileFilter* filter = gtk_file_filter_new();
            gtk_file_filter_set_name(filter,wxGTK_CONV(wildDescriptions[n]));
            wxString after = wildFilters[n];
            do
            {
                wxString ext = after.BeforeFirst(wxT(';'));
                gtk_file_filter_add_pattern(filter,wxGTK_CONV(ext));
                if (after.Find(wxT(';')) == wxNOT_FOUND)
                    break;
                after = after.AfterLast(wxT(';'));
            }
            while (!after.empty());
            
            gtk_file_chooser_add_filter(chooser, filter);
        }
    }
#else
    wxGenericFileDialog::SetWildcard( wildCard );
#endif
}

void wxFileDialog::SetFilterIndex(int filterIndex)
{
#ifdef __WXGTK24__
    m_filterIndex = filterIndex;

    GtkFileChooser *chooser = GTK_FILE_CHOOSER(m_widget);
    GSList *fnode = gtk_file_chooser_list_filters(chooser);
    GSList *filters = fnode;
    int i = 0;
    while (fnode)
    {
        if (i == filterIndex)
        {
            gtk_file_chooser_set_filter(chooser, GTK_FILE_FILTER(fnode->data));
            m_filterIndex = i;
            break;
        }
        i++;
        fnode = fnode->next;
    }
    g_slist_free(filters);
#else
    wxGenericFileDialog::SetFilterIndex( filterIndex );
#endif
}

void wxFileDialog::UpdateDialog()
{
#ifdef __WXGTK24__
    // set currently selected directory to match the path:
    if (!m_dir.empty() && wxDirExists(m_dir))
    {
        // NB: This is important -- if we set directory only and not the path,
        //     then dialog will still remember old path set using previous
        //     call to gtk_chooser_set_filename. If the previous directory
        //     was a subdirectory of the directory we want to select now,
        //     the dialog would still contain directory selector controls
        //     for the subdirectory (with the parent directory selected),
        //     instead of showing only the parent directory as expected.
        //     This way, we force GtkFileChooser to really change the
        //     directory. Finally, it doesn't have to be done if filename
        //     is not empty because of the code that sets the filename below.
        if (m_fileName.empty())
            gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(m_widget),
                                          wxGTK_CONV(m_dir));
        
        gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(m_widget),
                                            wxGTK_CONV(m_dir));
    }
    
    // if the user set only the directory (e.g. by calling SetDirectory)
    // and not the default filename, then we don't want to set the filename:
    if (!m_fileName.empty())
    {
        gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(m_widget),
                                      wxGTK_CONV(m_path));
                                          
        // pre-fill the filename when saving, too (there's no text entry
        // control when opening a file, so it doesn't make sense to
        // do this when opening files):
        if (GetWindowStyle() & wxSAVE)
        {
            gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(m_widget),
                                              wxGTK_CONV(m_fileName));
        }
    }
#endif
}

void wxFileDialog::UpdateFromDialog()
{
#ifdef __WXGTK24__
    // update filterIndex
    GSList *fnode = gtk_file_chooser_list_filters(GTK_FILE_CHOOSER(m_widget));
    GSList *filters = fnode;
    GtkFileFilter *current =
        gtk_file_chooser_get_filter(GTK_FILE_CHOOSER(m_widget));
    
    int i = 0;
    m_filterIndex = 0;
    while (fnode)
    {
        if (fnode->data == (gpointer)current)
        {
            m_filterIndex = i;
            break;
        }
        i++;
        fnode = fnode->next;
    }
    g_slist_free(filters);
#endif
}

#endif // wxUSE_FILEDLG
