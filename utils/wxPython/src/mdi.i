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
                     const wxPoint& pos = wxPyDefaultPosition,
                     const wxSize& size = wxPyDefaultSize,
                     long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                     const char* name = "frame");

    %pragma(python) addtomethod = "__init__:wx._StdFrameCallbacks(self)"

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
                    const wxPoint& pos = wxPyDefaultPosition,
                    const wxSize& size = wxPyDefaultSize,
                    long style = wxDEFAULT_FRAME_STYLE,
                    const char* name = "frame");

    %pragma(python) addtomethod = "__init__:wx._StdFrameCallbacks(self)"

    void Activate();
    void Maximize();
    void Restore();

};


//---------------------------------------------------------------------------

class wxMDIClientWindow : public wxWindow {
public:
    wxMDIClientWindow(wxMDIParentFrame* parent, long style = 0);
    %pragma(python) addtomethod = "__init__:wx._StdWindowCallbacks(self)"
    %pragma(python) addtomethod = "__init__:wx._StdOnScrollCallbacks(self)"
};

//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
//
// $Log$
// Revision 1.6  1999/02/25 07:08:34  RD
// wxPython version 2.0b5
//
// Revision 1.5  1998/12/15 20:41:21  RD
// Changed the import semantics from "from wxPython import *" to "from
// wxPython.wx import *"  This is for people who are worried about
// namespace pollution, they can use "from wxPython import wx" and then
// prefix all the wxPython identifiers with "wx."
//
// Added wxTaskbarIcon for wxMSW.
//
// Made the events work for wxGrid.
//
// Added wxConfig.
//
// Added wxMiniFrame for wxGTK, (untested.)
//
// Changed many of the args and return values that were pointers to gdi
// objects to references to reflect changes in the wxWindows API.
//
// Other assorted fixes and additions.
//
// Revision 1.4  1998/10/02 06:40:41  RD
//
// Version 0.4 of wxPython for MSW.
//
// Revision 1.3  1998/08/18 19:48:18  RD
// more wxGTK compatibility things.
//
// It builds now but there are serious runtime problems...
//
// Revision 1.2  1998/08/15 07:36:39  RD
// - Moved the header in the .i files out of the code that gets put into
// the .cpp files.  It caused CVS conflicts because of the RCS ID being
// different each time.
//
// - A few minor fixes.
//
// Revision 1.1  1998/08/09 08:25:51  RD
// Initial version
//
//
