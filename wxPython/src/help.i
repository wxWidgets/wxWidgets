/////////////////////////////////////////////////////////////////////////////
// Name:        help.i
// Purpose:     Context sensitive help classes, and etc.
//
// Author:      Robin Dunn
//
// Created:     28-July-2001
// RCS-ID:      $Id$
// Copyright:   (c) 2001 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module help

%{
#include "wxPython.h"
#include <wx/cshelp.h>
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import windows.i
%import misc.i
%import controls.i


//----------------------------------------------------------------------

enum {
    wxFRAME_EX_CONTEXTHELP,
    wxDIALOG_EX_CONTEXTHELP,
    wxID_CONTEXT_HELP,
    wxEVT_HELP,
    wxEVT_DETAILED_HELP,
};


%pragma(python) code = "
# Help events
def EVT_HELP(win, id, func):
    win.Connect(id, -1, wxEVT_HELP, func)

def EVT_HELP_RANGE(win, id, id2, func):
    win.Connect(id, id2, wxEVT_HELP, func)

def EVT_DETAILED_HELP(win, id, func):
    win.Connect(id, -1, wxEVT_DETAILED_HELP, func)

def EVT_DETAILED_HELP_RANGE(win, id, id2, func):
    win.Connect(id, id2, wxEVT_DETAILED_HELP, func)
"

//----------------------------------------------------------------------

class wxHelpEvent : public wxCommandEvent
{
public:
    wxHelpEvent(wxEventType type = wxEVT_NULL,
                wxWindowID id = 0,
                const wxPoint& pt = wxDefaultPosition);
    const wxPoint& GetPosition();
    void SetPosition(const wxPoint& pos);
    const wxString& GetLink();
    void SetLink(const wxString& link);
    const wxString& GetTarget();
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
    wxContextHelpButton(wxWindow* parent, wxWindowID id = wxID_CONTEXT_HELP,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxBU_AUTODRAW);
    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
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

    %addmethods { void Destroy() { delete self; } }

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
//----------------------------------------------------------------------
// This file gets appended to the shadow class file.
//----------------------------------------------------------------------

%pragma(python) include="_helpextras.py";

//---------------------------------------------------------------------------
