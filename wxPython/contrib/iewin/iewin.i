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


%module(package="wx") iewin

%{
#include "wx/wxPython/wxPython.h"
#include "wx/wxPython/pyclasses.h"
#include "wx/wxPython/pyistream.h"    

#include "IEHtmlWin.h"
%}

//---------------------------------------------------------------------------

%import core.i
%pythoncode { wx = _core }
%pythoncode { __docfilter__ = wx.__DocFilter(globals()) }
%pythoncode {
    import warnings
    warnings.warn("This module is deprecated.  Please use the wx.lib.iewin module instead.",    
                  DeprecationWarning, stacklevel=2)
}

MAKE_CONST_WXSTRING_NOSWIG(PanelNameStr);

%include _iewin_rename.i


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


%pythoncode {    
EVT_MSHTML_BEFORENAVIGATE2      = wx.PyEventBinder(wxEVT_COMMAND_MSHTML_BEFORENAVIGATE2, 1)
EVT_MSHTML_NEWWINDOW2           = wx.PyEventBinder(wxEVT_COMMAND_MSHTML_NEWWINDOW2, 1)
EVT_MSHTML_DOCUMENTCOMPLETE     = wx.PyEventBinder(wxEVT_COMMAND_MSHTML_DOCUMENTCOMPLETE, 1)
EVT_MSHTML_PROGRESSCHANGE       = wx.PyEventBinder(wxEVT_COMMAND_MSHTML_PROGRESSCHANGE, 1)
EVT_MSHTML_STATUSTEXTCHANGE     = wx.PyEventBinder(wxEVT_COMMAND_MSHTML_STATUSTEXTCHANGE, 1)
EVT_MSHTML_TITLECHANGE          = wx.PyEventBinder(wxEVT_COMMAND_MSHTML_TITLECHANGE, 1)
}

//---------------------------------------------------------------------------

enum wxIEHtmlRefreshLevel {
    wxIEHTML_REFRESH_NORMAL = 0,
    wxIEHTML_REFRESH_IFEXPIRED = 1,
    wxIEHTML_REFRESH_CONTINUE = 2,
    wxIEHTML_REFRESH_COMPLETELY = 3
};


MustHaveApp(wxIEHtmlWin);

class wxIEHtmlWin : public wxWindow /* wxActiveX */ 
{
public:
    %pythonAppend wxIEHtmlWin      "self._setOORInfo(self)"
    
    wxIEHtmlWin(wxWindow * parent, wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPyPanelNameStr);

    void LoadUrl(const wxString&);
    bool LoadString(wxString html);
    bool LoadStream(wxInputStream *is);

    %pythoncode { Navigate = LoadUrl }

    void SetCharset(wxString charset);
    void SetEditMode(bool seton);
    bool GetEditMode();
    wxString GetStringSelection(bool asHTML = false);
    wxString GetText(bool asHTML = false);

    bool GoBack();
    bool GoForward();
    bool GoHome();
    bool GoSearch();
    %Rename(RefreshPage, bool,  Refresh(wxIEHtmlRefreshLevel level));
    bool Stop();

};


//---------------------------------------------------------------------------


