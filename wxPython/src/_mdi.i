/////////////////////////////////////////////////////////////////////////////
// Name:        _mdi.i
// Purpose:     SWIG interface for MDI related class definitions
//
// Author:      Robin Dunn
//
// Created:     26-May-1998
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
%newgroup

%{
#include <wx/mdi.h>
%}

//---------------------------------------------------------------------------

#define IDM_WINDOWTILE          4001
#define IDM_WINDOWTILEHOR       4001
#define IDM_WINDOWCASCADE       4002
#define IDM_WINDOWICONS         4003
#define IDM_WINDOWNEXT          4004
#define IDM_WINDOWTILEVERT      4005
#define IDM_WINDOWPREV          4006
#define wxFIRST_MDI_CHILD       4100
#define wxLAST_MDI_CHILD        4600



MustHaveApp(wxMDIParentFrame);

class wxMDIParentFrame : public wxFrame {
public:
    %pythonAppend wxMDIParentFrame         "self._setOORInfo(self)"
    %pythonAppend wxMDIParentFrame()       ""

    wxMDIParentFrame(wxWindow *parent,
                     const wxWindowID id=-1,
                     const wxString& title = wxPyEmptyString,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                     const wxString& name = wxPyFrameNameStr);
    %RenameCtor(PreMDIParentFrame, wxMDIParentFrame());

    bool Create(wxWindow *parent,
                     const wxWindowID id=-1,
                     const wxString& title = wxPyEmptyString,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                     const wxString& name = wxPyFrameNameStr);


    void ActivateNext();
    void ActivatePrevious();
    void ArrangeIcons();
    void Cascade();
    wxMDIChildFrame* GetActiveChild();
    wxMDIClientWindow* GetClientWindow();
//    wxWindow* GetToolBar();

    // TODO:  This isn't handled by the standard event-table system...
    //wxMDIClientWindow* OnCreateClient();


#ifdef __WXMSW__
    wxMenu* GetWindowMenu();
    void SetWindowMenu(wxMenu* menu);
    void SetToolBar(wxToolBar* toolbar);
#endif
    void Tile(wxOrientation orient = wxHORIZONTAL);

    %property(ActiveChild, GetActiveChild, doc="See `GetActiveChild`");
    %property(ClientWindow, GetClientWindow, doc="See `GetClientWindow`");
//    %property(ToolBar, GetToolBar, doc="See `GetToolBar`");
    
};

//---------------------------------------------------------------------------

MustHaveApp(wxMDIChildFrame);

class wxMDIChildFrame : public wxFrame {
public:
    %pythonAppend wxMDIChildFrame         "self._setOORInfo(self)"
    %pythonAppend wxMDIChildFrame()       ""
    %typemap(out) wxMDIChildFrame*;    // turn off this typemap

    wxMDIChildFrame(wxMDIParentFrame* parent,
                    const wxWindowID id=-1,
                    const wxString& title = wxPyEmptyString,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxDEFAULT_FRAME_STYLE,
                    const wxString& name = wxPyFrameNameStr);
    %RenameCtor(PreMDIChildFrame, wxMDIChildFrame());

    // Turn it back on again
    %typemap(out) wxMDIChildFrame* { $result = wxPyMake_wxObject($1, $owner); }

    bool Create(wxMDIParentFrame* parent,
                    const wxWindowID id=-1,
                    const wxString& title = wxPyEmptyString,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxDEFAULT_FRAME_STYLE,
                    const wxString& name = wxPyFrameNameStr);

    void Activate();
    void Maximize(bool maximize=true);
    void Restore();

};


//---------------------------------------------------------------------------

MustHaveApp(wxMDIClientWindow);

class wxMDIClientWindow : public wxWindow {
public:
    %pythonAppend wxMDIClientWindow         "self._setOORInfo(self)"
    %pythonAppend wxMDIClientWindow()       ""
    %typemap(out) wxMDIClientWindow*;    // turn off this typemap

    wxMDIClientWindow(wxMDIParentFrame* parent, long style = 0);
    %RenameCtor(PreMDIClientWindow, wxMDIClientWindow());

    // Turn it back on again
    %typemap(out) wxMDIClientWindow* { $result = wxPyMake_wxObject($1, $owner); }

    bool Create(wxMDIParentFrame* parent, long style = 0);
};


//---------------------------------------------------------------------------
