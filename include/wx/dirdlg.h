#ifndef _WX_DIRDLG_H_BASE_
#define _WX_DIRDLG_H_BASE_

#if wxUSE_DIRDLG

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

WXDLLEXPORT_DATA(extern const wxChar*) wxDirDialogNameStr;
WXDLLEXPORT_DATA(extern const wxChar*) wxDirDialogDefaultFolderStr;
WXDLLEXPORT_DATA(extern const wxChar*) wxEmptyString;

#if defined(__WXMSW__)
    #if defined(__WIN16__) || (defined(__GNUWIN32__) && !wxUSE_NORLANDER_HEADERS) || defined(__SALFORDC__)
        #include "wx/generic/dirdlgg.h"
    #else
        #include "wx/msw/dirdlg.h"
    #endif
#elif defined(__WXMOTIF__)
    #include "wx/generic/dirdlgg.h"
#elif defined(__WXGTK__)
    #include "wx/generic/dirdlgg.h"
#elif defined(__WXMAC__)
    #include "wx/mac/dirdlg.h"
#elif defined(__WXPM__)
    #include "wx/os2/dirdlg.h"
#elif defined(__WXSTUBS__)
    #include "wx/stubs/dirdlg.h"
#endif

// ----------------------------------------------------------------------------
// common ::wxDirSelector() function
// ----------------------------------------------------------------------------

WXDLLEXPORT_DATA(extern const wxChar*) wxDirSelectorPromptStr;

WXDLLEXPORT wxString
wxDirSelector(const wxString& message = wxDirSelectorPromptStr,
              const wxString& defaultPath = wxEmptyString,
              long style = 0,
              const wxPoint& pos = wxDefaultPosition,
              wxWindow *parent = NULL);

#endif // wxUSE_DIRDLG

#endif
    // _WX_DIRDLG_H_BASE_
