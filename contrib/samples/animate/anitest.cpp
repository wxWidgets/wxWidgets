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

#ifndef __WXMSW__
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
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
    EVT_MENU(wxID_EXIT, MyFrame::OnQuit)
#if wxUSE_FILEDLG
    EVT_MENU(wxID_OPEN, MyFrame::OnOpen)
#endif // wxUSE_FILEDLG

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

    MyFrame* frame = new MyFrame((wxFrame *)NULL, -1, _T("Animation Demo"),
                        wxPoint(-1, -1), wxSize(500, 400),
                        wxDEFAULT_FRAME_STYLE);

    // Give it an icon
    frame->SetIcon(wxICON(mondrian));

    // Make a menubar
    wxMenu *file_menu = new wxMenu;

#if wxUSE_FILEDLG
    file_menu->Append(wxID_OPEN, _T("&Open Animation...\tCtrl+O"), _T("Open a GIF animation"));
#endif // wxUSE_FILEDLG
    file_menu->Append(wxID_EXIT, _T("&Exit\tAlt+X"), _T("Quit the program"));

    wxMenu *help_menu = new wxMenu;
    help_menu->Append(wxID_ABOUT, _T("&About\tF1"));

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, _T("&File"));
    menu_bar->Append(help_menu, _T("&Help"));

    // Associate the menu bar with the frame
    frame->SetMenuBar(menu_bar);

#if wxUSE_STATUSBAR
    frame->CreateStatusBar();
#endif // wxUSE_STATUSBAR

    frame->Show(true);

    SetTopWindow(frame);

    return true;
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
    m_canvas = new MyCanvas(this, wxPoint(0, 0), wxDefaultSize);
#if 0
    m_player.SetDestroyAnimation(false);
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
    (void)wxMessageBox(_T("wxWidgets 2 Animation Demo\n")
                       _T("Author: Julian Smart (c) 2001\n"),
                       _T("About Animation Demo"));
}

#if wxUSE_FILEDLG
void MyFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog dialog(this, _T("Please choose an animated GIF"),
        wxEmptyString, wxEmptyString, wxT("*.gif"), wxFD_OPEN);
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
            wxMessageBox(_T("Sorry, this animation was not a valid animated GIF."));
        }
    }
}
#endif // wxUSE_FILEDLG


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
    SetBackgroundColour(wxColour(_T("YELLOW")));
}

void MyCanvas::OnPaint(wxPaintEvent& WXUNUSED(event))
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
