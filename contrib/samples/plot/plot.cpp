/////////////////////////////////////////////////////////////////////////////
// Name:        plot.cpp
// Purpose:     wxPlotWindow sample
// Author:      Robert Roebling
// Modified by:
// Created:
// Copyright:   (C) 1999, Robert Roebling
// RCS-ID:      $Id$
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

#include "wx/plot/plot.h"

#include "wx/image.h"
#include "wx/listctrl.h"
#include "wx/sizer.h"
#include "wx/log.h"
#include "wx/intl.h"
#include "wx/math.h"

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

private:

    wxPlotWindow   *m_plot;
#if wxUSE_LOG
    wxTextCtrl     *m_log;
#endif // wxUSE_LOG

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

IMPLEMENT_DYNAMIC_CLASS( MyFrame, wxFrame )

BEGIN_EVENT_TABLE(MyFrame,wxFrame)
    EVT_MENU                 (wxID_ABOUT, MyFrame::OnAbout)
    EVT_MENU                 (wxID_EXIT,  MyFrame::OnQuit)
    EVT_PLOT_CLICKED         (wxID_ANY,   MyFrame::OnPlotClick)
    EVT_PLOT_DOUBLECLICKED   (wxID_ANY,   MyFrame::OnPlotDClick)
END_EVENT_TABLE()

MyFrame::MyFrame()
       : wxFrame( (wxFrame *)NULL, wxID_ANY, _T("wxPlotWindow sample"),
                  wxPoint(20,20), wxSize(470,500) )
{
    wxMenu *fileMenu = new wxMenu();
    fileMenu->Append( wxID_EXIT, _T("E&xit\tAlt-X"), _T("Quit this program"));

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT, _T("&About...\tF1"), _T("Show about dialog"));

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    SetMenuBar( menuBar );

#if wxUSE_STATUSBAR
    CreateStatusBar(2);
    int widths[] = { -1, 100 };
    SetStatusWidths( 2, widths );
#endif // wxUSE_STATUSBAR

    m_plot = new wxPlotWindow( this, wxID_ANY, wxPoint(0,0), wxSize(100,100), wxSUNKEN_BORDER | wxPLOT_DEFAULT );
    m_plot->SetUnitsPerValue( 0.01 );
//    m_plot->SetScrollOnThumbRelease( true );

    //Add a blue, 16pt chart title
    wxString titleText( _T("The Chart Title") );
    wxFont titleFont( *wxNORMAL_FONT );
    titleFont.SetPointSize( 16 );
    wxColour titleColour( *wxBLUE );
    m_plot->AddChartTitle( titleText, titleFont, titleColour );

    m_plot->Add( new MyPlotCurve( 0,  -1.5, 1.5 ) );
    m_plot->Add( new MyPlotCurve( 50, -1.5, 1.5 ) );
    wxPlotOnOffCurve *oo = new wxPlotOnOffCurve( 10 );
    oo->Add( 10, 20 );
    oo->Add( 25, 30 );
    oo->Add( 100, 400 );
    oo->Add( 1000, 2000 );
    m_plot->Add( oo );

#if wxUSE_LOG
    m_log = new wxTextCtrl( this, wxID_ANY, _T("This is the log window.\n"), wxPoint(0,0), wxSize(100,100), wxTE_MULTILINE );
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
    topsizer->Fit(this);
    topsizer->SetSizeHints(this);
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
    frame->Show( true );

    return true;
}

