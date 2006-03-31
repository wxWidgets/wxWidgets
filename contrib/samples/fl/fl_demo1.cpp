/////////////////////////////////////////////////////////////////////////////
// Name:        fl_demo1.cpp
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by: Sebastian Haase (June 21, 2001)
// Created:     04/11/98
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
#include "wx/fl/controlbar.h"     // core API

// extra plugins
#include "wx/fl/barhintspl.h"    // bevel for bars with "X"s and grooves
#include "wx/fl/rowdragpl.h"     // NC-look with draggable rows
#include "wx/fl/cbcustom.h"      // customization plugin
#include "wx/fl/hintanimpl.h"

// beauty-care
#include "wx/fl/gcupdatesmgr.h"  // smooth d&d
#include "wx/fl/antiflickpl.h"   // double-buffered repaint of decorations
#include "wx/fl/dyntbar.h"       // auto-layout toolbar
#include "wx/fl/dyntbarhnd.h"    // control-bar dimension handler for it

#include "fl_demo1.h"

// comment it out if it breaks, (this is my workaround for MSDev 4.0 linker)

char  wxDummyChar;


IMPLEMENT_APP (MyApp)

bool MyApp::OnInit(void)
{
    MyFrame *frame = new MyFrame(NULL);

    frame->SetBackgroundColour( wxColour(192,192,192) );

    wxMenu *file_menu = new wxMenu;

    file_menu->Append( NEW_TEST_LOAD, _("&Load layouts")  );
    file_menu->Append( NEW_TEST_SAVE, _("&Store layouts") );
    file_menu->Append( NEW_TEST_EXIT, _("E&xit") );

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu,   _("&File"));

    frame->SetMenuBar(menu_bar);

#if wxUSE_STATUSBAR
    frame->CreateStatusBar(3);
#endif // wxUSE_STATUSBAR

    frame->Show(true);

    frame->mpClientWnd->Refresh();

    SetTopWindow(frame);


    wxMessageBox(_("Hello, this demo has a bunch of yet-not-fixed-bugs and missing functionality\n\
The ONLY purpose is to demonstrate self-layouting toolbars,\nflat-bitmapped-buttons and 2-new FL-plugins \
(cbRowDragPlugin & cbBarHintsPlugin)\n\n\
BTW, disabled images and label-text are rendered at run-time") );

    return true;
}

/***** Implementation for class MyFrame *****/

BEGIN_EVENT_TABLE( MyFrame, wxFrame )
    // EVT_CHAR_HOOK(MyFrame::OnKeyDown)
    // EVT_PAINT( MyFrame::OnPaint )
    EVT_MENU( NEW_TEST_SAVE, MyFrame::OnSave )
    EVT_MENU( NEW_TEST_LOAD, MyFrame::OnLoad )
    EVT_MENU( NEW_TEST_EXIT, MyFrame::OnExit )
END_EVENT_TABLE()

void MyFrame::OnLoad( wxCommandEvent& WXUNUSED(event) )
{
    wxMessageBox(_T("Hey - you found a BIG question-mark !!"));
}

void MyFrame::OnSave( wxCommandEvent& WXUNUSED(event) )
{
    wxMessageBox(_("Hey - you found another BIG question-mark !!"));
}

void MyFrame::OnExit( wxCommandEvent& WXUNUSED(event) )
{
    Destroy();
}

wxTextCtrl* MyFrame::CreateTextCtrl( const wxString& value )
{
    wxTextCtrl* pCtrl =

        new wxTextCtrl( this, wxID_ANY, value,
        wxDefaultPosition, wxSize(0,0), wxTE_MULTILINE );

    pCtrl->SetBackgroundColour( wxColour( 255,255,255 ) );

    return pCtrl;
}

