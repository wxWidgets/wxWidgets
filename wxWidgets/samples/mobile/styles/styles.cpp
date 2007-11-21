/////////////////////////////////////////////////////////////////////////////
// Name:        styles.cpp
// Author:      Robert Roebling
// Created:     04/07/02
// Copyright:
/////////////////////////////////////////////////////////////////////////////

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
#if wxUSE_STATUSBAR
    CreateStatusBar(1);
    SetStatusText( _T("Welcome to Styles!") );
#endif // wxUSE_STATUSBAR

    wxImage image;
    image.LoadFile( _T("marble.jpg"), wxBITMAP_TYPE_JPEG );

    wxBitmap bitmap( image );
#ifdef __WXUNIVERSAL__
    SetBackground( bitmap, 0, wxTILE );
#endif

    new wxStaticText( this, wxID_ANY, _T("This is text"), wxPoint( 20,50 ) );

    new wxCheckBox( this, wxID_ANY, _T("This is a checkbox"), wxPoint( 20,70 ) );
}

void MyFrame::CreateMyMenuBar()
{
    wxMenu *file_menu = new wxMenu;
    file_menu->Append( ID_ABOUT, _T("About..."), _T("Program info") );
    file_menu->AppendSeparator();
    file_menu->Append( ID_QUIT, _T("Quit..."), _T("Quit program") );

    wxMenuBar *menu_bar = new wxMenuBar();
    menu_bar->Append( file_menu, _T("&File") );

    SetMenuBar( menu_bar );
}

void MyFrame::OnAbout( wxCommandEvent &WXUNUSED(event) )
{
}

void MyFrame::OnQuit( wxCommandEvent &WXUNUSED(event) )
{
     Close( true );
}

void MyFrame::OnCloseWindow( wxCloseEvent &WXUNUSED(event) )
{
    Destroy();
}

//------------------------------------------------------------------------------
// MyApp
//------------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    wxInitAllImageHandlers();

    SetVendorName(_T("Free world"));
    SetAppName(_T("Styles"));

    MyFrame *frame = new MyFrame( NULL, wxID_ANY, _T("Styles"), wxPoint(20,20), wxSize(500,340) );
    frame->Show( true );

    return true;
}

int MyApp::OnExit()
{
    return 0;
}

