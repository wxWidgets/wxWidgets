/////////////////////////////////////////////////////////////////////////////
// Name:        app.h
// Purpose:     wxApp class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __APPH__
#define __APPH__

#ifdef __GNUG__
#pragma interface "app.h"
#endif

#include "wx/defs.h"
#include "wx/object.h"

class WXDLLEXPORT wxFrame;
class WXDLLEXPORT wxWindow;
class WXDLLEXPORT wxApp ;
class WXDLLEXPORT wxKeyEvent;
class WXDLLEXPORT wxLog;

#if USE_WXCONFIG
  class WXDLLEXPORT wxConfig;
#endif //USE_WXCONFIG 

#define wxPRINT_WINDOWS         1
#define wxPRINT_POSTSCRIPT      2

WXDLLEXPORT_DATA(extern wxApp*) wxTheApp;

void WXDLLEXPORT wxCleanUp(void);
void WXDLLEXPORT wxCommonCleanUp(void); // Call this from the platform's wxCleanUp()
void WXDLLEXPORT wxCommonInit(void);    // Call this from the platform's initialization

// Force an exit from main loop
void WXDLLEXPORT wxExit(void);

// Yield to other apps/messages
bool WXDLLEXPORT wxYield(void);

// Represents the application. Derive OnInit and declare
// a new App object to start application
class WXDLLEXPORT wxApp: public wxEvtHandler
{
  DECLARE_DYNAMIC_CLASS(wxApp)
  wxApp(void);
  inline ~wxApp(void) {}

  static void SetInitializerFunction(wxAppInitializerFunction fn) { m_appInitFn = fn; }
  static wxAppInitializerFunction GetInitializerFunction(void) { return m_appInitFn; }

  virtual int MainLoop(void);
  void ExitMainLoop(void);
  bool Initialized(void);
  virtual bool Pending(void) ;
  virtual void Dispatch(void) ;

  virtual void OnIdle(wxIdleEvent& event);

  // Windows specific. Intercept keyboard input.
#if WXWIN_COMPATIBILITY == 2
  virtual bool OldOnCharHook(wxKeyEvent& event);
#endif

// Generic
  virtual bool OnInit(void) { return FALSE; };

  // No specific tasks to do here.
  virtual bool OnInitGui(void) { return TRUE; }

  // Called to set off the main loop
  virtual int OnRun(void) { return MainLoop(); };
  virtual int OnExit(void) { return 0; };
  inline void SetPrintMode(int mode) { m_printMode = mode; }
  inline int GetPrintMode(void) const { return m_printMode; }
  
  inline void SetExitOnFrameDelete(bool flag) { m_exitOnFrameDelete = flag; }
  inline bool GetExitOnFrameDelete(void) const { return m_exitOnFrameDelete; }

/*
  inline void SetShowFrameOnInit(bool flag) { m_showOnInit = flag; }
  inline bool GetShowFrameOnInit(void) const { return m_showOnInit; }
*/

  inline wxString GetAppName(void) const {
      if (m_appName != "")
        return m_appName;
      else return m_className;
    }

  inline void SetAppName(const wxString& name) { m_appName = name; };
  inline wxString GetClassName(void) const { return m_className; }
  inline void SetClassName(const wxString& name) { m_className = name; }
  wxWindow *GetTopWindow(void) const ;
  inline void SetTopWindow(wxWindow *win) { m_topWindow = win; }

  inline void SetWantDebugOutput(bool flag) { m_wantDebugOutput = flag; }
  inline bool GetWantDebugOutput(void) { return m_wantDebugOutput; }

  // Send idle event to all top-level windows.
  // Returns TRUE if more idle time is requested.
  bool SendIdleEvents(void);

  // Send idle event to window and all subwindows
  // Returns TRUE if more idle time is requested.
  bool SendIdleEvents(wxWindow* win);

  inline void SetAuto3D(const bool flag) { m_auto3D = flag; }
  inline bool GetAuto3D(void) const { return m_auto3D; }

  // Creates a log object
  virtual wxLog* CreateLogTarget();

#if USE_WXCONFIG
  // override this function to create a global wxConfig object of different
  // than default type (right now the default implementation returns NULL)
  virtual wxConfig* CreateConfig() { return NULL; }
#endif //USE_WXCONFIG 

public:
//  void (*work_proc)(wxApp*app); // work procedure;
  int               argc;
  char **           argv;

protected:
  bool              m_wantDebugOutput ;
  wxString          m_className;
  wxString          m_appName;
  wxWindow *        m_topWindow;
  bool              m_exitOnFrameDelete;
  bool              m_showOnInit;
  int               m_printMode; // wxPRINT_WINDOWS, wxPRINT_POSTSCRIPT
  bool              m_auto3D ;   // Always use 3D controls, except
                                 // where overriden
  static wxAppInitializerFunction	m_appInitFn;

/* Windows-specific wxApp definitions */

public:

  // Implementation
  static bool Initialize(WXHINSTANCE instance);
  static void CommonInit(void);
  static bool RegisterWindowClasses(void);
  static void CleanUp(void);
  static void CommonCleanUp(void);
  virtual bool DoMessage(void);
  virtual bool ProcessMessage(WXMSG* pMsg);
  void DeletePendingObjects(void);
  bool ProcessIdle(void);

/*
  inline void SetPendingCleanup(bool flag) { m_pendingCleanup = flag; }
  inline bool GetPendingCleanup(void) { return m_pendingCleanup; }

  bool DoResourceCleanup(void);
  // Set resource collection scheme on or off.
  inline void SetResourceCollection(bool flag) { m_resourceCollection = flag; }
  inline bool GetResourceCollection(void) { return m_resourceCollection; }
*/

public:
  static long       sm_lastMessageTime;
  int               m_nCmdShow;

protected:
  bool              m_keepGoing ;
//  bool              m_resourceCollection;
//  bool              m_pendingCleanup; // TRUE if we need to check the GDI object lists for cleanup

DECLARE_EVENT_TABLE()
};

#if !defined(_WINDLL) || (defined(_WINDLL) && defined(WXMAKINGDLL))
int WXDLLEXPORT wxEntry(WXHINSTANCE hInstance, WXHINSTANCE hPrevInstance, char *lpszCmdLine,
                    int nCmdShow, bool enterLoop = TRUE);
#else
int WXDLLEXPORT wxEntry(WXHINSTANCE hInstance);
#endif

#endif
    // __APPH__

