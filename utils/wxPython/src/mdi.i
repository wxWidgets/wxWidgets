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

    void ActivateNext();
    void ActivatePrevious();
    void ArrangeIcons();
    void Cascade();
    void GetClientSize(int* OUTPUT, int* OUTPUT);
    wxMDIChildFrame* GetActiveChild();
    wxMDIClientWindow* GetClientWindow();
    wxWindow* GetToolBar();

    // TODO:  This isn't handled by the standard event-table system...
    //wxMDIClientWindow* OnCreateClient();

    void SetToolBar(wxToolBar* toolbar);
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

    void Activate();
    void Maximize();
    void Restore();
};


//---------------------------------------------------------------------------

class wxMDIClientWindow : public wxWindow {
public:
    wxMDIClientWindow(wxMDIParentFrame* parent, long style = 0);
};

//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
//
// $Log$
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
