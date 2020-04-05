/////////////////////////////////////////////////////////////////////////////
// Name:        anitest.cpp
// Purpose:     Animation sample
// Author:      Julian Smart
// Modified by: Francesco Montorsi
// Created:     02/07/2001
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

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

#include "wx/aboutdlg.h"
#include "wx/artprov.h"
#include "wx/colordlg.h"
#include "wx/wfstream.h"

#include "anitest.h"

#if !wxUSE_ANIMATIONCTRL
    #error Cannot compile this sample if wxAnimationCtrl is not enabled
#endif

#ifdef wxHAS_NATIVE_ANIMATIONCTRL
    #include "wx/generic/animate.h"
#endif

wxIMPLEMENT_APP(MyApp);

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
    ID_SET_BGCOLOR,
    ID_USE_GENERIC
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_PLAY, MyFrame::OnPlay)
    EVT_MENU(ID_SET_NULL_ANIMATION, MyFrame::OnSetNullAnimation)
    EVT_MENU(ID_SET_INACTIVE_BITMAP, MyFrame::OnSetInactiveBitmap)
    EVT_MENU(ID_SET_NO_AUTO_RESIZE, MyFrame::OnSetNoAutoResize)
    EVT_MENU(ID_SET_BGCOLOR, MyFrame::OnSetBgColor)
#ifdef wxHAS_NATIVE_ANIMATIONCTRL
    EVT_MENU(ID_USE_GENERIC, MyFrame::OnUseGeneric)
#endif // wxHAS_NATIVE_ANIMATIONCTRL

    EVT_MENU(wxID_STOP, MyFrame::OnStop)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
    EVT_MENU(wxID_EXIT, MyFrame::OnQuit)
#if wxUSE_FILEDLG
    EVT_MENU(wxID_OPEN, MyFrame::OnOpen)
#endif // wxUSE_FILEDLG

    EVT_SIZE(MyFrame::OnSize)
    EVT_UPDATE_UI(wxID_ANY, MyFrame::OnUpdateUI)
wxEND_EVENT_TABLE()

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

    MyFrame* frame = new MyFrame((wxFrame *)NULL, wxID_ANY, "Animation Demo",
                                 wxDefaultPosition, wxSize(500, 400),
                                 wxDEFAULT_FRAME_STYLE);
    frame->Show(true);

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
       : wxFrame(parent, id, title, pos, size, style)
{
    SetIcon(wxICON(sample));

    // Make a menubar
    wxMenu *file_menu = new wxMenu;

#if wxUSE_FILEDLG
    file_menu->Append(wxID_OPEN, "&Open Animation...\tCtrl+O", "Loads an animation");
#endif // wxUSE_FILEDLG
    file_menu->Append(wxID_EXIT);

    wxMenu *play_menu = new wxMenu;
    play_menu->Append(ID_PLAY, "Play\tCtrl+P", "Play the animation");
    play_menu->Append(wxID_STOP, "Stop\tCtrl+S", "Stop the animation");
    play_menu->AppendSeparator();
    play_menu->Append(ID_SET_NULL_ANIMATION, "Set null animation",
                      "Sets the empty animation in the control");
    play_menu->AppendCheckItem(ID_SET_INACTIVE_BITMAP, "Set inactive bitmap",
                               "Sets an inactive bitmap for the control");
    play_menu->AppendCheckItem(ID_SET_NO_AUTO_RESIZE, "Set no autoresize",
                               "Tells the control not to resize automatically");
    play_menu->Append(ID_SET_BGCOLOR, "Set background colour...",
                      "Sets the background colour of the control");

#ifdef wxHAS_NATIVE_ANIMATIONCTRL
    play_menu->AppendSeparator();
    play_menu->AppendCheckItem(ID_USE_GENERIC, "Use &generic animation\tCtrl+G",
                               "Selects whether native or generic version is used");
#endif // wxHAS_NATIVE_ANIMATIONCTRL

    wxMenu *help_menu = new wxMenu;
    help_menu->Append(wxID_ABOUT);

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, "&File");
    menu_bar->Append(play_menu, "&Animation");
    menu_bar->Append(help_menu, "&Help");

    // Associate the menu bar with this frame
    SetMenuBar(menu_bar);

