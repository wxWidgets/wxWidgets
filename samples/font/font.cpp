/////////////////////////////////////////////////////////////////////////////
// Name:        font.cpp
// Purpose:     wxFont demo
// Author:      Vadim Zeitlin
// Modified by:
// Created:     30.09.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vadim Zeitlin
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

#include <wx/choicdlg.h>
#include <wx/fontdlg.h>
#include <wx/fontenum.h>

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
    void OnEnumerateFamiliesForEncoding(wxCommandEvent& event);
    void OnEnumerateFamilies(wxCommandEvent& WXUNUSED(event))
        { DoEnumerateFamilies(FALSE); }
    void OnEnumerateFixedFamilies(wxCommandEvent& WXUNUSED(event))
        { DoEnumerateFamilies(TRUE); }
    void OnEnumerateEncodings(wxCommandEvent& event);

    void OnSize(wxSizeEvent& event);

protected:
    void DoEnumerateFamilies(bool fixedWidthOnly,
                             wxFontEncoding encoding = wxFONTENCODING_SYSTEM);

    void DoChangeFont(const wxFont& font, const wxColour& col = wxNullColour);

    void Resize(const wxSize& size, const wxFont& font = wxNullFont);

    wxTextCtrl *m_textctrl;
    MyCanvas   *m_canvas;

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
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
    Font_EnumFamiliesForEncoding,
    Font_EnumFamilies,
    Font_EnumFixedFamilies,
    Font_EnumEncodings,
    Font_Max
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
    EVT_MENU(Font_EnumFamiliesForEncoding, MyFrame::OnEnumerateFamiliesForEncoding)
    EVT_MENU(Font_EnumFamilies, MyFrame::OnEnumerateFamilies)
    EVT_MENU(Font_EnumFixedFamilies, MyFrame::OnEnumerateFixedFamilies)
    EVT_MENU(Font_EnumEncodings, MyFrame::OnEnumerateEncodings)

    EVT_SIZE(MyFrame::OnSize)
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
    MyFrame *frame = new MyFrame("Font wxWindows demo",
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
       : wxFrame((wxFrame *)NULL, -1, title, pos, size), m_textctrl(NULL)
{
    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    menuFile->Append(Font_About, "&About...\tCtrl-A", "Show about dialog");
    menuFile->AppendSeparator();
    menuFile->Append(Font_Quit, "E&xit\tAlt-X", "Quit this program");

    wxMenu *menuFont = new wxMenu;
    menuFont->Append(Font_Choose, "&Select font...\tCtrl-S",
                     "Select a standard font");
    menuFont->AppendSeparator();
    menuFont->Append(Font_EnumFamilies, "Enumerate font &families\tCtrl-F");
    menuFont->Append(Font_EnumFixedFamilies,
                     "Enumerate f&ixed font families\tCtrl-I");
    menuFont->Append(Font_EnumEncodings,
                     "Enumerate &encodings\tCtrl-E");
    menuFont->Append(Font_EnumFamiliesForEncoding,
                     "Find font for en&coding...\tCtrl-C",
                     "Find font families for given encoding");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuFont, "F&ont");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    m_textctrl = new wxTextCtrl(this, -1,
                                "Paste text here to see how it looks\n"
                                "like in the given font",
                                wxDefaultPosition, wxDefaultSize,
                                wxTE_MULTILINE);

    m_canvas = new MyCanvas(this);

    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar();
    SetStatusText("Welcome to wxWindows font demo!");
}


// event handlers
void MyFrame::OnEnumerateEncodings(wxCommandEvent& WXUNUSED(event))
{
    class MyEncodingEnumerator : public wxFontEnumerator
    {
    public:
        MyEncodingEnumerator() { m_n = 0; }

        const wxString& GetText() const { return m_text; }

    protected:
        virtual bool OnFontEncoding(const wxString& family,
                                    const wxString& encoding)
        {
            wxString text;
            text.Printf("Encoding %d: %s (available in family '%s')\n",
                         ++m_n, encoding.c_str(), family.c_str());
            m_text += text;

            return TRUE;
        }

    private:
        size_t m_n;

        wxString m_text;
    } fontEnumerator;

    fontEnumerator.EnumerateEncodings();

    wxLogMessage("Enumerating all available encodings:\n%s",
                 fontEnumerator.GetText().c_str());
}

