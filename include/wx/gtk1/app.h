/////////////////////////////////////////////////////////////////////////////
// Name:        app.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKAPPH__
#define __GTKAPPH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/window.h"
#include "wx/frame.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxApp;
class wxLog;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern wxApp *wxTheApp;

//-----------------------------------------------------------------------------
// global functions
//-----------------------------------------------------------------------------

void wxExit(void);
bool wxYield(void);

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

#define wxPRINT_WINDOWS         1
#define wxPRINT_POSTSCRIPT      2

//-----------------------------------------------------------------------------
// wxApp
//-----------------------------------------------------------------------------

class wxApp: public wxEvtHandler
{
  DECLARE_DYNAMIC_CLASS(wxApp)

  public:

    wxApp();
    ~wxApp();

    static void SetInitializerFunction(wxAppInitializerFunction fn) { m_appInitFn = fn; }
    static wxAppInitializerFunction GetInitializerFunction() { return m_appInitFn; }

    /* this may have to be overwritten when special, non-default visuals have
       to be set. it is also platform dependent as only X knows about displays
       and visuals. */
    virtual bool InitVisual();

    virtual bool OnInit();
    virtual bool OnInitGui();
    virtual int OnRun();
    virtual int OnExit();

    wxWindow *GetTopWindow();
    void SetTopWindow( wxWindow *win );
    virtual int MainLoop();
    void ExitMainLoop();
    bool Initialized();
    virtual bool Pending();
    virtual void Dispatch();

    inline void SetWantDebugOutput(bool flag) { m_wantDebugOutput = flag; }
    inline bool GetWantDebugOutput() { return m_wantDebugOutput; }

    void OnIdle( wxIdleEvent &event );
    bool SendIdleEvents();
    bool SendIdleEvents( wxWindow* win );

    inline wxString GetAppName() const 
    {
      if (m_appName != "")
        return m_appName;
      else return m_className;
    }
    
    inline void SetAppName(const wxString& name) { m_appName = name; };
    inline wxString GetClassName() const { return m_className; }
    inline void SetClassName(const wxString& name) { m_className = name; }
    const wxString& GetVendorName() const { return m_vendorName; }
    void SetVendorName(const wxString& name) { m_vendorName = name; }

    inline void SetExitOnFrameDelete(bool flag) { m_exitOnFrameDelete = flag; }
    inline bool GetExitOnFrameDelete() const { return m_exitOnFrameDelete; }

    void SetPrintMode(int WXUNUSED(mode) ) {};
    int GetPrintMode() const { return wxPRINT_POSTSCRIPT; };

    // override this function to create default log target of arbitrary
    // user-defined classv (default implementation creates a wxLogGui object)
    virtual wxLog *CreateLogTarget();

  // GTK implementation

    static void CommonInit();
    static void CommonCleanUp();

    bool ProcessIdle();
    void DeletePendingObjects();

    bool          m_initialized;
    bool          m_exitOnFrameDelete;
    bool          m_wantDebugOutput;
    wxWindow     *m_topWindow;
    
    gint          m_idleTag;

    int         argc;
    char      **argv;

    static wxAppInitializerFunction m_appInitFn;
    
private:
  wxString m_vendorName,
           m_appName,
           m_className;

  DECLARE_EVENT_TABLE()
};

#endif // __GTKAPPH__