#if wxUSE_STATUSBAR
    CreateStatusBar();
#endif // wxUSE_STATUSBAR

    // use a wxBoxSizer otherwise wxFrame will automatically
    // resize the m_animationCtrl to fill its client area on
    // user resizes
    wxSizer *sz = new wxBoxSizer(wxVERTICAL);
    sz->Add(new wxStaticText(this, wxID_ANY, "wxAnimationCtrl:"),
            wxSizerFlags().Centre().Border());

    m_animationCtrl = new wxAnimationCtrl(this, wxID_ANY);
    if (m_animationCtrl->LoadFile("throbber.gif"))
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
    {
        wxLogError("Invalid animation");
    }
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
        RecreateAnimation(style);
    }
}

void MyFrame::OnSetBgColor(wxCommandEvent& WXUNUSED(event))
{
    wxColour clr = wxGetColourFromUser(this, m_animationCtrl->GetBackgroundColour(),
                                       "Choose the background colour");

    if (clr.IsOk())
        m_animationCtrl->SetBackgroundColour(clr);
}

void MyFrame::RecreateAnimation(long style)
{
    // save status of the control before destroying it

    // We can't reuse the existing animation if we're switching from native to
    // generic control or vice versa (as indicated by the absence of change in
    // the style, which is the only other reason we can get called). We could
    // save the file name we loaded it from and recreate it, of course, but for
    // now, for simplicity, just start without any animation in this case.
    wxAnimation curr;
#ifdef wxHAS_NATIVE_ANIMATIONCTRL
    if ( style != m_animationCtrl->GetWindowStyle() )
        curr = m_animationCtrl->GetAnimation();
#endif // wxHAS_NATIVE_ANIMATIONCTRL

    wxBitmap inactive = m_animationCtrl->GetInactiveBitmap();
    wxColour bg = m_animationCtrl->GetBackgroundColour();

    // destroy & rebuild
    wxAnimationCtrlBase *old = m_animationCtrl;

#ifdef wxHAS_NATIVE_ANIMATIONCTRL
    if ( GetMenuBar()->IsChecked(ID_USE_GENERIC) )
        m_animationCtrl = new wxGenericAnimationCtrl(this, wxID_ANY, curr,
                                                     wxDefaultPosition,
                                                     wxDefaultSize,
                                                     style);
    else
#endif // wxHAS_NATIVE_ANIMATIONCTRL
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

#ifdef wxHAS_NATIVE_ANIMATIONCTRL

void MyFrame::OnUseGeneric(wxCommandEvent& WXUNUSED(event))
{
    RecreateAnimation(m_animationCtrl->GetWindowStyle());
}

#endif // wxHAS_NATIVE_ANIMATIONCTRL

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    wxAboutDialogInfo info;
    info.SetName(_("wxAnimationCtrl and wxAnimation sample"));
    info.SetDescription(_("This sample program demonstrates the usage of wxAnimationCtrl"));
    info.SetCopyright("(C) 2006 Julian Smart");

    info.AddDeveloper("Julian Smart");
    info.AddDeveloper("Guillermo Rodriguez Garcia");
    info.AddDeveloper("Francesco Montorsi");

    wxAboutBox(info, this);
}

#if wxUSE_FILEDLG
void MyFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog dialog(this, "Please choose an animation",
                        wxEmptyString, wxEmptyString, "*.gif;*.ani", wxFD_OPEN);
    if (dialog.ShowModal() == wxID_OK)
    {
        wxString filename(dialog.GetPath());

        wxAnimation temp(m_animationCtrl->CreateAnimation());
        if (!temp.LoadFile(filename))
        {
            wxLogError("Sorry, this animation is not a valid format for wxAnimation.");
            return;
        }

        m_animationCtrl->SetAnimation(temp);
        m_animationCtrl->Play();

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

