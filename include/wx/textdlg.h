#ifndef _WX_TEXTDLG_H_BASE_
#define _WX_TEXTDLG_H_BASE_

#ifdef __WXGTK__
#include "wx/gtk/textdlg.h"
#else
#include "wx/generic/textdlgg.h"
#endif

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
