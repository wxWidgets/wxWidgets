/////////////////////////////////////////////////////////////////////////////
// Name:        xrc
// Purpose:     topic overview
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/*!

 @page overview_xrc XML-based resource system overview

 Classes: #wxXmlResource, #wxXmlResourceHandler
 The XML-based resource system, known as XRC, allows user interface elements such as
 dialogs, menu bars and toolbars, to be stored in text files and loaded into
 the application at run-time. XRC files can also be compiled into binary XRS files or C++
 code (the former makes it possible to store all resources in a single file and the latter
 is useful when you want to embed the resources into the executable).

 There are several advantages to using XRC resources.

 @li Recompiling and linking an application is not necessary if the
 resources change.
 @li If you use a dialog designer that generates C++ code, it can be hard
 to reintegrate this into existing C++ code. Separation of resources and code
 is a more elegant solution.
 @li You can choose between different alternative resource files at run time, if necessary.
 @li The XRC format uses sizers for flexibility, allowing dialogs to be resizable
 and highly portable.
 @li The XRC format is a wxWidgets standard,
 and can be generated or postprocessed by any program that understands it. As it is based
 on the XML standard, existing XML editors can be used for simple editing purposes.


 XRC was written by Vaclav Slavik.
 @li @ref overview_xrcconcepts
 @li @ref overview_binaryresourcefiles
 @li @ref overview_embeddedresource
 @li @ref overview_xrccppsample
 @li @ref overview_xrcsample
 @li @ref overview_xrcfileformat
 @li @ref overview_xrccppheader
 @li @ref overview_newresourcehandlers


 @section overview_xrcconcepts XRC concepts

 These are the typical steps for using XRC files in your application.


 @li Include the appropriate headers: normally "wx/xrc/xmlres.h" will suffice;
 @li If you are going to use @ref binaryresourcefiles_overview, install
 wxFileSystem archive handler first with @c wxFileSystem::AddHandler(new wxArchiveFSHandler);
 @li call @c wxXmlResource::Get()-InitAllHandlers() from your wxApp::OnInit function,
 and then call @c wxXmlResource::Get()-Load("myfile.xrc") to load the resource file;
 @li to create a dialog from a resource, create it using the default constructor, and then
 load it using for example @c wxXmlResource::Get()-LoadDialog(dlg, this, "dlg1");
 @li set up event tables as usual but use the @c XRCID(str) macro to translate from XRC string names
 to a suitable integer identifier, for example @c EVT_MENU(XRCID("quit"), MyFrame::OnQuit).

 To create an XRC file, you can use one of the following methods.

 @li Create the file by hand;
 @li use #wxDesigner, a commercial dialog designer/RAD tool;
 @li use #DialogBlocks, a commercial dialog editor;
 @li use #XRCed, a wxPython-based
 dialog editor that you can find in the @c wxPython/tools subdirectory of the wxWidgets
 CVS archive;
 @li use #wxGlade, a GUI designer written in wxPython. At the moment it can generate Python, C++ and XRC;


 A complete list of third-party tools that write to XRC can be found at #www.wxwidgets.org/lnk_tool.htm.

 It is highly recommended that you use a resource editing tool, since it's fiddly writing
 XRC files by hand.

 You can use wxXmlResource::Load in a number of ways.
 You can pass an XRC file (XML-based text resource file)
 or a @ref binaryresourcefiles_overview (extension ZIP or XRS) containing other XRC.

 You can also use @ref embeddedresource_overview

 @section overview_binaryresourcefiles Using binary resource files

 To compile binary resource files, use the command-line wxrc utility. It takes one or more file parameters
 (the input XRC files) and the following switches and options:

 @li -h (--help): show a help message
 @li -v (--verbose): show verbose logging information
 @li -c (--cpp-code): write C++ source rather than a XRS file
 @li -e (--extra-cpp-code): if used together with -c, generates C++ header file
 containing class definitions for the windows defined by the XRC file (see special subsection)
 @li -u (--uncompressed): do not compress XML files (C++ only)
 @li -g (--gettext): output underscore-wrapped strings that poEdit or gettext can scan. Outputs to stdout, or a file if -o is used
 @li -n (--function) name: specify C++ function name (use with -c)
 @li -o (--output) filename: specify the output file, such as resource.xrs or resource.cpp
 @li -l (--list-of-handlers) filename: output a list of necessary handlers to this file


 For example:

 @code
   % wxrc resource.xrc
   % wxrc resource.xrc -o resource.xrs
   % wxrc resource.xrc -v -c -o resource.cpp
 @endcode

 @note
 XRS file is essentially a renamed ZIP archive which means that you can manipulate
 it with standard ZIP tools. Note that if you are using XRS files, you have
 to initialize the #wxFileSystem archive handler first! It is a simple
 thing to do:
 @code
   #include wx/filesys.h
   #include wx/fs_arc.h
   ...
   wxFileSystem::AddHandler(new wxArchiveFSHandler);
 @endcode


 @section overview_embeddedresource Using embedded resources

 It is sometimes useful to embed resources in the executable itself instead
 of loading an external file (e.g. when your app is small and consists only of one
 exe file). XRC provides means to convert resources into regular C++ file that
 can be compiled and included in the executable.

 Use the @c -c switch to
 @c wxrc utility to produce C++ file with embedded resources. This file will
 contain a function called @e InitXmlResource (unless you override this with
 a command line switch). Use it to load the resource:

 @code
   extern void InitXmlResource(); // defined in generated file
   ...
   wxXmlResource::Get()-InitAllHandlers();
   InitXmlResource();
   ...
 @endcode


 @section overview_xrccppsample XRC C++ sample

 This is the C++ source file (xrcdemo.cpp) for the XRC sample.

 @code
 #include "wx/wx.h"
 #include "wx/image.h"
 #include "wx/xrc/xmlres.h"

 // the application icon
 #if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__)
     #include "rc/appicon.xpm"
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
     // return: if OnInit() returns @false, the application terminates)
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
     void OnDlg1(wxCommandEvent& event);
     void OnDlg2(wxCommandEvent& event);

 private:
     // any class wishing to process wxWidgets events must use this macro
     DECLARE_EVENT_TABLE()
 };

 // ----------------------------------------------------------------------------
 // event tables and other macros for wxWidgets
 // ----------------------------------------------------------------------------

 BEGIN_EVENT_TABLE(MyFrame, wxFrame)
     EVT_MENU(XRCID("menu_quit"),  MyFrame::OnQuit)
     EVT_MENU(XRCID("menu_about"), MyFrame::OnAbout)
     EVT_MENU(XRCID("menu_dlg1"), MyFrame::OnDlg1)
     EVT_MENU(XRCID("menu_dlg2"), MyFrame::OnDlg2)
 END_EVENT_TABLE()

 IMPLEMENT_APP(MyApp)

 // ----------------------------------------------------------------------------
 // the application class
 // ----------------------------------------------------------------------------

 // 'Main program' equivalent: the program execution "starts" here
 bool MyApp::OnInit()
 {
     wxImage::AddHandler(new wxGIFHandler);
     wxXmlResource::Get()-InitAllHandlers();
     wxXmlResource::Get()-Load("rc/resource.xrc");

     MyFrame *frame = new MyFrame("XML resources demo",
                                  wxPoint(50, 50), wxSize(450, 340));
     frame-Show(@true);
     return @true;
 }

 // ----------------------------------------------------------------------------
 // main frame
 // ----------------------------------------------------------------------------

 // frame constructor
 MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
        : wxFrame((wxFrame *)@NULL, -1, title, pos, size)
 {
     SetIcon(wxICON(appicon));

     SetMenuBar(wxXmlResource::Get()-LoadMenuBar("mainmenu"));
     SetToolBar(wxXmlResource::Get()-LoadToolBar(this, "toolbar"));
 }

 // event handlers
 void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
 {
     // @true is to force the frame to close
     Close(@true);
 }

 void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
 {
     wxString msg;
     msg.Printf( _T("This is the about dialog of XML resources demo.\n")
                 _T("Welcome to %s"), wxVERSION_STRING);

     wxMessageBox(msg, "About XML resources demo", wxOK | wxICON_INFORMATION, this);
 }

 void MyFrame::OnDlg1(wxCommandEvent& WXUNUSED(event))
 {
     wxDialog dlg;
     wxXmlResource::Get()-LoadDialog(, this, "dlg1");
     dlg.ShowModal();
 }

 void MyFrame::OnDlg2(wxCommandEvent& WXUNUSED(event))
 {
     wxDialog dlg;
     wxXmlResource::Get()-LoadDialog(, this, "dlg2");
     dlg.ShowModal();
 }
 @endcode


 @section overview_xrcsample XRC resource file sample

 This is the XML file (resource.xrc) for the XRC sample.

 @code
 ?xml version="1.0"?
 resource version="2.3.0.1"
   object class="wxMenuBar" name="mainmenu"
     stylewxMB_DOCKABLE/style
     object class="wxMenu" name="menu_file"
       label_File/label
       stylewxMENU_TEAROFF/style
       object class="wxMenuItem" name="menu_about"
         label_About.../label
         bitmapfilesave.gif/bitmap
       /object
       object class="separator"/
       object class="wxMenuItem" name="menu_dlg1"
         labelDialog 1/label
       /object
       object class="wxMenuItem" name="menu_dlg2"
         labelDialog 2/label
       /object
       object class="separator"/
       object class="wxMenuItem" name="menu_quit"
         labelE_xit\tAlt-X/label
       /object
     /object
   /object
   object class="wxToolBar" name="toolbar"
     stylewxTB_FLAT|wxTB_DOCKABLE/style
     margins2,2/margins
     object class="tool" name="menu_open"
       bitmapfileopen.gif/bitmap
       tooltipOpen catalog/tooltip
     /object
     object class="tool" name="menu_save"
       bitmapfilesave.gif/bitmap
       tooltipSave catalog/tooltip
     /object
     object class="tool" name="menu_update"
       bitmapupdate.gif/bitmap
       tooltipUpdate catalog - synchronize it with sources/tooltip
     /object
     separator/
     object class="tool" name="menu_quotes"
       bitmapquotes.gif/bitmap
       toggle1/toggle
       tooltipDisplay quotes around the string?/tooltip
     /object
     object class="separator"/
     object class="tool" name="menu_fuzzy"
       bitmapfuzzy.gif/bitmap
       tooltipToggled if selected string is fuzzy translation/tooltip
       toggle1/toggle
     /object
   /object
   object class="wxDialog" name="dlg1"
     object class="wxBoxSizer"
       object class="sizeritem"
         object class="wxBitmapButton"
           bitmapfuzzy.gif/bitmap
           focusfileopen.gif/focus
         /object
       /object
       object class="sizeritem"
         object class="wxPanel"
           object class="wxStaticText"
             labelfdgdfgdfgdfg/label
           /object
           stylewxBORDER\_SUNKEN/style
         /object
         flagwxALIGN_CENTER/flag
       /object
       object class="sizeritem"
         object class="wxButton"
           labelButtonek/label
         /object
         border10d/border
         flagwxALL/flag
       /object
       object class="sizeritem"
         object class="wxHtmlWindow"
           htmlcodeh1Hi,/h1man/htmlcode
           size100,45d/size
         /object
       /object
       object class="sizeritem"
         object class="wxNotebook"
           object class="notebookpage"
             object class="wxPanel"
               object class="wxBoxSizer"
                 object class="sizeritem"
                   object class="wxHtmlWindow"
                     htmlcodeHello, we are inside a uNOTEBOOK/u.../htmlcode
                     size50,50d/size
                   /object
                   option1/option
                 /object
               /object
             /object
             labelPage/label
           /object
           object class="notebookpage"
             object class="wxPanel"
               object class="wxBoxSizer"
                 object class="sizeritem"
                   object class="wxHtmlWindow"
                     htmlcodeHello, we are inside a uNOTEBOOK/u.../htmlcode
                     size50,50d/size
                   /object
                 /object
               /object
             /object
             labelPage 2/label
           /object
           usenotebooksizer1/usenotebooksizer
         /object
         flagwxEXPAND/flag
       /object
       orientwxVERTICAL/orient
     /object
   /object
   object class="wxDialog" name="dlg2"
     object class="wxBoxSizer"
       orientwxVERTICAL/orient
       object class="sizeritem" name="dfgdfg"
         object class="wxTextCtrl"
           size200,200d/size
           stylewxTE_MULTILINE|wxBORDER_SUNKEN/style
           valueHello, this is an ordinary multiline\n         textctrl..../value
         /object
         option1/option
         flagwxEXPAND|wxALL/flag
         border10/border
       /object
       object class="sizeritem"
         object class="wxBoxSizer"
           object class="sizeritem"
             object class="wxButton" name="wxID_OK"
               labelOk/label
               default1/default
             /object
           /object
           object class="sizeritem"
             object class="wxButton" name="wxID_CANCEL"
               labelCancel/label
             /object
             border10/border
             flagwxLEFT/flag
           /object
         /object
         flagwxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_RIGHT/flag
         border10/border
       /object
     /object
     titleSecond testing dialog/title
   /object
 /resource
 @endcode


 @section overview_xrcfileformat XRC file format

 Please see Technical Note 14 (docs/tech/tn0014.txt) in your wxWidgets
 distribution.

 @section overview_xrccppheader C++ header file generation

 Using the @c -e switch together with @c -c, a C++ header file is written
 containing class definitions for the GUI windows defined in the XRC file.
 This code generation can make it easier to use XRC and automate program
 development.
 The classes can be used as basis for development, freeing the
 programmer from dealing with most of the XRC specifics (e.g. @c XRCCTRL).

 For each top level window defined in the XRC file a C++ class definition is
 generated, containing as class members the named widgets of the window.
 A default constructor for each class is also generated. Inside the constructor
 all XRC loading is done and all class members representing widgets are initialized.

 A simple example will help understand how the scheme works. Suppose you have
 a XRC file defining a top level window @c TestWnd_Base, which subclasses @c wxFrame (any
 other class like @c wxDialog will do also), and has subwidgets @c wxTextCtrl A and @c wxButton B.
 The XRC file and corresponding class definition in the header file will be something like:

 @code
 ?xml version="1.0"?
 resource version="2.3.0.1"
     object class="wxFrame" name="TestWnd_Base"
         size-1,-1/size
         titleTest/title
         object class="wxBoxSizer"
             orientwxHORIZONTAL/orient
             object class="sizeritem"
                 object class="wxTextCtrl" name="A"
                     labelTest label/label
                 /object
             /object
             object class="sizeritem"
                 object class="wxButton" name="B"
                     labelTest button/label
                 /object
             /object
         /object
     /object
 /resource


 class TestWnd_Base : public wxFrame {
 protected:
  wxTextCtrl* A;
  wxButton* B;

 private:
  void InitWidgetsFromXRC(){
   wxXmlResource::Get()-LoadObject(this,@NULL,"TestWnd","wxFrame");
   A = XRCCTRL(*this,"A",wxTextCtrl);
   B = XRCCTRL(*this,"B",wxButton);
  }
 public:
 TestWnd::TestWnd(){
   InitWidgetsFromXRC();
  }
 };
 @endcode

 The generated window class can be used as basis for the full window class. The
 class members which represent widgets may be accessed by name instead of using
 @c XRCCTRL every time you wish to reference them (note that they are @c protected class members),
 though you must still use @c XRCID to refer to widget IDs in the event
 table.
 Example:

 @code
 #include "resource.h"

 class TestWnd : public TestWnd_Base {
  public:
   TestWnd(){
    // A, B already initialised at this point
    A-SetValue("Updated in TestWnd::TestWnd");
    B-SetValue("Nice :)");
   }
   void OnBPressed(wxEvent& event){
    Close();
   }
   DECLARE_EVENT_TABLE();
 };

 BEGIN_EVENT_TABLE(TestWnd,TestWnd_Base)
 EVT_BUTTON(XRCID("B"),TestWnd::OnBPressed)
 END_EVENT_TABLE()
 @endcode

 It is also possible to access the wxSizerItem of a sizer that is part of
 a resource.  This can be done using @c XRCSIZERITEM as shown.  The
 resource file can have something like this for a sizer item.

 @code
 object class="spacer" name="area"
   size400, 300/size
 /object
 @endcode

 The code can then access the sizer item by using @c XRCSIZERITEM and
 @c XRCID together.

 @code
 wxSizerItem* item = XRCSIZERITEM(*this, "area");
 @endcode


 @section overview_newresourcehandlers Adding new resource handlers

 Adding a new resource handler is pretty easy.
 Typically, to add an handler for the @c MyControl class, you'll want to create
 the @c xh_mycontrol.h @c xh_mycontrol.cpp files.

 The header needs to contains the @c MyControlXmlHandler class definition:

 @code
 class MyControlXmlHandler : public wxXmlResourceHandler
 {
 public:

     // Constructor.
     MyControlXmlHandler();

     // Creates the control and returns a pointer to it.
     virtual wxObject *DoCreateResource();

     // Returns @true if we know how to create a control for the given node.
     virtual bool CanHandle(wxXmlNode *node);

     // Register with wxWidgets' dynamic class subsystem.
     DECLARE_DYNAMIC_CLASS(MyControlXmlHandler)
 };
 @endcode

 The implementation of your custom XML handler will typically look as:

 @code
 // Register with wxWidgets' dynamic class subsystem.
 IMPLEMENT_DYNAMIC_CLASS(MyControlXmlHandler, wxXmlResourceHandler)

 MyControlXmlHandler::MyControlXmlHandler()
 {
     // this call adds support for all wxWindows class styles
     // (e.g. wxBORDER_SIMPLE, wxBORDER_SUNKEN, wxWS_EX_* etc etc)
     AddWindowStyles();

     // if MyControl class supports e.g. MYCONTROL_DEFAULT_STYLE
     // you should use:
     //     XRC_ADD_STYLE(MYCONTROL_DEFAULT_STYLE);
 }

 wxObject *MyControlXmlHandler::DoCreateResource()
 {
     // the following macro will init a pointer named "control"
     // with a new instance of the MyControl class, but will NOT
     // Create() it!
     XRC_MAKE_INSTANCE(control, MyControl)

     // this is the point where you'll typically need to do the most
     // important changes: here the control is created and initialized.
     // You'll want to use the wxXmlResourceHandler's getters to
     // do most of your work.
     // If e.g. the MyControl::Create function looks like:
     //
     //    bool MyControl::Create(wxWindow *parent, int id,
     //                           const wxBitmap , const wxPoint ,
     //                           const wxBitmap , const wxPoint ,
     //                           const wxString , const wxFont ,
     //                           const wxPoint , const wxSize ,
     //                           long style = MYCONTROL_DEFAULT_STYLE,
     //                           const wxString  = wxT("MyControl"));
     //
     // then the XRC for your component should look like:
     //
     //    object class="MyControl" name="some_name"
     //        first-bitmapfirst.xpm/first-bitmap
     //        second-bitmaptext.xpm/second-bitmap
     //        first-pos3,3/first-pos
     //        second-pos4,4/second-pos
     //        the-titlea title/the-title
     //        title-font
     //           !-- the standard XRC tags for describing a font: size, style, weight, etc --
     //        /title-font
     //        !-- XRC also accepts other usual tags for wxWindow-derived classes:
     //             like e.g. name, style, size, position, etc --
     //    /object
     //
     // and the code to read your custom tags from the XRC file is just:
     control-Create(m_parentAsWindow, GetID(),
                     GetBitmap(wxT("first-bitmap")),
                     GetPosition(wxT("first-pos")),
                     GetBitmap(wxT("second-bitmap")),
                     GetPosition(wxT("second-pos")),
                     GetText(wxT("the-title")),
                     GetFont(wxT("title-font")),
                     GetPosition(), GetSize(), GetStyle(), GetName());

     SetupWindow(control);

     return control;
 }

 bool MyControlXmlHandler::CanHandle(wxXmlNode *node)
 {
     // this function tells XRC system that this handler can parse
     // the object class="MyControl" tags
     return IsOfClass(node, wxT("MyControl"));
 }
 @endcode

 You may want to check the #wxXmlResourceHandler documentation
 to see how many built-in getters it contains. It's very easy to retrieve also complex structures
 out of XRC files using them.

 */


