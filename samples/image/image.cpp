/*
 * Program: image
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

#include "wx/image.h"

// derived classes

class MyFrame;
class MyApp;

// MyCanvas

class MyCanvas: public wxScrolledWindow
{
public:
    MyCanvas() {};
    MyCanvas( wxWindow *parent, wxWindowID, const wxPoint &pos, const wxSize &size );
    ~MyCanvas();
    void OnPaint( wxPaintEvent &event );

    wxBitmap  *my_horse;
    wxBitmap  *my_square;

    DECLARE_DYNAMIC_CLASS(MyCanvas)
    DECLARE_EVENT_TABLE()
};

// MyFrame

class MyFrame: public wxFrame
{
public:
    MyFrame();

    void OnAbout( wxCommandEvent &event );
    void OnQuit( wxCommandEvent &event );

    MyCanvas         *m_canvas;

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

// MyCanvas

IMPLEMENT_DYNAMIC_CLASS(MyCanvas, wxScrolledWindow)

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
  EVT_PAINT(MyCanvas::OnPaint)
END_EVENT_TABLE()

MyCanvas::MyCanvas( wxWindow *parent, const wxWindowID id,
                    const wxPoint &pos, const wxSize &size )
        : wxScrolledWindow( parent, id, pos, size, wxSUNKEN_BORDER )
{
  SetBackgroundColour(* wxWHITE);

  wxBitmap bitmap( 100, 100 );

  wxMemoryDC dc;
  dc.SelectObject( bitmap );
  dc.SetBrush( wxBrush( "orange", wxSOLID ) );
  dc.SetPen( *wxWHITE_PEN );
  dc.DrawRectangle( 0, 0, 100, 100 );
  dc.SelectObject( wxNullBitmap );

  wxString dir("");

#ifdef __WXGTK__
  dir = wxString("../");
#endif

//#ifdef __WXMOTIF__
  dir = wxString("../");
//#endif

#ifndef __WXMOTIF__
  wxImage image( bitmap );
  image.SaveFile( dir + wxString("test.png"), wxBITMAP_TYPE_PNG );
#else
  wxImage image;
#endif
  
  image.LoadFile( dir + wxString("horse.png"), wxBITMAP_TYPE_PNG );
  my_horse = new wxBitmap( image.ConvertToBitmap() );
  
#ifndef __WXMOTIF__
  image.LoadFile( dir + wxString("test.png"), wxBITMAP_TYPE_PNG );
  my_square = new wxBitmap( image.ConvertToBitmap() );
#endif
}

MyCanvas::~MyCanvas()
{
  delete my_horse;
#ifndef __WXMOTIF__
  delete my_square;
#endif
}

void MyCanvas::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
  wxPaintDC dc( this );
  PrepareDC( dc );

#ifndef __WXMOTIF__
  dc.DrawText( "Loaded image", 30, 100 );
  if (my_square->Ok()) dc.DrawBitmap( *my_square, 30, 120 );

  dc.DrawText( "Drawn directly", 150, 100 );
  dc.SetBrush( wxBrush( "orange", wxSOLID ) );
  dc.SetPen( *wxWHITE_PEN );
  dc.DrawRectangle( 150, 120, 100, 100 );
#endif

  if (my_horse->Ok()) dc.DrawBitmap( *my_horse, 30, 240 );
}

// MyFrame

const int ID_QUIT  = 108;
const int ID_ABOUT = 109;

IMPLEMENT_DYNAMIC_CLASS( MyFrame, wxFrame )

BEGIN_EVENT_TABLE(MyFrame,wxFrame)
  EVT_MENU    (ID_ABOUT, MyFrame::OnAbout)
  EVT_MENU    (ID_QUIT,  MyFrame::OnQuit)
END_EVENT_TABLE()

MyFrame::MyFrame()
       : wxFrame( (wxFrame *)NULL, -1, "wxImage sample",
                  wxPoint(20,20), wxSize(470,360) )
{
  wxMenu *file_menu = new wxMenu();
  file_menu->Append( ID_ABOUT, "About..");
  file_menu->Append( ID_QUIT, "Exit");

  wxMenuBar *menu_bar = new wxMenuBar();
  menu_bar->Append(file_menu, "File");

  SetMenuBar( menu_bar );

  CreateStatusBar(2);
  int widths[] = { -1, 100 };
  SetStatusWidths( 2, widths );

  m_canvas = new MyCanvas( this, -1, wxPoint(0,0), wxSize(10,10) );
  m_canvas->SetScrollbars( 10, 10, 50, 50 );
}

void MyFrame::OnQuit( wxCommandEvent &WXUNUSED(event) )
{
  Close( TRUE );
}

void MyFrame::OnAbout( wxCommandEvent &WXUNUSED(event) )
{
  (void)wxMessageBox( "wxImage demo\n"
                      "Robert Roebling (c) 1998",
                      "About wxImage Demo", wxICON_INFORMATION | wxOK );
}

//-----------------------------------------------------------------------------
// MyApp
//-----------------------------------------------------------------------------

bool MyApp::OnInit()
{
  wxImage::AddHandler( new wxPNGHandler );

  wxFrame *frame = new MyFrame();
  frame->Show( TRUE );

  return TRUE;
}

