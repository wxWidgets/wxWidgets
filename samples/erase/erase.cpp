/////////////////////////////////////////////////////////////////////////////
// Name:        erase.cpp
// Purpose:     Erase wxWidgets sample
// Author:      Robert Roebling
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma implementation "erase.cpp"
    #pragma interface "erase.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------
// the application icon
#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
    #include "mondrian.xpm"
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};


class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

private:
    DECLARE_EVENT_TABLE()
};


class MyCanvas : public wxScrolledWindow
{
public:
    MyCanvas( MyFrame *parent );

    void OnPaint( wxPaintEvent &event );
    void OnChar( wxKeyEvent &event );
    void OnEraseBackground( wxEraseEvent &event );

    wxBitmap    m_bitmap;
    wxString    m_text;

private:
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

enum
{
    // menu items
    ID_MENU_QUIT = 1,
};


// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame(_T("Erase sample"),
                                 wxPoint(50, 50), wxSize(450, 340));

    frame->Show(true);

    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_MENU_QUIT,  MyFrame::OnQuit)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
END_EVENT_TABLE()

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, wxID_ANY, title, pos, size)
{
    SetIcon(wxICON(mondrian));

    wxMenu *menuFile = new wxMenu(_T(""), wxMENU_TEAROFF);

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT, _T("&About...\tCtrl-A"), _T("Show about dialog"));

    menuFile->Append(ID_MENU_QUIT, _T("E&xit\tAlt-X"), _T("Quit this program"));

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxWidgets erase sample!"));
#endif // wxUSE_STATUSBAR

    (void)new MyCanvas( this );
}


void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("This sample shows how you can draw custom background."),
                 _T("About Erase Sample"), wxOK | wxICON_INFORMATION, this);
}


BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_PAINT(  MyCanvas::OnPaint)
    EVT_CHAR(  MyCanvas::OnChar)
    EVT_ERASE_BACKGROUND(  MyCanvas::OnEraseBackground)
END_EVENT_TABLE()

MyCanvas::MyCanvas( MyFrame *parent )
        : wxScrolledWindow( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                            wxScrolledWindowStyle |
                            wxNO_FULL_REPAINT_ON_RESIZE|
                            wxSUNKEN_BORDER )
{
    SetScrollbars( 10, 10, 40, 100, 0, 0 );

    m_bitmap = wxBitmap( wxICON(mondrian) );

    new wxStaticBitmap( this, wxID_ANY, m_bitmap, wxPoint(80,20) );
}

void MyCanvas::OnChar( wxKeyEvent &event )
{
#if wxUSE_UNICODE
    if (event.m_uniChar)
    {
        m_text += event.m_uniChar;
        Refresh();
        return;
    }
#endif

    // some test cases
    switch (event.m_keyCode)
    {
        case WXK_UP: m_text += wxT( "<UP>" ); break;
        case WXK_LEFT: m_text += wxT( "<LEFT>" ); break;
        case WXK_RIGHT: m_text += wxT( "<RIGHT>" ); break;
        case WXK_DOWN: m_text += wxT( "<DOWN>" ); break;
        case WXK_RETURN: m_text += wxT( "<ENTER>" ); break;
        default: m_text += (wxChar)event.m_keyCode; break;
    }

}

void MyCanvas::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dc(this);
    PrepareDC( dc );

    dc.SetBrush( *wxBLACK_BRUSH );
    dc.DrawRectangle( 0,0,200,50 );

    dc.DrawBitmap( m_bitmap, 10, 20, true );

    dc.SetTextForeground(*wxBLUE);
    dc.DrawText(_T("This text is drawn from OnPaint"), 65, 65);

    wxString tmp;
    tmp.Printf( _T("Hit any key to display more text: %s"), m_text.c_str() );
    int w,h;
    dc.GetTextExtent( tmp, &w, &h );
    dc.SetBrush( *wxWHITE_BRUSH );
    dc.DrawRectangle( 65, 85, w, h );
    dc.DrawText( tmp, 65, 85 );

#if 0
    wxRegionIterator upd( GetUpdateRegion() );
    while (upd)
    {
        wxLogDebug( _T("Paint: %d %d %d %d"), upd.GetX(), upd.GetY(), upd.GetWidth(), upd.GetHeight() );
        upd ++;
    }
#endif

#if 0
    wxSize size = GetSize();
    wxSize client_size = GetClientSize();
    wxLogDebug( _T("size %d %d client_size %d %d"), size.x, size.y, client_size.x, client_size.y );
#endif

#if 0
    int i;
    dc.SetPen( *wxWHITE_PEN );
    for (i = 0; i < 20; i += 2)
       dc.DrawLine( i,i, i+100,i );

    dc.SetPen( *wxWHITE_PEN );
    for (i = 200; i < 220; i += 2)
       dc.DrawLine( i-200,i, i-100,i );

    wxRegion region( 110, 110, 80, 80 );
    wxRegion hole( 130, 130, 40, 1 );
    region.Intersect( hole );
    dc.SetClippingRegion( region );

    dc.SetBrush( *wxRED_BRUSH );
    dc.DrawRectangle( 100, 100, 200, 200 );

    dc.DestroyClippingRegion();

    dc.SetPen( *wxTRANSPARENT_PEN );

    wxRegion strip( 110, 200, 30, 1 );
    wxRegionIterator it( strip );
    while (it)
    {
        dc.DrawRectangle( it.GetX(), it.GetY(), it.GetWidth(), it.GetHeight() );
        it ++;
    }
#endif // 0
}

void MyCanvas::OnEraseBackground( wxEraseEvent& event )
{
    wxDC& dc = *event.GetDC();
    dc.SetPen(*wxGREEN_PEN);

    // this line is needed, otherwise the junk is left on win the background
    dc.Clear();

    wxSize size = GetClientSize();
    for ( int x = 0; x < size.x; x+= 10 )
    {
        dc.DrawLine(x, 0, x, size.y);
    }

    for ( int y = 0; y < size.y; y+= 10 )
    {
        dc.DrawLine(0, y, size.x, y);
    }

    dc.SetTextForeground(*wxRED);
    dc.DrawText(_T("This text is drawn from OnEraseBackground"), 60, 60);
}

