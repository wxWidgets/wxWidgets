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
    bool InitToolbar(wxToolBar* toolBar);
};

// Define a new frame
class MyFrame: public wxFrame
{
public:
    MyFrame(wxFrame *parent, wxWindowID id = -1, const wxString& title = "wxToolBar Sample",
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE);

    void OnCloseWindow(wxCloseEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnToolLeftClick(wxCommandEvent& event);
    void OnToolEnter(wxCommandEvent& event);

private:
    wxTextCtrl*         m_textWindow;

DECLARE_EVENT_TABLE()
};

#define ID_TOOLBAR  500

