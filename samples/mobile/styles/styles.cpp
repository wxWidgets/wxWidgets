/////////////////////////////////////////////////////////////////////////////
// Name:        styles.cpp
// Author:      Robert Roebling
// Created:     04/07/02
// Copyright:   
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "styles.cpp"
#endif

// For compilers that support precompilation
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/image.h"

// Include private headers
#include "styles.h"

//------------------------------------------------------------------------------
// MyFrame
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame,wxFrame)
    EVT_MENU(ID_ABOUT, MyFrame::OnAbout)
    EVT_MENU(ID_QUIT, MyFrame::OnQuit)
    EVT_CLOSE(MyFrame::OnCloseWindow)
END_EVENT_TABLE()

MyFrame::MyFrame( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxFrame( parent, id, title, position, size, style )
{
    // Create menu and status bar.
    CreateMyMenuBar();
    CreateStatusBar(1);
    SetStatusText( "Welcome to Styles!" );
    
    wxImage image;
    image.LoadFile( "marble.jpg", wxBITMAP_TYPE_JPEG );
    
    wxBitmap bitmap( image );
#ifdef __WXUNIVERSAL__
    SetBackground( bitmap, 0, wxTILE );
#endif
    
    new wxStaticText( this, -1, "This is text", wxPoint( 20,50 ) );
    
    new wxCheckBox( this, -1, "This is a checkbox", wxPoint( 20,70 ) );
}

void MyFrame::CreateMyMenuBar()
{
    wxMenu *file_menu = new wxMenu;
    file_menu->Append( ID_ABOUT, "About...", "Program info" );
    file_menu->AppendSeparator();
    file_menu->Append( ID_QUIT, "Quit...", "Quit program" );

    wxMenuBar *menu_bar = new wxMenuBar();
    menu_bar->Append( file_menu, "&File" );
    
    SetMenuBar( menu_bar );
}

void MyFrame::OnAbout( wxCommandEvent &event )
{
}

void MyFrame::OnQuit( wxCommandEvent &event )
{
     Close( TRUE );
}

void MyFrame::OnCloseWindow( wxCloseEvent &event )
{
    Destroy();
}

//------------------------------------------------------------------------------
// MyApp
//------------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

MyApp::MyApp()
{
}

bool MyApp::OnInit()
{
    wxInitAllImageHandlers();

    SetVendorName("Free world");
    SetAppName("Styles");
    
    MyFrame *frame = new MyFrame( NULL, -1, "Styles", wxPoint(20,20), wxSize(500,340) );
    frame->Show( TRUE );
    
    return TRUE;
}

int MyApp::OnExit()
{
    return 0;
}

