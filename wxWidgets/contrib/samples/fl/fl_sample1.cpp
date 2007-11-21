/////////////////////////////////////////////////////////////////////////////
// Name:        fl_sample1.cpp
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by: Sebastian Haase (June 21, 2001)
// Created:     24/11/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/textctrl.h"

// fl headers
#include "wx/fl/controlbar.h"

// plugins used
#include "wx/fl/barhintspl.h"
#include "wx/fl/hintanimpl.h"

#define ID_LOAD  102
#define ID_STORE 103
#define ID_QUIT  104

class MyApp: public wxApp
{
public:
    bool OnInit(void);
};

class MyFrame: public wxFrame
{
protected:
    wxFrameLayout* mpLayout;
    wxWindow*      mpClientWnd;
    wxPanel*       mpInternalFrm;

    wxTextCtrl* CreateTextCtrl( const wxString& value );

public:
    MyFrame( wxWindow* parent, const wxChar *title );
    ~MyFrame();

    void OnLoad( wxCommandEvent& event );
    void OnStore( wxCommandEvent& event );
    void OnQuit( wxCommandEvent& event );

    bool OnClose(void) { return true; }

    DECLARE_EVENT_TABLE()
};

/***** Implementation for class MyApp *****/

IMPLEMENT_APP    (MyApp)

bool MyApp::OnInit(void)
{
    // wxWidgets boiler-plate:

    MyFrame *frame = new MyFrame(NULL, _("wxFrameLayout sample"));

    wxMenu *file_menu = new wxMenu;

    file_menu->Append( ID_LOAD,  _("&Load layout")  );
    file_menu->Append( ID_STORE, _("&Store layout")  );
    file_menu->AppendSeparator();

    file_menu->Append( ID_QUIT, _("E&xit") );

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, _("&File"));

#if wxUSE_STATUSBAR
    frame->CreateStatusBar(3);
#endif // wxUSE_STATUSBAR
    frame->SetMenuBar(menu_bar);

    frame->Show(true);

    SetTopWindow(frame);

    return true;
}

/***** Immlementation for class MyFrame *****/

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU( ID_LOAD,  MyFrame::OnLoad  )
    EVT_MENU( ID_STORE, MyFrame::OnStore )
    EVT_MENU( ID_QUIT,  MyFrame::OnQuit  )
END_EVENT_TABLE()

MyFrame::MyFrame( wxWindow* parent, const wxChar *title )
    : wxFrame( parent, wxID_ANY, title, wxDefaultPosition,
          wxSize( 700, 500 ),
          wxCLIP_CHILDREN | wxMINIMIZE_BOX | wxMAXIMIZE_BOX |
          wxRESIZE_BORDER   | wxSYSTEM_MENU  | wxCAPTION | wxCLOSE_BOX,
          wxT("freimas") )
{
    mpInternalFrm = (wxPanel*)this;

    mpClientWnd = CreateTextCtrl( _("Client window") );

    // btw, creation of internal frame is needed for wxGtk version
    // to act correctly (since menu-bar is a separate window there..)

    mpLayout = new wxFrameLayout( mpInternalFrm, mpClientWnd );

#if defined(__WXGTK__) || defined(__WXX11__)
    // real-time dosn't work well under wxGtk yet
    cbCommonPaneProperties props;
    mpLayout->GetPaneProperties( props );

    props.mRealTimeUpdatesOn = false; // off

    mpLayout->SetPaneProperties( props, wxALL_PANES );
#endif

    mpLayout->PushDefaultPlugins();
    mpLayout->AddPlugin( CLASSINFO( cbBarHintsPlugin ) ); // facny "X"es and beveal for barso
    //mpLayout->AddPlugin( CLASSINFO( cbHintAnimationPlugin ) );

    cbDimInfo sizes( 80,65, // when docked horizontally
                     80,65, // when docked vertically
                     80,30, // when floated
                     true,  // the bar is fixed-size
                     5,     // vertical gap (bar border)
                     5      // horizontal gap (bar border)
                   );

    // drop-in 20 bars
    for( int i = 1; i <= 20; ++i )
    {
        wxChar buf[4];
        wxSprintf( buf, wxT("%d"), i );
        wxString name = wxString(wxT("Bar-"));
        name += buf;

        sizes.mIsFixed = i % 5 > 0; // every fifth bar is not fixed-size

        if ( !sizes.mIsFixed ) name += wxT(" (flexible)");

        mpLayout->AddBar( CreateTextCtrl(name),// bar window
                          sizes, i % MAX_PANES,// alignment ( 0-top,1-bottom, etc)
                          0,                   // insert into 0th row (vert. position)
                          0,                   // offset from the start of row (in pixels)
                          name                 // name to refere in customization pop-ups
                        );
    }
}

MyFrame::~MyFrame()
{
    // layout is not a window, should be released manually
    if ( mpLayout )
        delete mpLayout;
}

wxTextCtrl* MyFrame::CreateTextCtrl( const wxString& value )
{
    wxTextCtrl* pCtrl = new wxTextCtrl( mpInternalFrm, wxID_ANY, value,
                                wxPoint(0,0), wxSize(1,1), wxTE_MULTILINE );

    pCtrl->SetBackgroundColour( wxColour( 255,255,255 ) );

    return pCtrl;
}

void MyFrame::OnLoad( wxCommandEvent& WXUNUSED(event) )
{
    wxMessageBox(wxT("Hey - you found a BIG question-mark !!"));
}

void MyFrame::OnStore( wxCommandEvent& WXUNUSED(event) )
{
    wxMessageBox(wxT("Hey - you found another BIG question-mark !!"));
}

void MyFrame::OnQuit( wxCommandEvent& WXUNUSED(event) )
{
    Show( false ); // TRICK:: hide it, to avoid flickered destruction

    Close(true);
}
