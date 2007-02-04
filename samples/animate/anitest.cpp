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

#include "wx/aboutdlg.h"
#include "wx/artprov.h"
#include "wx/colordlg.h"
#include "anitest.h"

#if !wxUSE_ANIMATIONCTRL
    #error Cannot compile this sample if wxAnimationCtrl is not enabled
#endif


IMPLEMENT_APP(MyApp)

// ---------------------------------------------------------------------------
// global variables
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// event tables
// ---------------------------------------------------------------------------

enum
{
    ID_PLAY = 1,
    ID_SET_NULL_ANIMATION,
    ID_SET_INACTIVE_BITMAP,
    ID_SET_NO_AUTO_RESIZE,
    ID_SET_BGCOLOR
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_PLAY, MyFrame::OnPlay)
    EVT_MENU(ID_SET_NULL_ANIMATION, MyFrame::OnSetNullAnimation)
    EVT_MENU(ID_SET_INACTIVE_BITMAP, MyFrame::OnSetInactiveBitmap)
    EVT_MENU(ID_SET_NO_AUTO_RESIZE, MyFrame::OnSetNoAutoResize)
    EVT_MENU(ID_SET_BGCOLOR, MyFrame::OnSetBgColor)

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
    if ( !wxApp::OnInit() )
        return false;

    // Create the main frame window

    MyFrame* frame = new MyFrame((wxFrame *)NULL, wxID_ANY, _T("Animation Demo"),
                                 wxDefaultPosition, wxSize(500, 400),
                                 wxDEFAULT_FRAME_STYLE);

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
    play_menu->AppendSeparator();
    play_menu->Append(ID_SET_NULL_ANIMATION, _T("Set null animation"),
                      _T("Sets the empty animation in the control"));
    play_menu->AppendCheckItem(ID_SET_INACTIVE_BITMAP, _T("Set inactive bitmap"),
                               _T("Sets an inactive bitmap for the control"));
    play_menu->AppendCheckItem(ID_SET_NO_AUTO_RESIZE, _T("Set no autoresize"),
                               _T("Tells the control not to resize automatically"));
    play_menu->Append(ID_SET_BGCOLOR, _T("Set background colour..."),
                      _T("Sets the background colour of the control"));

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
    // use a wxBoxSizer otherwise wxFrame will automatically
    // resize the m_animationCtrl to fill its client area on
    // user resizes
    wxSizer *sz = new wxBoxSizer(wxVERTICAL);
    sz->Add(new wxStaticText(this, wxID_ANY, wxT("wxAnimationCtrl:")),
            wxSizerFlags().Centre().Border());

    m_animationCtrl = new wxAnimationCtrl(this, wxID_ANY);
    if (m_animationCtrl->LoadFile(wxT("throbber.gif")))
        m_animationCtrl->Play();

    sz->Add(m_animationCtrl, wxSizerFlags().Centre().Border());
    SetSizer(sz);
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

void MyFrame::OnSetNullAnimation(wxCommandEvent& WXUNUSED(event))
{
    m_animationCtrl->SetAnimation(wxNullAnimation);
}

void MyFrame::OnSetInactiveBitmap(wxCommandEvent& event)
{
    if (event.IsChecked())
    {
        // set a dummy bitmap as the inactive bitmap
        wxBitmap bmp = wxArtProvider::GetBitmap(wxART_MISSING_IMAGE);
        m_animationCtrl->SetInactiveBitmap(bmp);
    }
    else
        m_animationCtrl->SetInactiveBitmap(wxNullBitmap);
}

void MyFrame::OnSetNoAutoResize(wxCommandEvent& event)
{
    // recreate the control with the new flag if necessary
    long style = wxAC_DEFAULT_STYLE |
                    (event.IsChecked() ? wxAC_NO_AUTORESIZE : 0);

    if (style != m_animationCtrl->GetWindowStyle())
    {
        // save status of the control before destroying it
        wxAnimation curr = m_animationCtrl->GetAnimation();
        wxBitmap inactive = m_animationCtrl->GetInactiveBitmap();
        wxColour bg = m_animationCtrl->GetBackgroundColour();

        // destroy & rebuild
        wxAnimationCtrl *old = m_animationCtrl;
        m_animationCtrl = new wxAnimationCtrl(this, wxID_ANY, curr,
                                              wxDefaultPosition, wxDefaultSize,
                                              style);

        GetSizer()->Replace(old, m_animationCtrl);
        delete old;

        // load old status in new control
        m_animationCtrl->SetInactiveBitmap(inactive);
        m_animationCtrl->SetBackgroundColour(bg);

        GetSizer()->Layout();
    }
}

void MyFrame::OnSetBgColor(wxCommandEvent& WXUNUSED(event))
{
    wxColour clr = wxGetColourFromUser(this, m_animationCtrl->GetBackgroundColour(),
                                       wxT("Choose the background colour"));

    if (clr.IsOk())
        m_animationCtrl->SetBackgroundColour(clr);
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    wxAboutDialogInfo info;
    info.SetName(_("wxAnimationCtrl and wxAnimation sample"));
    info.SetDescription(_("This sample program demonstrates the usage of wxAnimationCtrl"));
    info.SetCopyright(_T("(C) 2006 Julian Smart"));

    info.AddDeveloper(_T("Julian Smart"));
    info.AddDeveloper(_T("Guillermo Rodriguez Garcia"));
    info.AddDeveloper(_T("Francesco Montorsi"));

    wxAboutBox(info);
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

        GetSizer()->Layout();
    }
}
#endif // wxUSE_FILEDLG

void MyFrame::OnUpdateUI(wxUpdateUIEvent& WXUNUSED(event) )
{
    GetMenuBar()->FindItem(wxID_STOP)->Enable(m_animationCtrl->IsPlaying());
    GetMenuBar()->FindItem(ID_PLAY)->Enable(!m_animationCtrl->IsPlaying());
    GetMenuBar()->FindItem(ID_SET_NO_AUTO_RESIZE)->Enable(!m_animationCtrl->IsPlaying());
}

