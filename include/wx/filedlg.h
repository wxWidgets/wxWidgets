#ifndef _WX_FILEDLG_H_BASE_
#define _WX_FILEDLG_H_BASE_

#if defined(__WXMSW__)
#include "wx/msw/filedlg.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/filedlg.h"
#elif defined(__WXGTK__)
#include "wx/gtk/filedlg.h"
#elif defined(__WXQT__)
#include "wx/qt/filedlg.h"
#elif defined(__WXMAC__)
#include "wx/mac/filedlg.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/filedlg.h"
#endif

// File selector - backward compatibility
WXDLLEXPORT wxString
wxFileSelector(const char *message = wxFileSelectorPromptStr,
               const char *default_path = NULL,
               const char *default_filename = NULL,
               const char *default_extension = NULL,
               const char *wildcard = wxFileSelectorDefaultWildcardStr,
               int flags = 0,
               wxWindow *parent = NULL,
               int x = -1, int y = -1);

// An extended version of wxFileSelector
WXDLLEXPORT wxString
wxFileSelectorEx(const char *message = wxFileSelectorPromptStr,
                 const char *default_path = NULL,
                 const char *default_filename = NULL,
                 int *indexDefaultExtension = NULL,
                 const char *wildcard = wxFileSelectorDefaultWildcardStr,
                 int flags = 0,
                 wxWindow *parent = NULL,
                 int x = -1, int y = -1);

// Ask for filename to load
WXDLLEXPORT wxString
wxLoadFileSelector(const char *what,
                   const char *extension,
                   const char *default_name = (const char *)NULL,
                   wxWindow *parent = (wxWindow *) NULL);

// Ask for filename to save
WXDLLEXPORT wxString
wxSaveFileSelector(const char *what,
                   const char *extension,
                   const char *default_name = (const char *) NULL,
                   wxWindow *parent = (wxWindow *) NULL);

#endif
    // _WX_FILEDLG_H_BASE_
