/////////////////////////////////////////////////////////////////////////////
// Name:        univ.cpp
// Purpose:     wxUniversal demo
// Author:      Vadim Zeitlin
// Id:          $Id$
// Copyright:   (c) 2000 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

#ifdef __GNUG__
    #pragma implementation "univ.cpp"
    #pragma interface "univ.cpp"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/dcclient.h"

    #include "wx/bmpbuttn.h"
    #include "wx/button.h"
    #include "wx/checkbox.h"
    #include "wx/checklst.h"
    #include "wx/listbox.h"
    #include "wx/radiobox.h"
    #include "wx/radiobut.h"
    #include "wx/scrolbar.h"
    #include "wx/scrolwin.h"
    #include "wx/statbox.h"
    #include "wx/stattext.h"
#endif

#include "wx/statbmp.h"
#include "wx/statline.h"

#include "wx/univ/theme.h"

#define DEBUG_SCROLL
//#define DEBUG_LISTBOX

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

#include "bricks.xpm"
#include "tip.xpm"
#include "open.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    Univ_Button1,
    Univ_Button2
};

// ----------------------------------------------------------------------------
// our classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyUnivApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this method is called when wxWindows is initializing and should be used
    // for the earliest initialization possible
    virtual bool OnInitGui();

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();

    // get the standard bg colour
    const wxColour& GetBgColour() const { return m_colourBg; }

protected:
    wxColour m_colourBg;
};

// Define a new frame type: this is going to be our main frame
class MyUnivFrame : public wxFrame
{
public:
    // ctor(s)
    MyUnivFrame(const wxString& title);

protected:
    // event handlers
    void OnButton(wxCommandEvent& event);
    void OnCheckBox(wxCommandEvent& event);
    void OnRadioBox(wxCommandEvent& event);
    void OnListBox(wxCommandEvent& event);
    void OnLeftUp(wxMouseEvent& event);

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

// Define a new canvas class: we will use it for drawing
class MyUnivCanvas : public wxScrolledWindow
{
public:
    MyUnivCanvas(wxWindow *parent,
                 const wxPoint& pos,
                 const wxSize& size,
                 long style = 0)
        : wxScrolledWindow(parent, -1, pos, size, style) { }

protected:
    void OnPaint(wxPaintEvent& event);

private:
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// misc macros
// ----------------------------------------------------------------------------

IMPLEMENT_APP(MyUnivApp)

WX_USE_THEME(win32);
WX_USE_THEME(gtk);

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyUnivFrame, wxFrame)
    EVT_BUTTON(-1, MyUnivFrame::OnButton)
    EVT_CHECKBOX(-1, MyUnivFrame::OnCheckBox)
    EVT_RADIOBUTTON(-1, MyUnivFrame::OnRadioBox)
    EVT_LISTBOX(-1, MyUnivFrame::OnListBox)

    EVT_LEFT_UP(MyUnivFrame::OnLeftUp)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyUnivCanvas, wxScrolledWindow)
    EVT_PAINT(MyUnivCanvas::OnPaint)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// app class
// ----------------------------------------------------------------------------

bool MyUnivApp::OnInitGui()
{
#ifdef __WXMSW__
    m_colourBg = *wxLIGHT_GREY;
#else
    m_colourBg = wxColour(0xd6d6d6);
#endif

    if ( argc > 1 )
    {
        wxString themeName = argv[1];
        wxTheme *theme = wxTheme::Create(themeName);
        if ( theme )
        {
            // HACK: this will be done by wxTheme itself later, but for now
            //       manually use the right colours
            if ( themeName == _T("gtk") )
                m_colourBg = wxColour(0xd6d6d6);
            else if ( themeName == _T("win32") )
                m_colourBg = *wxLIGHT_GREY;

            wxTheme::Set(theme);
        }
    }

    return wxApp::OnInitGui();
}

bool MyUnivApp::OnInit()
{
    wxFrame *frame = new MyUnivFrame(_T("wxUniversal demo"));
    frame->Show();

#ifdef DEBUG_SCROLL
    wxLog::AddTraceMask(_T("scroll"));
#endif
#ifdef DEBUG_LISTBOX
    wxLog::AddTraceMask(_T("listbox"));
#endif

    return TRUE;
}

// ----------------------------------------------------------------------------
// top level frame class
// ----------------------------------------------------------------------------

