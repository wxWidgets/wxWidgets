/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/filedlg.h
// Purpose:     wxFileDialog class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
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

class WXDLLEXPORT wxFileDialog: public wxDialog
{
public:
    wxFileDialog(wxWindow *parent,
                 const wxString& message = wxFileSelectorPromptStr,
                 const wxString& defaultDir = wxEmptyString,
                 const wxString& defaultFile = wxEmptyString,
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
    void GetPaths(wxArrayString& paths) const;
    wxString GetDirectory() const { return m_dir; }
    wxString GetFilename() const { return m_fileName; }
    void GetFilenames(wxArrayString& files) const { files = m_fileNames; }
    wxString GetWildcard() const { return m_wildCard; }
    long GetStyle() const { return m_dialogStyle; }
    int GetFilterIndex() const { return m_filterIndex ; }

    virtual int ShowModal();

protected:
    wxString      m_message;
    long          m_dialogStyle;
    wxWindow *    m_parent;
    wxString      m_dir;
    wxString      m_path;       // Full path
    wxString      m_fileName;
    wxArrayString m_fileNames;
    wxString      m_wildCard;
    int           m_filterIndex;

private:
    DECLARE_DYNAMIC_CLASS(wxFileDialog)
    DECLARE_NO_COPY_CLASS(wxFileDialog)
};

#endif // _WX_FILEDLG_H_

