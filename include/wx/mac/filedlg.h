/////////////////////////////////////////////////////////////////////////////
// Name:        filedlg.h
// Purpose:     wxFileDialog class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FILEDLG_H_
#define _WX_FILEDLG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "filedlg.h"
#endif

#include "wx/dialog.h"

/*
 * File selector
 */

class WXDLLEXPORT wxFileDialog: public wxDialog
{
DECLARE_DYNAMIC_CLASS(wxFileDialog)
protected:
    wxString    m_message;
    long        m_dialogStyle;
    wxWindow *  m_parent;
    wxString    m_dir;
    wxString    m_path; // Full path
    wxString    m_fileName;
    wxArrayString m_fileNames;
    wxArrayString m_paths;
    wxString    m_wildCard;
    int         m_filterIndex;
public:
    wxFileDialog(wxWindow *parent, const wxString& message = wxFileSelectorPromptStr,
        const wxString& defaultDir = wxEmptyString, const wxString& defaultFile = wxEmptyString, const wxString& wildCard = wxFileSelectorDefaultWildcardStr,
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
    inline wxString GetDirectory() const { return m_dir; }
    inline wxString GetFilename() const { return m_fileName; }
    void GetPaths(wxArrayString& paths) const { paths = m_paths; }
    void GetFilenames(wxArrayString& files) const { files = m_fileNames; }
    inline wxString GetWildcard() const { return m_wildCard; }
    inline long GetStyle() const { return m_dialogStyle; }
    inline int GetFilterIndex() const { return m_filterIndex ; }

    int ShowModal();
    
    // not supported for file dialog, RR
    virtual void DoSetSize(int WXUNUSED(x), int WXUNUSED(y),
                           int WXUNUSED(width), int WXUNUSED(height),
                           int WXUNUSED(sizeFlags) = wxSIZE_AUTO) {}

};

#endif // _WX_FILEDLG_H_
