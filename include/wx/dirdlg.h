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

// Universal and non-port related switches with need for generic implementation
#if defined(__WXMSW__) && (defined(__WXUNIVERSAL__) || \
                           defined(__SALFORDC__)    || \
                           !wxUSE_OLE               || \
                           (defined (__GNUWIN32__) && !wxUSE_NORLANDER_HEADERS))

    #include "wx/generic/dirdlgg.h"
    #define wxDirDialog wxGenericDirDialog

// MS Smartphone
#elif defined(__SMARTPHONE__) && defined(__WXWINCE__)

    #include "wx/generic/dirdlgg.h"
    #define wxDirDialog wxGenericDirDialog

// Native MSW
#elif defined(__WXMSW__)

    #include "wx/msw/dirdlg.h"

// Native Mac
#elif defined(__WXMAC__)

    #include "wx/mac/dirdlg.h"

// Other ports use generic implementation
#elif defined(__WXMOTIF__) || \
      defined(__WXGTK__)   || \
      defined(__WXX11__)   || \
      defined(__WXMGL__)   || \
      defined(__WXCOCOA__) || \
      defined(__WXPM__)

    #include "wx/generic/dirdlgg.h"
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
