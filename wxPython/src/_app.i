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




class wxPyApp : public wxEvtHandler {
public:

    %addtofunc wxPyApp
       "self._setCallbackInfo(self, PyApp)
        self._setOORInfo(self)";

    %extend {
        wxPyApp() {
            wxPythonApp = new wxPyApp();
            return wxPythonApp;
        }
    }

    ~wxPyApp();

    void _setCallbackInfo(PyObject* self, PyObject* _class);


    DocDeclStr(
        wxString, GetAppName() const,
        "Get the application name.");
    DocDeclStr(
        void, SetAppName(const wxString& name),
        "Set the application name. This value may be used automatically\n"
        "by wxConfig and such.");

    DocDeclStr(
        wxString, GetClassName() const,
        "Get the application's class name.");
    DocDeclStr(
        void, SetClassName(const wxString& name),
        "Set the application's class name. This value may be used for X-resources if\n"
        "applicable for the platform");

    DocDeclStr(
        const wxString&, GetVendorName() const,
        "Get the application's vendor name.");
    DocDeclStr(
        void, SetVendorName(const wxString& name),
        "Set the application's vendor name. This value may be used automatically\n"
        "by wxConfig and such.");


    DocDeclStr(
        wxAppTraits*, GetTraits(),
        "Create the app traits object to which we delegate for everything which either\n"
        "should be configurable by the user (then he can change the default behaviour\n"
        "simply by overriding CreateTraits() and returning his own traits object) or\n"
        "which is GUI/console dependent as then wxAppTraits allows us to abstract the\n"
        "differences behind the common facade");


    DocDeclStr(
        virtual void, ProcessPendingEvents(),
        "Process all events in the wxPendingEvents list -- it is necessary to call this\n"
        "function to process posted events. This happens during each event loop\n"
        "iteration.");


    DocDeclStr(
        virtual bool, Yield(bool onlyIfNeeded = False),
        "Process all currently pending events right now, instead of waiting until\n"
        "return to the event loop.  It is an error to call Yield() recursively unless\n"
        "the value of onlyIfNeeded is True.\n"
        "\n"
        "WARNING: This function is dangerous as it can lead to unexpected\n"
        "         reentrancies (i.e. when called from an event handler it\n"
        "         may result in calling the same event handler again), use\n"
        "         with _extreme_ care or, better, don't use at all!\n");

    
    DocDeclStr(
        virtual void, WakeUpIdle(),
        "Make sure that idle events are sent again");


    DocDeclStr(
        virtual int, MainLoop(),
        "Execute the main GUI loop, the function returns when the loop ends.");

    
    DocDeclStr(
        virtual void, Exit(),
        "Exit the main loop thus terminating the application.");

    
    DocDeclStr(
        virtual void, ExitMainLoop(),
        "Exit the main GUI loop during the next iteration (i.e. it does not\n"
        "stop the program immediately!)");

    
    DocDeclStr(
        virtual bool, Pending(),
        "Returns True if there are unprocessed events in the event queue.");

    
    DocDeclStr(
        virtual bool, Dispatch(),
        "Process the first event in the event queue (blocks until an event\n"
        "appears if there are none currently)");


    DocDeclStr(
        virtual bool, ProcessIdle(),
        "Called from the MainLoop when the application becomes idle and sends an\n"
        "IdleEvent to all interested parties.  Returns True is more idle events are\n"
        "needed, False if not.");

    
    DocDeclStr(
        virtual bool, SendIdleEvents(wxWindow* win, wxIdleEvent& event),
        "Send idle event to window and all subwindows.  Returns True if more idle time\n"
        "is requested.");


    DocDeclStr(
        virtual bool, IsActive() const,
        "Return True if our app has focus.");

    
    DocDeclStr(
        void, SetTopWindow(wxWindow *win),
        "Set the \"main\" top level window");

    DocDeclStr(
        virtual wxWindow*, GetTopWindow() const,
        "Return the \"main\" top level window (if it hadn't been set previously with\n"
        "SetTopWindow(), will return just some top level window and, if there not any,\n"
        "will return None)");        


