/////////////////////////////////////////////////////////////////////////////
// Name:        filedlgg.h
// Purpose:     wxFileDialog
// Author:      Robert Roebling
// Modified by:
// Created:     8/17/99
// Copyright:   (c) Robert Roebling
// RCS-ID:      $Id$
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FILEDLGG_H_
#define _WX_FILEDLGG_H_

#ifdef __GNUG__
#pragma interface "filedlgg.h"
#endif

#include "wx/defs.h"

#include "wx/dialog.h"
#include "wx/checkbox.h"
#include "wx/listctrl.h"
#include "wx/textctrl.h"
#include "wx/choice.h"

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

WXDLLEXPORT_DATA(extern const wxChar *)wxFileSelectorPromptStr;
WXDLLEXPORT_DATA(extern const wxChar *)wxFileSelectorDefaultWildcardStr;

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxFileData;
class wxFileCtrl;
class wxFileDialog;

//-----------------------------------------------------------------------------
//  wxFileData
//-----------------------------------------------------------------------------

class wxFileData : public wxObject
{
private:
    wxString m_name;
    wxString m_fileName;
    long     m_size;
    int      m_hour;
    int      m_minute;
    int      m_year;
    int      m_month;
    int      m_day;
    wxString m_permissions;
    bool     m_isDir;
    bool     m_isLink;
    bool     m_isExe;

public:
    wxFileData() {}
    wxFileData( const wxString &name, const wxString &fname );
    wxString GetName() const;
    wxString GetFullName() const;
    wxString GetHint() const;
    wxString GetEntry( const int num );
    bool IsDir();
    bool IsLink();
    bool IsExe();
    long GetSize();
    void MakeItem( wxListItem &item );
    void SetNewName( const wxString &name, const wxString &fname );
    
private:
    DECLARE_DYNAMIC_CLASS(wxFileData);
};

//-----------------------------------------------------------------------------
//  wxFileCtrl
//-----------------------------------------------------------------------------

class wxFileCtrl : public wxListCtrl
{
private:
    wxString      m_dirName;
    bool          m_showHidden;
    wxString      m_wild;

public:
    wxFileCtrl();
    wxFileCtrl( wxWindow *win, const wxWindowID id, 
      const wxString &dirName, const wxString &wild,
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
      const long style = wxLC_LIST, const wxValidator &validator = wxDefaultValidator,
      const wxString &name = _T("filelist") );
    void ChangeToListMode();
    void ChangeToReportMode();
    void ChangeToIconMode();
    void ShowHidden( bool show = TRUE );
    long Add( wxFileData *fd, wxListItem &item );
    void Update();
    virtual void StatusbarText( char *WXUNUSED(text) ) {};
    void MakeDir();
    void GoToParentDir();
    void GoToHomeDir();
    void GoToDir( const wxString &dir );
    void SetWild( const wxString &wild );
    void GetDir( wxString &dir );
    void OnListDeleteItem( wxListEvent &event );
    void OnListEndLabelEdit( wxListEvent &event );

private:    
    DECLARE_DYNAMIC_CLASS(wxFileCtrl);
    DECLARE_EVENT_TABLE()
};

//-------------------------------------------------------------------------
// File selector
//-------------------------------------------------------------------------

class wxFileDialog: public wxDialog
{
public:
    wxFileDialog() { }

    wxFileDialog(wxWindow *parent,
                 const wxString& message = wxFileSelectorPromptStr,
                 const wxString& defaultDir = "",
                 const wxString& defaultFile = "",
                 const wxString& wildCard = wxFileSelectorDefaultWildcardStr,
                 long style = 0,
                 const wxPoint& pos = wxDefaultPosition);
    ~wxFileDialog();

    void SetMessage(const wxString& message) { m_message = message; }
    void SetPath(const wxString& path);
    void SetDirectory(const wxString& dir) { m_dir = dir; }
    void SetFilename(const wxString& name) { m_fileName = name; }
    void SetWildcard(const wxString& wildCard) { m_wildCard = wildCard; }
    void SetStyle(long style) { m_dialogStyle = style; }
    void SetFilterIndex(int filterIndex) { m_filterIndex = filterIndex; }

    wxString GetMessage() const { return m_message; }
    wxString GetPath() const { return m_path; }
    wxString GetDirectory() const { return m_dir; }
    wxString GetFilename() const { return m_fileName; }
    wxString GetWildcard() const { return m_wildCard; }
    long GetStyle() const { return m_dialogStyle; }
    int GetFilterIndex() const { return m_filterIndex ; }
    
    void OnSelected( wxListEvent &event );
    void OnActivated( wxListEvent &event );
    void OnList( wxCommandEvent &event );
    void OnReport( wxCommandEvent &event );
    void OnUp( wxCommandEvent &event );
    void OnHome( wxCommandEvent &event );
    void OnListOk( wxCommandEvent &event );
    void OnNew( wxCommandEvent &event );
    void OnChoice( wxCommandEvent &event );
    void OnTextEnter( wxCommandEvent &event );
    
protected:    
    wxString    m_message;
    long        m_dialogStyle;
    wxString    m_dir;
    wxString    m_path; // Full path
    wxString    m_fileName;
    wxString    m_wildCard;
    int         m_filterIndex;
    wxChoice   *m_choice;
    wxTextCtrl *m_text;
    wxFileCtrl *m_list;
    
private:
    DECLARE_DYNAMIC_CLASS(wxFileDialog)
    DECLARE_EVENT_TABLE()
};

#define wxOPEN 1
#define wxSAVE 2
#define wxOVERWRITE_PROMPT 4
#define wxHIDE_READONLY 8
#define wxFILE_MUST_EXIST 16

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

