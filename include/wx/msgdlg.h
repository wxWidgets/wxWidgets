#ifndef _WX_MSGDLG_H_BASE_
#define _WX_MSGDLG_H_BASE_

#include "wx/setup.h"

#if wxUSE_MSGDLG

#if defined(__WXUNIVERSAL__) || defined(__WXGPE__)
#include "wx/generic/msgdlgg.h"
#elif defined(__WXMSW__)
#include "wx/msw/msgdlg.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/msgdlg.h"
#elif defined(__WXGTK__) && defined(__WXGTK20__)
#include "wx/gtk/msgdlg.h"
#elif defined(__WXGTK__)
#include "wx/generic/msgdlgg.h"
#elif defined(__WXMAC__)
#include "wx/mac/msgdlg.h"
#elif defined(__WXCOCOA__)
#include "wx/generic/msgdlgg.h"
#elif defined(__WXPM__)
#include "wx/os2/msgdlg.h"
#endif

// ----------------------------------------------------------------------------
// wxMessageBox: the simplest way to use wxMessageDialog
// ----------------------------------------------------------------------------

int WXDLLEXPORT wxMessageBox(const wxString& message,
                            const wxString& caption = wxMessageBoxCaptionStr,
                            long style = wxOK | wxCENTRE,
                            wxWindow *parent = NULL,
                            int x = wxDefaultCoord, int y = wxDefaultCoord);

#endif // wxUSE_MSGDLG

#endif
    // _WX_MSGDLG_H_BASE_
