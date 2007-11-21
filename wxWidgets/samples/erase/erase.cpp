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


class MyCanvas : public wxScrolledWindow
{
public:
    MyCanvas( wxFrame *parent );

    void UseBuffer(bool useBuffer) { m_useBuffer = useBuffer; Refresh(); }
    void EraseBg(bool eraseBg) { m_eraseBg = eraseBg; Refresh(); }

private:
    void OnPaint( wxPaintEvent &event );
    void OnChar( wxKeyEvent &event );
    void OnEraseBackground( wxEraseEvent &event );

    void DoPaint(wxDC& dc);


    wxBitmap    m_bitmap;
    wxString    m_text;

    // use wxMemoryDC in OnPaint()?
    bool m_useBuffer;

    // paint custom background in OnEraseBackground()?
    bool m_eraseBg;


    DECLARE_EVENT_TABLE()
};

class MyFrame : public wxFrame
{
public:
    MyFrame();

    void OnUseBuffer(wxCommandEvent& event);
    void OnEraseBg(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

private:
    MyCanvas *m_canvas;

    DECLARE_EVENT_TABLE()
};


// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

enum
{
    // menu items
    Erase_Menu_UseBuffer = 100,
    Erase_Menu_EraseBg,
    Erase_Menu_Exit = wxID_EXIT,
    Erase_Menu_About = wxID_ABOUT
};


// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame;

    frame->Show(true);

    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Erase_Menu_UseBuffer,  MyFrame::OnUseBuffer)
    EVT_MENU(Erase_Menu_EraseBg,  MyFrame::OnEraseBg)
    EVT_MENU(Erase_Menu_Exit,  MyFrame::OnQuit)
    EVT_MENU(Erase_Menu_About, MyFrame::OnAbout)
END_EVENT_TABLE()

// frame constructor
MyFrame::MyFrame()
       : wxFrame(NULL, wxID_ANY, _T("Erase sample"),
                 wxPoint(50, 50), wxSize(450, 340))
{
    SetIcon(wxICON(mondrian));

    wxMenu *menuFile = new wxMenu(_T(""), wxMENU_TEAROFF);
    menuFile->AppendCheckItem(Erase_Menu_UseBuffer, _T("&Use memory DC\tCtrl-M"));
    menuFile->AppendCheckItem(Erase_Menu_EraseBg, _T("Custom &background\tCtrl-B"));
    menuFile->AppendSeparator();
    menuFile->Append(Erase_Menu_Exit, _T("E&xit\tAlt-X"), _T("Quit this program"));


    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Erase_Menu_About, _T("&About...\tCtrl-A"), _T("Show about dialog"));

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxWidgets erase sample!"));
#endif // wxUSE_STATUSBAR

    m_canvas = new MyCanvas( this );
}


void MyFrame::OnUseBuffer(wxCommandEvent& event)
{
    m_canvas->UseBuffer(event.IsChecked());
}

void MyFrame::OnEraseBg(wxCommandEvent& event)
{
    m_canvas->EraseBg(event.IsChecked());
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

MyCanvas::MyCanvas( wxFrame *parent )
        : wxScrolledWindow( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                            wxScrolledWindowStyle | wxSUNKEN_BORDER )
{
    m_eraseBg =
    m_useBuffer = false;

    SetScrollbars( 10, 10, 40, 100, 0, 0 );

    m_bitmap = wxBitmap( wxICON(mondrian) );

    new wxStaticBitmap( this, wxID_ANY, m_bitmap, wxPoint(80,20) );

    SetFocusIgnoringChildren();
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

void MyCanvas::DoPaint(wxDC& dc)
{
    dc.SetBrush( *wxBLACK_BRUSH );
    dc.DrawRectangle( 10,10,200,50 );

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

void MyCanvas::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dcWin(this);
    PrepareDC( dcWin );

    if ( m_useBuffer )
    {
        const wxSize size = GetClientSize();
        wxMemoryDC dc;
        wxBitmap bmp(size.x, size.y);
        dc.SelectObject(bmp);
        dc.Blit(0, 0, size.x, size.y, &dcWin, 0, 0);
        dc.DrawText(_T("(copy of background)"), 5, 120 );

        DoPaint(dc);

        dcWin.Blit(0, 0, size.x, size.y, &dc, 0, 0);
    }
    else
    {
        DoPaint(dcWin);
    }
}

void MyCanvas::OnEraseBackground( wxEraseEvent& event )
{
    if ( !m_eraseBg )
    {
        event.Skip();
        return;
    }

    wxDC& dc = *event.GetDC();
    dc.SetPen(*wxGREEN_PEN);

    PrepareDC( dc );

    // clear any junk currently displayed
    dc.Clear();

    const wxSize size = GetClientSize();
    for ( int x = 0; x < size.x; x += 15 )
    {
        dc.DrawLine(x, 0, x, size.y);
    }

    for ( int y = 0; y < size.y; y += 15 )
    {
        dc.DrawLine(0, y, size.x, y);
    }

    dc.SetTextForeground(*wxRED);
    dc.DrawText(_T("This text is drawn from OnEraseBackground"), 60, 160);
}

