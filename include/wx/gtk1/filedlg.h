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

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface
#endif

#include "wx/dialog.h"

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
    
private:
    DECLARE_DYNAMIC_CLASS(wxFileDialog)
};

#endif // __GTKFILEDLGH__
