/////////////////////////////////////////////////////////////////////////////
// Name:        filedlg.h
// Purpose:     wxFileDialog class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FILEDLG_H_
#define _WX_FILEDLG_H_

#ifdef __GNUG__
#pragma interface "filedlg.h"
#endif

#include "wx/dialog.h"

/*
 * File selector
 */

WXDLLEXPORT_DATA(extern const char*) wxFileSelectorPromptStr;
WXDLLEXPORT_DATA(extern const char*) wxFileSelectorDefaultWildcardStr;

class WXDLLEXPORT wxFileDialog: public wxDialog
{
DECLARE_DYNAMIC_CLASS(wxFileDialog)
public:
    wxString    m_message;
    long        m_dialogStyle;
    wxWindow *  m_parent;
    wxString    m_dir;
    wxString    m_path; // Full path
    wxString    m_fileName;
    wxString    m_wildCard;
    int         m_filterIndex;

    // For Motif
    wxPoint     m_pos;
    static wxString m_fileSelectorAnswer;
    static bool m_fileSelectorReturned;

public:
    wxFileDialog(wxWindow *parent, const wxString& message = wxFileSelectorPromptStr,
        const wxString& defaultDir = "", const wxString& defaultFile = "", const wxString& wildCard = wxFileSelectorDefaultWildcardStr,
        long style = 0, const wxPoint& pos = wxDefaultPosition);

    inline void SetMessage(const wxString& message) { m_message = message; }
    inline void SetPath(const wxString& path) { m_path = path; }
    inline void SetDirectory(const wxString& dir) { m_dir = dir; }
    inline void SetFilename(const wxString& name) { m_fileName = name; }
    inline void SetWildcard(const wxString& wildCard) { m_wildCard = wildCard; }
    inline void SetStyle(long style) { m_dialogStyle = style; }
    inline void SetFilterIndex(int filterIndex) { m_filterIndex = filterIndex; }

    inline wxString GetMessage() const { return m_message; }
    inline wxString GetPath() const { return m_path; }
    inline void GetPaths(wxArrayString& a) { a.Empty(); a.Add(m_path); }
    inline wxString GetDirectory() const { return m_dir; }
    inline wxString GetFilename() const { return m_fileName; }
    inline void GetFilenames(wxArrayString& a) { a.Empty();
       a.Add( m_fileName); }
    inline wxString GetWildcard() const { return m_wildCard; }
    inline long GetStyle() const { return m_dialogStyle; }
    inline int GetFilterIndex() const { return m_filterIndex ; }

    int ShowModal();
};

#define wxOPEN              0x0001
#define wxSAVE              0x0002
#define wxOVERWRITE_PROMPT  0x0004
#define wxHIDE_READONLY     0x0008
#define wxFILE_MUST_EXIST   0x0010

// File selector - backward compatibility
WXDLLEXPORT wxString wxFileSelector(const char *message = wxFileSelectorPromptStr, const char *default_path = NULL,
                     const char *default_filename = NULL, const char *default_extension = NULL,
                     const char *wildcard = wxFileSelectorDefaultWildcardStr, int flags = 0,
                     wxWindow *parent = NULL, int x = -1, int y = -1);

// An extended version of wxFileSelector
WXDLLEXPORT wxString wxFileSelectorEx(const char *message = wxFileSelectorPromptStr, const char *default_path = NULL,
                     const char *default_filename = NULL, int *indexDefaultExtension = NULL,
                     const char *wildcard = wxFileSelectorDefaultWildcardStr, int flags = 0,
                     wxWindow *parent = NULL, int x = -1, int y = -1);

// Generic file load dialog
WXDLLEXPORT wxString wxLoadFileSelector(const char *what, const char *extension, const char *default_name = NULL, wxWindow *parent = NULL);

// Generic file save dialog
WXDLLEXPORT wxString wxSaveFileSelector(const char *what, const char *extension, const char *default_name = NULL, wxWindow *parent = NULL);

#endif
    // _WX_FILEDLG_H_
