/////////////////////////////////////////////////////////////////////////////
// Name:        anitest.cpp
// Purpose:     Animation sample
// Author:      Julian Smart
// Modified by:
// Created:     02/07/2001
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__)
    #include "mondrian.xpm"
#endif

#include "anitest.h"

IMPLEMENT_APP(MyApp)

// ---------------------------------------------------------------------------
// global variables
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// event tables
// ---------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ANITEST_ABOUT, MyFrame::OnAbout)
    EVT_MENU(ANITEST_QUIT, MyFrame::OnQuit)
    EVT_MENU(ANITEST_OPEN, MyFrame::OnOpen)

    EVT_SIZE(MyFrame::OnSize)
END_EVENT_TABLE()

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// MyApp
// ---------------------------------------------------------------------------

// Initialise this in OnInit, not statically
bool MyApp::OnInit()
{
    // Create the main frame window

    MyFrame* frame = new MyFrame((wxFrame *)NULL, -1, "Animation Demo",
                        wxPoint(-1, -1), wxSize(500, 400),
                        wxDEFAULT_FRAME_STYLE);

    // Give it an icon
#ifdef __WXMSW__
    frame->SetIcon(wxIcon("mdi_icn"));
#else
    frame->SetIcon(wxIcon( mondrian_xpm ));
#endif

    // Make a menubar
    wxMenu *file_menu = new wxMenu;

    file_menu->Append(ANITEST_OPEN, "&Open Animation...\tCtrl+O", "Open a GIF animation");
    file_menu->Append(ANITEST_QUIT, "&Exit\tAlt+X", "Quit the program");

    wxMenu *help_menu = new wxMenu;
    help_menu->Append(ANITEST_ABOUT, "&About\tF1");

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, "&File");
    menu_bar->Append(help_menu, "&Help");

    // Associate the menu bar with the frame
    frame->SetMenuBar(menu_bar);

    frame->CreateStatusBar();

    frame->Show(TRUE);

    SetTopWindow(frame);

    return TRUE;
}

// ---------------------------------------------------------------------------
// MyFrame
// ---------------------------------------------------------------------------

// Define my frame constructor
MyFrame::MyFrame(wxWindow *parent,
                 const wxWindowID id,
                 const wxString& title,
                 const wxPoint& pos,
                 const wxSize& size,
                 const long style)
       : wxFrame(parent, id, title, pos, size,
                          style | wxNO_FULL_REPAINT_ON_RESIZE)
{
//    m_animation = NULL;
    m_canvas = new MyCanvas(this, wxPoint(0, 0), wxSize(-1, -1));
#if 0
    m_player.SetDestroyAnimation(FALSE);
    m_player.SetWindow(m_canvas);
    m_player.SetPosition(wxPoint(0, 0));
#endif
    m_animationCtrl = new wxGIFAnimationCtrl(m_canvas, -1, wxEmptyString,
        wxPoint(0, 0), wxSize(200, 200));
}

MyFrame::~MyFrame()
{
//    m_player.Stop();
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    (void)wxMessageBox("wxWindows 2 Animation Demo\n"
                       "Author: Julian Smart (c) 2001\n",
                       "About Animation Demo");
}

void MyFrame::OnOpen(wxCommandEvent& event)
{
    wxFileDialog dialog(this, wxT("Please choose an animated GIF"),
        wxEmptyString, wxEmptyString, wxT("*.gif"), wxOPEN);
    if (dialog.ShowModal() == wxID_OK)
    {
        wxString filename(dialog.GetPath());

        m_animationCtrl->Stop();
        if (m_animationCtrl->LoadFile(filename))
        {
            m_animationCtrl->Play();
        }
        else
        {
            wxMessageBox("Sorry, this animation was not a valid animated GIF.");
        }
    }
}


// ---------------------------------------------------------------------------
// MyCanvas
// ---------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_PAINT(MyCanvas::OnPaint)
END_EVENT_TABLE()

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size)
        : wxScrolledWindow(parent, -1, pos, size,
                           wxSUNKEN_BORDER |
                           wxNO_FULL_REPAINT_ON_RESIZE |
                           wxVSCROLL | wxHSCROLL)
{
    SetBackgroundColour(wxColour("YELLOW"));
}

void MyCanvas::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);
#if 0
    MyFrame* frame = (MyFrame*) GetParent();
    if (frame->GetPlayer().IsPlaying())
    {
        frame->GetPlayer().Draw(dc);
    }
#endif
}

