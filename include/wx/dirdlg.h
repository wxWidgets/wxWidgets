#ifndef _WX_DIRDLG_H_BASE_
#define _WX_DIRDLG_H_BASE_

#if wxUSE_DIRDLG

#include "wx/dialog.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

WXDLLEXPORT_DATA(extern const wxChar*) wxDirDialogNameStr;
WXDLLEXPORT_DATA(extern const wxChar*) wxDirDialogDefaultFolderStr;
WXDLLEXPORT_DATA(extern const wxChar*) wxDirSelectorPromptStr;
WXDLLEXPORT_DATA(extern const wxChar*) wxEmptyString;

#define wxDD_DEFAULT_STYLE \
    (wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxDD_NEW_DIR_BUTTON)

/*
    The interface (TODO: make the other classes really derive from it!) is
    something like this:

class WXDLLEXPORT wxDirDialogBase : public wxDialog
{
public:
    wxDirDialogBase(wxWindow *parent,
                    const wxString& title = wxFileSelectorPromptStr,
                    const wxString& defaultPath = wxEmptyString,
                    long style = wxDD_DEFAULT_STYLE,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& sz = wxDefaultSize,
                    const wxString& name = _T("dirdialog"));

    void SetMessage(const wxString& message);
    void SetPath(const wxString& path);
    void SetStyle(long style);

    wxString GetMessage() const;
    wxString GetPath() const;
    long GetStyle() const;
};

*/

#if defined(__WXMSW__)
    #if defined(__WIN16__) || (defined(__GNUWIN32__) && !wxUSE_NORLANDER_HEADERS) || defined(__SALFORDC__) || !wxUSE_OLE
        #include "wx/generic/dirdlgg.h"
        #define wxDirDialog wxGenericDirDialog
    #else
        #include "wx/msw/dirdlg.h"
    #endif
#elif defined(__WXMOTIF__)
    #include "wx/generic/dirdlgg.h"
#elif defined(__WXGTK__)
    #include "wx/generic/dirdlgg.h"
#elif defined(__WXX11__)
    #include "wx/generic/dirdlgg.h"
#elif defined(__WXMGL__)
    #include "wx/generic/dirdlgg.h"
#elif defined(__WXMAC__)
    #include "wx/mac/dirdlg.h"
#elif defined(__WXPM__)
    #include "wx/generic/dirdlgg.h"
#endif

#if !defined(__WXMSW__) && !defined(__WXMAC__)
    #define wxDirDialog wxGenericDirDialog
#endif

// ----------------------------------------------------------------------------
// common ::wxDirSelector() function
// ----------------------------------------------------------------------------

WXDLLEXPORT wxString
wxDirSelector(const wxString& message = wxDirSelectorPromptStr,
              const wxString& defaultPath = wxEmptyString,
              long style = wxDD_DEFAULT_STYLE,
              const wxPoint& pos = wxDefaultPosition,
              wxWindow *parent = NULL);

#endif // wxUSE_DIRDLG

#endif
    // _WX_DIRDLG_H_BASE_
