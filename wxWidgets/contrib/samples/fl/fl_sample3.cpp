/////////////////////////////////////////////////////////////////////////////
// Name:        fl_sample3.cpp
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

#define ID_BUTT 145
#define ID_BUTT2 146

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

    wxButton * my_butt;

    wxTextCtrl* CreateTextCtrl( const wxString& value );
public:
    MyFrame( wxWindow* parent, const wxChar *title );
    ~MyFrame();

    void OnLoad( wxCommandEvent& event );
    void OnStore( wxCommandEvent& event );
    void OnQuit( wxCommandEvent& event );

    void OnButt( wxCommandEvent& event );
    void OnButt2( wxCommandEvent& event );
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
    EVT_BUTTON( ID_BUTT, MyFrame::OnButt )
    EVT_BUTTON( ID_BUTT2, MyFrame::OnButt2 )
END_EVENT_TABLE()

MyFrame::MyFrame( wxWindow* parent, const wxChar *title )
    : wxFrame( parent, wxID_ANY, title, wxDefaultPosition,
          wxSize( 700, 500 ),
          wxCLIP_CHILDREN | wxMINIMIZE_BOX | wxMAXIMIZE_BOX |
          wxRESIZE_BORDER   | wxSYSTEM_MENU  | wxCAPTION | wxCLOSE_BOX,
          wxT("freimas") )
{

    mpClientWnd = new wxWindow(this, wxID_ANY);
    mpLayout = new wxFrameLayout( this, mpClientWnd );

    ///  mpLayout->PushDefaultPlugins();
    ///  mpLayout->AddPlugin( CLASSINFO( cbBarHintsPlugin ) ); // facny "X"es and beveal for barso
    ///  //mpLayout->AddPlugin( CLASSINFO( cbHintAnimationPlugin ) );

    cbDimInfo sizes( 80,65,     // when docked horizontally
                     80,165,    // when docked vertically
                     180,30,    // when floated
                     true,      // the bar is fixed-size
                     5,         // vertical gap (bar border)
                     5          // horizontal gap (bar border)
                   );


    // drop-in some bars

    for( int i = 1; i <= 11; ++i )
    {
        wxChar buf[4];
        wxSprintf( buf, wxT("%d"), i );
        wxString name = wxString(wxT("Bar-"));
        name += buf;

        sizes.mIsFixed = (i !=3); // every fifth bar is not fixed-size

        if ( !sizes.mIsFixed ) name += wxT(" (flexible)");
        //      mpLayout->AddBar( CreateTextCtrl(name), // bar window
        if(i != 4 && i!= 5 && i!=11) {
            mpLayout->AddBar( new wxTextCtrl(this, wxID_ANY, name), // bar window
                              sizes,
                              i % MAX_PANES, // alignment ( 0-top,1-bottom, etc)
                              0,             // insert into 0th row (vert. position)
                              0,             // offset from the start of row (in pixels)
                              name           // name to refere in customization pop-ups
                            );
        } else if(i==4){
            mpLayout->AddBar( new wxTextCtrl(this, wxID_ANY, name), // bar window
                              cbDimInfo( 100,100, 100,100, 100,100, true, 5, 5),
                              i % MAX_PANES, // alignment ( 0-top,1-bottom, etc)
                              0,             // insert into 0th row (vert. position)
                              0,             // offset from the start of row (in pixels)
                              name           // name to refere in customization pop-ups
                            );
        } else if(i==5) {
            my_butt = new wxButton(this, ID_BUTT, name);
            mpLayout->AddBar( my_butt,       // bar window
                              cbDimInfo( 100,100, 200,200, 400,400, true, 5, 5),
                              i % MAX_PANES, // alignment ( 0-top,1-bottom, etc)
                              0,             // insert into 0th row (vert. position)
                              0,             // offset from the start of row (in pixels)
                              name           // name to refere in customization pop-ups
                            );
        } else if(i==11) {
            mpLayout->AddBar( new wxButton(this, ID_BUTT2, name+wxT("_2")),
                              cbDimInfo( 100,100, 200,200, 400,400, true, 5, 5),
                              i % MAX_PANES, // alignment ( 0-top,1-bottom, etc)
                              0,             // insert into 0th row (vert. position)
                              0,             // offset from the start of row (in pixels)
                              name           // name to refere in customization pop-ups
                            );
        }

        //      mpLayout->RecalcLayout(true);
        //        Layout();
        //        Refresh();
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
    wxTextCtrl* pCtrl = new wxTextCtrl( this, wxID_ANY, value,
                                wxPoint(0,0), wxSize(1,1), wxTE_MULTILINE );

    pCtrl->SetBackgroundColour( wxColour( 255,255,255 ) );

    return pCtrl;
}

void MyFrame::OnLoad( wxCommandEvent& WXUNUSED(event) )
{
    wxMessageBox(_("Hey - you found a BIG question-mark !!"));
}

void MyFrame::OnStore( wxCommandEvent& WXUNUSED(event) )
{
    wxMessageBox(_("Hey - you found another BIG question-mark !!"));
}

void MyFrame::OnQuit( wxCommandEvent& WXUNUSED(event) )
{
    Show( false ); // TRICK:: hide it, to avoid flickered destruction

    Close(true);
}

void MyFrame::OnButt( wxCommandEvent& WXUNUSED(event) )
{
    static int i =0;

    // cbBarInfo* FindBarByName( const wxString& name );

    switch(i % 2) {
    case 0:
        {
            cbBarInfo* x = mpLayout->FindBarByName(wxString(wxT("Bar-1")));
            if(x)
                mpLayout->InverseVisibility(x);
            else
                wxBell();
            break;
        }
    case 1:
        {
            cbBarInfo* x = mpLayout->FindBarByName(wxString(wxT("Bar-6")));
            if(x)
            {
                if(i % 4 == 1)
                {
                    mpLayout->SetBarState(x, wxCBAR_FLOATING, true);
                    //mpLayout->RecalcLayout(true);
                    mpLayout->RepositionFloatedBar(x);
                }
                else
                {
                    mpLayout->SetBarState(x, 0, true);
                    //mpLayout->RecalcLayout(true);
                    //mpLayout->RepositionFloatedBar(x);
                }
                //  //  //  x->mState = wxCBAR_FLOATING;
                //  //  //  mpLayout->ApplyBarProperties(x);
            }
            else
            {
                wxBell();
            }

            break;
        }
    }
    i++;
}

void MyFrame::OnButt2( wxCommandEvent& WXUNUSED(event) )
{
    static int i =0;

    // cbBarInfo* FindBarByName( const wxString& name );

    switch(i % 2) {
    case 0:
        {
            cbBarInfo* x = mpLayout->FindBarByName(wxString(wxT("Bar-1")));
            if(x)
            {
                for(int a=0;a<MAX_BAR_STATES;a++)
                {
                    x->mDimInfo.mSizes[a].x = 200;
                    x->mDimInfo.mSizes[a].y = 200;
                }
                x->mpBarWnd->SetSize(200,200);
                mpLayout->SetBarState(x, wxCBAR_FLOATING, true);  // HACK !!!
                mpLayout->SetBarState(x, 0, true);                // HACK !!!
                wxYield();                                        // HACK !!! needed to resize BEFORE redraw
                mpLayout->RefreshNow( true );                     // HACK !!! needed to trigger redraw
            }
            else
            {
                wxBell();
            }

            break;
        }
    case 1:
        {
            cbBarInfo* x = mpLayout->FindBarByName(wxString(wxT("Bar-1")));
            if(x)
            {
                //mpLayout->InverseVisibility(x);
                for(int a=0;a<MAX_BAR_STATES;a++)
                {
                    // see cbPaneDrawPlugin::OnSizeBarWindow( cbSizeBarWndEvent& event )
                    x->mDimInfo.mSizes[a].x = 10 + 2 + 2*x->mDimInfo.mHorizGap;
                    x->mDimInfo.mSizes[a].y = 10 + 2 + 2*x->mDimInfo.mVertGap;
                }
                x->mpBarWnd->SetSize(10,10);
                mpLayout->SetBarState(x, wxCBAR_FLOATING, true);  // HACK !!!
                mpLayout->SetBarState(x, 0, true);                // HACK !!!
                wxYield();                                        // HACK !!! needed to resize BEFORE redraw
                mpLayout->RefreshNow( true );                     // HACK !!! needed to trigger redraw

                // mpLayout->SetBarState(x, wxCBAR_FLOATING, true);
                // mpLayout->RecalcLayout(true);
                // mpLayout->RepositionFloatedBar(x);
                // mpLayout->RecalcLayout(true);
                // mpLayout->RepositionFloatedBar(x);
                // mpLayout->SetBarState(x, 0, true);
                // wxYield();
                // mpLayout->RefreshNow( true );
                // mpLayout->RecalcLayout(true);
            }
            else
            {
                wxBell();
            }

            break;
        }
    }
    i++;
}
