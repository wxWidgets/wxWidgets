/////////////////////////////////////////////////////////////////////////////
// Name:        filedlgg.h
// Purpose:     wxFileDialog
// Author:      Robert Roebling
// Modified by:
// Created:     8/17/99
// Copyright:   (c) Robert Roebling
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FILEDLGG_H_
#define _WX_FILEDLGG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "filedlgg.h"
#endif

#include "wx/dialog.h"

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

WXDLLEXPORT_DATA(extern const wxChar *)wxFileSelectorPromptStr;
WXDLLEXPORT_DATA(extern const wxChar *)wxFileSelectorDefaultWildcardStr;

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxCheckBox;
class wxChoice;
class wxFileData;
class wxFileCtrl;
class wxFileDialog;
class wxListEvent;
class wxStaticText;
class wxTextCtrl;

//-------------------------------------------------------------------------
// File selector
//-------------------------------------------------------------------------

class wxFileDialog: public wxDialog
{
public:
    wxFileDialog() { }

    wxFileDialog(wxWindow *parent,
                 const wxString& message = wxFileSelectorPromptStr,
                 const wxString& defaultDir = _T(""),
                 const wxString& defaultFile = _T(""),
                 const wxString& wildCard = wxFileSelectorDefaultWildcardStr,
                 long style = 0,
                 const wxPoint& pos = wxDefaultPosition);
    virtual ~wxFileDialog();

    void SetMessage(const wxString& message) { SetTitle(message); }
    void SetPath(const wxString& path);
    void SetDirectory(const wxString& dir) { m_dir = dir; }
    void SetFilename(const wxString& name) { m_fileName = name; }
    void SetWildcard(const wxString& wildCard) { m_wildCard = wildCard; }
    void SetStyle(long style) { m_dialogStyle = style; }
    void SetFilterIndex(int filterIndex);

    wxString GetMessage() const { return m_message; }
    wxString GetPath() const { return m_path; }
    wxString GetDirectory() const { return m_dir; }
    wxString GetFilename() const { return m_fileName; }
    wxString GetWildcard() const { return m_wildCard; }
    long GetStyle() const { return m_dialogStyle; }
    int GetFilterIndex() const { return m_filterIndex; }

    // for multiple file selection
    void GetPaths(wxArrayString& paths) const;
    void GetFilenames(wxArrayString& files) const;

    // implementation only from now on
    // -------------------------------

    virtual int ShowModal();

    void OnSelected( wxListEvent &event );
    void OnActivated( wxListEvent &event );
    void OnList( wxCommandEvent &event );
    void OnReport( wxCommandEvent &event );
    void OnUp( wxCommandEvent &event );
    void OnHome( wxCommandEvent &event );
    void OnListOk( wxCommandEvent &event );
    void OnNew( wxCommandEvent &event );
    void OnChoiceFilter( wxCommandEvent &event );
    void OnTextEnter( wxCommandEvent &event );
    void OnTextChange( wxCommandEvent &event );
    void OnCheck( wxCommandEvent &event );

    void HandleAction( const wxString &fn );

protected:
    // use the filter with the given index
    void DoSetFilterIndex(int filterindex);

    wxString       m_message;
    long           m_dialogStyle;
    wxString       m_dir;
    wxString       m_path; // Full path
    wxString       m_fileName;
    wxString       m_wildCard;
    int            m_filterIndex;
    wxString       m_filterExtension;
    wxChoice      *m_choice;
    wxTextCtrl    *m_text;
    wxFileCtrl    *m_list;
    wxCheckBox    *m_check;
    wxStaticText  *m_static;

private:
    DECLARE_DYNAMIC_CLASS(wxFileDialog)
    DECLARE_EVENT_TABLE()

    // these variables are preserved between wxFileDialog calls
    static long ms_lastViewStyle;     // list or report?
    static bool ms_lastShowHidden;    // did we show hidden files?
};

// File selector - backward compatibility
WXDLLEXPORT wxString
wxFileSelector(const wxChar *message = wxFileSelectorPromptStr,
               const wxChar *default_path = NULL,
               const wxChar *default_filename = NULL,
               const wxChar *default_extension = NULL,
               const wxChar *wildcard = wxFileSelectorDefaultWildcardStr,
               int flags = 0,
               wxWindow *parent = NULL,
               int x = -1, int y = -1);

// An extended version of wxFileSelector
WXDLLEXPORT wxString
wxFileSelectorEx(const wxChar *message = wxFileSelectorPromptStr,
                 const wxChar *default_path = NULL,
                 const wxChar *default_filename = NULL,
                 int *indexDefaultExtension = NULL,
                 const wxChar *wildcard = wxFileSelectorDefaultWildcardStr,
                 int flags = 0,
                 wxWindow *parent = NULL,
                 int x = -1, int y = -1);

// Ask for filename to load
WXDLLEXPORT wxString
wxLoadFileSelector(const wxChar *what,
                   const wxChar *extension,
                   const wxChar *default_name = (const wxChar *)NULL,
                   wxWindow *parent = (wxWindow *) NULL);

// Ask for filename to save
WXDLLEXPORT wxString
wxSaveFileSelector(const wxChar *what,
                   const wxChar *extension,
                   const wxChar *default_name = (const wxChar *) NULL,
                   wxWindow *parent = (wxWindow *) NULL);



#endif
    // _WX_DIRDLGG_H_


