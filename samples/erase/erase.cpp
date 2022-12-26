/////////////////////////////////////////////////////////////////////////////
// Name:        samples/erase/erase.cpp
// Purpose:     Erase wxWidgets sample
// Author:      Robert Roebling, Vadim Zeitlin
// Created:     04/01/98
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


// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/custombgwin.h"
#include "wx/dcbuffer.h"
#include "wx/artprov.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class MyApp : public wxApp
{
public:
    virtual bool OnInit() override;
};


class MyCanvas : public wxCustomBackgroundWindow<wxScrolledWindow>
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

    // use wxMemoryDC in OnPaint()?
    bool m_useBuffer;

    // use background bitmap?
    bool m_useBgBmp;

    // erase background in OnPaint()?
    bool m_eraseBgInPaint;


    wxDECLARE_EVENT_TABLE();
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

    wxDECLARE_EVENT_TABLE();
};

class ControlWithTransparency : public wxWindow
{
public:
    ControlWithTransparency(wxWindow *parent,
                            const wxPoint& pos,
                            const wxSize& size)
    {
        wxString reason;
        if ( parent->IsTransparentBackgroundSupported(&reason) )
        {
            SetBackgroundStyle (wxBG_STYLE_TRANSPARENT);
            m_message = "This is custom control with transparency";
        }
        else
        {
            m_message = "Transparency not supported, check tooltip.";
        }

        Create (parent, wxID_ANY, pos, size, wxBORDER_NONE);
        Bind(wxEVT_PAINT, &ControlWithTransparency::OnPaint, this);

        if ( !reason.empty() )
        {
            // This can be only done now, after creating the window.
            SetToolTip(reason);
        }
    }

private:
    void OnPaint( wxPaintEvent& WXUNUSED(event) )
    {
        wxPaintDC dc(this);

        dc.SetPen(*wxRED_PEN);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle(GetClientSize());

        dc.SetTextForeground(*wxBLUE);
        dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
        dc.DrawText(m_message, 0, 2);

        // Draw some bitmap/icon to ensure transparent bitmaps are indeed
        //  transparent on transparent windows
        wxBitmap bmp(wxArtProvider::GetBitmap(wxART_WARNING, wxART_MENU));
        wxIcon icon(wxArtProvider::GetIcon(wxART_GOTO_LAST, wxART_MENU));
        dc.DrawBitmap (bmp, GetSize().x - 1 - bmp.GetWidth(), 2);
        dc.DrawIcon(icon, GetSize().x - 1 - bmp.GetWidth()-icon.GetWidth(), 2);
    }

    wxString m_message;
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

wxIMPLEMENT_APP(MyApp);

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

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
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
wxEND_EVENT_TABLE()

// frame constructor
MyFrame::MyFrame()
       : wxFrame(nullptr, wxID_ANY, "Erase sample",
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
    helpMenu->Append(Erase_Menu_About, "&About\tCtrl-A", "Show about dialog");

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


wxBEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_PAINT(MyCanvas::OnPaint)
    EVT_ERASE_BACKGROUND(MyCanvas::OnEraseBackground)
wxEND_EVENT_TABLE()

MyCanvas::MyCanvas(wxFrame *parent)
{
    Create(parent, wxID_ANY);

    m_useBuffer = false;
    m_useBgBmp = false;
    m_eraseBgInPaint = false;

    SetScrollbars( 10, 10, 40, 100, 0, 0 );

    m_bitmap = wxBitmap( wxICON(sample) );

    new wxStaticBitmap( this, wxID_ANY, m_bitmap, wxPoint(80,20) );

    new wxStaticText(this, wxID_ANY,
                     "Right bitmap is a wxStaticBitmap,\n"
                     "left one drawn directly",
                     wxPoint(150, 20));

    new ControlWithTransparency(this, wxPoint(65, 125), wxSize(350, 22));

    SetFocusIgnoringChildren();
    SetBackgroundColour(*wxCYAN);
}

void MyCanvas::DoPaint(wxDC& dc)
{
    PrepareDC(dc);

    if ( m_eraseBgInPaint )
    {
        dc.SetBackground(*wxLIGHT_GREY);

        // Erase the entire virtual area, not just the client area.
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(GetBackgroundColour());
        dc.DrawRectangle(GetVirtualSize());

        dc.DrawText("Background erased in OnPaint", 65, 110);
    }
    else if ( GetBackgroundStyle() == wxBG_STYLE_PAINT )
    {
        dc.SetTextForeground(*wxRED);
        dc.DrawText("You must enable erasing background in OnPaint to avoid "
                    "display corruption", 65, 110);
    }

    dc.DrawBitmap( m_bitmap, 20, 20, true );

    dc.SetTextForeground(*wxRED);
    dc.DrawText("This text is drawn from OnPaint", 65, 65);
}

void MyCanvas::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    if ( m_useBuffer )
    {
        wxAutoBufferedPaintDC dc(this);
        DoPaint(dc);
    }
    else
    {
        wxPaintDC dc(this);
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

    // clear any junk currently displayed
    dc.Clear();

    PrepareDC( dc );

    const wxSize size = GetVirtualSize();
    for ( int x = 0; x < size.x; x += 15 )
    {
        dc.DrawLine(x, 0, x, size.y);
    }

    for ( int y = 0; y < size.y; y += 15 )
    {
        dc.DrawLine(0, y, size.x, y);
    }

    dc.SetTextForeground(*wxRED);
    dc.SetBackgroundMode(wxBRUSHSTYLE_SOLID);
    dc.DrawText("This text is drawn from OnEraseBackground", 60, 160);
}

