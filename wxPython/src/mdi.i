/////////////////////////////////////////////////////////////////////////////
// Name:        mdi.i
// Purpose:     MDI related class definitions for wxPython
//
// Author:      Robin Dunn
//
// Created:     5/26/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


%module mdi

%{
#include "helpers.h"
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import misc.i
%import windows.i
%import frames.i

%pragma(python) code = "import wx"

//----------------------------------------------------------------------

class wxMDIParentFrame : public wxFrame {
public:
    wxMDIParentFrame(wxWindow *parent,
                     const wxWindowID id,
                     const wxString& title,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                     const char* name = "frame");

    %pragma(python) addtomethod = "__init__:#wx._StdFrameCallbacks(self)"

    void ActivateNext();
    void ActivatePrevious();
    void ArrangeIcons();
    void Cascade();
    wxMDIChildFrame* GetActiveChild();
    wxMDIClientWindow* GetClientWindow();
    wxWindow* GetToolBar();

    // TODO:  This isn't handled by the standard event-table system...
    //wxMDIClientWindow* OnCreateClient();


#ifdef __WXMSW__
    wxMenu* GetWindowMenu();
    void SetWindowMenu(wxMenu* menu);
    void SetToolBar(wxToolBar* toolbar);
#endif
    void Tile();
};

//---------------------------------------------------------------------------

class wxMDIChildFrame : public wxFrame {
public:
    wxMDIChildFrame(wxMDIParentFrame* parent,
                    const wxWindowID id,
                    const wxString& title,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxDEFAULT_FRAME_STYLE,
                    const char* name = "frame");

    %pragma(python) addtomethod = "__init__:#wx._StdFrameCallbacks(self)"

    void Activate();
    void Maximize(bool maximize);
    void Restore();

};


//---------------------------------------------------------------------------

class wxMDIClientWindow : public wxWindow {
public:
    wxMDIClientWindow(wxMDIParentFrame* parent, long style = 0);
    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"
    %pragma(python) addtomethod = "__init__:#wx._StdOnScrollCallbacks(self)"
};

//---------------------------------------------------------------------------
