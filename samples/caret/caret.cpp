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
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all <standard< wxWidgets headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/log.h"
#endif

#include "wx/caret.h"
#include "wx/numdlg.h"

// ----------------------------------------------------------------------------
// ressources
// ----------------------------------------------------------------------------
// the application icon
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__)
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

// MyCanvas is a canvas on which you can type
class MyCanvas: public wxScrolledWindow
{
public:
    MyCanvas() { }
    MyCanvas( wxWindow *parent );
    ~MyCanvas();

    wxChar& CharAt(int x, int y) { return *(m_text + x + m_xChars * y); }

    // operations
    void SetFontSize(int fontSize);
    void CreateCaret();
    void MoveCaret(int x, int y);

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
    // move the caret to m_xCaret, m_yCaret
    void DoMoveCaret();

    // update the geometry
    void ChangeSize();

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


// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnSetBlinkTime(wxCommandEvent& event);
    void OnSetFontSize(wxCommandEvent& event);
    void OnCaretMove(wxCommandEvent& event);

private:
    MyCanvas *m_canvas;

    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Caret_Quit = 1,
    Caret_About,
    Caret_SetBlinkTime,
    Caret_SetFontSize,
    Caret_Move,

    // controls start here (the numbers are, of course, arbitrary)
    Caret_Text = 1000
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Caret_Quit,  MyFrame::OnQuit)
    EVT_MENU(Caret_About, MyFrame::OnAbout)
    EVT_MENU(Caret_SetBlinkTime, MyFrame::OnSetBlinkTime)
    EVT_MENU(Caret_SetFontSize, MyFrame::OnSetFontSize)
    EVT_MENU(Caret_Move, MyFrame::OnCaretMove)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
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
    if ( !wxApp::OnInit() )
        return false;

    // create and show the main application window
    MyFrame *frame = new MyFrame(_T("Caret wxWidgets sample"),
                                 wxPoint(50, 50), wxSize(450, 340));

    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, wxID_ANY, title, pos, size)
{
    // set the frame icon
    SetIcon(wxICON(mondrian));

    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    menuFile->Append(Caret_SetBlinkTime, _T("&Blink time...\tCtrl-B"));
    menuFile->Append(Caret_SetFontSize, _T("&Font size...\tCtrl-S"));
    menuFile->Append(Caret_Move, _T("&Move caret\tCtrl-C"));
    menuFile->AppendSeparator();
    menuFile->Append(Caret_About, _T("&About...\tCtrl-A"), _T("Show about dialog"));
    menuFile->AppendSeparator();
    menuFile->Append(Caret_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, _T("&File"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
    
    m_canvas = new MyCanvas(this);

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxWidgets!"));
#endif // wxUSE_STATUSBAR
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("The caret wxWidgets sample.\n(c) 1999 Vadim Zeitlin"),
                 _T("About Caret"), wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnCaretMove(wxCommandEvent& WXUNUSED(event))
{
    m_canvas->MoveCaret(10, 10);
}

void MyFrame::OnSetBlinkTime(wxCommandEvent& WXUNUSED(event))
{
    long blinkTime = wxGetNumberFromUser
                     (
                      _T("The caret blink time is the time between two blinks"),
                      _T("Time in milliseconds:"),
                      _T("wxCaret sample"),
                      wxCaret::GetBlinkTime(), 0, 10000,
                      this
                     );
    if ( blinkTime != -1 )
    {
        wxCaret::SetBlinkTime((int)blinkTime);
        m_canvas->CreateCaret();
        wxLogStatus(this, _T("Blink time set to %ld milliseconds."), blinkTime);
    }
}

void MyFrame::OnSetFontSize(wxCommandEvent& WXUNUSED(event))
{
    long fontSize = wxGetNumberFromUser
                    (
                        _T("The font size also determines the caret size so\nthis demonstrates resizing the caret."),
                        _T("Font size (in points):"),
                        _T("wxCaret sample"),
                        12, 1, 100,
                        this
                    );

    if ( fontSize != -1 )
    {
        m_canvas->SetFontSize((int)fontSize);
    }
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
        : wxScrolledWindow( parent, wxID_ANY,
                            wxDefaultPosition, wxDefaultSize,
                            wxSUNKEN_BORDER )
{
    m_text = (wxChar *)NULL;

    SetBackgroundColour(*wxWHITE);

    SetFontSize(12);

    m_xCaret = m_yCaret =
    m_xChars = m_yChars = 0;

    m_xMargin = m_yMargin = 5;

    CreateCaret();
}

MyCanvas::~MyCanvas()
{
    free(m_text);
}

void MyCanvas::CreateCaret()
{
    wxCaret *caret = new wxCaret(this, m_widthChar, m_heightChar);
    SetCaret(caret);

    caret->Move(m_xMargin, m_yMargin);
    caret->Show();
}

void MyCanvas::SetFontSize(int fontSize)
{
    m_font = wxFont(fontSize, wxFONTFAMILY_TELETYPE,
                    wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

    wxClientDC dc(this);
    dc.SetFont(m_font);
    m_heightChar = dc.GetCharHeight();
    m_widthChar = dc.GetCharWidth();

    wxCaret *caret = GetCaret();
    if ( caret )
    {
        caret->SetSize(m_widthChar, m_heightChar);

        ChangeSize();
    }
}

void MyCanvas::MoveCaret(int x, int y)
{
    m_xCaret = x;
    m_yCaret = y;

    DoMoveCaret();
}

void MyCanvas::DoMoveCaret()
{
    wxLogStatus(_T("Caret is at (%d, %d)"), m_xCaret, m_yCaret);

    GetCaret()->Move(m_xMargin + m_xCaret * m_widthChar,
                     m_yMargin + m_yCaret * m_heightChar);
}

void MyCanvas::OnSize(wxSizeEvent& event)
{
    ChangeSize();

    event.Skip();
}

void MyCanvas::ChangeSize()
{
    wxSize size = GetClientSize();
    m_xChars = (size.x - 2*m_xMargin) / m_widthChar;
    m_yChars = (size.y - 2*m_yMargin) / m_heightChar;
    if ( !m_xChars )
        m_xChars = 1;
    if ( !m_yChars )
        m_yChars = 1;

    free(m_text);
    m_text = (wxChar *)calloc(m_xChars * m_yChars, sizeof(wxChar));

#if wxUSE_STATUSBAR
    wxFrame *frame = wxDynamicCast(GetParent(), wxFrame);

    if ( frame && frame->GetStatusBar() )
    {
        wxString msg;
        msg.Printf(_T("Panel size is (%d, %d)"), m_xChars, m_yChars);
        frame->SetStatusText(msg, 1);
    }
#endif // wxUSE_STATUSBAR
}

// NB: this method is horrible inefficient especially because the caret
//     needs to be redrawn often and in this case we only have to redraw
//     the caret location and not the entire window - in a real program we
//     would use GetUpdateRegion() and iterate over rectangles it contains
void MyCanvas::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxCaretSuspend cs(this);
    wxPaintDC dc( this );
    PrepareDC( dc );
    dc.Clear();

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
    switch ( event.GetKeyCode() )
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
            if ( !event.AltDown() && wxIsprint(event.GetKeyCode()) )
            {
                wxChar ch = (wxChar)event.GetKeyCode();
                CharAt(m_xCaret, m_yCaret) = ch;

                wxCaretSuspend cs(this);
                wxClientDC dc(this);
                dc.SetFont(m_font);
                dc.SetBackgroundMode(wxSOLID); // overwrite old value
                dc.DrawText(ch, m_xMargin + m_xCaret * m_widthChar,
                                m_yMargin + m_yCaret * m_heightChar );

                NextChar();
            }
            else
            {
                event.Skip();
            }
    }

    DoMoveCaret();
}