MyFrame::MyFrame(wxFrame *frame)
    : wxFrame( frame, wxID_ANY, _("wxWidgets 2.0 wxFrameLayout Test Application"), wxDefaultPosition,
          wxSize( 700, 500 ),
          wxCLIP_CHILDREN | wxMINIMIZE_BOX | wxMAXIMIZE_BOX |
          wxRESIZE_BORDER   | wxSYSTEM_MENU  | wxCAPTION | wxCLOSE_BOX,
          wxT("freimas") )
{
    mpClientWnd = CreateTextCtrl( _("Client window") );

    mpLayout = new wxFrameLayout( this, mpClientWnd );

#if defined(__WXGTK__) || defined(__WXX11__)
    cbCommonPaneProperties props;
    mpLayout->GetPaneProperties( props );

    props.mRealTimeUpdatesOn = false; // real-time OFF!!!

    mpLayout->SetPaneProperties( props, wxALL_PANES );
#endif

    mpLayout->SetUpdatesManager( new cbGCUpdatesMgr() );

    // this is now default...
    //mpLayout->SetMargins( 1,1,1,1 ); // gaps for vertical/horizontal/right/left panes

    // setup plugins for testing
    mpLayout->PushDefaultPlugins();

    mpLayout->AddPlugin( CLASSINFO( cbBarHintsPlugin ) ); // fancy "X"es and bevel for bars
    mpLayout->AddPlugin( CLASSINFO( cbHintAnimationPlugin ) );
    mpLayout->AddPlugin( CLASSINFO( cbRowDragPlugin  ) );
    mpLayout->AddPlugin( CLASSINFO( cbAntiflickerPlugin ) );
    mpLayout->AddPlugin( CLASSINFO( cbSimpleCustomizationPlugin ) );

    // drop in some bars
    cbDimInfo sizes0( 200,45, // when docked horizontally
                      200,85, // when docked vertically
                      175,35, // when floated
                      false,  // the bar is not fixed-size
                      4,      // vertical gap (bar border)
                      4       // horizontal gap (bar border)
                    );

    cbDimInfo sizes1( 150,35, // when docked horizontally
                      150,85, // when docked vertically
                      175,35, // when floated
                      true,   // the bar is not fixed-size
                      4,      // vertical gap (bar border)
                      4       // horizontal gap (bar border)
                    );

    cbDimInfo sizes2( 195,35, // when docked horizontally
                      185,37, // when docked vertically
                      195,35, // when floated
                      true,   // the bar is not fixed-size
                      4,      // vertical gap (bar border)
                      4,      // horizontal gap (bar border)
                      new cbDynToolBarDimHandler()
                    );

    mpLayout->AddBar( CreateTextCtrl(_("Hello")),  // bar window
                      sizes0, FL_ALIGN_TOP,     // alignment ( 0-top,1-bottom, etc)
                      0,                        // insert into 0th row (vert. position)
                      0,                        // offset from the start of row (in pixels)
                      _("InfoViewer1"),            // name to refer in customization pop-ups
                      true
                    );

    mpLayout->AddBar( CreateTextCtrl(_("Bye")),    // bar window
                      sizes0, FL_ALIGN_TOP,     // alignment ( 0-top,1-bottom, etc)
                      1,                        // insert into 0th row (vert. position)
                      0,                        // offset from the start of row (in pixels)
                      _("InfoViewer2"),            // name to refer in customization pop-ups
                      true
                    );

    mpLayout->AddBar( CreateTextCtrl(_("Fixed0")), // bar window
                      sizes1, FL_ALIGN_TOP,     // alignment ( 0-top,1-bottom, etc)
                      0,                        // insert into 0th row (vert. position)
                      0,                        // offset from the start of row (in pixels)
                      _("ToolBar1"),               // name to refer in customization pop-ups
                      true
                    );

    wxDynamicToolBar* pToolBar = new wxDynamicToolBar();

    pToolBar->Create( this, wxID_ANY );

    // 1001-1006 ids of command events fired by added tool-buttons

    pToolBar->AddTool( 1001, wxString(wxT(BMP_DIR)) + wxT("new.bmp") );
    pToolBar->AddSeparator();
    pToolBar->AddTool( 1002, wxString(wxT(BMP_DIR)) + wxT("open.bmp") );
    pToolBar->AddTool( 1003, wxString(wxT(BMP_DIR)) + wxT("save.bmp") );
#if wxUSE_STATLINE
    pToolBar->AddSeparator(new wxMySeparatorLine(pToolBar, wxID_ANY));
#endif // wxUSE_STATLINE
    pToolBar->AddTool( 1004, wxString(wxT(BMP_DIR)) + wxT("cut.bmp") );
    pToolBar->AddTool( 1005, wxString(wxT(BMP_DIR)) + wxT("copy.bmp") );
    pToolBar->AddTool( 1006, wxString(wxT(BMP_DIR)) + wxT("paste.bmp") );


    mpLayout->AddBar( pToolBar,             // bar window (can be NULL)
                      sizes2, FL_ALIGN_TOP, // alignment ( 0-top,1-bottom, etc)
                      0,                    // insert into 0th row (vert. position)
                      0,                    // offset from the start of row (in pixels)
                      wxT("ToolBar2"),           // name to refer in customization pop-ups
                      false
                    );

    mpLayout->EnableFloating( true ); // off, thinking about wxGtk...
}

MyFrame::~MyFrame()
{
    if ( mpLayout)
        delete mpLayout; // should be destroyed manually
}

#define LINE_SIZE 3
#if wxUSE_STATLINE
void wxMySeparatorLine::DoSetSize( int x, int y,
                                   int width, int height,
                                   int sizeFlags)
{
    if (width < height)
    {
        x += (width - LINE_SIZE) / 2;
        width = LINE_SIZE;
    }
    else
    {
        y += (height - LINE_SIZE) / 2;
        height = LINE_SIZE;
    }

    wxStaticLine::DoSetSize(x, y, width, height, sizeFlags);
}
#endif // wxUSE_STATLINE
