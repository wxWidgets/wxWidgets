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
#define wxFIRST_MDI_CHILD       4100
#define wxLAST_MDI_CHILD        4600



MustHaveApp(wxMDIParentFrame);

class wxMDIParentFrame : public wxFrame {
public:
    %pythonAppend wxMDIParentFrame         "self._setOORInfo(self)"
    %pythonAppend wxMDIParentFrame()       ""

    wxMDIParentFrame(wxWindow *parent,
                     const wxWindowID id,
                     const wxString& title,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                     const wxString& name = wxPyFrameNameStr);
    %name(PreMDIParentFrame)wxMDIParentFrame();

    bool Create(wxWindow *parent,
                     const wxWindowID id,
                     const wxString& title,
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

MustHaveApp(wxMDIChildFrame);

class wxMDIChildFrame : public wxFrame {
public:
    %pythonAppend wxMDIChildFrame         "self._setOORInfo(self)"
    %pythonAppend wxMDIChildFrame()       ""

    wxMDIChildFrame(wxMDIParentFrame* parent,
                    const wxWindowID id,
                    const wxString& title,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxDEFAULT_FRAME_STYLE,
                    const wxString& name = wxPyFrameNameStr);
    %name(PreMDIChildFrame)wxMDIChildFrame();

    bool Create(wxMDIParentFrame* parent,
                    const wxWindowID id,
                    const wxString& title,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxDEFAULT_FRAME_STYLE,
                    const wxString& name = wxPyFrameNameStr);

    void Activate();
    void Maximize(bool maximize);
    void Restore();

};


//---------------------------------------------------------------------------

MustHaveApp(wxMDIClientWindow);

class wxMDIClientWindow : public wxWindow {
public:
    %pythonAppend wxMDIClientWindow         "self._setOORInfo(self)"
    %pythonAppend wxMDIClientWindow()       ""

    wxMDIClientWindow(wxMDIParentFrame* parent, long style = 0);
    %name(PreMDIClientWindow)wxMDIClientWindow();

    bool Create(wxMDIParentFrame* parent, long style = 0);
};


//---------------------------------------------------------------------------
