/////////////////////////////////////////////////////////////////////////////
// Name:        sashtest.h
// Purpose:     Layout window/sash sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/toolbar.h"

// Define a new application
class MyApp: public wxApp
{
  public:
    bool OnInit(void) wxOVERRIDE;
};

class MyCanvas: public wxScrolledWindow
{
  public:
    MyCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size);
    virtual void OnDraw(wxDC& dc) wxOVERRIDE;
    void OnEvent(wxMouseEvent& event);

    wxDECLARE_EVENT_TABLE();
};

// Define a new frame
class MyFrame: public wxMDIParentFrame
{
  public:

    MyFrame(wxWindow *parent, const wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, const long style);

    void OnSize(wxSizeEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnNewWindow(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnToggleWindow(wxCommandEvent& event);
    void OnSashDrag(wxSashEvent& event);

protected:
    wxSashLayoutWindow* m_topWindow;
    wxSashLayoutWindow* m_leftWindow1;
    wxSashLayoutWindow* m_leftWindow2;
    wxSashLayoutWindow* m_bottomWindow;

    wxDECLARE_EVENT_TABLE();
};

class MyChild: public wxMDIChildFrame
{
  public:
    MyCanvas *canvas;
    MyChild(wxMDIParentFrame *parent, const wxString& title, const wxPoint& pos, const wxSize& size);
    ~MyChild(void);
    void OnActivate(wxActivateEvent& event);
    void OnQuit(wxCommandEvent& event);

    wxDECLARE_EVENT_TABLE();
};

#define SASHTEST_QUIT        wxID_EXIT
#define SASHTEST_NEW_WINDOW  2
#define SASHTEST_REFRESH     3
#define SASHTEST_CHILD_QUIT  4
#define SASHTEST_ABOUT       wxID_ABOUT
#define SASHTEST_TOGGLE_WINDOW 6

#define ID_WINDOW_TOP       100
#define ID_WINDOW_LEFT1     101
#define ID_WINDOW_LEFT2     102
#define ID_WINDOW_BOTTOM    103

