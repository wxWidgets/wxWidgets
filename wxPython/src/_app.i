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

    // set/get the application name
    wxString GetAppName() const;
    void SetAppName(const wxString& name);

    // set/get the app class name
    wxString GetClassName() const;
    void SetClassName(const wxString& name);

    // set/get the vendor name
    const wxString& GetVendorName() const;
    void SetVendorName(const wxString& name);

    // create the app traits object to which we delegate for everything which
    // either should be configurable by the user (then he can change the
    // default behaviour simply by overriding CreateTraits() and returning his
    // own traits object) or which is GUI/console dependent as then wxAppTraits
    // allows us to abstract the differences behind the common façade
    wxAppTraits *GetTraits();

    // process all events in the wxPendingEvents list -- it is necessary to
    // call this function to process posted events. This happens during each
    // event loop iteration.
    virtual void ProcessPendingEvents();

    // process all currently pending events right now
    //
    // it is an error to call Yield() recursively unless the value of
    // onlyIfNeeded is TRUE
    //
    // WARNING: this function is dangerous as it can lead to unexpected
    //          reentrancies (i.e. when called from an event handler it
    //          may result in calling the same event handler again), use
    //          with _extreme_ care or, better, don't use at all!
    virtual bool Yield(bool onlyIfNeeded = false);

    // make sure that idle events are sent again
    virtual void WakeUpIdle();


    // execute the main GUI loop, the function returns when the loop ends
    virtual int MainLoop();

    // exit the main loop thus terminating the application
    virtual void Exit();

    // exit the main GUI loop during the next iteration (i.e. it does not
    // stop the program immediately!)
    virtual void ExitMainLoop();

    // returns TRUE if there are unprocessed events in the event queue
    virtual bool Pending();

    // process the first event in the event queue (blocks until an event
    // apperas if there are none currently)
    virtual bool Dispatch();


    // this virtual function is called in the GUI mode when the application
    // becomes idle and normally just sends wxIdleEvent to all interested
    // parties
    //
    // it should return TRUE if more idle events are needed, FALSE if not
    virtual bool ProcessIdle() ;

    // Send idle event to window and all subwindows
    // Returns TRUE if more idle time is requested.
    virtual bool SendIdleEvents(wxWindow* win, wxIdleEvent& event);

    // Perform standard OnIdle behaviour: call from port's OnIdle
    void OnIdle(wxIdleEvent& event);


    // return TRUE if our app has focus
    virtual bool IsActive() const;

    // set the "main" top level window
    void SetTopWindow(wxWindow *win);

    // return the "main" top level window (if it hadn't been set previously
    // with SetTopWindow(), will return just some top level window and, if
    // there are none, will return NULL)
    virtual wxWindow *GetTopWindow() const;

    // control the exit behaviour: by default, the program will exit the
    // main loop (and so, usually, terminate) when the last top-level
    // program window is deleted. Beware that if you disable this behaviour
    // (with SetExitOnFrameDelete(FALSE)), you'll have to call
    // ExitMainLoop() explicitly from somewhere.
    void SetExitOnFrameDelete(bool flag);
    bool GetExitOnFrameDelete() const;

#if 0 
    // Get display mode that is used use. This is only used in framebuffer
    // wxWin ports (such as wxMGL).
    virtual wxVideoMode GetDisplayMode() const;
    
    // Set display mode to use. This is only used in framebuffer wxWin
    // ports (such as wxMGL). This method should be called from
    // wxApp::OnInitGui
    virtual bool SetDisplayMode(const wxVideoMode& info);
#endif
    
    // set use of best visual flag (see below)
    void SetUseBestVisual( bool flag );
    bool GetUseBestVisual() const;

    // set/get printing mode: see wxPRINT_XXX constants.
    //
    // default behaviour is the normal one for Unix: always use PostScript
    // printing.
    virtual void SetPrintMode(int mode);
    int GetPrintMode() const;

    // Get/Set OnAssert behaviour.  The following flags may be or'd together:
    //
    //  wxPYAPP_ASSERT_SUPPRESS         Don't do anything
    //  wxPYAPP_ASSERT_EXCEPTION        Turn it into a Python exception if possible
    //  wxPYAPP_ASSERT_DIALOG           Display a message dialog
    //  wxPYAPP_ASSERT_LOG              Write the assertion info to the wxLog
    int  GetAssertMode();
    void SetAssertMode(int mode);


    static bool GetMacDefaultEncodingIsPC();
    static bool GetMacSupportPCMenuShortcuts();
    static long GetMacAboutMenuItemId();
    static long GetMacPreferencesMenuItemId();
    static long GetMacExitMenuItemId();
    static wxString GetMacHelpMenuTitleName();

    static void SetMacDefaultEncodingIsPC(bool val);
    static void SetMacSupportPCMenuShortcuts(bool val);
    static void SetMacAboutMenuItemId(long val);
    static void SetMacPreferencesMenuItemId(long val);
    static void SetMacExitMenuItemId(long val);
    static void SetMacHelpMenuTitleName(const wxString& val);

    // For internal use only
    void _BootstrapApp();

#ifdef __WXMSW__
    // returns 400, 470, 471 for comctl32.dll 4.00, 4.70, 4.71 or 0 if it
    // wasn't found at all
    static int GetComCtl32Version();
#endif
};



//---------------------------------------------------------------------------
%newgroup;

// Force an exit from main loop
void  wxExit();

// Yield to other apps/messages
bool wxYield();
bool wxYieldIfNeeded();
bool wxSafeYield(wxWindow* win=NULL, bool onlyIfNeeded=FALSE);

// Cause the message queue to become empty again
void  wxWakeUpIdle();

// Send an event to be processed later
void wxPostEvent(wxEvtHandler *dest, wxEvent& event);

// This is used to cleanup after wxWindows when Python shuts down.
%inline %{
    void wxApp_CleanUp() {
        __wxPyCleanup();
    }
%} 

// Return a reference to the current wxApp object.
%inline %{
    wxPyApp* wxGetApp() {
        return (wxPyApp*)wxTheApp;
    }
%}


//---------------------------------------------------------------------------
// Include some extra wxApp related python code here

%pythoncode "_app_ex.py"

//---------------------------------------------------------------------------

