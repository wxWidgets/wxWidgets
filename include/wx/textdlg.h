/////////////////////////////////////////////////////////////////////////////
// Name:        textdlg.h
// Purpose:     wxTextEntryDialog base header
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TEXTDLG_H_BASE_
#define _WX_TEXTDLG_H_BASE_

#include "wx/generic/textdlgg.h"

// ----------------------------------------------------------------------------
// function to get a number from user
// ----------------------------------------------------------------------------

long WXDLLEXPORT
wxGetNumberFromUser(const wxString& message,
                    const wxString& prompt,
                    const wxString& caption,
                    long value = 0,
                    long min = 0,
                    long max = 100,
                    wxWindow *parent = (wxWindow *)NULL,
                    const wxPoint& pos = wxDefaultPosition);

#endif
    // _WX_TEXTDLG_H_BASE_
