/////////////////////////////////////////////////////////////////////////////
// Name:        app.h
// Purpose:     wxApp class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_APP_H_
#define _WX_APP_H_

#ifdef __GNUG__
#pragma interface "app.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/gdicmn.h"
#include "wx/event.h"

// ----------------------------------------------------------------------------
// forward declarations
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxFrame;
class WXDLLEXPORT wxWindow;
class WXDLLEXPORT wxApp;
class WXDLLEXPORT wxKeyEvent;
class WXDLLEXPORT wxLog;
class WXDLLEXPORT wxEventLoop;

// ----------------------------------------------------------------------------
// the wxApp class for wxX11 - see wxAppBase for more details
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxApp : public wxAppBase
{
    DECLARE_DYNAMIC_CLASS(wxApp)
        
public:
    wxApp();
    ~wxApp();
    
    // override base class (pure) virtuals
    // -----------------------------------
    
    virtual int MainLoop();
    virtual void ExitMainLoop();
    virtual bool Initialized();
    virtual bool Pending();
    virtual void Dispatch();
    virtual bool Yield(bool onlyIfNeeded = FALSE);
    virtual bool ProcessIdle();
    
    virtual bool OnInitGui();
    
    // implementation from now on
    // --------------------------
    
    void OnIdle(wxIdleEvent& event);
    
    // Send idle event to all top-level windows.
    // Returns TRUE if more idle time is requested.
    bool SendIdleEvents();
    
    // Send idle event to window and all subwindows
    // Returns TRUE if more idle time is requested.
    bool SendIdleEvents(wxWindow* win);
    
    // Processes an X event.
    virtual bool ProcessXEvent(WXEvent* event);
    
#ifdef __WXDEBUG__
    virtual void OnAssert(const wxChar *file, int line, const wxChar* cond, const wxChar *msg);
#endif // __WXDEBUG__
    
protected:
    bool                  m_showOnInit;
    
public:
    // Implementation
    static bool Initialize();
    static void CleanUp();
    
    void DeletePendingObjects();
    
    WXWindow       GetTopLevelWidget() const { return m_topLevelWidget; }
    WXColormap     GetMainColormap(WXDisplay* display);
    long           GetMaxRequestSize() const { return m_maxRequestSize; }
    
    // This handler is called when a property change event occurs
    virtual bool HandlePropertyChange(WXEvent *event);
    
    // Values that can be passed on the command line.
    // Returns -1, -1 if none specified.
    const wxSize& GetInitialSize() const { return m_initialSize; }
    bool GetShowIconic() const { return m_showIconic; }
    
#if wxUSE_UNICODE
    // Global context for Pango layout. Either use X11
    // or use Xft rendering according to GDK_USE_XFT
    // environment variable
    PangoContext* GetPangoContext();
#endif    
    
    // We need this before creating the app
    static   WXDisplay* GetDisplay() { return ms_display; }
    static   WXDisplay* ms_display;

public:
    static long           sm_lastMessageTime;
    bool                  m_showIconic;    
    wxSize                m_initialSize;

#if !wxUSE_NANOX    
    // Someone find a better place for these
    int                   m_visualType;   // TrueColor, DirectColor etc.
    int                   m_visualDepth;
    int                   m_visualColormapSize;
    void                 *m_visualColormap;
    int                   m_visualScreen;
    unsigned long         m_visualRedMask;
    unsigned long         m_visualGreenMask;
    unsigned long         m_visualBlueMask;
    int                   m_visualRedShift;
    int                   m_visualGreenShift;
    int                   m_visualBlueShift;
    int                   m_visualRedPrec;
    int                   m_visualGreenPrec;
    int                   m_visualBluePrec;
    
    unsigned char        *m_colorCube;
#endif
    
protected:
    bool                  m_keepGoing;
    
    WXWindow              m_topLevelWidget;
    WXColormap            m_mainColormap;
    long                  m_maxRequestSize;
    wxEventLoop*          m_mainLoop;
    
    DECLARE_EVENT_TABLE()
};

int WXDLLEXPORT wxEntry( int argc, char *argv[] );

#endif
// _WX_APP_H_

