/////////////////////////////////////////////////////////////////////////////
// Name:        printing.cpp
// Purpose:     wxHtml sample: wxHtmlEasyPrinting test
// Author:      Vaclav Slavik
// Copyright:   (c) 1998-2009 wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/image.h"
#include "wx/html/htmlwin.h"
#include "wx/html/htmprint.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../../sample.xpm"
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

    virtual bool OnInit() wxOVERRIDE;
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor and dtor

    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    virtual ~MyFrame();

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnPageSetup(wxCommandEvent& event);
    void OnPrint(wxCommandEvent& event);
    void OnPreview(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);

    void OnPrintSmall(wxCommandEvent& event);
    void OnPrintNormal(wxCommandEvent& event);
    void OnPrintHuge(wxCommandEvent& event);


private:
    wxHtmlWindow *m_Html;
    wxHtmlEasyPrinting *m_Prn;
    wxString m_Name;

    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Minimal_Quit = 1,
    Minimal_Print,
    Minimal_Preview,
    Minimal_PageSetup,
    Minimal_Open,
    Minimal_PrintSmall,
    Minimal_PrintNormal,
    Minimal_PrintHuge

};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit, MyFrame::OnQuit)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
    EVT_MENU(Minimal_Print, MyFrame::OnPrint)
    EVT_MENU(Minimal_Preview, MyFrame::OnPreview)
    EVT_MENU(Minimal_PageSetup, MyFrame::OnPageSetup)
    EVT_MENU(Minimal_Open, MyFrame::OnOpen)
    EVT_MENU(Minimal_PrintSmall, MyFrame::OnPrintSmall)
    EVT_MENU(Minimal_PrintNormal, MyFrame::OnPrintNormal)
    EVT_MENU(Minimal_PrintHuge, MyFrame::OnPrintHuge)
wxEND_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
wxIMPLEMENT_APP(MyApp);

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

#if wxUSE_LIBPNG
    wxImage::AddHandler(new wxPNGHandler);
#endif
#if wxUSE_LIBJPEG
    wxImage::AddHandler(new wxJPEGHandler);
#endif
#if wxUSE_GIF
    wxImage::AddHandler(new wxGIFHandler);
#endif

    MyFrame *frame = new MyFrame(_("Printing test"),
        wxDefaultPosition, wxSize(640, 480));

    // Show it
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
    SetIcon(wxICON(sample));

    // create a menu bar
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(Minimal_Open, _("Open...\tCtrl-O"));
    menuFile->AppendSeparator();
    menuFile->Append(Minimal_PageSetup, _("Page &Setup"));
    menuFile->Append(Minimal_Preview, _("Print pre&view..."));
    menuFile->Append(Minimal_Print, _("Print...\tCtrl-P"));
    menuFile->AppendSeparator();
    menuFile->Append(wxID_ABOUT, _("&About"));
    menuFile->AppendSeparator();
    menuFile->Append(Minimal_Quit, _("&Exit"));

    wxMenu *menuFonts = new wxMenu;
    menuFonts->AppendRadioItem(Minimal_PrintSmall, _("&Small Printer Fonts"));
    menuFonts->AppendRadioItem(Minimal_PrintNormal, _("&Normal Printer Fonts"));
    menuFonts->AppendRadioItem(Minimal_PrintHuge, _("&Huge Printer Fonts"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, _("&File"));
    menuBar->Append(menuFonts, _("F&onts"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
    CreateStatusBar(1);
#endif // wxUSE_STATUSBAR

    m_Html = new wxHtmlWindow(this);
    m_Html -> SetRelatedFrame(this, _("HTML : %s"));
#if wxUSE_STATUSBAR
    m_Html -> SetRelatedStatusBar(0);
#endif // wxUSE_STATUSBAR
    m_Name = "test.htm";
    m_Html -> LoadPage(m_Name);

    m_Prn = new wxHtmlEasyPrinting(_("Easy Printing Demo"), this);
    m_Prn -> SetHeader(m_Name + "(@PAGENUM@/@PAGESCNT@)<hr>", wxPAGE_ALL);

    // To specify where the AFM files are kept on Unix,
    // you may wish to do something like this
    // m_Prn->GetPrintData()->SetFontMetricPath("/home/julians/afm");
}

// frame destructor
MyFrame::~MyFrame()
{
    wxDELETE(m_Prn);
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}


void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_("HTML printing sample\n\n(c) Vaclav Slavik, 1999"));
}


void MyFrame::OnPageSetup(wxCommandEvent& WXUNUSED(event))
{
    m_Prn -> PageSetup();
}


void MyFrame::OnPrint(wxCommandEvent& WXUNUSED(event))
{
    m_Prn -> PrintFile(m_Name);
}


void MyFrame::OnPreview(wxCommandEvent& WXUNUSED(event))
{
    m_Prn -> PreviewFile(m_Name);
}


void MyFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog dialog(this, _("Open HTML page"), "", "", "*.htm", 0);

    if (dialog.ShowModal() == wxID_OK)
    {
        m_Name = dialog.GetPath();
        m_Html -> LoadPage(m_Name);
        m_Prn -> SetHeader(m_Name + "(@PAGENUM@/@PAGESCNT@)<hr>", wxPAGE_ALL);
    }
}


void MyFrame::OnPrintSmall(wxCommandEvent& WXUNUSED(event))
{
    m_Prn->SetStandardFonts(8);
}

void MyFrame::OnPrintNormal(wxCommandEvent& WXUNUSED(event))
{
    m_Prn->SetStandardFonts(12);
}

void MyFrame::OnPrintHuge(wxCommandEvent& WXUNUSED(event))
{
    m_Prn->SetStandardFonts(28);
}