void MyFrame::DoEnumerateFamilies(bool fixedWidthOnly, wxFontEncoding encoding)
{
    class MyFontEnumerator : public wxFontEnumerator
    {
    public:
        bool GotAny() const { return !m_facenames.IsEmpty(); }

        const wxArrayString& GetFacenames() const { return m_facenames; }

    protected:
        virtual bool OnFontFamily(const wxString& family)
        {
            m_facenames.Add(family);

            return TRUE;
        }

    private:
        wxArrayString m_facenames;
    } fontEnumerator;

    fontEnumerator.EnumerateFamilies(encoding, fixedWidthOnly);

    if ( fontEnumerator.GotAny() )
    {
        int n, nFacenames = fontEnumerator.GetFacenames().GetCount();
        wxLogStatus(this, "Found %d %sfonts",
                    nFacenames, fixedWidthOnly ? "fixed width " : "");

        wxString *facenames = new wxString[nFacenames];
        for ( n = 0; n < nFacenames; n++ )
            facenames[n] = fontEnumerator.GetFacenames().Item(n);

        n = wxGetSingleChoiceIndex("Choose a facename", "Font demo",
                                   nFacenames, facenames, this);
        if ( n != -1 )
        {
            wxFont font(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
                        wxFONTWEIGHT_NORMAL, FALSE, facenames[n], encoding);

            DoChangeFont(font);
        }

        delete [] facenames;
    }
    else
    {
        wxLogWarning("No such fonts found.");
    }
}

void MyFrame::OnEnumerateFamiliesForEncoding(wxCommandEvent& WXUNUSED(event))
{
    static wxFontEncoding encodings[] =
    {
        wxFONTENCODING_ISO8859_1,
        wxFONTENCODING_ISO8859_2,
        wxFONTENCODING_ISO8859_5,
        wxFONTENCODING_ISO8859_7,
        wxFONTENCODING_ISO8859_15,
        wxFONTENCODING_KOI8,
        wxFONTENCODING_CP1250,
        wxFONTENCODING_CP1251,
        wxFONTENCODING_CP1252,
    };

    static const char *encodingNames[] =
    {
        "West European (Latin 1)",
        "Central European (Latin 2)",
        "Cyrillic (Latin 5)",
        "Greek (Latin 7)",
        "West European new (Latin 0)",
        "KOI8-R",
        "Windows Latin 2",
        "Windows Cyrillic",
        "Windows Latin 1",
    };

    int n = wxGetSingleChoiceIndex("Choose an encoding", "Font demo",
                                   WXSIZEOF(encodingNames),
                                   (char **)encodingNames,
                                   this);

    if ( n != -1 )
    {
        DoEnumerateFamilies(FALSE, encodings[n]);
    }
}

void MyFrame::DoChangeFont(const wxFont& font, const wxColour& col)
{
    Resize(GetSize(), font);

    m_canvas->SetTextFont(font);
    if ( col.Ok() )
        m_canvas->SetColour(col);
    m_canvas->Refresh();

    m_textctrl->SetFont(font);
    if ( col.Ok() )
        m_textctrl->SetForegroundColour(col);
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
        wxFont font = retData.GetChosenFont();
        wxColour colour = retData.GetColour();

        DoChangeFont(font, colour);
    }
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox("wxWindows font demo\n"
                 "(c) 1999 Vadim Zeitlin",
                 "About Font",
                 wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnSize(wxSizeEvent& event)
{
    wxSize size = event.GetSize();

    Resize(size);
}

void MyFrame::Resize(const wxSize& size, const wxFont& font)
{
    if ( !m_textctrl )
        return;

    wxCoord h;
    if ( font.Ok() )
    {
        wxClientDC dc(this);
        dc.SetFont(font);

        h = 4*dc.GetCharHeight() + 4;
    }
    else
    {
        h = m_textctrl->GetSize().y;
    }

    m_textctrl->SetSize(0, 0, size.x, h);
    m_canvas->SetSize(0, h, size.x, size.y - h);
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
        y = 2*h;

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
