/////////////////////////////////////////////////////////////////////////////
// Name:        samples/erase/erase.cpp
// Purpose:     Erase wxWidgets sample
// Author:      Robert Roebling, Vadim Zeitlin
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Robert Roebling
//              (c) 2009 Vadim Zeitlin
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

#include "wx/dcbuffer.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon
#if !defined(__WXMSW__) && !defined(__WXPM__)
    #include "../sample.xpm"
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
    MyCanvas(wxFrame *parent);

    void UseBuffer(bool useBuffer) { m_useBuffer = useBuffer; Refresh(); }
    bool UsesBuffer() const { return m_useBuffer; }

    void UseBgBitmap(bool useBgBmp)
    {
        m_useBgBmp = useBgBmp;
        SetBackgroundBitmap(m_useBgBmp ? GetBgBitmap() : wxBitmap());

        Refresh();
    }

    void EraseBgInPaint(bool erase) { m_eraseBgInPaint = erase; Refresh(); }

private:
    void OnPaint( wxPaintEvent &event );
    void OnChar( wxKeyEvent &event );
    void OnEraseBackground( wxEraseEvent &event );

    void DoPaint(wxDC& dc);

    // Create an easily recognizable background bitmap.
    static wxBitmap GetBgBitmap()
    {
        static const int BMP_SIZE = 40;

        wxBitmap bmp(BMP_SIZE, BMP_SIZE);
        wxMemoryDC dc(bmp);
        dc.SetBackground(*wxCYAN);
        dc.Clear();

        dc.SetPen(*wxBLUE_PEN);
        dc.DrawLine(0, BMP_SIZE/2, BMP_SIZE/2, 0);
        dc.DrawLine(BMP_SIZE/2, 0, BMP_SIZE, BMP_SIZE/2);
        dc.DrawLine(BMP_SIZE, BMP_SIZE/2, BMP_SIZE/2, BMP_SIZE);
        dc.DrawLine(BMP_SIZE/2, BMP_SIZE, 0, BMP_SIZE/2);

        return bmp;
    }

    wxBitmap    m_bitmap;
    wxString    m_text;

    // use wxMemoryDC in OnPaint()?
    bool m_useBuffer;

    // use background bitmap?
    bool m_useBgBmp;

    // erase background in OnPaint()?
    bool m_eraseBgInPaint;


    DECLARE_EVENT_TABLE()
};

class MyFrame : public wxFrame
{
public:
    MyFrame();

private:
    void OnUseBuffer(wxCommandEvent& event);
    void OnUseBgBitmap(wxCommandEvent& event);
    void OnEraseBgInPaint(wxCommandEvent& event);
    void OnChangeBgStyle(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    // we can only use double-buffering with wxBG_STYLE_PAINT
    void OnUpdateUIUseBuffer(wxUpdateUIEvent& event)
    {
        event.Enable( m_canvas->GetBackgroundStyle() == wxBG_STYLE_PAINT );
    }

    void OnUpdateUIChangeBgStyle(wxUpdateUIEvent& event)
    {
        event.Enable( !m_canvas->UsesBuffer() );
    }

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
    Erase_Menu_UseBgBitmap,
    Erase_Menu_EraseBgInPaint,
    Erase_Menu_BgStyleErase,
    Erase_Menu_BgStyleSystem,
    Erase_Menu_BgStylePaint,
    Erase_Menu_Exit = wxID_EXIT,
    Erase_Menu_About = wxID_ABOUT
};


// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    MyFrame *frame = new MyFrame;

    frame->Show(true);

    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Erase_Menu_UseBuffer, MyFrame::OnUseBuffer)
    EVT_MENU(Erase_Menu_UseBgBitmap, MyFrame::OnUseBgBitmap)
    EVT_MENU(Erase_Menu_EraseBgInPaint, MyFrame::OnEraseBgInPaint)
    EVT_MENU_RANGE(Erase_Menu_BgStyleErase, Erase_Menu_BgStylePaint,
                   MyFrame::OnChangeBgStyle)

    EVT_MENU(Erase_Menu_Exit,  MyFrame::OnQuit)
    EVT_MENU(Erase_Menu_About, MyFrame::OnAbout)

    EVT_UPDATE_UI(Erase_Menu_UseBuffer, MyFrame::OnUpdateUIUseBuffer)
    EVT_UPDATE_UI_RANGE(Erase_Menu_BgStyleErase, Erase_Menu_BgStylePaint,
                        MyFrame::OnUpdateUIChangeBgStyle)
END_EVENT_TABLE()

