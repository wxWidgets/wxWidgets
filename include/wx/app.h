/////////////////////////////////////////////////////////////////////////////
// Name:        wx/app.h
// Purpose:     wxAppBase class and macros used for declaration of wxApp
//              derived class in the user code
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_APP_H_BASE_
#define _WX_APP_H_BASE_

// ----------------------------------------------------------------------------
// headers we have to include here
// ----------------------------------------------------------------------------

#include "wx/event.h"       // for the base class

#if wxUSE_GUI
    #include "wx/window.h"  // for wxTopLevelWindows
#endif // wxUSE_GUI

#include "wx/build.h"

class WXDLLEXPORT wxApp;
class WXDLLEXPORT wxAppTraits;
class WXDLLEXPORT wxCmdLineParser;
class WXDLLEXPORT wxLog;
class WXDLLEXPORT wxMessageOutput;

// ----------------------------------------------------------------------------
// typedefs
// ----------------------------------------------------------------------------

// the type of the function used to create a wxApp object on program start up
typedef wxApp* (*wxAppInitializerFunction)();

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

enum
{
    wxPRINT_WINDOWS = 1,
    wxPRINT_POSTSCRIPT = 2
};

// ----------------------------------------------------------------------------
// support for framebuffer ports
// ----------------------------------------------------------------------------

#if wxUSE_GUI
// VS: Fullscreen/framebuffer application needs to choose display mode prior
//     to wxWindows initialization. This class holds information about display
//     mode. It is used by  wxApp::Set/GetDisplayMode.
class WXDLLEXPORT wxDisplayModeInfo
{
public:
    wxDisplayModeInfo() : m_ok(FALSE) {}
    wxDisplayModeInfo(unsigned width, unsigned height, unsigned depth)
        : m_width(width), m_height(height), m_depth(depth), m_ok(TRUE) {}

    unsigned GetWidth() const { return m_width; }
    unsigned GetHeight() const { return m_height; }
    unsigned GetDepth() const { return m_depth; }
    bool IsOk() const { return m_ok; }

private:
    unsigned m_width, m_height, m_depth;
    bool     m_ok;
};
#endif // wxUSE_GUI

// ----------------------------------------------------------------------------
// wxAppConsole: wxApp for non-GUI applications
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxAppConsole : public wxEvtHandler
{
public:
    // ctor and dtor
    wxAppConsole();
    virtual ~wxAppConsole();


    // the virtual functions which may/must be overridden in the derived class
    // -----------------------------------------------------------------------

    // Called before OnRun(), this is a good place to do initialization -- if
    // anything fails, return false from here to prevent the program from
    // continuing. The command line is normally parsed here, call the base
    // class OnInit() to do it.
    virtual bool OnInit();

    // This is the replacement for the normal main(): all program work should
    // be done here. When OnRun() returns, the programs starts shutting down.
    virtual int OnRun() = 0;

    // This is only called if OnInit() returned true so it's a good place to do
    // any cleanup matching the initializations done there.
    virtual int OnExit();

    // Called when a fatal exception occurs, this function should take care not
    // to do anything which might provoke a nested exception! It may be
    // overridden if you wish to react somehow in non-default way (core dump
    // under Unix, application crash under Windows) to fatal program errors,
    // however extreme care should be taken if you don't want this function to
    // crash.
    virtual void OnFatalException() { }

    // Called from wxExit() function, should terminate the application a.s.a.p.
    virtual void Exit();


    // application info: name, description, vendor
    // -------------------------------------------

    // NB: all these should be set by the application itself, there are no
    //     reasonable default except for the application name which is taken to
    //     be argv[0]

        // set/get the application name
    wxString GetAppName() const
    {
        return m_appName.empty() ? m_className : m_appName;
    }
    void SetAppName(const wxString& name) { m_appName = name; }

        // set/get the app class name
    wxString GetClassName() const { return m_className; }
    void SetClassName(const wxString& name) { m_className = name; }

        // set/get the vendor name
    const wxString& GetVendorName() const { return m_vendorName; }
    void SetVendorName(const wxString& name) { m_vendorName = name; }


    // cmd line parsing stuff
    // ----------------------

    // all of these methods may be overridden in the derived class to
    // customize the command line parsing (by default only a few standard
    // options are handled)
    //
    // you also need to call wxApp::OnInit() from YourApp::OnInit() for all
    // this to work

#if wxUSE_CMDLINE_PARSER
    // this one is called from OnInit() to add all supported options
    // to the given parser
    virtual void OnInitCmdLine(wxCmdLineParser& parser);

    // called after successfully parsing the command line, return TRUE
    // to continue and FALSE to exit
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);

    // called if "--help" option was specified, return TRUE to continue
    // and FALSE to exit
    virtual bool OnCmdLineHelp(wxCmdLineParser& parser);

    // called if incorrect command line options were given, return
    // FALSE to abort and TRUE to continue
    virtual bool OnCmdLineError(wxCmdLineParser& parser);
