/////////////////////////////////////////////////////////////////////////////
// Name:        test.cpp
// Purpose:     wxHtml testing example
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "test.cpp"
    #pragma interface "test.cpp"
#endif

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
#include <wx/fs_inet.h>

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
      void OnPageOpen(wxCommandEvent& event);
      void OnBack(wxCommandEvent& event);
      void OnForward(wxCommandEvent& event);

   private:
        wxHtmlWindow *m_Html;
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
   Minimal_PageOpen,
   Minimal_Back,
   Minimal_Forward,
   
    // controls start here (the numbers are, of course, arbitrary)
   Minimal_Text = 1000
   };

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
   BEGIN_EVENT_TABLE(MyFrame, wxFrame)
   EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
   EVT_MENU(Minimal_PageOpen, MyFrame::OnPageOpen)
   EVT_MENU(Minimal_Back, MyFrame::OnBack)
   EVT_MENU(Minimal_Forward, MyFrame::OnForward)
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
     wxInitAllImageHandlers();
     #if wxUSE_FS_INET && wxUSE_STREAMS && wxUSE_SOCKETS
     wxFileSystem::AddHandler(new wxInternetFSHandler);
     #endif

      SetVendorName("wxWindows");
      SetAppName("wxHtmlTest"); 
      // the following call to wxConfig::Get will use it to create an object...

    // Create the main application window
      MyFrame *frame = new MyFrame("wxHtmlWindow testing application",
         wxPoint(50, 50), wxSize(640, 480));
   
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
   : wxFrame((wxFrame *)NULL, -1, title, pos, size, wxDEFAULT_FRAME_STYLE, "html_test_app")
   {
    // create a menu bar
      wxMenu *menuFile = new wxMenu;
      wxMenu *menuNav = new wxMenu;

      menuFile->Append(Minimal_PageOpen, "&Open HTML page...");
      menuFile->AppendSeparator();
      menuFile->Append(Minimal_Quit, "&Close frame");
      menuNav->Append(Minimal_Back, "Go &BACK");
      menuNav->Append(Minimal_Forward, "Go &FORWARD");

    // now append the freshly created menu to the menu bar...
      wxMenuBar *menuBar = new wxMenuBar;
      menuBar->Append(menuFile, "&File");
      menuBar->Append(menuNav, "&Navigate");

    // ... and attach this menu bar to the frame
      SetMenuBar(menuBar);
   
      CreateStatusBar(1);

      m_Html = new wxHtmlWindow(this);
      m_Html -> SetRelatedFrame(this, "HTML : %s");
      m_Html -> SetRelatedStatusBar(0);
      m_Html -> ReadCustomization(wxConfig::Get());
      m_Html -> LoadPage("test.htm");
   }


// event handlers

   void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
   {
    // TRUE is to force the frame to close
    m_Html -> WriteCustomization(wxConfig::Get());
    delete wxConfig::Set(NULL);
    Close(TRUE);
   }

   void MyFrame::OnPageOpen(wxCommandEvent& WXUNUSED(event))
   {
     wxString p = wxFileSelector("Open HTML document", "", "", "", "HTML files|*.htm");
     if (p != wxEmptyString)
       m_Html -> LoadPage(p);
   }



   void MyFrame::OnBack(wxCommandEvent& WXUNUSED(event))
   {
   if (!m_Html -> HistoryBack()) wxMessageBox("You reached prehistory era!");
   }


   void MyFrame::OnForward(wxCommandEvent& WXUNUSED(event))
   {
   if (!m_Html -> HistoryForward()) wxMessageBox("No more items in history!");
   }
