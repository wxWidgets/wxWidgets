/////////////////////////////////////////////////////////////////////////////
// Name:        filedlg.h
// Purpose:     wxFileDialog base header
// Author:      Robert Roebling
// Modified by:
// Created:     8/17/99
// Copyright:   (c) Robert Roebling
// RCS-ID:
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FILEDLG_H_BASE_
#define _WX_FILEDLG_H_BASE_

#if wxUSE_FILEDLG

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "filedlg.h"
#endif

#include "wx/dialog.h"
#include "wx/arrstr.h"

//----------------------------------------------------------------------------
// wxFileDialog data
//----------------------------------------------------------------------------

enum
{
    wxOPEN              = 0x0001,
    wxSAVE              = 0x0002,
    wxOVERWRITE_PROMPT  = 0x0004,
#if WXWIN_COMPATIBILITY_2_4
    wxHIDE_READONLY     = 0x0008,
#endif
    wxFILE_MUST_EXIST   = 0x0010,
    wxMULTIPLE          = 0x0020,
    wxCHANGE_DIR        = 0x0040
};

WXDLLEXPORT_DATA(extern const wxChar*) wxFileSelectorPromptStr;
WXDLLEXPORT_DATA(extern const wxChar*) wxFileSelectorDefaultWildcardStr;

//----------------------------------------------------------------------------
// wxFileDialogBase
//----------------------------------------------------------------------------

class WXDLLEXPORT wxFileDialogBase: public wxDialog
{
public:
    wxFileDialogBase () {}

    wxFileDialogBase(wxWindow *parent,
                 const wxString& message = wxFileSelectorPromptStr,
                 const wxString& defaultDir = wxEmptyString,
                 const wxString& defaultFile = wxEmptyString,
                 const wxString& wildCard = wxFileSelectorDefaultWildcardStr,
                 long style = 0,
                 const wxPoint& pos = wxDefaultPosition);

    virtual void SetMessage(const wxString& message) { m_message = message; }
    virtual void SetPath(const wxString& path) { m_path = path; }
    virtual void SetDirectory(const wxString& dir) { m_dir = dir; }
    virtual void SetFilename(const wxString& name) { m_fileName = name; }
    virtual void SetWildcard(const wxString& wildCard) { m_wildCard = wildCard; }
    virtual void SetStyle(long style) { m_dialogStyle = style; }
    virtual void SetFilterIndex(int filterIndex) { m_filterIndex = filterIndex; }

    virtual wxString GetMessage() const { return m_message; }
    virtual wxString GetPath() const { return m_path; }
    virtual void GetPaths(wxArrayString& paths) const { paths.Empty(); paths.Add(m_path); }
    virtual wxString GetDirectory() const { return m_dir; }
    virtual wxString GetFilename() const { return m_fileName; }
    virtual void GetFilenames(wxArrayString& files) const { files.Empty(); files.Add(m_fileName); }
    virtual wxString GetWildcard() const { return m_wildCard; }
    virtual long GetStyle() const { return m_dialogStyle; }
    virtual int GetFilterIndex() const { return m_filterIndex; }

    // Utility functions

#if WXWIN_COMPATIBILITY_2_4
    // Parses the wildCard, returning the number of filters.
    // Returns 0 if none or if there's a problem,
    // The arrays will contain an equal number of items found before the error.
    // wildCard is in the form:
    // "All files (*)|*|Image Files (*.jpeg *.png)|*.jpg;*.png"
    static int ParseWildcard(const wxString& wildCard,
                             wxArrayString& descriptions,
                             wxArrayString& filters);
#endif // WXWIN_COMPATIBILITY_2_4

    // Append first extension to filePath from a ';' separated extensionList
    // if filePath = "path/foo.bar" just return it as is
    // if filePath = "foo[.]" and extensionList = "*.jpg;*.png" return "foo.jpg"
    // if the extension is "*.j?g" (has wildcards) or "jpg" then return filePath
    static wxString AppendExtension(const wxString &filePath,
                                    const wxString &extensionList);

protected:
    wxString      m_message;
    long          m_dialogStyle;
    wxWindow     *m_parent;
    wxString      m_dir;
    wxString      m_path;       // Full path
    wxString      m_fileName;
    wxString      m_wildCard;
    int           m_filterIndex;

private:
    DECLARE_DYNAMIC_CLASS(wxFileDialogBase)
    DECLARE_NO_COPY_CLASS(wxFileDialogBase)
};

//----------------------------------------------------------------------------
// wxFileDialog convenience functions
//----------------------------------------------------------------------------

// File selector - backward compatibility
WXDLLEXPORT wxString
wxFileSelector(const wxChar *message = wxFileSelectorPromptStr,
               const wxChar *default_path = NULL,
               const wxChar *default_filename = NULL,
               const wxChar *default_extension = NULL,
               const wxChar *wildcard = wxFileSelectorDefaultWildcardStr,
               int flags = 0,
               wxWindow *parent = NULL,
               int x = wxDefaultCoord, int y = wxDefaultCoord);

// An extended version of wxFileSelector
WXDLLEXPORT wxString
wxFileSelectorEx(const wxChar *message = wxFileSelectorPromptStr,
                 const wxChar *default_path = NULL,
                 const wxChar *default_filename = NULL,
                 int *indexDefaultExtension = NULL,
                 const wxChar *wildcard = wxFileSelectorDefaultWildcardStr,
                 int flags = 0,
                 wxWindow *parent = NULL,
                 int x = wxDefaultCoord, int y = wxDefaultCoord);

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


#if defined (__WXUNIVERSAL__)
#include "wx/generic/filedlgg.h"
#elif defined(__WXMSW__)
#include "wx/msw/filedlg.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/filedlg.h"
#elif defined(__WXGTK__)
#include "wx/generic/filedlgg.h"
#elif defined(__WXX11__)
#include "wx/generic/filedlgg.h"
#elif defined(__WXMGL__)
#include "wx/generic/filedlgg.h"
#elif defined(__WXMAC__)
#include "wx/mac/filedlg.h"
#elif defined(__WXCOCOA__)
#include "wx/generic/filedlgg.h"
#elif defined(__WXPM__)
#include "wx/os2/filedlg.h"
#endif

#endif // wxUSE_FILEDLG

#endif // _WX_FILEDLG_H_BASE_