MyUnivFrame::MyUnivFrame(const wxString& title)
           : wxFrame(NULL, -1, title, wxDefaultPosition, wxSize(700, 700))
{
    SetBackgroundColour(wxGetApp().GetBgColour());

    wxStaticText *text;

    new wxStaticText(this, _T("Test static text"), wxPoint(10, 10));
    new wxStaticText(this,
                     _T("&Multi line\n(and very very very very long)\nstatic text"),
                     wxPoint(210, 10));

    (new wxStaticText(this, _T("&Disabled text"), wxPoint(10, 30)))->Disable();

    new wxStaticLine(this, wxPoint(190, 10), 50, wxLI_VERTICAL);

    text = new wxStaticText(this, _T("Demo of &border styles:"), wxPoint(10, 60));
    text->SetFont(*wxITALIC_FONT);
    text->SetBackgroundColour(*wxWHITE);
    text->SetForegroundColour(*wxBLUE);

    new wxStaticLine(this, wxPoint(10, 80), 120, wxLI_HORIZONTAL);

    wxCoord x = 10;
    #define CREATE_STATIC_BORDER_DEMO(border) \
        (new wxStaticText(this, -1,  _T(#border), \
                     wxPoint(x, 100), wxSize(70, -1), wx##border##_BORDER)); \
        x += 80

    CREATE_STATIC_BORDER_DEMO(NO);
    CREATE_STATIC_BORDER_DEMO(SIMPLE);
    CREATE_STATIC_BORDER_DEMO(SUNKEN);
    CREATE_STATIC_BORDER_DEMO(RAISED);
    CREATE_STATIC_BORDER_DEMO(STATIC);
    CREATE_STATIC_BORDER_DEMO(DOUBLE);

    #undef CREATE_STATIC_BORDER_DEMO

    wxStaticBox *box = new wxStaticBox(this, _T("&Alignments demo:"),
                                       wxPoint(10, 150),
                                       wxSize(500, 120));
    box->SetForegroundColour(*wxRED);
    box->SetBackground(bricks_xpm, 0, wxTILE);

    x = 15;
    #define CREATE_STATIC_ALIGN_DEMO(align) \
        (new wxStaticText(this, -1,  _T(#align), \
                     wxPoint(x, 175), wxSize(70, 70), \
                     wxSIMPLE_BORDER | wxALIGN_##align)); \
        x += 80

    CREATE_STATIC_ALIGN_DEMO(LEFT);
    CREATE_STATIC_ALIGN_DEMO(RIGHT);
    CREATE_STATIC_ALIGN_DEMO(CENTRE);
    CREATE_STATIC_ALIGN_DEMO(TOP);
    CREATE_STATIC_ALIGN_DEMO(BOTTOM);
    CREATE_STATIC_ALIGN_DEMO(CENTRE_VERTICAL);

    #undef CREATE_STATIC_ALIGN_DEMO

    new wxButton(this, Univ_Button1, _T("&Press me"), wxPoint(10, 300));
    new wxButton(this, Univ_Button2, _T("&And me"), wxPoint(100, 300));

    new wxStaticBitmap(this, tip_xpm, wxPoint(10, 350));
    new wxStaticBitmap(this, -1, tip_xpm, wxPoint(50, 350),
                       wxDefaultSize, wxSUNKEN_BORDER);

#if 0
    wxScrollBar *sb;
    sb = new wxScrollBar(this, -1, wxPoint(200, 300), wxSize(300, -1));
    sb->SetScrollbar(0, 10, 100, 10);
    sb = new wxScrollBar(this, -1, wxPoint(200, 330), wxSize(-1, 150), wxSB_VERTICAL);
    sb->SetScrollbar(50, 50, 100, 10);
#elif 0
    wxWindow *win = new wxWindow(this, -1,
                                 wxPoint(200, 300),
                                 wxSize(300, 150),
                                 wxSUNKEN_BORDER);
    win->SetScrollbar(wxHORIZONTAL, 0, 10, 30);
    win->SetScrollbar(wxVERTICAL, 0, 5, 30);
#else
    wxScrolledWindow *win = new MyUnivCanvas(this,
                                             wxPoint(200, 300),
                                             wxSize(300, 150),
                                             wxSUNKEN_BORDER);
    win->SetScrollbars(10, 10, 100, 100, 0, 0);
    //win->Scroll(10, 0);
#endif

    new wxButton(this, -1, open_xpm, _T("&Open..."), wxPoint(10, 420));

    wxBitmap bmp1(wxTheApp->GetStdIcon(wxICON_INFORMATION)),
             bmp2(wxTheApp->GetStdIcon(wxICON_WARNING)),
             bmp3(wxTheApp->GetStdIcon(wxICON_QUESTION));
    wxBitmapButton *bmpBtn = new wxBitmapButton
                                 (
                                  this, -1,
                                  bmp1,
                                  wxPoint(10, 470)
                                 );
    bmpBtn->SetBitmapSelected(bmp2);
    bmpBtn->SetBitmapFocus(bmp3);

    static const wxString choices[] =
    {
        _T("This"),
        _T("is one of my"),
        _T("really"),
        _T("wonderful"),
        _T("examples"),
    };
    wxListBox *lbox = new wxListBox(this, -1, wxPoint(550, 10), wxDefaultSize,
                                    WXSIZEOF(choices), choices,
                                    wxLB_MULTIPLE | wxLB_ALWAYS_SB);
    lbox = new wxListBox(this, -1, wxPoint(550, 300), wxDefaultSize,
                         0, NULL, wxLB_HSCROLL | wxLB_EXTENDED);
    for ( int i = 1; i <= 20; i++ )
    {
        lbox->Append(wxString::Format(_T("%sentry %d"),
                     i % 10 ? _T("") : _T("very very long "), i));
    }

    new wxCheckBox(this, -1, _T("Check me"), wxPoint(10, 550));
    new wxCheckBox(this, -1, _T("Don't check me"),
                   wxPoint(150, 550), wxDefaultSize,
                   wxALIGN_RIGHT);
    new wxRadioButton(this, -1, _T("Toggle me"), wxPoint(10, 600));
    new wxRadioButton(this, -1, _T("And then me"), wxPoint(150, 600));

    new wxRadioBox(this, -1, _T("&Horizontal"),
                   wxPoint(100, 470), wxDefaultSize,
                   WXSIZEOF(choices), choices,
                   WXSIZEOF(choices),
                   wxRA_SPECIFY_COLS);
    new wxRadioBox(this, -1, _T("&Vertical"),
                   wxPoint(300, 550), wxDefaultSize,
                   WXSIZEOF(choices), choices,
                   WXSIZEOF(choices),
                   wxRA_SPECIFY_ROWS);

    wxCheckListBox *checkLbox = new wxCheckListBox(this, -1,
                                                   wxPoint(500, 550),
                                                   wxDefaultSize,
                                                   WXSIZEOF(choices), choices);
    checkLbox->Check(2);
}

void MyUnivFrame::OnButton(wxCommandEvent& event)
{
    int btn = event.GetId();
    if ( btn == Univ_Button1 || btn == Univ_Button2 )
    {
        wxLogDebug(_T("Button %d pressed."), btn == Univ_Button1 ? 1 : 2);
    }
    else
    {
        wxLogDebug(_T("Another button pressed."));
    }
}

void MyUnivFrame::OnCheckBox(wxCommandEvent& event)
{
    wxLogDebug(_T("Checkbox became %schecked."),
               event.IsChecked() ? _T("") : _T("un"));
}

void MyUnivFrame::OnRadioBox(wxCommandEvent& event)
{
    wxLogDebug(_T("Radio button selected."));
}

void MyUnivFrame::OnListBox(wxCommandEvent& event)
{
    wxLogDebug(_T("Listbox item %d selected."), event.GetInt());
}

void MyUnivFrame::OnLeftUp(wxMouseEvent& event)
{
    if ( event.ControlDown() )
    {
        Close();
    }
}

// ----------------------------------------------------------------------------
// MyUnivCanvas
// ----------------------------------------------------------------------------

void MyUnivCanvas::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);
    PrepareDC(dc);

    static bool s_oddRepaint = TRUE;
    s_oddRepaint = !s_oddRepaint;
    wxCoord x, y;
    GetViewStart(&x, &y);
#ifdef DEBUG_SCROLL
    wxLogDebug("Repainting with %s pen (at %dx%d)",
               s_oddRepaint ? "red" : "green",
               x, y);
#endif // DEBUG_SCROLL
    dc.SetPen(s_oddRepaint ? *wxRED_PEN: *wxGREEN_PEN);
    dc.SetTextForeground(s_oddRepaint ? *wxRED : *wxGREEN);

    dc.DrawLine(0, 0, 1000, 1000);
    dc.DrawText(_T("This is the top of the canvas"), 10, 10);
    dc.DrawLabel(_T("This is the bottom of the canvas"),
                 wxRect(0, 950, 950, 50), wxALIGN_RIGHT | wxBOTTOM);

    event.Skip();
}