// frame constructor
MyFrame::MyFrame()
       : wxFrame(NULL, wxID_ANY, "Erase sample",
                 wxPoint(50, 50), wxSize(450, 340))
{
    SetIcon(wxICON(sample));

    wxMenu *menuFile = new wxMenu("", wxMENU_TEAROFF);
    menuFile->AppendCheckItem(Erase_Menu_UseBuffer, "&Use memory DC\tCtrl-M");
    menuFile->AppendCheckItem(Erase_Menu_UseBgBitmap,
                              "Use background &bitmap\tCtrl-B");
    menuFile->AppendCheckItem(Erase_Menu_EraseBgInPaint,
                              "&Erase background in EVT_PAINT\tCtrl-R");
    menuFile->AppendSeparator();
    menuFile->AppendRadioItem(Erase_Menu_BgStyleErase,
                              "Use wxBG_STYLE_&ERASE\tCtrl-E");
    menuFile->AppendRadioItem(Erase_Menu_BgStyleSystem,
                              "Use wxBG_STYLE_&SYSTEM\tCtrl-S");
    menuFile->AppendRadioItem(Erase_Menu_BgStylePaint,
                              "Use wxBG_STYLE_&PAINT\tCtrl-P");
    menuFile->AppendSeparator();
    menuFile->Append(Erase_Menu_Exit, "E&xit\tAlt-X", "Quit this program");


    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Erase_Menu_About, "&About...\tCtrl-A", "Show about dialog");

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, "&File");
    menuBar->Append(helpMenu, "&Help");

    SetMenuBar(menuBar);

    m_canvas = new MyCanvas( this );
}


void MyFrame::OnUseBuffer(wxCommandEvent& event)
{
    m_canvas->UseBuffer(event.IsChecked());
}

void MyFrame::OnUseBgBitmap(wxCommandEvent& event)
{
    m_canvas->UseBgBitmap(event.IsChecked());
}

void MyFrame::OnEraseBgInPaint(wxCommandEvent& event)
{
    m_canvas->EraseBgInPaint(event.IsChecked());
}

void MyFrame::OnChangeBgStyle(wxCommandEvent& event)
{
    int style = wxBG_STYLE_ERASE + event.GetId() - Erase_Menu_BgStyleErase;
    m_canvas->SetBackgroundStyle(static_cast<wxBackgroundStyle>(style));

    m_canvas->Refresh();
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox
    (
        "This sample shows differences between different background styles "
        "and how you may draw custom background.\n"
        "\n"
        "(c) 1998 Robert Roebling\n"
        "(c) 2009 Vadim Zeitlin\n",
        "About Erase Sample",
        wxOK | wxICON_INFORMATION,
        this
    );
}


BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_PAINT(MyCanvas::OnPaint)
    EVT_CHAR(MyCanvas::OnChar)
    EVT_ERASE_BACKGROUND(MyCanvas::OnEraseBackground)
END_EVENT_TABLE()

MyCanvas::MyCanvas(wxFrame *parent)
        : wxScrolledWindow(parent, wxID_ANY)
{
    m_useBuffer = false;
    m_useBgBmp = false;
    m_eraseBgInPaint = false;

    SetScrollbars( 10, 10, 40, 100, 0, 0 );

    m_bitmap = wxBitmap( wxICON(sample) );

    new wxStaticBitmap( this, wxID_ANY, m_bitmap, wxPoint(80,20) );

    new wxStaticText(this, wxID_ANY,
                     "Left bitmap is a wxStaticBitmap,\n"
                     "right one drawn directly",
                     wxPoint(150, 20));

    SetFocusIgnoringChildren();
    SetBackgroundColour(*wxCYAN);
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
    if ( m_eraseBgInPaint )
    {
        dc.SetBackground(*wxLIGHT_GREY);
        dc.Clear();

        dc.DrawText("Background erased in OnPaint", 65, 110);
    }
    else if ( GetBackgroundStyle() == wxBG_STYLE_PAINT )
    {
        dc.SetTextForeground(*wxRED);
        dc.DrawText("You must enable erasing background in OnPaint to avoid "
                    "display corruption", 65, 110);
    }

    dc.SetBrush( *wxBLACK_BRUSH );

    dc.DrawBitmap( m_bitmap, 20, 20, true );

    dc.SetTextForeground(*wxRED);
    dc.DrawText("This text is drawn from OnPaint", 65, 65);

    wxString tmp;
    tmp.Printf("Hit any key to display more text: %s", m_text);

    int w,h;
    dc.GetTextExtent( tmp, &w, &h );
    dc.DrawRectangle( 65, 85, w, h );
    dc.DrawText( tmp, 65, 85 );
}

void MyCanvas::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    if ( m_useBuffer )
    {
        wxAutoBufferedPaintDC dc(this);
        PrepareDC(dc);

        DoPaint(dc);
    }
    else
    {
        wxPaintDC dc(this);
        PrepareDC(dc);

        DoPaint(dc);
    }
}

void MyCanvas::OnEraseBackground( wxEraseEvent& event )
{
    // We must not erase the background ourselves if we asked wxPanel to erase
    // it using a background bitmap.
    if ( m_useBgBmp )
    {
        event.Skip();
        return;
    }

    wxASSERT_MSG
    (
        GetBackgroundStyle() == wxBG_STYLE_ERASE,
        "shouldn't be called unless background style is \"erase\""
    );

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
    dc.SetBackgroundMode(wxSOLID);
    dc.DrawText("This text is drawn from OnEraseBackground", 60, 160);
}

