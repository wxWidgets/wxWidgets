/////////////////////////////////////////////////////////////////////////////
// Name:        _cshelp.i
// Purpose:     Context sensitive help classes, and etc.
//
// Author:      Robin Dunn
//
// Created:     28-July-2001
// RCS-ID:      $Id$
// Copyright:   (c) 2001 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// not a %module

//---------------------------------------------------------------------------
%newgroup

%{
%}

//----------------------------------------------------------------------

enum {
    wxFRAME_EX_CONTEXTHELP,
    wxDIALOG_EX_CONTEXTHELP,
};
%constant wxEventType wxEVT_HELP;
%constant wxEventType wxEVT_DETAILED_HELP;


%pythoncode { 
EVT_HELP = wx.PyEventBinder( wxEVT_HELP, 1)
EVT_HELP_RANGE = wx.PyEventBinder(  wxEVT_HELP, 2)
EVT_DETAILED_HELP = wx.PyEventBinder( wxEVT_DETAILED_HELP, 1)
EVT_DETAILED_HELP_RANGE = wx.PyEventBinder( wxEVT_DETAILED_HELP, 2)
}

//----------------------------------------------------------------------

// A help event is sent when the user clicks on a window in context-help mode.

class wxHelpEvent : public wxCommandEvent
{
public:
    wxHelpEvent(wxEventType type = wxEVT_NULL,
                wxWindowID winid = 0,
                const wxPoint& pt = wxDefaultPosition);

    // Position of event (in screen coordinates)
    const wxPoint& GetPosition() const;
    void SetPosition(const wxPoint& pos);

    // Optional link to further help
    const wxString& GetLink() const;
    void SetLink(const wxString& link);

    // Optional target to display help in. E.g. a window specification
    const wxString& GetTarget() const;
    void SetTarget(const wxString& target);
};

//---------------------------------------------------------------------------

class wxContextHelp : public wxObject {
public:
    wxContextHelp(wxWindow* window = NULL, bool doNow = TRUE);
    ~wxContextHelp();

    bool BeginContextHelp(wxWindow* window = NULL);
    bool EndContextHelp();
};


//----------------------------------------------------------------------

class wxContextHelpButton : public wxBitmapButton {
public:
    %addtofunc wxContextHelpButton "self._setOORInfo(self)"

    wxContextHelpButton(wxWindow* parent, wxWindowID id = wxID_CONTEXT_HELP,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxBU_AUTODRAW);
};


//----------------------------------------------------------------------

class wxHelpProvider 
{
public:
    static wxHelpProvider *Set(wxHelpProvider *helpProvider);
    static wxHelpProvider *Get();

    // Virtuals...
    wxString GetHelp(const wxWindow *window);
    bool ShowHelp(wxWindow *window);
    void AddHelp(wxWindow *window, const wxString& text);
    %name(AddHelpById)void AddHelp(wxWindowID id, const wxString& text);

    %extend { void Destroy() { delete self; } }

};

 
//----------------------------------------------------------------------

class wxSimpleHelpProvider : public wxHelpProvider
{
public:
    wxSimpleHelpProvider();
};


//----------------------------------------------------------------------

// TODO: Add this once the wxHelpController is in wxPython...

//  class WXDLLEXPORT wxHelpControllerHelpProvider : public wxSimpleHelpProvider
//  {
//  public:
//      wxHelpControllerHelpProvider(wxHelpController* hc = NULL);
//      void SetHelpController(wxHelpController* hc);
//      wxHelpController* GetHelpController();
//  };




//----------------------------------------------------------------------
//---------------------------------------------------------------------------
