/////////////////////////////////////////////////////////////////////////////
// Name:        filedlg.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKFILEDLGH__
#define __GTKFILEDLGH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/dialog.h"

//-------------------------------------------------------------------------
// File selector
//-------------------------------------------------------------------------

extern const char *wxFileSelectorPromptStr;
extern const char *wxFileSelectorDefaultWildcardStr;

class wxFileDialog: public wxDialog
{
DECLARE_DYNAMIC_CLASS(wxFileDialog)

public:
    wxFileDialog() { }

    wxFileDialog(wxWindow *parent,
                 const wxString& message = wxFileSelectorPromptStr,
                 const wxString& defaultDir = "",
                 const wxString& defaultFile = "",
                 const wxString& wildCard = wxFileSelectorDefaultWildcardStr,
                 long style = 0,
                 const wxPoint& pos = wxDefaultPosition);

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
    
protected:    
    
    wxString    m_message;
    long        m_dialogStyle;
    wxWindow *  m_parent;
    wxString    m_dir;
    wxString    m_path; // Full path
    wxString    m_fileName;
    wxString    m_wildCard;
    int         m_filterIndex;
};

#define wxOPEN 1
#define wxSAVE 2
#define wxOVERWRITE_PROMPT 4
#define wxHIDE_READONLY 8
#define wxFILE_MUST_EXIST 16

// File selector - backward compatibility
WXDLLEXPORT wxString
wxFileSelector(const char *message = wxFileSelectorPromptStr,
               const char *default_path = NULL,
               const char *default_filename = NULL,
               const char *default_extension = NULL,
               const char *wildcard = wxFileSelectorDefaultWildcardStr,
               int flags = 0,
               wxWindow *parent = NULL,
               int x = -1, int y = -1);

// An extended version of wxFileSelector
WXDLLEXPORT wxString
wxFileSelectorEx(const char *message = wxFileSelectorPromptStr,
                 const char *default_path = NULL,
                 const char *default_filename = NULL,
                 int *indexDefaultExtension = NULL,
                 const char *wildcard = wxFileSelectorDefaultWildcardStr,
                 int flags = 0,
                 wxWindow *parent = NULL,
                 int x = -1, int y = -1);

// Ask for filename to load
WXDLLEXPORT wxString
wxLoadFileSelector(const char *what,
                   const char *extension,
                   const char *default_name = (const char *)NULL,
                   wxWindow *parent = (wxWindow *) NULL);

// Ask for filename to save
WXDLLEXPORT wxString
wxSaveFileSelector(const char *what,
                   const char *extension,
                   const char *default_name = (const char *) NULL,
                   wxWindow *parent = (wxWindow *) NULL);

#endif
    // __GTKFILEDLGH__
