/////////////////////////////////////////////////////////////////////////////
// Name:        iewin.i
// Purpose:     Internet Explorer in a wxWindow
//
// Author:      Robin Dunn
//
// Created:     20-Apr-2001
// RCS-ID:      $Id$
// Copyright:   (c) 2001 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


%module iewin

%{
#include "wxPython.h"
#include "IEHtmlWin.h"
#include "pyistream.h"
%}

//---------------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

%extern wx.i
%extern windows.i
%extern _defs.i
%extern misc.i
%extern events.i
%extern streams.i

%pragma(python) code = "import wx"

//---------------------------------------------------------------------------

%{
    // Put some wx default wxChar* values into wxStrings.
    DECLARE_DEF_STRING(PanelNameStr);
%}

//---------------------------------------------------------------------------

class wxMSHTMLEvent : public wxNotifyEvent
{
public:
    wxMSHTMLEvent(wxEventType commandType = wxEVT_NULL, int id = 0);
    wxString GetText1();
    long GetLong1();
    long GetLong2();
};


enum {
    wxEVT_COMMAND_MSHTML_BEFORENAVIGATE2,
    wxEVT_COMMAND_MSHTML_NEWWINDOW2,
    wxEVT_COMMAND_MSHTML_DOCUMENTCOMPLETE,
    wxEVT_COMMAND_MSHTML_PROGRESSCHANGE,
    wxEVT_COMMAND_MSHTML_STATUSTEXTCHANGE,
    wxEVT_COMMAND_MSHTML_TITLECHANGE,
};


%pragma(python) code = "
def EVT_MSHTML_BEFORENAVIGATE2(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_MSHTML_BEFORENAVIGATE2, func)

def EVT_MSHTML_NEWWINDOW2(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_MSHTML_NEWWINDOW2, func)

def EVT_MSHTML_DOCUMENTCOMPLETE(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_MSHTML_DOCUMENTCOMPLETE, func)

def EVT_MSHTML_PROGRESSCHANGE(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_MSHTML_PROGRESSCHANGE, func)

def EVT_MSHTML_STATUSTEXTCHANGE(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_MSHTML_STATUSTEXTCHANGE, func)

def EVT_MSHTML_TITLECHANGE(win, id, func):
    win.Connect(id, -1, wxEVT_COMMAND_MSHTML_TITLECHANGE, func)
"

//---------------------------------------------------------------------------

enum wxIEHtmlRefreshLevel {
    wxIEHTML_REFRESH_NORMAL = 0,
    wxIEHTML_REFRESH_IFEXPIRED = 1,
    wxIEHTML_REFRESH_CONTINUE = 2,
    wxIEHTML_REFRESH_COMPLETELY = 3
};


class wxIEHtmlWin : public wxWindow /* wxActiveX */
{
public:
    wxIEHtmlWin(wxWindow * parent, wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPyPanelNameStr);

    void LoadUrl(const wxString&);
    bool LoadString(wxString html);
    bool LoadStream(wxInputStream *is);

    %pragma(python) addtoclass = "Navigate = LoadUrl"

    void SetCharset(wxString charset);
    void SetEditMode(bool seton);
    bool GetEditMode();
    wxString GetStringSelection(bool asHTML = FALSE);
    wxString GetText(bool asHTML = FALSE);

    bool GoBack();
    bool GoForward();
    bool GoHome();
    bool GoSearch();
    %name(RefreshPage)bool Refresh(wxIEHtmlRefreshLevel level);
    bool Stop();

};


//---------------------------------------------------------------------------

%init %{

    wxClassInfo::CleanUpClasses();
    wxClassInfo::InitializeClasses();

%}

//----------------------------------------------------------------------

%pragma(python) include="_iewinextras.py";

//---------------------------------------------------------------------------
