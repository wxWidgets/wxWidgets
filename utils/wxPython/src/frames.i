/////////////////////////////////////////////////////////////////////////////
// Name:        frames.i
// Purpose:     SWIG definitions of various window classes
//
// Author:      Robin Dunn
//
// Created:     8/27/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


%module frames

%{
#include "helpers.h"
#include <wx/minifram.h>
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import misc.i
%import gdi.i
%import windows.i
%import stattool.i

%pragma(python) code = "import wx"

//----------------------------------------------------------------------

class wxFrame : public wxWindow {
public:
    wxFrame(wxWindow* parent, const wxWindowID id, const wxString& title,
            const wxPoint& pos = wxPyDefaultPosition,
            const wxSize& size = wxPyDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE,
            char* name = "frame");

    %pragma(python) addtomethod = "__init__:wx._StdFrameCallbacks(self)"

    void Centre(int direction = wxBOTH);
#ifdef __WXMSW__
    void Command(int id);
#endif
    wxStatusBar* CreateStatusBar(int number = 1,
                                 long style = wxST_SIZEGRIP,
                                 wxWindowID id = -1,
                                 char* name = "statusBar");
    wxToolBar* CreateToolBar(long style = wxNO_BORDER|wxTB_HORIZONTAL|wxTB_FLAT,
                             wxWindowID id = -1,
                             char* name = "toolBar");

    wxMenuBar* GetMenuBar();
    wxStatusBar* GetStatusBar();
    wxString GetTitle();
    wxToolBar* GetToolBar();
    void Iconize(bool iconize);
    bool IsIconized();
    void Maximize(bool maximize);
    void SetAcceleratorTable(const wxAcceleratorTable& accel);
    void SetIcon(const wxIcon& icon);
    void SetMenuBar(wxMenuBar* menuBar);
    void SetStatusBar(wxStatusBar *statusBar);
    void SetStatusText(const wxString& text, int number = 0);
    void SetStatusWidths(int LCOUNT, int* LIST); // uses typemap
    void SetTitle(const wxString& title);
    void SetToolBar(wxToolBar* toolbar);

};

//---------------------------------------------------------------------------

class wxMiniFrame : public wxFrame {
public:
    wxMiniFrame(wxWindow* parent, const wxWindowID id, const wxString& title,
                const wxPoint& pos = wxPyDefaultPosition,
                const wxSize& size = wxPyDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                char* name = "frame");

    %pragma(python) addtomethod = "__init__:wx._StdFrameCallbacks(self)"
};


//---------------------------------------------------------------------------





