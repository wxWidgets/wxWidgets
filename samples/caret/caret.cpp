/////////////////////////////////////////////////////////////////////////////
// Name:        caret.cpp
// Purpose:     wxCaret sample
// Author:      Robert Roebling
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all <standard< wxWindows headers
#ifndef WX_PRECOMP
    #include <wx/wx.h>

    #include <wx/log.h>
#endif

#include "wx/caret.h"

// ----------------------------------------------------------------------------
// ressources
// ----------------------------------------------------------------------------
// the application icon
#if defined(__WXGTK__) || defined(__WXMOTIF__)
    #include "mondrian.xpm"
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

// MyCanvas is a canvas on which you can type
class MyCanvas: public wxScrolledWindow
{
public:
    MyCanvas() { }
    MyCanvas( wxWindow *parent );
    ~MyCanvas();

    wxChar& CharAt(int x, int y) { return *(m_text + x + m_xChars * y); }

    // caret movement
    void Home() { m_xCaret = 0; }
    void End() { m_xCaret = m_xChars - 1; }
    void FirstLine() { m_yCaret = 0; }
    void LastLine() { m_yCaret = m_yChars - 1; }
    void PrevChar() { if ( !m_xCaret-- ) { End(); PrevLine(); } }
    void NextChar() { if ( ++m_xCaret == m_xChars ) { Home(); NextLine(); } }
    void PrevLine() { if ( !m_yCaret-- ) LastLine(); }
    void NextLine() { if ( ++m_yCaret == m_yChars ) FirstLine(); }

    // event handlers
    void OnPaint( wxPaintEvent &event );
    void OnSize( wxSizeEvent &event );
    void OnChar( wxKeyEvent &event );

private:
    wxFont   m_font;

    // the margin around the text (looks nicer)
    int      m_xMargin, m_yMargin;

    // size (in pixels) of one character
    long     m_widthChar, m_heightChar;

    // position (in text coords) of the caret
    int      m_xCaret, m_yCaret;

    // the size (in text coords) of the window
    int      m_xChars, m_yChars;

    // the text
    wxChar  *m_text;

    DECLARE_DYNAMIC_CLASS(MyCanvas)
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Minimal_Quit = 1,
    Minimal_About,
    Minimal_Test1,
    Minimal_Test2,

    // controls start here (the numbers are, of course, arbitrary)
    Minimal_Text = 1000,
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // Create the main application window
    MyFrame *frame = new MyFrame("Minimal wxWindows App",
                                 wxPoint(50, 50), wxSize(450, 340));

    // Show it and tell the application that it's our main window
    // @@@ what does it do exactly, in fact? is it necessary here?
    frame->Show(TRUE);
    SetTopWindow(frame);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.
    return TRUE;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    // set the frame icon
    SetIcon(wxICON(mondrian));

    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    menuFile->Append(Minimal_About, "&About...\tCtrl-A", "Show about dialog");
    menuFile->AppendSeparator();
    menuFile->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
    
    (void) new MyCanvas( this );

    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText("Welcome to wxWindows!");
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("This is the about dialog of minimal sample.\n")
                _T("Welcome to %s")
#ifdef wxBETA_NUMBER
               _T(" (beta %d)!")
#endif // wxBETA_NUMBER
               , wxVERSION_STRING
#ifdef wxBETA_NUMBER
               , wxBETA_NUMBER
#endif // wxBETA_NUMBER
              );

    wxMessageBox(msg, "About Minimal", wxOK | wxICON_INFORMATION, this);
}


// ----------------------------------------------------------------------------
// MyCanvas
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(MyCanvas, wxScrolledWindow)

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_PAINT(MyCanvas::OnPaint)
    EVT_SIZE(MyCanvas::OnSize)
    EVT_CHAR(MyCanvas::OnChar)
END_EVENT_TABLE()

MyCanvas::MyCanvas( wxWindow *parent )
        : wxScrolledWindow( parent, -1,
                            wxDefaultPosition, wxDefaultSize,
                            wxSUNKEN_BORDER )
{
    m_text = (wxChar *)NULL;

    SetBackgroundColour(* wxWHITE);

    m_font = *wxNORMAL_FONT;

    wxClientDC dc(this);
    dc.SetFont( m_font );
    m_heightChar = dc.GetCharHeight();
    m_widthChar = dc.GetCharWidth();

    wxCaret *caret = new wxCaret(this, m_widthChar, m_heightChar);
    SetCaret(caret);

    m_xCaret = m_yCaret =
    m_xChars = m_yChars = 0;

    m_xMargin = m_yMargin = 5;
    caret->Move(m_xMargin, m_yMargin);
    caret->Show();
}

MyCanvas::~MyCanvas()
{
    free(m_text);
}

void MyCanvas::OnSize( wxSizeEvent &event )
{
    m_xChars = (event.GetSize().x - 2*m_xMargin) / m_widthChar;
    m_yChars = (event.GetSize().y - 2*m_yMargin) / m_heightChar;
    if ( !m_xChars )
        m_xChars = 1;
    if ( !m_yChars )
        m_yChars = 1;

    free(m_text);
    m_text = (wxChar *)calloc(m_xChars * m_yChars, sizeof(wxChar));

    wxFrame *frame = wxDynamicCast(GetParent(), wxFrame);

    if ( frame && frame->GetStatusBar() )
    {
        wxString msg;
        msg.Printf(_T("Panel size is (%d, %d)"), m_xChars, m_yChars);

        frame->SetStatusText(msg, 1);
    }

    event.Skip();
}

void MyCanvas::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dc( this );
    PrepareDC( dc );

    dc.SetFont( m_font );

    for ( int y = 0; y < m_yChars; y++ )
    {
        wxString line;

        for ( int x = 0; x < m_xChars; x++ )
        {
            wxChar ch = CharAt(x, y);
            if ( !ch )
                ch = _T(' ');
            line += ch;
        }

        dc.DrawText( line, m_xMargin, m_yMargin + y * m_heightChar );
    }
}

void MyCanvas::OnChar( wxKeyEvent &event )
{
    switch ( event.KeyCode() )
    {
        case WXK_LEFT:
            PrevChar();
            break;

        case WXK_RIGHT:
            NextChar();
            break;

        case WXK_UP:
            PrevLine();
            break;

        case WXK_DOWN:
            NextLine();
            break;

        case WXK_HOME:
            Home();
            break;

        case WXK_END:
            End();
            break;

        case WXK_RETURN:
            Home();
            NextLine();
            break;

        default:
            if ( wxIsprint(event.KeyCode()) )
            {
                CharAt(m_xCaret, m_yCaret) = (wxChar)event.KeyCode();
                NextChar();
            }
            else
            {
                // don't refresh
                return;
            }
    }

    wxLogStatus(_T("Caret is at (%d, %d)"), m_xCaret, m_yCaret);

    GetCaret()->Move(m_xMargin + m_xCaret * m_widthChar,
                     m_yMargin + m_yCaret * m_heightChar);

    Refresh();
}

