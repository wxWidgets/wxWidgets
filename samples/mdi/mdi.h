/////////////////////////////////////////////////////////////////////////////
// Name:        mdi.cpp
// Purpose:     MDI sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __WXMSW__
#ifdef __WIN95__
#define wxToolBar wxToolBar95
#else
#define wxToolBar wxToolBarMSW
#endif
#endif

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

#ifdef __WXMSW__

class TestRibbon: public wxToolBar
{
  public:
  TestRibbon(wxFrame *frame, int x = 0, int y = 0, int w = -1, int h = -1,
            long style = wxNO_BORDER, int direction = wxVERTICAL, int RowsOrColumns = 2);
  bool OnLeftClick(int toolIndex, bool toggled);
  void OnMouseEnter(int toolIndex);
  void OnPaint(wxPaintEvent& event);
  
  DECLARE_EVENT_TABLE()
};

#endif

// Define a new frame
class MyFrame: public wxMDIParentFrame
{
  public:
    wxTextCtrl *textWindow;
    
#ifdef __WXMSW__    
    TestRibbon* toolBar;
#endif

    MyFrame(wxWindow *parent, const wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, const long style);
    bool OnClose(void);
    void OnSize(wxSizeEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnNewWindow(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);

DECLARE_EVENT_TABLE()
};

class MyChild: public wxMDIChildFrame
{
  public:
    MyCanvas *canvas;
    MyChild(wxMDIParentFrame *parent, const wxString& title, const wxPoint& pos, const wxSize& size, const long style);
    ~MyChild(void);
    bool OnClose(void);
    void OnSize(wxSizeEvent& event);
    void OnActivate(wxActivateEvent& event);
    void OnQuit(wxCommandEvent& event);

DECLARE_EVENT_TABLE()
};

#define MDI_QUIT        1
#define MDI_NEW_WINDOW  2
#define MDI_REFRESH     3
#define MDI_CHILD_QUIT  4
#define MDI_ABOUT       5
