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

%define DOCSTRING
"wx.webkit.WebKitCtrl for Mac OSX."
%enddef

%module(package="wx", docstring=DOCSTRING) webkit

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
    wxString GetPageURL()   { return wxEmptyString; }
    wxString GetPageTitle() { return wxEmptyString; }
};


enum {
    wxWEBKIT_STATE_START = 0,
    wxWEBKIT_STATE_NEGOTIATING = 0,
    wxWEBKIT_STATE_REDIRECTING = 0,
    wxWEBKIT_STATE_TRANSFERRING = 0,
    wxWEBKIT_STATE_STOP = 0,
    wxWEBKIT_STATE_FAILED = 0,

    wxEVT_WEBKIT_STATE_CHANGED = 0
};

class wxWebKitStateChangedEvent : public wxCommandEvent
{
public:
    wxWebKitStateChangedEvent( wxWindow* win =  NULL )
    { wxPyRaiseNotImplemented(); }

    int GetState() { return 0; }
    void SetState(const int state) {}
    wxString GetURL() { return wxEmptyString; }
    void SetURL(const wxString& url) {}
};

 
#endif
%}

// Now define it for SWIG, using either the real class or the dummy above.

MustHaveApp(wxWebKitCtrl);

class wxWebKitCtrl : public wxControl
{
public:
    %pythonAppend wxWebKitCtrl         "self._setOORInfo(self)"
    %pythonAppend wxWebKitCtrl()       ""
    
    wxWebKitCtrl(wxWindow *parent,
                    wxWindowID winID = -1,
                    const wxString& strURL = wxPyEmptyString,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize, long style = 0,
                    const wxValidator& validator = wxDefaultValidator,
                    const wxString& name = wxPyWebKitNameStr);

    %RenameCtor(PreWebKitCtrl, wxWebKitCtrl());

    
    bool Create(wxWindow *parent,
                wxWindowID winID = -1,
                const wxString& strURL = wxPyEmptyString,
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
    wxString GetPageURL();
    wxString GetPageTitle();

    %property(PageSource, GetPageSource, SetPageSource, doc="See `GetPageSource` and `SetPageSource`");
    %property(PageTitle, GetPageTitle, doc="See `GetPageTitle`");
    %property(PageURL, GetPageURL, doc="See `GetPageURL`");    
};


//---------------------------------------------------------------------------


enum {
    wxWEBKIT_STATE_START,
    wxWEBKIT_STATE_NEGOTIATING,
    wxWEBKIT_STATE_REDIRECTING,
    wxWEBKIT_STATE_TRANSFERRING,
    wxWEBKIT_STATE_STOP,
    wxWEBKIT_STATE_FAILED,
};


%constant wxEventType wxEVT_WEBKIT_STATE_CHANGED;


class wxWebKitStateChangedEvent : public wxCommandEvent
{
public:
    wxWebKitStateChangedEvent( wxWindow* win =  NULL );

    int GetState();
    void SetState(const int state);
    wxString GetURL();
    void SetURL(const wxString& url);

    %property(State, GetState, SetState, doc="See `GetState` and `SetState`");
    %property(URL, GetURL, SetURL, doc="See `GetURL` and `SetURL`");
};


%pythoncode %{
    EVT_WEBKIT_STATE_CHANGED = wx.PyEventBinder(wxEVT_WEBKIT_STATE_CHANGED)
%}


//---------------------------------------------------------------------------

%init %{

%}

//---------------------------------------------------------------------------