    DocDeclStr(
        void, SetExitOnFrameDelete(bool flag),
        "Control the exit behaviour: by default, the program will exit the main loop\n"
        "(and so, usually, terminate) when the last top-level program window is\n"
        "deleted.  Beware that if you disable this behaviour (with\n"
        "SetExitOnFrameDelete(False)), you'll have to call ExitMainLoop() explicitly\n"
        "from somewhere.\n");

    
    DocDeclStr(
        bool, GetExitOnFrameDelete() const,
        "Get the current exit behaviour setting.");

#if 0 
    // Get display mode that is used use. This is only used in framebuffer
    // wxWin ports (such as wxMGL).
    virtual wxVideoMode GetDisplayMode() const;
    
    // Set display mode to use. This is only used in framebuffer wxWin
    // ports (such as wxMGL). This method should be called from
    // wxApp::OnInitGui
    virtual bool SetDisplayMode(const wxVideoMode& info);
#endif
    
    
    DocDeclStr(
        void, SetUseBestVisual( bool flag ),
        "Set whether the app should try to use the best available visual on systems\n"
        "where more than one is available, (Sun, SGI, XFree86 4, etc.)");
    
    DocDeclStr(
        bool, GetUseBestVisual() const,
        "Get current UseBestVisual setting.");
    

    // set/get printing mode: see wxPRINT_XXX constants.
    //
    // default behaviour is the normal one for Unix: always use PostScript
    // printing.
    virtual void SetPrintMode(int mode);
    int GetPrintMode() const;


    DocDeclStr(
        void, SetAssertMode(int mode),
        "Set the OnAssert behaviour for debug and hybrid builds.  The following flags\n"
        "may be or'd together:\n"
        "\n"
        " wxPYAPP_ASSERT_SUPPRESS         Don't do anything\n"
        " wxPYAPP_ASSERT_EXCEPTION        Turn it into a Python exception if possible (default)\n"
        " wxPYAPP_ASSERT_DIALOG           Display a message dialog\n"
        " wxPYAPP_ASSERT_LOG              Write the assertion info to the wxLog\n");
    
    DocDeclStr(
        int,  GetAssertMode(),
        "Get the current OnAssert behaviour setting.");


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
        "For internal use only");

    DocStr(GetComCtl32Version,
           "Returns 400, 470, 471 for comctl32.dll 4.00, 4.70, 4.71 or 0 if it\n"
           "wasn't found at all.  Raises an exception on non-Windows platforms.");
#ifdef __WXMSW__
    static int GetComCtl32Version();
#else
    %extend {
        static int GetComCtl32Version()
            { PyErr_SetNone(PyExc_NotImplementedError); return 0; }
    }
#endif
};



//---------------------------------------------------------------------------
%newgroup;


DocDeclStr(
    void,  wxExit(),
    "Force an exit of the application.  Convenience for wx.GetApp().Exit()");


DocDeclStr(
    bool, wxYield(),
    "Yield to other apps/messages.  Convenience for wx.GetApp().Yield()");

DocDeclStr(
    bool, wxYieldIfNeeded(),
    "Yield to other apps/messages.  Convenience for wx.GetApp().Yield(True)");


DocDeclStr(
    bool, wxSafeYield(wxWindow* win=NULL, bool onlyIfNeeded=False),
    "This function is similar to wx.Yield, except that it disables the user input\n"
    "to all program windows before calling wx.Yield and re-enables it again\n"
    "afterwards. If win is not None, this window will remain enabled, allowing the\n"
    "implementation of some limited user interaction.\n"
    "\n"
    "Returns the result of the call to wx.Yield.");


DocDeclStr(
    void,  wxWakeUpIdle(),
    "Cause the message queue to become empty again, so idle events will be sent.");


DocDeclStr(
    void, wxPostEvent(wxEvtHandler *dest, wxEvent& event),
    "Send an event to a window or other wx.EvtHandler to be processed later.");


DocStr(wxApp_CleanUp,
       "For internal use only, it is used to cleanup after wxWindows when Python shuts down.");
%inline %{
    void wxApp_CleanUp() {
        __wxPyCleanup();
    }
%} 


DocStr(wxGetApp,
       "Return a reference to the current wxApp object.");
%inline %{
    wxPyApp* wxGetApp() {
        return (wxPyApp*)wxTheApp;
    }
%}


//---------------------------------------------------------------------------
// Include some extra wxApp related python code here

%pythoncode "_app_ex.py"

//---------------------------------------------------------------------------

