/*
 * Program: canvas
 *
 * Author: Robert Roebling
 *
 * Copyright: (C) 1998, Robert Roebling
 *
 */
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/image.h>
#include <wx/file.h>
#include <wx/timer.h>

#include "smile.xpm"

#include "wx/canvas/canvas.h"

// derived classes

class MyFrame;
class MyApp;

// MyFrame

class MyFrame: public wxFrame
{
public:
    MyFrame();
    ~MyFrame();

    void OnAbout( wxCommandEvent &event );
    void OnNewFrame( wxCommandEvent &event );
    void OnQuit( wxCommandEvent &event );
    void OnTimer( wxTimerEvent &event );

    wxCanvas        *m_canvas;
    wxCanvasObject  *m_co;
    wxTimer         *m_timer;

private:
    DECLARE_DYNAMIC_CLASS(MyFrame)
    DECLARE_EVENT_TABLE()
};

// MyApp

class MyApp: public wxApp
{
public:
    virtual bool OnInit();
};

// main program

IMPLEMENT_APP(MyApp)

// MyFrame

const int ID_QUIT  = 108;
const int ID_ABOUT = 109;

IMPLEMENT_DYNAMIC_CLASS( MyFrame, wxFrame )

BEGIN_EVENT_TABLE(MyFrame,wxFrame)
  EVT_MENU    (ID_ABOUT, MyFrame::OnAbout)
  EVT_MENU    (ID_QUIT,  MyFrame::OnQuit)
  EVT_TIMER   (-1,       MyFrame::OnTimer)
END_EVENT_TABLE()

MyFrame::MyFrame()
       : wxFrame( (wxFrame *)NULL, -1, "wxCanvas sample",
                  wxPoint(20,20), wxSize(470,360) )
{
  wxMenu *file_menu = new wxMenu();
  file_menu->Append( ID_ABOUT, "&About...");
  file_menu->AppendSeparator();
  file_menu->Append( ID_QUIT, "E&xit");

  wxMenuBar *menu_bar = new wxMenuBar();
  menu_bar->Append(file_menu, "&File");

  SetMenuBar( menu_bar );

  CreateStatusBar(2);
  int widths[] = { -1, 100 };
  SetStatusWidths( 2, widths );

  m_canvas = new wxCanvas( this, -1, wxPoint(0,0), wxSize(10,10) );

  m_canvas->SetArea( 400, 600 );
  m_canvas->SetColour( 255, 255, 255 );
  
  wxBitmap bitmap( smile_xpm );
  wxImage image( bitmap );
  m_co = new wxCanvasImage( image, 10, 50 );
  m_canvas->Append( m_co );
  
  m_canvas->Append( new wxCanvasImage( image, 40, 50 ) );
  
  wxButton *button = new wxButton( m_canvas, -1, "Hello", wxPoint(80,50) );
  m_canvas->Append( new wxCanvasControl( button ) );

  m_canvas->Append( new wxCanvasText( "Hello", 180, 50, "/home/robert/TrueType/times.ttf", 20 ) );
  
  m_timer = new wxTimer( this );
  m_timer->Start( 100, FALSE );
}

MyFrame::~MyFrame()
{
   delete m_timer;
}

void MyFrame::OnQuit( wxCommandEvent &WXUNUSED(event) )
{
    Close( TRUE );
}

void MyFrame::OnTimer( wxTimerEvent &WXUNUSED(event) )
{
    m_co->Move( m_co->GetX()+1, m_co->GetY() );
    wxWakeUpIdle();
}

void MyFrame::OnAbout( wxCommandEvent &WXUNUSED(event) )
{
   (void)wxMessageBox( "wxCanvas demo\n"
                      "Robert Roebling (c) 1998,2000",
                      "About wxCanvas Demo", wxICON_INFORMATION | wxOK );
}

//-----------------------------------------------------------------------------
// MyApp
//-----------------------------------------------------------------------------

bool MyApp::OnInit()
{
#if wxUSE_LIBPNG
  wxImage::AddHandler( new wxPNGHandler );
#endif

  wxFrame *frame = new MyFrame();
  frame->Show( TRUE );

  return TRUE;
}


