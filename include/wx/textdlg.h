/////////////////////////////////////////////////////////////////////////////
// Name:        wx/textdlg.h
// Purpose:     wxTextEntryDialog class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TEXTDLG_H_BASE_
#define _WX_TEXTDLG_H_BASE_

#include "wx/defs.h"

extern WXDLLIMPEXP_DATA_CORE(const char) wxGetTextFromUserPromptStr[];
extern WXDLLIMPEXP_DATA_CORE(const char) wxGetPasswordFromUserPromptStr[];

#define wxTextEntryDialogStyle (wxOK | wxCANCEL | wxCENTRE | wxWS_EX_VALIDATE_RECURSIVELY)

#if defined(__WXQT__)
    #include "wx/qt/textdlg.h"
#else
    #include "wx/generic/textdlgg.h"
#endif

// ----------------------------------------------------------------------------
// function to get a string from user
// ----------------------------------------------------------------------------

WXDLLIMPEXP_CORE wxString
    wxGetTextFromUser(const wxString& message,
                    const wxString& caption = wxGetTextFromUserPromptStr,
                    const wxString& default_value = wxEmptyString,
                    wxWindow *parent = NULL,
                    wxCoord x = wxDefaultCoord,
                    wxCoord y = wxDefaultCoord,
                    bool centre = true);

WXDLLIMPEXP_CORE wxString
    wxGetPasswordFromUser(const wxString& message,
                        const wxString& caption = wxGetPasswordFromUserPromptStr,
                        const wxString& default_value = wxEmptyString,
                        wxWindow *parent = NULL,
                        wxCoord x = wxDefaultCoord,
                        wxCoord y = wxDefaultCoord,
                        bool centre = true);

#endif // _WX_TEXTDLG_H_BASE_
