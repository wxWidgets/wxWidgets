/////////////////////////////////////////////////////////////////////////////
// Name:        webkit.i
// Purpose:     Embedding Apple's WebKit in wxWidgets
//
// Author:      Robin Dunn / Kevin Ollivier
//
// Created:     18-Oct-2004
// RCS-ID:      $Id$
// Copyright:   (c) 2004 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module webkit

%{

#include "wx/wxPython/wxPython.h"
#include "wx/wxPython/pyclasses.h"
#include "wx/wxPython/pyistream.h"

#ifdef __WXMAC__  // avoid a bug in Carbon headers
#define scalb scalbn
#endif

#if wxUSE_WEBKIT
#include "wx/html/webkit.h"
#endif
%}

//---------------------------------------------------------------------------

%import core.i
%pythoncode { wx = _core }
%pythoncode { __docfilter__ = wx.__DocFilter(globals()) }


%include _webkit_rename.i

//---------------------------------------------------------------------------

// Put some wx default wxChar* values into wxStrings.
MAKE_CONST_WXSTRING_NOSWIG(EmptyString);
MAKE_CONST_WXSTRING2(WebKitNameStr, wxT("webkitctrl"))



    
%{
#if !wxUSE_WEBKIT
// a dummy class for ports that don't have wxWebKitCtrl
class wxWebKitCtrl : public wxControl
{
public:
    wxWebKitCtrl(wxWindow *parent,
                 wxWindowID winID,
                 const wxString& strURL,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize, long style = 0,
                 const wxValidator& validator = wxDefaultValidator,
                 const wxString& name = wxPyWebKitNameStr)
    { wxPyRaiseNotImplemented(); }

    wxWebKitCtrl() { wxPyRaiseNotImplemented(); }
    
    bool Create(wxWindow *parent,
                wxWindowID winID,
                const wxString& strURL,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxPyWebKitNameStr)
    { return false; }

    void LoadURL(const wxString &url) {}

    bool CanGoBack() { return false; }
    bool CanGoForward() { return false; }
    bool GoBack() { return false; }
    bool GoForward() { return false; }
    void Reload() {}
    void Stop() {}
    bool CanGetPageSource() { return false; }
    wxString GetPageSource() { return wxEmptyString; }
    void SetPageSource(wxString& source, const wxString& baseUrl = wxEmptyString) {}
};
#endif
%}

// Now define it for SWIG.
class wxWebKitCtrl : public wxControl
{
public:
    wxWebKitCtrl(wxWindow *parent,
                    wxWindowID winID,
                    const wxString& strURL,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize, long style = 0,
                    const wxValidator& validator = wxDefaultValidator,
                    const wxString& name = wxPyWebKitNameStr);

    %name(PreWebKitCtrl)wxWebKitCtrl();

    
    bool Create(wxWindow *parent,
                wxWindowID winID,
                const wxString& strURL,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxPyWebKitNameStr);

    void LoadURL(const wxString &url);

    bool CanGoBack();
    bool CanGoForward();
    bool GoBack();
    bool GoForward();
    void Reload();
    void Stop();
    bool CanGetPageSource();
    wxString GetPageSource();
    void SetPageSource(wxString& source, const wxString& baseUrl = wxPyEmptyString);
};


%init %{

%}
