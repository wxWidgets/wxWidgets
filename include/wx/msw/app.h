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

#include "wx/event.h"
#include "wx/icon.h"

class WXDLLEXPORT wxFrame;
class WXDLLEXPORT wxWindow;
class WXDLLEXPORT wxApp ;
class WXDLLEXPORT wxKeyEvent;
class WXDLLEXPORT wxLog;

// Represents the application. Derive OnInit and declare
// a new App object to start application
class WXDLLEXPORT wxApp : public wxAppBase
{
    DECLARE_DYNAMIC_CLASS(wxApp)

public:
    wxApp();
    virtual ~wxApp();

    // override base class (pure) virtuals
    virtual bool Initialize(int& argc, wxChar **argv);
    virtual void CleanUp();

    virtual int MainLoop();
    virtual void ExitMainLoop();
    virtual bool Initialized();
    virtual bool Pending();
    virtual void Dispatch();

    virtual bool Yield(bool onlyIfNeeded = FALSE);
    virtual void WakeUpIdle();

    virtual void SetPrintMode(int mode) { m_printMode = mode; }
    virtual int GetPrintMode() const { return m_printMode; }

    // implementation only
    void OnIdle(wxIdleEvent& event);
    void OnEndSession(wxCloseEvent& event);
    void OnQueryEndSession(wxCloseEvent& event);

protected:
    int    m_printMode; // wxPRINT_WINDOWS, wxPRINT_POSTSCRIPT

    /* Windows-specific wxApp definitions */

public:

    // Implementation
    static bool RegisterWindowClasses();
    static bool UnregisterWindowClasses();

    // message processing
    // ------------------

    // process the given message
    virtual void DoMessage(WXMSG *pMsg);

    // retrieve the next message from the queue and process it
    virtual bool DoMessage();

    // preprocess the message
    virtual bool ProcessMessage(WXMSG* pMsg);

    // idle processing
    // ---------------

#if wxUSE_RICHEDIT
    // initialize the richedit DLL of (at least) given version, return TRUE if
    // ok (Win95 has version 1, Win98/NT4 has 1 and 2, W2K has 3)
    static bool InitRichEdit(int version = 2);
#endif // wxUSE_RICHEDIT

    // returns 400, 470, 471 for comctl32.dll 4.00, 4.70, 4.71 or 0 if it
    // wasn't found at all
    static int GetComCtl32Version();

public:
    // the SW_XXX value to be used for the frames opened by the application
    // (currently seems unused which is a bug -- TODO)
    static int m_nCmdShow;

protected:
    // we exit the main event loop when this flag becomes false
    bool m_keepGoing;

    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(wxApp)
};

int WXDLLEXPORT wxEntry(WXHINSTANCE hInstance, WXHINSTANCE hPrevInstance,
                        char *lpszCmdLine, int nCmdShow);

#endif // _WX_APP_H_

