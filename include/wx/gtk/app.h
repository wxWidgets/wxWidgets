/////////////////////////////////////////////////////////////////////////////
// Name:        app.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKAPPH__
#define __GTKAPPH__

#ifdef __GNUG__
#pragma interface
#endif

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
  
    wxApp(void);
    ~wxApp(void);
    
    static void SetInitializerFunction(wxAppInitializerFunction fn) { m_appInitFn = fn; }
    static wxAppInitializerFunction GetInitializerFunction(void) { return m_appInitFn; }
    virtual bool OnInit(void);
    virtual bool OnInitGui(void);
    virtual int OnRun(void);
    virtual bool OnIdle(void);
    virtual int OnExit(void);
    
    wxWindow *GetTopWindow(void);
    void SetTopWindow( wxWindow *win );
    virtual int MainLoop(void);
    void ExitMainLoop(void);
    bool Initialized(void);
    virtual bool Pending(void);
    virtual void Dispatch(void);
    void DeletePendingObjects(void);
    
    inline wxString GetAppName(void) const {
      if (m_appName != "")
        return m_appName;
      else return m_className;
    }
    inline void SetAppName(const wxString& name) { m_appName = name; };
    inline wxString GetClassName(void) const { return m_className; }
    inline void SetClassName(const wxString& name) { m_className = name; }

    inline void SetExitOnFrameDelete(bool flag) { m_exitOnFrameDelete = flag; }
    inline bool GetExitOnFrameDelete(void) const { return m_exitOnFrameDelete; }
  
    void SetPrintMode(int WXUNUSED(mode) ) {}; 
    int GetPrintMode(void) const { return wxPRINT_POSTSCRIPT; };
  
    static void CommonInit(void);
    static void CommonCleanUp(void);    
    
    // override this function to create default log target of arbitrary
    // user-defined classv (default implementation creates a wxLogGui object)
    virtual wxLog *CreateLogTarget();
    
    bool          m_initialized;
    bool          m_exitOnFrameDelete;
    gint          m_idleTag;
    wxWindow     *m_topWindow;
    wxString      m_appName;
    wxString      m_className;
    
    int         argc;
    char      **argv;
    
    static wxAppInitializerFunction m_appInitFn;
};

#endif // __GTKAPPH__
