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

%{
    // Put some wx default wxChar* values into wxStrings.
    DECLARE_DEF_STRING(FrameNameStr);
%}

//----------------------------------------------------------------------

const int IDM_WINDOWTILE  = 4001;
const int IDM_WINDOWTILEHOR  = 4001;
const int IDM_WINDOWCASCADE = 4002;
const int IDM_WINDOWICONS = 4003;
const int IDM_WINDOWNEXT = 4004;
const int IDM_WINDOWTILEVERT = 4005;
const int wxFIRST_MDI_CHILD = 4100;
const int wxLAST_MDI_CHILD = 4600;



class wxMDIParentFrame : public wxFrame {
public:
    wxMDIParentFrame(wxWindow *parent,
                     const wxWindowID id,
                     const wxString& title,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                     const wxString& name = wxPyFrameNameStr);
    %name(wxPreMDIParentFrame)wxMDIParentFrame();

    bool Create(wxWindow *parent,
                     const wxWindowID id,
                     const wxString& title,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                     const wxString& name = wxPyFrameNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreMDIParentFrame:val._setOORInfo(val)"

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
                    const wxString& name = wxPyFrameNameStr);
    %name(wxPreMDIChildFrame)wxMDIChildFrame();

    bool Create(wxMDIParentFrame* parent,
                    const wxWindowID id,
                    const wxString& title,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxDEFAULT_FRAME_STYLE,
                    const wxString& name = wxPyFrameNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreMDIChildFrame:val._setOORInfo(val)"

    void Activate();
    void Maximize(bool maximize);
    void Restore();

};


//---------------------------------------------------------------------------

class wxMDIClientWindow : public wxWindow {
public:
    wxMDIClientWindow(wxMDIParentFrame* parent, long style = 0);
    %name(wxPreMDIClientWindow)wxMDIClientWindow();

    bool Create(wxMDIParentFrame* parent, long style = 0);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreMDIClientWindow:val._setOORInfo(val)"
};

//---------------------------------------------------------------------------
