/////////////////////////////////////////////////////////////////////////////
// Name:        virtua;.cpp
// Purpose:     wxHtml testing example
//              demonstrates virtual file systems feature
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


#include <wx/html/htmlwin.h>




/*


TAG HANDER FOR 'MYBIND' TAG


*/

#include <wx/html/m_templ.h>


TAG_HANDLER_BEGIN(MYBIND, "MYBIND")

    TAG_HANDLER_PROC(tag)
    {
        wxWindow *wnd;
        int ax, ay;
        int fl = 0;

        tag.ScanParam("X", "%i", &ax);
        tag.ScanParam("Y", "%i", &ay);
        if (tag.HasParam("FLOAT")) fl = ax;

        wnd = new wxTextCtrl( m_WParser -> GetWindow(), -1, tag.GetParam("NAME"),
                              wxPoint(0,0), wxSize(ax, ay), wxTE_MULTILINE );
        wnd -> Show(TRUE);

        m_WParser -> OpenContainer() -> InsertCell(new wxHtmlWidgetCell(wnd, fl));

        return FALSE;
    }

TAG_HANDLER_END(MYBIND)



TAGS_MODULE_BEGIN(MyBind)

    TAGS_MODULE_ADD(MYBIND)

TAGS_MODULE_END(MyBind)









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
      void OnBack(wxCommandEvent& event);
      void OnForward(wxCommandEvent& event);

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
   Minimal_Quit = 1,
   Minimal_About,
   Minimal_Back,
   Minimal_Forward,
   
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

wxHtmlWindow *html;

// frame constructor
   MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
   : wxFrame((wxFrame *)NULL, -1, title, pos, size)
   {
    // create a menu bar
      wxMenu *menuFile = new wxMenu;
      wxMenu *menuNav = new wxMenu;

      menuFile->Append(Minimal_Quit, "E&xit");
      menuNav->Append(Minimal_Back, "Go &BACK");
      menuNav->Append(Minimal_Forward, "Go &FORWARD");

    // now append the freshly created menu to the menu bar...
      wxMenuBar *menuBar = new wxMenuBar;
      menuBar->Append(menuFile, "&File");
      menuBar->Append(menuNav, "&Navigate");

    // ... and attach this menu bar to the frame
      SetMenuBar(menuBar);
   
      CreateStatusBar(1);

      html = new wxHtmlWindow(this);
      html -> SetRelatedFrame(this, "VFS Demo: '%s'");
      html -> SetRelatedStatusBar(1);
      html -> LoadPage("start.htm");
   }


// event handlers

   void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
   {
    // TRUE is to force the frame to close
      Close(TRUE);
   }

   void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
   {
   }



   void MyFrame::OnBack(wxCommandEvent& WXUNUSED(event))
   {
   if (!html -> HistoryBack()) wxMessageBox("You reached prehistory era!");
   }


   void MyFrame::OnForward(wxCommandEvent& WXUNUSED(event))
   {
   if (!html -> HistoryForward()) wxMessageBox("No more items in history!");
   }