#endif // wxUSE_CMDLINE_PARSER


    // miscellaneous customization functions
    // -------------------------------------

    // create the app traits object to which we delegate for everything which
    // either should be configurable by the user (then he can change the
    // default behaviour simply by overriding CreateTraits() and returning his
    // own traits object) or which is GUI/console dependent as then wxAppTraits
    // allows us to abstract the differences behind the common façade
    wxAppTraits *GetTraits();

    // the functions below shouldn't be used now that we have wxAppTraits
#if WXWIN_COMPATIBILITY_2_4

#if wxUSE_LOG
        // override this function to create default log target of arbitrary
        // user-defined class (default implementation creates a wxLogGui
        // object) -- this log object is used by default by all wxLogXXX()
        // functions.
    virtual wxLog *CreateLogTarget();
#endif // wxUSE_LOG

        // similar to CreateLogTarget() but for the global wxMessageOutput
        // object
    virtual wxMessageOutput *CreateMessageOutput();

#endif // WXWIN_COMPATIBILITY_2_4


    // event processing functions
    // --------------------------

    // this method allows to filter all the events processed by the program, so
    // you should try to return quickly from it to avoid slowing down the
    // program to the crawl
    //
    // return value should be -1 to continue with the normal event processing,
    // or TRUE or FALSE to stop further processing and pretend that the event
    // had been already processed or won't be processed at all, respectively
    virtual int FilterEvent(wxEvent& event);

    // process all events in the wxPendingEvents list -- it is necessary to
    // call this function to process posted events. This happens during each
    // event loop iteration in GUI mode but if there is no main loop, it may be
    // also called directly.
    virtual void ProcessPendingEvents();

    // doesn't do anything in this class, just a hook for GUI wxApp
    virtual bool Yield(bool WXUNUSED(onlyIfNeeded) = false) { return true; }

    // make sure that idle events are sent again
    virtual void WakeUpIdle() { }


    // debugging support
    // -----------------

    // this function is called when an assert failure occurs, the base class
    // version does the normal processing (i.e. shows the usual assert failure
    // dialog box)
    //
    // the arguments are the place where the assert occured, the text of the
    // assert itself and the user-specified message
#ifdef __WXDEBUG__
    virtual void OnAssert(const wxChar *file,
                          int line,
                          const wxChar *cond,
                          const wxChar *msg);
#endif // __WXDEBUG__

    // check that the wxBuildOptions object (constructed in the application
    // itself, usually the one from IMPLEMENT_APP() macro) matches the build
    // options of the library and abort if it doesn't
    static bool CheckBuildOptions(const wxBuildOptions& buildOptions);


    // implementation only from now on
    // -------------------------------

    // helpers for dynamic wxApp construction
    static void SetInitializerFunction(wxAppInitializerFunction fn)
        { ms_appInitFn = fn; }
    static wxAppInitializerFunction GetInitializerFunction()
        { return ms_appInitFn; }


    // command line arguments (public for backwards compatibility)
    int      argc;
    wxChar **argv;

protected:
    // the function which creates the traits object when GetTraits() needs it
    // for the first time
    virtual wxAppTraits *CreateTraits();


    // function used for dynamic wxApp creation
    static wxAppInitializerFunction ms_appInitFn;

    // application info (must be set from the user code)
    wxString m_vendorName,      // vendor name (ACME Inc)
             m_appName,         // app name
             m_className;       // class name

    // the class defining the application behaviour, NULL initially and created
    // by GetTraits() when first needed
    wxAppTraits *m_traits;


    // the application object is a singleton anyhow, there is no sense in
    // copying it
    DECLARE_NO_COPY_CLASS(wxAppConsole)
};

