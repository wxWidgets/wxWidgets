
/////////////////////////////////////////////////////////////////////////////
// Name:        test.cpp
// Purpose:     wxHtml testing example
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "help.cpp"
    #pragma interface "help.cpp"
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
#include <wx/wxhtml.h>

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
      void OnHelp(wxCommandEvent& event);
      void OnClose(wxCloseEvent& event);
   private:
      wxHtmlHelpController help;
   
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
   Minimal_Help
   };

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
   BEGIN_EVENT_TABLE(MyFrame, wxFrame)
   EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
   EVT_MENU(Minimal_Help, MyFrame::OnHelp)
   EVT_CLOSE(MyFrame::OnClose)
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

      SetVendorName("wxWindows");
      SetAppName("wxHTMLHelp"); 

    // Create the main application window
      MyFrame *frame = new MyFrame("HTML Help Sample",
         wxPoint(50, 50), wxSize(150, 50));

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
   : wxFrame((wxFrame *)NULL, -1, title, pos, size), help()
   {
    // create a menu bar
      wxMenu *menuFile = new wxMenu;

      menuFile->Append(Minimal_Help, "&Help");
      menuFile->Append(Minimal_Quit, "E&xit");

    // now append the freshly created menu to the menu bar...
      wxMenuBar *menuBar = new wxMenuBar;
      menuBar->Append(menuFile, "&File");

    // ... and attach this menu bar to the frame
      SetMenuBar(menuBar);

      help.UseConfig(wxConfig::Get());
      bool ret;
      help.SetTempDir(".");
      ret = help.AddBook("helpfiles/testing.hhp");
      if (! ret)
	  wxMessageBox("Failed adding book helpfiles/testing.hhp");
      ret = help.AddBook("helpfiles/another.hhp");
      if (! ret)
	  wxMessageBox("Failed adding book helpfiles/another.hhp");
   }


// event handlers

   void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
   {
    // TRUE is to force the frame to close
      Close(TRUE);
   }

   void MyFrame::OnHelp(wxCommandEvent& WXUNUSED(event))
   {
       help.Display("Main page");
   }

   void MyFrame::OnClose(wxCloseEvent& event)
   {
       // Close the help frame; this will cause the config data to
       // get written.
       if ( help.GetFrame() ) // returns NULL if no help frame active
           help.GetFrame()->Close(TRUE);
       // now we can safely delete the config pointer
       event.Skip();   
       delete wxConfig::Set(NULL);
   }







