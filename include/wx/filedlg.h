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

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "filedlg.h"
#endif

//----------------------------------------------------------------------------
// wxFileDialog data and generic functions
//----------------------------------------------------------------------------

enum
{
    wxOPEN              = 0x0001,
    wxSAVE              = 0x0002,
    wxOVERWRITE_PROMPT  = 0x0004,
    wxHIDE_READONLY     = 0x0008,
    wxFILE_MUST_EXIST   = 0x0010,
    wxMULTIPLE          = 0x0020,
    wxCHANGE_DIR        = 0x0040
};

WXDLLEXPORT_DATA(extern const wxChar*) wxFileSelectorPromptStr;
WXDLLEXPORT_DATA(extern const wxChar*) wxFileSelectorDefaultWildcardStr;

// Parses the filterStr, returning the number of filters.
// Returns 0 if none or if there's a problem, they arrays will contain an equal
// number of items found before the error.
// filterStr is in the form:
// "All files (*.*)|*.*|Image Files (*.jpeg *.png)|*.jpg;*.png"
extern int wxParseFileFilter(const wxString& filterStr,
                             wxArrayString& descriptions,
                             wxArrayString& filters);

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