// ----------------------------------------------------------------------------
// wxAppBase: the common part of wxApp implementations for all platforms
// ----------------------------------------------------------------------------

#if wxUSE_GUI

class WXDLLEXPORT wxAppBase : public wxAppConsole
{
public:
    wxAppBase();
    virtual ~wxAppBase();

    // the virtual functions which may/must be overridden in the derived class
    // -----------------------------------------------------------------------

        // a platform-dependent version of OnInit(): the code here is likely to
        // depend on the toolkit. default version does nothing.
        //
        // Override: rarely.
    virtual bool OnInitGui();

        // called to start program execution - the default version just enters
        // the main GUI loop in which events are received and processed until
        // the last window is not deleted (if GetExitOnFrameDelete) or
        // ExitMainLoop() is called. In console mode programs, the execution
        // of the program really starts here
        //
        // Override: rarely in GUI applications, always in console ones.
    virtual int OnRun();

        // exit the main loop thus terminating the application
    virtual void Exit();


    // the worker functions - usually not used directly by the user code
    // -----------------------------------------------------------------

        // execute the main GUI loop, the function returns when the loop ends
    virtual int MainLoop() = 0;

        // exit the main GUI loop during the next iteration (i.e. it does not
        // stop the program immediately!)
    virtual void ExitMainLoop() = 0;

        // returns TRUE if the program is initialized
    virtual bool Initialized() = 0;

        // returns TRUE if there are unprocessed events in the event queue
    virtual bool Pending() = 0;

        // process the first event in the event queue (blocks until an event
        // apperas if there are none currently)
    virtual void Dispatch() = 0;

        // process all currently pending events right now
        //
        // it is an error to call Yield() recursively unless the value of
        // onlyIfNeeded is TRUE
        //
        // WARNING: this function is dangerous as it can lead to unexpected
        //          reentrancies (i.e. when called from an event handler it
        //          may result in calling the same event handler again), use
        //          with _extreme_ care or, better, don't use at all!
    virtual bool Yield(bool onlyIfNeeded = FALSE) = 0;

        // this virtual function is called in the GUI mode when the application
        // becomes idle and normally just sends wxIdleEvent to all interested
        // parties
        //
        // it should return TRUE if more idle events are needed, FALSE if not
    virtual bool ProcessIdle() = 0;


    // top level window functions
    // --------------------------

        // return TRUE if our app has focus
    virtual bool IsActive() const { return m_isActive; }

        // set the "main" top level window
    void SetTopWindow(wxWindow *win) { m_topWindow = win; }

        // return the "main" top level window (if it hadn't been set previously
        // with SetTopWindow(), will return just some top level window and, if
        // there are none, will return NULL)
    virtual wxWindow *GetTopWindow() const
    {
        if (m_topWindow)
            return m_topWindow;
        else if (wxTopLevelWindows.GetCount() > 0)
            return wxTopLevelWindows.GetFirst()->GetData();
        else
            return (wxWindow *)NULL;
    }

        // control the exit behaviour: by default, the program will exit the
        // main loop (and so, usually, terminate) when the last top-level
        // program window is deleted. Beware that if you disable this behaviour
        // (with SetExitOnFrameDelete(FALSE)), you'll have to call
        // ExitMainLoop() explicitly from somewhere.
    void SetExitOnFrameDelete(bool flag)
        { m_exitOnFrameDelete = flag ? Yes : No; }
    bool GetExitOnFrameDelete() const
        { return m_exitOnFrameDelete == Yes; }


    // display mode, visual, printing mode, ...
    // ------------------------------------------------------------------------

        // Get display mode that is used use. This is only used in framebuffer
        // wxWin ports (such as wxMGL).
    virtual wxDisplayModeInfo GetDisplayMode() const { return wxDisplayModeInfo(); }
        // Set display mode to use. This is only used in framebuffer wxWin
        // ports (such as wxMGL). This method should be called from
        // wxApp::OnInitGui
    virtual bool SetDisplayMode(const wxDisplayModeInfo& WXUNUSED(info)) { return TRUE; }

