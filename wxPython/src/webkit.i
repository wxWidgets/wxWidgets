/////////////////////////////////////////////////////////////////////////////
// Name:        webkit.i
// Purpose:     Embedding Apple's WebKit in wxWidgets
//
// Author:      Robin Dunn / Kevin Ollivier
//
// Created:     28-Feb-2003
// RCS-ID:      $Id$
// Copyright:   (c) 2001 by Total Control Software
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

#include "wx/html/webkit.h"
%}

//---------------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

%import core.i
%import windows.i
%import misc.i

%include _webkit_rename.i

%pragma(python) code = "import wx"

//---------------------------------------------------------------------------

%{
    // Put some wx default wxChar* values into wxStrings.
    DECLARE_DEF_STRING(PanelNameStr);
%}

%{
#if !wxUSE_WEBKIT
class wxWebKitCtrl : public wxControl
{
public:
    wxWebKitCtrl(wxWindow *parent,
                    wxWindowID winID,
                    const wxString& strURL,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize, long style = 0,
                    const wxValidator& validator = wxDefaultValidator,
                    const wxString& name = wxT("webkitctrl")) {}
    bool Create(wxWindow *parent,
                wxWindowID winID,
                const wxString& strURL,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxT("webkitctrl")) {return false;}

    void LoadURL(const wxString &url) {}

    bool CanGoBack() {return false;}
    bool CanGoForward() {return false;}
    bool GoBack() {return false;}
    bool GoForward() {return false;}
    void Reload() {}
    void Stop(){}
    bool CanGetPageSource(){return false;}
    wxString GetPageSource(){return wxEmptyString;}
    void SetPageSource(wxString& source, const wxString& baseUrl = wxEmptyString){}
};
#endif
%}

class wxWebKitCtrl : public wxControl
{
public:
    wxWebKitCtrl(wxWindow *parent,
                    wxWindowID winID,
                    const wxString& strURL,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize, long style = 0,
                    const wxValidator& validator = wxDefaultValidator,
                    const wxString& name = wxT("webkitctrl"));
    bool Create(wxWindow *parent,
                wxWindowID winID,
                const wxString& strURL,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxT("webkitctrl"));

    void LoadURL(const wxString &url);

    bool CanGoBack();
    bool CanGoForward();
    bool GoBack();
    bool GoForward();
    void Reload();
    void Stop();
    bool CanGetPageSource();
    wxString GetPageSource();
    void SetPageSource(wxString& source, const wxString& baseUrl = wxEmptyString);
};

%init %{

%}
