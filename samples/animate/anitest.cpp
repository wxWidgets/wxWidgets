/////////////////////////////////////////////////////////////////////////////
// Name:        anitest.cpp
// Purpose:     Animation sample
// Author:      Julian Smart
// Modified by: Francesco Montorsi
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
    #include "sample.xpm"
#endif

#include "anitest.h"

IMPLEMENT_APP(MyApp)

// ---------------------------------------------------------------------------
// global variables
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// event tables
// ---------------------------------------------------------------------------

enum
{
    ID_PLAY = 1
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_PLAY, MyFrame::OnPlay)
    EVT_MENU(wxID_STOP, MyFrame::OnStop)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
    EVT_MENU(wxID_EXIT, MyFrame::OnQuit)
#if wxUSE_FILEDLG
    EVT_MENU(wxID_OPEN, MyFrame::OnOpen)
#endif // wxUSE_FILEDLG

    EVT_SIZE(MyFrame::OnSize)
    EVT_UPDATE_UI(wxID_ANY, MyFrame::OnUpdateUI)
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

    MyFrame* frame = new MyFrame((wxFrame *)NULL, wxID_ANY, _T("Animation Demo"));

    // Give it an icon
    frame->SetIcon(wxICON(sample));

    // Make a menubar
    wxMenu *file_menu = new wxMenu;

#if wxUSE_FILEDLG
    file_menu->Append(wxID_OPEN, _T("&Open Animation...\tCtrl+O"), _T("Loads an animation"));
#endif // wxUSE_FILEDLG
    file_menu->Append(wxID_EXIT);

    wxMenu *play_menu = new wxMenu;
    play_menu->Append(ID_PLAY, _T("Play\tCtrl+P"), _T("Play the animation"));
    play_menu->Append(wxID_STOP, _T("Stop\tCtrl+P"), _T("Stop the animation"));

    wxMenu *help_menu = new wxMenu;
    help_menu->Append(wxID_ABOUT);

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, _T("&File"));
    menu_bar->Append(play_menu, _T("&Animation"));
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

#include "wx/wfstream.h"

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
    //m_canvas = new MyCanvas(this, wxPoint(0, 0), wxDefaultSize);

    //wxSizer *sz = new wxBoxSizer(wxVERTICAL);

    m_animationCtrl = new wxAnimationCtrl(this, wxID_ANY, wxNullAnimation,
                                          wxPoint(0,0),wxSize(100,100));
    if (m_animationCtrl->LoadFile(wxT("throbber.gif")))
        m_animationCtrl->Play();

    //sz->Add(m_animationCtrl, 1, wxGROW);

    //SetSizer(sz);
}

MyFrame::~MyFrame()
{
}

void MyFrame::OnPlay(wxCommandEvent& WXUNUSED(event))
{
    if (!m_animationCtrl->Play())
        wxLogError(wxT("Invalid animation"));
}

void MyFrame::OnStop(wxCommandEvent& WXUNUSED(event))
{
    m_animationCtrl->Stop();
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    /*

    FIXME: on wxGTK at least using File->About command it shows
    the message dialog but does not focus it

    */

    (void)wxMessageBox(_T("wxWidgets 2 Animation Demo\n")
                       _T("Author: Julian Smart (c) 2001\n"),
                       _T("About Animation Demo"));
}

#if wxUSE_FILEDLG
void MyFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog dialog(this, _T("Please choose an animation"),
                        wxEmptyString, wxEmptyString, wxT("*.gif;*.ani"), wxFD_OPEN);
    if (dialog.ShowModal() == wxID_OK)
    {
        wxString filename(dialog.GetPath());

        // enable one of the two chunk of codes to test different parts of wxAnimation/wxAnimationCtrl
#if 0
        if (m_animationCtrl->LoadFile(filename))
            m_animationCtrl->Play();
        else
            wxMessageBox(_T("Sorry, this animation is not a valid format for wxAnimation."));
#else
    #if 0
        wxAnimation temp;
        if (!temp.LoadFile(filename))
        {
            wxLogError(wxT("Sorry, this animation is not a valid format for wxAnimation."));
            return;
        }

        m_animationCtrl->SetAnimation(temp);
        m_animationCtrl->Play();
    #else
        wxFileInputStream stream(filename);
        if (!stream.Ok())
        {
            wxLogError(wxT("Sorry, this animation is not a valid format for wxAnimation."));
            return;
        }

        wxAnimation temp;
        if (!temp.Load(stream))
        {
            wxLogError(wxT("Sorry, this animation is not a valid format for wxAnimation."));
            return;
        }

        m_animationCtrl->SetAnimation(temp);
        m_animationCtrl->Play();
    #endif
#endif
    }
}
#endif // wxUSE_FILEDLG

void MyFrame::OnUpdateUI(wxUpdateUIEvent& WXUNUSED(event) )
{
    GetMenuBar()->FindItem(wxID_STOP)->Enable(m_animationCtrl->IsPlaying());
    GetMenuBar()->FindItem(ID_PLAY)->Enable(!m_animationCtrl->IsPlaying());
}

// ---------------------------------------------------------------------------
// MyCanvas
// ---------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    //EVT_PAINT(MyCanvas::OnPaint)
END_EVENT_TABLE()

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size)
        : wxScrolledWindow(parent, wxID_ANY, pos, size,
                           wxSUNKEN_BORDER |
                           wxNO_FULL_REPAINT_ON_RESIZE |
                           wxVSCROLL | wxHSCROLL)
{
    SetBackgroundColour(wxColour(_T("YELLOW")));
}

void MyCanvas::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    //wxPaintDC dc(this);

    //dc.DrawRotatedText(wxT("Background"),
}