        // set use of best visual flag (see below)
    void SetUseBestVisual( bool flag ) { m_useBestVisual = flag; }
    bool GetUseBestVisual() const { return m_useBestVisual; }

        // set/get printing mode: see wxPRINT_XXX constants.
        //
        // default behaviour is the normal one for Unix: always use PostScript
        // printing.
    virtual void SetPrintMode(int WXUNUSED(mode)) { }
    int GetPrintMode() const { return wxPRINT_POSTSCRIPT; }


    // miscellaneous other stuff
    // ------------------------------------------------------------------------

    // called by toolkit-specific code to set the app status: active (we have
    // focus) or not and also the last window which had focus before we were
    // deactivated
    virtual void SetActive(bool isActive, wxWindow *lastFocus);


protected:
    // override base class method to use GUI traits
    virtual wxAppTraits *CreateTraits();


    // the main top level window (may be NULL)
    wxWindow *m_topWindow;

    // if Yes, exit the main loop when the last top level window is deleted, if
    // No don't do it and if Later -- only do it once we reach our OnRun()
    //
    // the explanation for using this strange scheme is given in appcmn.cpp
    enum
    {
        Later = -1,
        No,
        Yes
    } m_exitOnFrameDelete;

    // TRUE if the apps whats to use the best visual on systems where
    // more than one are available (Sun, SGI, XFree86 4.0 ?)
    bool m_useBestVisual;

    // does any of our windows has focus?
    bool m_isActive;


    DECLARE_NO_COPY_CLASS(wxAppBase)
};

#endif // wxUSE_GUI

// ----------------------------------------------------------------------------
// now include the declaration of the real class
// ----------------------------------------------------------------------------

#if wxUSE_GUI
    #if defined(__WXMSW__)
        #include "wx/msw/app.h"
    #elif defined(__WXMOTIF__)
        #include "wx/motif/app.h"
    #elif defined(__WXMGL__)
        #include "wx/mgl/app.h"
    #elif defined(__WXGTK__)
        #include "wx/gtk/app.h"
    #elif defined(__WXX11__)
        #include "wx/x11/app.h"
    #elif defined(__WXMAC__)
        #include "wx/mac/app.h"
    #elif defined(__WXCOCOA__)
        #include "wx/cocoa/app.h"
    #elif defined(__WXPM__)
        #include "wx/os2/app.h"
    #endif
#else // !GUI
    // can't use typedef because wxApp forward declared as a class
    class WXDLLEXPORT wxApp : public wxAppConsole
    {
    };
#endif // GUI/!GUI

// ----------------------------------------------------------------------------
// the global data
// ----------------------------------------------------------------------------

// the one and only application object - use of wxTheApp in application code
// is discouraged, consider using DECLARE_APP() after which you may call
// wxGetApp() which will return the object of the correct type (i.e. MyApp and
// not wxApp)
WXDLLEXPORT_DATA(extern wxApp*) wxTheApp;

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

// event loop related functions only work in GUI programs
// ------------------------------------------------------

// Force an exit from main loop
extern void WXDLLEXPORT wxExit();

// Yield to other apps/messages
extern bool WXDLLEXPORT wxYield();

// Yield to other apps/messages
extern void WXDLLEXPORT wxWakeUpIdle();


// console applications may avoid using DECLARE_APP and IMPLEMENT_APP macros
// and call these functions instead at the program startup and termination
// -------------------------------------------------------------------------

#if !wxUSE_GUI

// initialize the library (may be called as many times as needed, but each
// call to wxInitialize() must be matched by wxUninitialize())
extern bool WXDLLEXPORT wxInitialize();

// clean up - the library can't be used any more after the last call to
// wxUninitialize()
extern void WXDLLEXPORT wxUninitialize();

// create an object of this class on stack to initialize/cleanup thel ibrary
// automatically
class WXDLLEXPORT wxInitializer
{
public:
    // initialize the library
    wxInitializer() { m_ok = wxInitialize(); }

