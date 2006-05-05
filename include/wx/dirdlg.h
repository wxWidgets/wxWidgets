/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dirdlg.h
// Purpose:     wxDirDialog base class
// Author:      Robert Roebling
// Modified by:
// Created:
// Copyright:   (c) Robert Roebling
// RCS-ID:      $Id$
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DIRDLG_H_BASE_
#define _WX_DIRDLG_H_BASE_

#if wxUSE_DIRDLG

#include "wx/dialog.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

extern WXDLLEXPORT_DATA(const wxChar) wxDirDialogNameStr[];
extern WXDLLEXPORT_DATA(const wxChar) wxDirDialogDefaultFolderStr[];
extern WXDLLEXPORT_DATA(const wxChar) wxDirSelectorPromptStr[];


#ifdef __WXWINCE__
    #define wxDD_DEFAULT_STYLE \
        (wxDEFAULT_DIALOG_STYLE | wxDD_NEW_DIR_BUTTON)
#else
    #define wxDD_DEFAULT_STYLE \
        (wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxDD_NEW_DIR_BUTTON)
#endif

//-------------------------------------------------------------------------
// wxDirDialogBase
//-------------------------------------------------------------------------

class WXDLLEXPORT wxDirDialogBase : public wxDialog
{
public:
    wxDirDialogBase(wxWindow *parent,
                    const wxString& title = wxDirSelectorPromptStr,
                    const wxString& defaultPath = wxEmptyString,
                    long style = wxDD_DEFAULT_STYLE,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& sz = wxDefaultSize,
                    const wxString& name = wxDirDialogNameStr)
        : wxDialog(parent, wxID_ANY, title, pos, sz, style, name)
        , m_path(defaultPath)
    {}
    wxDirDialogBase() {}

    virtual ~wxDirDialogBase() {}

    virtual void SetMessage(const wxString& message) { m_message = message; }
    virtual void SetPath(const wxString& path) { m_path = path; }
    virtual void SetStyle(long style) { SetWindowStyle(style); }

    virtual wxString GetMessage() const { return m_message; }
    virtual wxString GetPath() const { return m_path; }
    virtual long GetStyle() const { return GetWindowStyle(); }

protected:
    wxString m_message;
    wxString m_path;
};


// Universal and non-port related switches with need for generic implementation
#if defined(__WXUNIVERSAL__)

    #include "wx/generic/dirdlgg.h"
    #define wxDirDialog wxGenericDirDialog

#elif defined(__WXMSW__) && (defined(__SALFORDC__)    || \
                             !wxUSE_OLE               || \
                             (defined (__GNUWIN32__) && !wxUSE_NORLANDER_HEADERS))

    #include "wx/generic/dirdlgg.h"
    #define wxDirDialog wxGenericDirDialog

// MS PocketPC or MS Smartphone
#elif defined(__WXMSW__) && defined(__WXWINCE__) && !defined(__HANDHELDPC__)

    #include "wx/generic/dirdlgg.h"
    #define wxDirDialog wxGenericDirDialog

// Native MSW
#elif defined(__WXMSW__)

    #include "wx/msw/dirdlg.h"

// Native GTK for gtk2.x and generic for gtk1.x
#elif defined(__WXGTK__)

#if defined( __WXGTK20__ )
    #include "wx/gtk/dirdlg.h"
    #define wxDirDialog wxDirDialogGTK
#else
    #include "wx/generic/dirdlgg.h"
    #define wxDirDialog wxGenericDirDialog
#endif

// Native Mac
#elif defined(__WXMAC__)

    #include "wx/mac/dirdlg.h"

// Native Cocoa
#elif defined(__WXCOCOA__)

    #include "wx/cocoa/dirdlg.h"

// Other ports use generic implementation
#elif defined(__WXMOTIF__) || \
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
