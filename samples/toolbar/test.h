/////////////////////////////////////////////////////////////////////////////
// Name:        test.h
// Purpose:     wxToolBar sample
// Author:      Julian Smart
// Modified by:
// Created:     23/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// Define a new application
class MyApp: public wxApp
{
  public:
    bool OnInit(void);
};

// Define a new frame
class MyFrame: public wxFrame
{
public:
    MyFrame(wxFrame *parent, wxWindowID id = -1, const wxString& title = "wxToolBar Sample",
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE);

    void OnCloseWindow(wxCloseEvent& event);
    void OnMenuHighlight(wxMenuEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

private:
    wxTextCtrl*         m_textWindow;

DECLARE_EVENT_TABLE()
};

class TestToolBar: public wxToolBar
{
  public:
  TestToolBar(wxFrame *parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxNO_BORDER, int direction = wxVERTICAL, int RowsOrColumns = 2);
  bool OnLeftClick(int toolIndex, bool toggled);
  void OnMouseEnter(int toolIndex);
  void OnPaint(wxPaintEvent& event);
  
  DECLARE_EVENT_TABLE()
};

#define TEST_QUIT                100
#define TEST_ABOUT               101

