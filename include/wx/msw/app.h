/////////////////////////////////////////////////////////////////////////////
// Name:        app.h
// Purpose:     wxApp class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_APP_H_
#define _WX_APP_H_

#ifdef __GNUG__
#pragma interface "app.h"
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/event.h"

class WXDLLEXPORT wxFrame;
class WXDLLEXPORT wxWindow;
class WXDLLEXPORT wxApp ;
class WXDLLEXPORT wxKeyEvent;
class WXDLLEXPORT wxLog;

static const int wxPRINT_WINDOWS = 1;
static const int wxPRINT_POSTSCRIPT = 2;

WXDLLEXPORT_DATA(extern wxApp*) wxTheApp;

// Force an exit from main loop
void WXDLLEXPORT wxExit();

// Yield to other apps/messages
bool WXDLLEXPORT wxYield();

// Represents the application. Derive OnInit and declare
// a new App object to start application
class WXDLLEXPORT wxApp: public wxEvtHandler
{
  DECLARE_DYNAMIC_CLASS(wxApp)
  wxApp();
  ~wxApp();

  static void SetInitializerFunction(wxAppInitializerFunction fn) { m_appInitFn = fn; }
  static wxAppInitializerFunction GetInitializerFunction() { return m_appInitFn; }

  virtual int MainLoop();
  void ExitMainLoop();
  bool Initialized();
  virtual bool Pending() ;
  virtual void Dispatch() ;

  void OnIdle(wxIdleEvent& event);
  void OnEndSession(wxCloseEvent& event);
  void OnQueryEndSession(wxCloseEvent& event);

  // Generic
  virtual bool OnInit() { return FALSE; };

  // No specific tasks to do here.
  virtual bool OnInitGui() { return TRUE; }

  // Called to set off the main loop
  virtual int OnRun() { return MainLoop(); };
  virtual int OnExit() { return 0; }

  // called when a fatal exception occurs, this function should take care not
  // to do anything which might provoke a nested exception!
  virtual void OnFatalException() { }

  void SetPrintMode(int mode) { m_printMode = mode; }
  int GetPrintMode() const { return m_printMode; }

  void SetExitOnFrameDelete(bool flag) { m_exitOnFrameDelete = flag; }
  bool GetExitOnFrameDelete() const { return m_exitOnFrameDelete; }

  const wxString& GetAppName() const {
      if (m_appName != _T(""))
        return m_appName;
      else return m_className;
    }

  void SetAppName(const wxString& name) { m_appName = name; };
  wxString GetClassName() const { return m_className; }
  void SetClassName(const wxString& name) { m_className = name; }

  void SetVendorName(const wxString& vendorName) { m_vendorName = vendorName; }
  const wxString& GetVendorName() const { return m_vendorName; }

  wxWindow *GetTopWindow() const ;
  void SetTopWindow(wxWindow *win) { m_topWindow = win; }

  void SetWantDebugOutput(bool flag) { m_wantDebugOutput = flag; }
  bool GetWantDebugOutput() { return m_wantDebugOutput; }

  // Send idle event to all top-level windows.
  // Returns TRUE if more idle time is requested.
  bool SendIdleEvents();

  // Send idle event to window and all subwindows
  // Returns TRUE if more idle time is requested.
  bool SendIdleEvents(wxWindow* win);

  void SetAuto3D(bool flag) { m_auto3D = flag; }
  bool GetAuto3D() const { return m_auto3D; }

  // Creates a log object
  virtual wxLog* CreateLogTarget();

public:
  int               argc;
  char **           argv;

protected:
  bool              m_wantDebugOutput ;
  wxString          m_className;
  wxString          m_appName,
                    m_vendorName;
  wxWindow *        m_topWindow;
  bool              m_exitOnFrameDelete;
  bool              m_showOnInit;
  int               m_printMode; // wxPRINT_WINDOWS, wxPRINT_POSTSCRIPT
  bool              m_auto3D ;   // Always use 3D controls, except
                                 // where overriden
  static wxAppInitializerFunction m_appInitFn;

/* Windows-specific wxApp definitions */

public:

  // Implementation
  static bool Initialize();
  static void CleanUp();

  static bool RegisterWindowClasses();
  // Convert Windows to argc, argv style
  void ConvertToStandardCommandArgs(char* p);
  virtual bool DoMessage();
  virtual bool ProcessMessage(WXMSG* pMsg);
  void DeletePendingObjects();
  bool ProcessIdle();
#if wxUSE_THREADS
  void ProcessPendingEvents();
#endif
  int GetComCtl32Version() const;

public:
  int               m_nCmdShow;

protected:
  bool              m_keepGoing ;

DECLARE_EVENT_TABLE()
};

#if !defined(_WINDLL) || (defined(_WINDLL) && defined(WXMAKINGDLL))
int WXDLLEXPORT wxEntry(WXHINSTANCE hInstance, WXHINSTANCE hPrevInstance, char *lpszCmdLine,
                    int nCmdShow, bool enterLoop = TRUE);
#else
int WXDLLEXPORT wxEntry(WXHINSTANCE hInstance);
#endif

#endif
    // _WX_APP_H_

