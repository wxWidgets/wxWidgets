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

#if wxUSE_FILEDLG && defined(__WXGTK24__)

#include "wx/filedlg.h"
#include "wx/utils.h"
#include "wx/intl.h"
#include "wx/filename.h"

#include <gtk/gtk.h>
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

    dialog->DoSetPath(filename);
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
    
    if (response == GTK_RESPONSE_CANCEL)
        gtk_filedialog_cancel_callback(w, dialog);
    else
        gtk_filedialog_ok_callback(w, dialog);
}

//-----------------------------------------------------------------------------
// wxFileDialog
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxFileDialog,wxFileDialogBase)

wxFileDialog::wxFileDialog(wxWindow *parent, const wxString& message,
                           const wxString& defaultDir,
                           const wxString& defaultFileName,
                           const wxString& wildCard,
                           long style, const wxPoint& pos)
    : wxFileDialogBase(parent, message, defaultDir, defaultFileName,
                       wildCard, style, pos)
{
    m_needParent = FALSE;

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
}

void wxFileDialog::SetPath(const wxString& path)
{
    DoSetPath(path);
    UpdateDialog();
}

void wxFileDialog::GetFilenames(wxArrayString& files) const 
{
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
}
void wxFileDialog::GetPaths(wxArrayString& paths) const 
{
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
}

void wxFileDialog::SetMessage(const wxString& message)
{
    m_message = message;
    SetTitle(message);
}

void wxFileDialog::SetDirectory(const wxString& dir)
{
    wxFileName fn(dir,m_fileName);
    SetPath(fn.GetFullPath());
}

void wxFileDialog::SetFilename(const wxString& name)
{
    m_fileName = name;
    wxFileName fn(m_dir,name);
    SetPath(fn.GetFullPath());
}

void wxFileDialog::SetWildcard(const wxString& wildCard)
{
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
}

void wxFileDialog::SetFilterIndex(int filterIndex)
{
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
}

void wxFileDialog::UpdateFromDialog()
{
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
}

void wxFileDialog::UpdateDialog()
{
    
    if (wxDirExists(m_path))
    {
        gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(m_widget),
                                            wxGTK_CONV(m_path));
    }
    else
    {
        gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(m_widget),
                                      wxGTK_CONV(m_path));

        // pre-fill the filename, too:
        if (GetWindowStyle() & wxSAVE)
        {
            gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(m_widget),
                                              wxGTK_CONV(m_fileName));
        }
    }
}

void wxFileDialog::DoSetPath(const wxString& path)
{
    if (!path.empty())
    {
        wxFileName fn(path);
        fn.MakeAbsolute();
        m_path = fn.GetFullPath();
        
        wxString ext;
        wxSplitPath(path, &m_dir, &m_fileName, &ext);
        if (!ext.empty())
        {
            m_fileName += wxT(".");
            m_fileName += ext;
        }
    }
    else
    {
        m_path = path;
    }
}

#endif // wxUSE_FILEDLG && defined(__WXGTK24__)
