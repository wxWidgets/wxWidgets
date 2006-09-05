/////////////////////////////////////////////////////////////////////////////
// Name:        servlet.cpp
// Purpose:     Minimal wxWindows OLE server sample
// Author:      Robert Roebling
// Modified by:
// Created:     20/04/99
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// For OLE stuff
#include "wxole.h"

#if defined(__WXGTK__) || defined(__WXMOTIF__)
    #include "mondrian.xpm"
#endif

//----------------------------------------------------------------------------
// MyOleControl
//----------------------------------------------------------------------------

class MyOleControl : public wxOleControl
{
public:

  MyOleControl();
  
  void OnPaint( wxPaintEvent &event );

private:
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// MyOleServer
//----------------------------------------------------------------------------

class MyOleServer : public wxOleServer
{
public:

  MyOleServer() : wxOleServer( "servlet" ) { }
  
  wxOleControl *CreateOleControl() { return new MyOleControl(); }
};

//----------------------------------------------------------------------------
// MyApp
//----------------------------------------------------------------------------

class MyApp : public wxApp
{
public:

    MyApp();
    virtual ~MyApp();

    virtual bool OnInit();
    
    wxOleServerEnv  *m_oleEnv;
    MyOleServer     *m_oleServer;
};

//----------------------------------------------------------------------------
// main
//----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

//----------------------------------------------------------------------------
// MyApp
//----------------------------------------------------------------------------

MyApp::MyApp()
{
}

MyApp::~MyApp()
{
    delete m_oleEnv;
    delete m_oleServer;
}

#include "gtk/gtk.h"

bool MyApp::OnInit()
{
    m_oleEnv = new wxOleServerEnv( "MyServer", "1.0" );
    m_oleServer = new MyOleServer();

    /* how do we get outta here ? */
    for (;;) wxYield();

    return TRUE;
}

//----------------------------------------------------------------------------
// MyOleControl
//----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyOleControl, wxOleControl)
    EVT_PAINT(MyOleControl::OnPaint)
END_EVENT_TABLE()

MyOleControl::MyOleControl() :
  wxOleControl( -1 )
{
  (void)new wxButton( this, -1, "Ole, Ole", wxPoint(5,40), wxSize(120,-1) );
  (void)new wxButton( this, -1, "Greetings", wxPoint(5,70), wxSize(120,-1) );
}
  
void MyOleControl::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
  wxPaintDC dc(this);
  dc.SetFont( wxFont( 24, wxDECORATIVE, wxNORMAL, wxNORMAL, FALSE, "charter" ) );
  dc.DrawText( "wxWidgets rules!", 5, 5 );
}
    
