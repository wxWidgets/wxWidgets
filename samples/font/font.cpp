/////////////////////////////////////////////////////////////////////////////
// Name:        font.cpp
// Purpose:     wxFont demo
// Author:      Vadim Zeitlin
// Modified by:
// Created:     30.09.99
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
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

#include <wx/fontdlg.h>

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

// MyCanvas is a canvas on which we show the font sample
class MyCanvas: public wxWindow
{
public:
    MyCanvas( wxWindow *parent );
    ~MyCanvas();

    // accessors for the frame
    const wxFont& GetTextFont() const { return m_font; }
    const wxColour& GetColour() const { return m_colour; }
    void SetTextFont(const wxFont& font) { m_font = font; }
    void SetColour(const wxColour& colour) { m_colour = colour; }

    // event handlers
    void OnPaint( wxPaintEvent &event );

private:
    wxColour m_colour;
    wxFont   m_font;

    DECLARE_EVENT_TABLE()
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    // accessors
    MyCanvas *GetCanvas() const { return m_canvas; }

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnSelectFont(wxCommandEvent& event);
    void OnCreateFont(wxCommandEvent& event);

protected:
    MyCanvas *m_canvas;

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

// A custom font dialog which allows to directly edit wxFont proprieties
class MyFontDialog : public wxDialog
{
public:
    MyFontDialog(MyFrame *frame);

    // event handlers
    void OnApply(wxCommandEvent& WXUNUSED(event)) { DoApply(); }

protected:
    void DoApply();

    MyCanvas *m_canvas;

private:
    //DECLARE_EVENT_TABLE() TODO
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Font_Quit = 1,
    Font_About,
    Font_Choose = 100,
    Font_Create
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Font_Quit,  MyFrame::OnQuit)
    EVT_MENU(Font_About, MyFrame::OnAbout)
    EVT_MENU(Font_Choose, MyFrame::OnSelectFont)
    EVT_MENU(Font_Create, MyFrame::OnCreateFont)
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

    menuFile->Append(Font_About, "&About...\tCtrl-A", "Show about dialog");
    menuFile->AppendSeparator();
    menuFile->Append(Font_Quit, "E&xit\tAlt-X", "Quit this program");

    wxMenu *menuFont = new wxMenu;
    menuFont->Append(Font_Choose, "&Select font...\tCtrl-F",
                     "Select a standard font");
    menuFont->Append(Font_Create, "&Create font...\tCtrl-C",
                     "Create a custom font");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuFont, "F&ont");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    m_canvas = new MyCanvas(this);

    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText("Welcome to wxWindows!");
}


// event handlers

void MyFrame::OnCreateFont(wxCommandEvent& WXUNUSED(event))
{
    MyFontDialog dialog(this);

    (void)dialog.ShowModal();
}

void MyFrame::OnSelectFont(wxCommandEvent& WXUNUSED(event))
{
    wxFontData data;
    data.SetInitialFont(m_canvas->GetTextFont());
    data.SetColour(m_canvas->GetColour());

    wxFontDialog dialog(this, &data);
    if ( dialog.ShowModal() == wxID_OK )
    {
        wxFontData retData = dialog.GetFontData();
        m_canvas->SetTextFont(retData.GetChosenFont());
        m_canvas->SetColour(retData.GetColour());
        m_canvas->Refresh();
    }
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox("wxWindows font demo.", "About Font",
                 wxOK | wxICON_INFORMATION, this);
}


// ----------------------------------------------------------------------------
// MyCanvas
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyCanvas, wxWindow)
    EVT_PAINT(MyCanvas::OnPaint)
END_EVENT_TABLE()

MyCanvas::MyCanvas( wxWindow *parent )
        : wxWindow( parent, -1 )
{
    m_font = *wxNORMAL_FONT;
    m_colour = *wxRED;
}

MyCanvas::~MyCanvas()
{
}

void MyCanvas::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dc(this);
    PrepareDC(dc);

    // set background
    dc.SetBackground(wxBrush("white", wxSOLID));
    dc.Clear();

    // output the font name/info
    wxString fontInfo;
    fontInfo.Printf("Font family is '%s', style '%s', weight '%s'",
                    m_font.GetFamilyString().c_str(),
                    m_font.GetStyleString().c_str(),
                    m_font.GetWeightString().c_str());

    dc.DrawText(fontInfo, 5, 5);

    // prepare to draw the font
    dc.SetFont(m_font);
    dc.SetTextForeground(m_colour);

    // the size of one cell (char + small margin)
    int w = dc.GetCharWidth() + 5,
        h = dc.GetCharHeight() + 4;

    // the origin for our table
    int x = 5,
        y = 2*h + 5;

    // print all font symbols from 32 to 256 in 7 rows of 32 chars each
    for ( int i = 1; i < 8; i++ )
    {
        for ( int j = 0; j < 32; j++ )
        {
            dc.DrawText(char(32*i + j), x + w*j, y + h*i);
        }
    }

    // draw the lines between them
    dc.SetPen(wxPen(wxColour("blue"), 1, wxSOLID));
    int l;

    // horizontal
    y += h;
    for ( l = 0; l < 8; l++ )
    {
        int yl = y + h*l - 2;
        dc.DrawLine(x - 2, yl, x + 32*w - 2, yl);
    }

    // and vertical
    for ( l = 0; l < 33; l++ )
    {
        int xl = x + w*l - 2;
        dc.DrawLine(xl, y, xl, y + 7*h - 2);
    }
}

// ----------------------------------------------------------------------------
// MyFontDialog
// ----------------------------------------------------------------------------

MyFontDialog::MyFontDialog(MyFrame *frame)
            : wxDialog(frame, -1, wxString("Edit font attributes"))
{
    m_canvas = frame->GetCanvas();

    // create controls
    wxSize sizeBtn = wxButton::GetDefaultSize();

    // TODO

    // position and size the dialog
    SetClientSize(4*sizeBtn.x, 10*sizeBtn.y);
    Centre();
}

void MyFontDialog::DoApply()
{
    wxFont font; //(size, family, style, weight, underlined, face, encoding);
    if ( !font.Ok() )
    {
        wxLogError("Font creation failed.");
    }
    else
    {
        m_canvas->SetTextFont(font);
        m_canvas->Refresh();
    }
}
