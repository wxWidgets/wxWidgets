/////////////////////////////////////////////////////////////////////////////
// Name:        test.cpp
// Purpose:     wxHtml testing example
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation
    #pragma interface
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
#if (( wxVERSION_NUMBER < 2100 ) || (( wxVERSION_NUMBER == 2100 ) && (wxBETA_NUMBER <= 4)))
#include <wx/imaggif.h>
#endif
#include <wx/config.h>

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
   class MyApp : public wxApp
   {
   private:
        wxHtmlHelpController help;
	wxConfig* config;

   public:
    // override base class virtuals
    // ----------------------------
   
    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
      bool OnInit();
      int OnExit();
   };

   IMPLEMENT_APP(MyApp)
   

   bool MyApp::OnInit()
   {
    config = new wxConfig("wxHTMLhelp");
   #if wxUSE_LIBPNG
    wxImage::AddHandler(new wxPNGHandler);
   #endif
   #if wxUSE_LIBJPEG
    wxImage::AddHandler(new wxJPEGHandler);
   #endif

    help.UseConfig(config);
    help.SetTempDir("tmp");
    help.AddBook("helpfiles/testing.hhp");
    help.Display("Main page");
      return TRUE;
   }
   
   int MyApp::OnExit()
   {
     delete config;
     return 0;
   }








