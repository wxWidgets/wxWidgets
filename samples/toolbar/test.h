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
    bool OnInit();
    bool InitToolbar(wxToolBar* toolBar, bool smallicons = FALSE);
};

// Define a new frame
class MyFrame: public wxFrame
{
public:
    MyFrame(wxFrame *parent,
            wxWindowID id = -1,
            const wxString& title = "wxToolBar Sample",
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE);

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnToggleToolbar(wxCommandEvent& event);
    void OnEnablePrint(wxCommandEvent& event) { DoEnablePrint(); }
    void OnToggleHelp(wxCommandEvent& event) { DoToggleHelp(); }

    void OnToolLeftClick(wxCommandEvent& event);
    void OnToolEnter(wxCommandEvent& event);

private:
    void DoEnablePrint();
    void DoToggleHelp();

    bool                m_smallToolbar;
    wxTextCtrl*         m_textWindow;

    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

const int ID_TOOLBAR = 500;

enum
{
    IDM_TOOLBAR_TOGGLETOOLBAR = 200,
    IDM_TOOLBAR_ENABLEPRINT,
    IDM_TOOLBAR_TOGGLEHELP
};
