/////////////////////////////////////////////////////////////////////////////
// Name:        extended.h
// Purpose:     Layout/foldpanelbar sample
// Author:      Jorgen Bodde
// Modified by:
// Created:     24/07/2004
// Copyright:   Jorgen Bodde based upon FoldPanelBar sample (c) Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "wx/toolbar.h"
#include "wx/foldbar/foldpanelbar.h"

wxMenuBar *CreateMenuBar(bool with_window);

enum
{
    ID_COLLAPSEME = 10000,
    ID_APPLYTOALL,
    ID_USE_HGRADIENT,
    ID_USE_VGRADIENT,
    ID_USE_SINGLE,
    ID_USE_RECTANGLE,
    ID_USE_FILLED_RECTANGLE
};

// Define a new application
class MyApp: public wxApp
{
  public:
    bool OnInit(void);
};

class MyCanvas: public wxScrolledWindow
{
  public:
    MyCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size);
    virtual void OnDraw(wxDC& dc);
    void OnEvent(wxMouseEvent& event);

    DECLARE_EVENT_TABLE()
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
    void OnFoldPanelBarDrag(wxSashEvent& event);
    void OnCreateBottomStyle(wxCommandEvent& event);
    void OnCreateNormalStyle(wxCommandEvent& event);

    void OnCollapseMe(wxCommandEvent &event);
    void OnExpandMe(wxCommandEvent &event);

    void OnSlideColour(wxScrollEvent &event);

    void OnStyleChange(wxCommandEvent &event);

protected:
    wxSashLayoutWindow* m_leftWindow1;

private:
    void ReCreateFoldPanel(int fpb_flags);
    wxFoldPanelBar *m_pnl;
    wxButton *m_btn;
    wxCheckBox *m_single;
    wxSlider *m_rslider1, *m_gslider1, *m_bslider1, *m_rslider2, *m_gslider2, *m_bslider2;
    int m_flags;

DECLARE_EVENT_TABLE()
};

class MyChild: public wxMDIChildFrame
{
  public:
    MyCanvas *canvas;
    MyChild(wxMDIParentFrame *parent, const wxString& title, const wxPoint& pos, const wxSize& size, const long style);
    ~MyChild(void);
    void OnActivate(wxActivateEvent& event);
    void OnQuit(wxCommandEvent& event);

DECLARE_EVENT_TABLE()
};

#define FPBTEST_QUIT          1
#define FPBTEST_NEW_WINDOW    2
#define FPBTEST_REFRESH       3
#define FPBTEST_CHILD_QUIT    4
#define FPBTEST_ABOUT         5
#define FPBTEST_TOGGLE_WINDOW 6
#define FPB_BOTTOM_STICK      7
#define FPB_SINGLE_FOLD       8

#define ID_WINDOW_TOP       100
#define ID_WINDOW_LEFT1     101
#define ID_WINDOW_LEFT2     102
#define ID_WINDOW_BOTTOM    103

