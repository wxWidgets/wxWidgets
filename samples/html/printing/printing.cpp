/////////////////////////////////////////////////////////////////////////////
// Name:        printimg.cpp
// Purpose:     wxHtmlEasyPrinting testing example
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/image.h>
#include <wx/html/htmlwin.h>

#include <wx/html/htmprint.h>


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

        void OnPrintSetup(wxCommandEvent& event);
        void OnPageSetup(wxCommandEvent& event);
        void OnPrint(wxCommandEvent& event);
        void OnPreview(wxCommandEvent& event);
        void OnOpen(wxCommandEvent& event);
        

    private:
        wxHtmlWindow *m_Html;
        wxHtmlEasyPrinting *m_Prn;
        wxString m_Name;
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
    Minimal_Quit = 1,
    Minimal_About,
    Minimal_Print,
    Minimal_Preview,
    Minimal_PageSetup,
    Minimal_PrintSetup,
    Minimal_Open

};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit, MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
    EVT_MENU(Minimal_Print, MyFrame::OnPrint)
    EVT_MENU(Minimal_Preview, MyFrame::OnPreview)
    EVT_MENU(Minimal_PageSetup, MyFrame::OnPageSetup)
    EVT_MENU(Minimal_PrintSetup, MyFrame::OnPrintSetup)
    EVT_MENU(Minimal_Open, MyFrame::OnOpen)
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
#if wxUSE_LIBPNG
    wxImage::AddHandler(new wxPNGHandler);
#endif
#if wxUSE_LIBJPEG
    wxImage::AddHandler(new wxJPEGHandler);
#endif
#if wxUSE_GIF
    wxImage::AddHandler(new wxGIFHandler);
#endif

    MyFrame *frame = new MyFrame("Printing test",
                                 wxPoint(150, 50), wxSize(640, 480));

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
    // create a menu bar
    wxMenu *menuFile = new wxMenu;
    wxMenu *menuNav = new wxMenu;

    menuFile->Append(Minimal_Open, "Open...\tCtrl-O");
    menuFile->AppendSeparator();
    menuFile->Append(Minimal_PageSetup, "Page Setup");
    menuFile->Append(Minimal_PrintSetup, "Printer Setup");
    menuFile->Append(Minimal_Print, "Print...");
    menuFile->Append(Minimal_Preview, "Preview...");
    menuFile->AppendSeparator();
    menuFile->Append(Minimal_About, "&About");
    menuFile->AppendSeparator();
    menuFile->Append(Minimal_Quit, "&Exit");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    CreateStatusBar(1);

    m_Html = new wxHtmlWindow(this);
    m_Html -> SetRelatedFrame(this, "HTML : %s");
    m_Html -> SetRelatedStatusBar(0);
    m_Name = "test.htm";
    m_Html -> LoadPage(m_Name);
    
    m_Prn = new wxHtmlEasyPrinting("Easy Printing Demo", this);
    m_Prn -> SetHeader(m_Name + "(@PAGENUM@/@PAGESCNT@)<hr>", wxPAGE_ALL);
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    delete m_Prn;    
    // TRUE is to force the frame to close
    Close(TRUE);
}


void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox("HTML printing sample\n\n(c) Vaclav Slavik, 1999");
}


void MyFrame::OnPrintSetup(wxCommandEvent& WXUNUSED(event))
{
    m_Prn -> PrinterSetup();
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
    wxFileDialog dialog(this, "Open HTML page", "", "", "*.htm", 0);

    if (dialog.ShowModal() == wxID_OK)
    {
        m_Name = dialog.GetPath();
        m_Html -> LoadPage(m_Name);
	m_Prn -> SetHeader(m_Name + "(@PAGENUM@/@PAGESCNT@)<hr>", wxPAGE_ALL);
    } 
}


