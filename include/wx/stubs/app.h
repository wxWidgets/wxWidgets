/////////////////////////////////////////////////////////////////////////////
// Name:        app.h
// Purpose:     wxApp class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_APP_H_
#define _WX_APP_H_

#ifdef __GNUG__
#pragma interface "app.h"
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/gdicmn.h"
#include "wx/event.h"

class WXDLLEXPORT wxFrame;
class WXDLLEXPORT wxWindow;
class WXDLLEXPORT wxApp ;
class WXDLLEXPORT wxKeyEvent;
class WXDLLEXPORT wxLog;

#define wxPRINT_WINDOWS         1
#define wxPRINT_POSTSCRIPT      2

WXDLLEXPORT_DATA(extern wxApp*) wxTheApp;

void WXDLLEXPORT wxCleanUp();
void WXDLLEXPORT wxCommonCleanUp(); // Call this from the platform's wxCleanUp()
void WXDLLEXPORT wxCommonInit();    // Call this from the platform's initialization

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
  inline ~wxApp() {}

  static void SetInitializerFunction(wxAppInitializerFunction fn) { m_appInitFn = fn; }
  static wxAppInitializerFunction GetInitializerFunction() { return m_appInitFn; }

  virtual int MainLoop();
  void ExitMainLoop();
  bool Initialized();
  virtual bool Pending() ;
  virtual void Dispatch() ;

  virtual void OnIdle(wxIdleEvent& event);

// Generic
  virtual bool OnInit() { return FALSE; };

  // No specific tasks to do here.
  virtual bool OnInitGui() { return TRUE; }

  // Called to set off the main loop
  virtual int OnRun() { return MainLoop(); };
  virtual int OnExit() { return 0; }

  inline void SetPrintMode(int mode) { m_printMode = mode; }
  inline int GetPrintMode() const { return m_printMode; }

  inline void SetExitOnFrameDelete(bool flag) { m_exitOnFrameDelete = flag; }
  inline bool GetExitOnFrameDelete() const { return m_exitOnFrameDelete; }

  inline wxString GetAppName() const {
      if (m_appName != "")
        return m_appName;
      else return m_className;
    }

  inline void SetAppName(const wxString& name) { m_appName = name; };
  inline wxString GetClassName() const { return m_className; }
  inline void SetClassName(const wxString& name) { m_className = name; }

  void SetVendorName(const wxString& vendorName) { m_vendorName = vendorName; }
  const wxString& GetVendorName() const { return m_vendorName; }

  wxWindow *GetTopWindow() const ;
  inline void SetTopWindow(wxWindow *win) { m_topWindow = win; }

  inline void SetWantDebugOutput(bool flag) { m_wantDebugOutput = flag; }
  inline bool GetWantDebugOutput() { return m_wantDebugOutput; }

  // Send idle event to all top-level windows.
  // Returns TRUE if more idle time is requested.
  bool SendIdleEvents();

  // Send idle event to window and all subwindows
  // Returns TRUE if more idle time is requested.
  bool SendIdleEvents(wxWindow* win);

  // Windows only, but for compatibility...
  inline void SetAuto3D(bool flag) { m_auto3D = flag; }
  inline bool GetAuto3D() const { return m_auto3D; }

  // Creates a log object
  virtual wxLog* CreateLogTarget();

public:
  // Will always be set to the appropriate, main-style values.
  int                   argc;
  char **               argv;

protected:
  bool                  m_wantDebugOutput ;
  wxString              m_className;
  wxString              m_appName,
                        m_vendorName;
  wxWindow *            m_topWindow;
  bool                  m_exitOnFrameDelete;
  bool                  m_showOnInit;
  int                   m_printMode; // wxPRINT_WINDOWS, wxPRINT_POSTSCRIPT
  bool                  m_auto3D ;   // Always use 3D controls, except
                                 // where overriden
  static wxAppInitializerFunction	m_appInitFn;

public:

  // Implementation
  static void CommonInit();
  static void CommonCleanUp();
  void DeletePendingObjects();
  bool ProcessIdle();

public:
  static long           sm_lastMessageTime;
  int                   m_nCmdShow;

protected:
  bool                  m_keepGoing ;

DECLARE_EVENT_TABLE()
};

// TODO: add platform-specific arguments
int WXDLLEXPORT wxEntry( int argc, char *argv[] );

#endif
    // _WX_APP_H_