    // has the initialization been successful? (explicit test)
    bool IsOk() const { return m_ok; }

    // has the initialization been successful? (implicit test)
    operator bool() const { return m_ok; }

    // dtor only does clean up if we initialized the library properly
    ~wxInitializer() { if ( m_ok ) wxUninitialize(); }

private:
    bool m_ok;
};

#endif // !wxUSE_GUI

// ----------------------------------------------------------------------------
// macros for dynamic creation of the application object
// ----------------------------------------------------------------------------

// Having a global instance of this class allows wxApp to be aware of the app
// creator function. wxApp can then call this function to create a new app
// object. Convoluted, but necessary.

class WXDLLEXPORT wxAppInitializer
{
public:
    wxAppInitializer(wxAppInitializerFunction fn)
        { wxApp::SetInitializerFunction(fn); }
};

// Here's a macro you can use if your compiler really, really wants main() to
// be in your main program (e.g. hello.cpp). Now IMPLEMENT_APP should add this
// code if required.

#if !wxUSE_GUI || defined(__WXMOTIF__) || defined(__WXGTK__) || defined(__WXPM__) || defined(__WXMGL__) || defined(__WXCOCOA__)
    #define IMPLEMENT_WXWIN_MAIN \
        extern int wxEntry( int argc, char **argv ); \
        int main(int argc, char **argv) { return wxEntry(argc, argv); }
#elif defined(__WXMAC__)
    // wxMac seems to have a specific wxEntry prototype
    #define IMPLEMENT_WXWIN_MAIN \
        extern int wxEntry( int argc, char **argv, bool enterLoop = TRUE ); \
        int main(int argc, char **argv) { return wxEntry(argc, argv); }
#elif defined(__WXMSW__) && defined(WXUSINGDLL)
    // NT defines APIENTRY, 3.x not
    #if !defined(WXAPIENTRY)
        #define WXAPIENTRY WXFAR wxSTDCALL
    #endif

    #include <windows.h>
    #include "wx/msw/winundef.h"

    #define IMPLEMENT_WXWIN_MAIN \
        extern "C" int WXAPIENTRY WinMain(HINSTANCE hInstance,\
                                          HINSTANCE hPrevInstance,\
                                          LPSTR m_lpCmdLine, int nCmdShow)\
        {\
            return wxEntry((WXHINSTANCE) hInstance,\
                           (WXHINSTANCE) hPrevInstance,\
                           m_lpCmdLine, nCmdShow);\
        }
#else
    #define IMPLEMENT_WXWIN_MAIN
#endif

#ifdef __WXUNIVERSAL__
    #include "wx/univ/theme.h"

    #define IMPLEMENT_WX_THEME_SUPPORT \
        WX_USE_THEME(win32); \
        WX_USE_THEME(gtk);
#else
    #define IMPLEMENT_WX_THEME_SUPPORT
#endif

// Use this macro if you want to define your own main() or WinMain() function
// and call wxEntry() from there.
#define IMPLEMENT_APP_NO_MAIN(appname)                   \
    wxApp *wxCreateApp()                                 \
    {                                                    \
        wxApp::CheckBuildOptions(wxBuildOptions());      \
        return new appname;                              \
    }                                                    \
    wxAppInitializer wxTheAppInitializer((wxAppInitializerFunction) wxCreateApp); \
    appname& wxGetApp() { return *(appname *)wxTheApp; }

// Same as IMPLEMENT_APP() normally but doesn't include themes support in
// wxUniversal builds
#define IMPLEMENT_APP_NO_THEMES(appname)    \
    IMPLEMENT_APP_NO_MAIN(appname)          \
    IMPLEMENT_WXWIN_MAIN

// Use this macro exactly once, the argument is the name of the wxApp-derived
// class which is the class of your application.
#define IMPLEMENT_APP(appname)              \
    IMPLEMENT_APP_NO_THEMES(appname)        \
    IMPLEMENT_WX_THEME_SUPPORT

// this macro can be used multiple times and just allows you to use wxGetApp()
// function
#define DECLARE_APP(appname) extern appname& wxGetApp();

#endif
    // _WX_APP_H_BASE_
