/*
 * Program: wxPlotWindow
 *
 * Author: Robert Roebling
 *
 * Copyright: (C) 1999, Robert Roebling
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

#include "wx/plot/plot.h"

#include "wx/image.h"
#include "wx/listctrl.h"
#include "wx/sizer.h"
#include "wx/log.h"
#include "wx/intl.h"

#include <math.h>

// derived classes

class MyPlotCurve;
class MyFrame;
class MyApp;

// MyPlotCurve

class MyPlotCurve: public wxPlotCurve
{
public:
    MyPlotCurve( int offsetY, double startY, double endY ) : 
        wxPlotCurve( offsetY, startY, endY ) {}
    
    virtual wxInt32 GetStartX()
        { return 0; }
    virtual wxInt32 GetEndX()
        { return 7000; }
    
    virtual double GetY( wxInt32 x )
        { 
            double dx = x;
            dx /= 100;
            return sin( dx );
        }
};

// MyFrame

class MyFrame: public wxFrame
{
public:
    MyFrame();

    void OnAbout( wxCommandEvent &event );
    void OnQuit( wxCommandEvent &event );
    
    void OnPlotClick( wxPlotEvent &event );
    void OnPlotDClick( wxPlotEvent &event );

    wxPlotWindow   *m_plot;
#if wxUSE_LOG
    wxTextCtrl     *m_log;
#endif // wxUSE_LOG

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
  EVT_MENU             (ID_ABOUT, MyFrame::OnAbout)
  EVT_MENU             (ID_QUIT,  MyFrame::OnQuit)
  EVT_PLOT_CLICKED         ( -1,  MyFrame::OnPlotClick)
  EVT_PLOT_DOUBLECLICKED   ( -1,  MyFrame::OnPlotDClick)
END_EVENT_TABLE()

MyFrame::MyFrame()
       : wxFrame( (wxFrame *)NULL, -1, _T("wxPlotWindow sample"),
                  wxPoint(20,20), wxSize(470,500) )
{
    wxMenu *file_menu = new wxMenu();
    file_menu->Append( ID_ABOUT, _T("&About.."));
    file_menu->Append( ID_QUIT, _T("E&xit\tAlt-X"));

    wxMenuBar *menu_bar = new wxMenuBar();
    menu_bar->Append(file_menu, _T("&File"));

    SetMenuBar( menu_bar );

#if wxUSE_STATUSBAR
    CreateStatusBar(2);
    int widths[] = { -1, 100 };
    SetStatusWidths( 2, widths );
#endif // wxUSE_STATUSBAR

    m_plot = new wxPlotWindow( this, -1, wxPoint(0,0), wxSize(100,100), wxSUNKEN_BORDER | wxPLOT_DEFAULT );
    m_plot->SetUnitsPerValue( 0.01 );
//    m_plot->SetScrollOnThumbRelease( TRUE );

    m_plot->Add( new MyPlotCurve( 0,  -1.5, 1.5 ) );
    m_plot->Add( new MyPlotCurve( 50, -1.5, 1.5 ) );
    wxPlotOnOffCurve *oo = new wxPlotOnOffCurve( 10 );
    oo->Add( 10, 20 );
    oo->Add( 25, 30 );
    oo->Add( 100, 400 );
    oo->Add( 1000, 2000 );
    m_plot->Add( oo );
  
#if wxUSE_LOG
    m_log = new wxTextCtrl( this, -1, _T("This is the log window.\n"), wxPoint(0,0), wxSize(100,100), wxTE_MULTILINE );
    wxLog *old_log = wxLog::SetActiveTarget( new wxLogTextCtrl( m_log ) );
    delete old_log;
#endif // wxUSE_LOG
    
    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );
    
    topsizer->Add( m_plot, 1, wxEXPAND );
#if wxUSE_LOG
    topsizer->Add( m_log, 0, wxEXPAND );
#endif // wxUSE_LOG

    SetAutoLayout( true );
    SetSizer( topsizer );
}

void MyFrame::OnQuit( wxCommandEvent &WXUNUSED(event) )
{
  Close( true );
}

void MyFrame::OnAbout( wxCommandEvent &WXUNUSED(event) )
{
  (void)wxMessageBox( _T("wxPlotWindow Demo\n")
                      _T("Robert Roebling (c) 1999,2000"),
                      _T("About wxPlotWindow Demo"), wxICON_INFORMATION | wxOK );
}

void MyFrame::OnPlotClick( wxPlotEvent &event )
{
    double x = event.GetPosition() * m_plot->GetUnitsPerValue();
    double y = event.GetCurve()->GetY( event.GetPosition() );
    wxLogMessage( _T("Clicked on curve at x coordinate: %f, value: %f"), x, y );
}

void MyFrame::OnPlotDClick( wxPlotEvent &event )
{
    double x = event.GetPosition() * m_plot->GetUnitsPerValue();
    double y = event.GetCurve()->GetY( event.GetPosition() );
    wxLogMessage( _T("Double clicked on curve at x coordinate: %f, value: %f"), x, y );
}

//-----------------------------------------------------------------------------
// MyApp
//-----------------------------------------------------------------------------

bool MyApp::OnInit()
{
    wxFrame *frame = new MyFrame();
    frame->Show( TRUE );

    return TRUE;
}

