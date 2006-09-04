/////////////////////////////////////////////////////////////////////////////
// Name:        _app.i
// Purpose:     SWIG interface for wxApp
//
// Author:      Robin Dunn
//
// Created:     9-Aug-2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
// TODOs:
//
// 1. Provide another app object that allows FilterEvent to be overloaded.
// 2. Wrap wxAppTraits and allow wxApp::CreateTraits to be overloaded.
//
//---------------------------------------------------------------------------
%newgroup;

%{
%}

enum {
    wxPYAPP_ASSERT_SUPPRESS  = 1,
    wxPYAPP_ASSERT_EXCEPTION = 2,
    wxPYAPP_ASSERT_DIALOG    = 4,
    wxPYAPP_ASSERT_LOG       = 8
};

enum
{
    wxPRINT_WINDOWS = 1,
    wxPRINT_POSTSCRIPT = 2
};



DocStr(wxPyApp,
"The ``wx.PyApp`` class is an *implementation detail*, please use the
`wx.App` class (or some other derived class) instead.", "");

class wxPyApp : public wxEvtHandler {
public:

    %pythonAppend wxPyApp
       "self._setCallbackInfo(self, PyApp, False)
        self._setOORInfo(self, False)";
    %typemap(out) wxPyApp*;    // turn off this typemap

    DocStr(wxPyApp,
           "Create a new application object, starting the bootstrap process.", "");
    %extend {
        wxPyApp() {
            wxPythonApp = new wxPyApp();
            return wxPythonApp;
        }
    }

    ~wxPyApp();

    // Turn it back on again
    %typemap(out) wxPyApp* { $result = wxPyMake_wxObject($1, $owner); }


    void _setCallbackInfo(PyObject* self, PyObject* _class, bool incref);


    DocDeclStr(
        wxString, GetAppName() const,
        "Get the application name.", "");
    DocDeclStr(
        void, SetAppName(const wxString& name),
        "Set the application name. This value may be used automatically by
`wx.Config` and such.", "");

    DocDeclStr(
        wxString, GetClassName() const,
        "Get the application's class name.", "");
    DocDeclStr(
        void, SetClassName(const wxString& name),
        "Set the application's class name. This value may be used for
X-resources if applicable for the platform", "");

    DocDeclStr(
        const wxString&, GetVendorName() const,
        "Get the application's vendor name.", "");
    DocDeclStr(
        void, SetVendorName(const wxString& name),
        "Set the application's vendor name. This value may be used
automatically by `wx.Config` and such.", "");


    DocDeclStr(
        wxAppTraits*, GetTraits(),
        "Return (and create if necessary) the app traits object to which we
delegate for everything which either should be configurable by the
user (then he can change the default behaviour simply by overriding
CreateTraits() and returning his own traits object) or which is
GUI/console dependent as then wx.AppTraits allows us to abstract the
differences behind the common facade.

:todo: Add support for overriding CreateAppTraits in wxPython.", "");


    DocDeclStr(
        virtual void, ProcessPendingEvents(),
        "Process all events in the Pending Events list -- it is necessary to
call this function to process posted events. This normally happens
during each event loop iteration.", "");


    DocDeclStr(
        virtual bool, Yield(bool onlyIfNeeded = false),
        "Process all currently pending events right now, instead of waiting
until return to the event loop.  It is an error to call ``Yield``
recursively unless the value of ``onlyIfNeeded`` is True.

:warning: This function is dangerous as it can lead to unexpected
      reentrancies (i.e. when called from an event handler it may
      result in calling the same event handler again), use with
      extreme care or, better, don't use at all!

:see: `wx.Yield`, `wx.YieldIfNeeded`, `wx.SafeYield`
", "");

    
    DocDeclStr(
        virtual void, WakeUpIdle(),
        "Make sure that idle events are sent again.
:see: `wx.WakeUpIdle`", "");

    
    DocDeclStr(
        static bool , IsMainLoopRunning() const,
        "Returns True if we're running the main loop, i.e. if the events can
currently be dispatched.", "");
    

    DocDeclStr(
        virtual int, MainLoop(),
        "Execute the main GUI loop, the function doesn't normally return until
all top level windows have been closed and destroyed.", "");

    
    DocDeclStr(
        virtual void, Exit(),
        "Exit the main loop thus terminating the application.
:see: `wx.Exit`", "");

    
    DocDeclStr(
        virtual wxLayoutDirection , GetLayoutDirection() const,
        "Return the layout direction for the current locale.", "");
    
    
    DocDeclStr(
        virtual void, ExitMainLoop(),
        "Exit the main GUI loop during the next iteration of the main
loop, (i.e. it does not stop the program immediately!)", "");

    
    DocDeclStr(
        virtual bool, Pending(),
        "Returns True if there are unprocessed events in the event queue.", "");

    
    DocDeclStr(
        virtual bool, Dispatch(),
        "Process the first event in the event queue (blocks until an event
appears if there are none currently)", "");


    DocDeclStr(
        virtual bool, ProcessIdle(),
        "Called from the MainLoop when the application becomes idle (there are
no pending events) and sends a `wx.IdleEvent` to all interested
parties.  Returns True if more idle events are needed, False if not.", "");

    
    DocDeclStr(
        virtual bool, SendIdleEvents(wxWindow* win, wxIdleEvent& event),
        "Send idle event to window and all subwindows.  Returns True if more
idle time is requested.", "");


    DocDeclStr(
        virtual bool, IsActive() const,
        "Return True if our app has focus.", "");

    
    DocDeclStr(
        void, SetTopWindow(wxWindow *win),
        "Set the *main* top level window", "");

    DocDeclStr(
        virtual wxWindow*, GetTopWindow() const,
        "Return the *main* top level window (if it hadn't been set previously
with SetTopWindow(), will return just some top level window and, if
there not any, will return None)", "");        


    DocDeclStr(
        void, SetExitOnFrameDelete(bool flag),
        "Control the exit behaviour: by default, the program will exit the main
loop (and so, usually, terminate) when the last top-level program
window is deleted.  Beware that if you disable this behaviour (with
SetExitOnFrameDelete(False)), you'll have to call ExitMainLoop()
explicitly from somewhere.", "");

    
    DocDeclStr(
        bool, GetExitOnFrameDelete() const,
        "Get the current exit behaviour setting.", "");

#if 0 
    // Get display mode that is in use. This is only used in framebuffer
    // wxWin ports (such as wxMGL).
    virtual wxVideoMode GetDisplayMode() const;
    
    // Set display mode to use. This is only used in framebuffer wxWin
    // ports (such as wxMGL). This method should be called from
    // wxApp::OnInitGui
    virtual bool SetDisplayMode(const wxVideoMode& info);
#endif
    
    
    DocDeclStr(
        void, SetUseBestVisual( bool flag ),
        "Set whether the app should try to use the best available visual on
systems where more than one is available, (Sun, SGI, XFree86 4, etc.)", "");
    
    DocDeclStr(
        bool, GetUseBestVisual() const,
        "Get current UseBestVisual setting.", "");
    

    // set/get printing mode: see wxPRINT_XXX constants.
    //
    // default behaviour is the normal one for Unix: always use PostScript
    // printing.
    virtual void SetPrintMode(int mode);
    int GetPrintMode() const;


    DocDeclStr(
        void, SetAssertMode(int mode),
        "Set the OnAssert behaviour for debug and hybrid builds.",
        "The following flags may be or'd together:

   =========================   =======================================
   wx.PYAPP_ASSERT_SUPPRESS    Don't do anything
   wx.PYAPP_ASSERT_EXCEPTION   Turn it into a Python exception if possible
                               (default)
   wx.PYAPP_ASSERT_DIALOG      Display a message dialog
   wx.PYAPP_ASSERT_LOG         Write the assertion info to the wx.Log
   =========================   =======================================

");
    
    DocDeclStr(
        int,  GetAssertMode(),
        "Get the current OnAssert behaviour setting.", "");


    static bool GetMacSupportPCMenuShortcuts();
    static long GetMacAboutMenuItemId();
    static long GetMacPreferencesMenuItemId();
    static long GetMacExitMenuItemId();
    static wxString GetMacHelpMenuTitleName();

    static void SetMacSupportPCMenuShortcuts(bool val);
    static void SetMacAboutMenuItemId(long val);
    static void SetMacPreferencesMenuItemId(long val);
    static void SetMacExitMenuItemId(long val);
    static void SetMacHelpMenuTitleName(const wxString& val);


    DocDeclStr(
        void, _BootstrapApp(),
        "For internal use only", "");

    DocStr(GetComCtl32Version,
           "Returns 400, 470, 471, etc. for comctl32.dll 4.00, 4.70, 4.71 or 0 if
it wasn't found at all.  Raises an exception on non-Windows platforms.", "");
#ifdef __WXMSW__
    static int GetComCtl32Version();
#else
    %extend {
        static int GetComCtl32Version()
            { wxPyRaiseNotImplemented(); return 0; }
    }
#endif

    %extend {
        DocStr(DisplayAvailable,
               "Tests if it is possible to create a GUI in the current environment.
This will mean different things on the different platforms.

   * On X Windows systems this function will return ``False`` if it is
     not able to open a connection to the X display, which can happen
     if $DISPLAY is not set, or is not set correctly.

   * On Mac OS X a ``False`` return value will mean that wx is not
     able to access the window manager, which can happen if logged in
     remotely or if running from the normal version of python instead
     of the framework version, (i.e., pythonw.)

   * On MS Windows...
", "");
        static bool DisplayAvailable() {
            return wxPyTestDisplayAvailable();
        }
    }
};



//---------------------------------------------------------------------------
%newgroup;


DocDeclStr(
    void,  wxExit(),
    "Force an exit of the application.  Convenience for wx.GetApp().Exit()", "");


DocDeclStr(
    bool, wxYield(),
    "Yield to other apps/messages.  Convenience for wx.GetApp().Yield()", "");

DocDeclStr(
    bool, wxYieldIfNeeded(),
    "Yield to other apps/messages.  Convenience for wx.GetApp().Yield(True)", "");


DocDeclStr(
    bool, wxSafeYield(wxWindow* win=NULL, bool onlyIfNeeded=false),
    "This function is similar to `wx.Yield`, except that it disables the
user input to all program windows before calling `wx.Yield` and
re-enables it again afterwards. If ``win`` is not None, this window
will remain enabled, allowing the implementation of some limited user
interaction.

:Returns: the result of the call to `wx.Yield`.", "");


DocDeclStr(
    void,  wxWakeUpIdle(),
    "Cause the message queue to become empty again, so idle events will be
sent.", "");


DocDeclStr(
    void, wxPostEvent(wxEvtHandler *dest, wxEvent& event),
    "Send an event to a window or other wx.EvtHandler to be processed
later.", "");


DocStr(wxApp_CleanUp,
       "For internal use only, it is used to cleanup after wxWidgets when
Python shuts down.", "");
%inline %{
    void wxApp_CleanUp() {
        __wxPyCleanup();
    }
%} 


DocDeclStrName(
    wxPyApp* , wxPyGetApp(),
    "Return a reference to the current wx.App object.", "",
    GetApp);
%{
    wxPyApp* wxPyGetApp() { return (wxPyApp*)wxTheApp; }
%}





DocDeclAStr(
    void , wxSetDefaultPyEncoding(const char* encoding),
    "SetDefaultPyEncoding(string encoding)",
    "Sets the encoding that wxPython will use when it needs to convert a
Python string or unicode object to or from a wxString.

The default encoding is the value of ``locale.getdefaultlocale()[1]``
but please be aware that the default encoding within the same locale
may be slightly different on different platforms.  For example, please
see http://www.alanwood.net/demos/charsetdiffs.html for differences
between the common latin/roman encodings.", "");

DocDeclAStr(
    const char* , wxGetDefaultPyEncoding(),
    "GetDefaultPyEncoding() -> string",
    "Gets the current encoding that wxPython will use when it needs to
convert a Python string or unicode object to or from a wxString.", "");


//---------------------------------------------------------------------------
// Include some extra wxApp related python code here

%pythoncode "_app_ex.py"

//---------------------------------------------------------------------------

