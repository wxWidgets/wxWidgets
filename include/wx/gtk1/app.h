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

#include "wx/frame.h"
#include "wx/icon.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxApp;
class wxLog;

//-----------------------------------------------------------------------------
// wxApp
//-----------------------------------------------------------------------------

class wxApp: public wxAppBase
{
public:
    wxApp();
    ~wxApp();

    /* override for altering the way wxGTK intializes the GUI
     * (palette/visual/colorcube). under wxMSW, OnInitGui() does nothing by
     * default. when overriding this method, the code in it is likely to be
     * platform dependent, otherwise use OnInit(). */
    virtual bool OnInitGui();

    // override base class (pure) virtuals
    virtual int MainLoop();
    virtual void ExitMainLoop();
    virtual bool Initialized();
    virtual bool Pending();
    virtual void Dispatch();

    virtual wxIcon GetStdIcon(int which) const;

    // implementation only from now on
    void OnIdle( wxIdleEvent &event );
    bool SendIdleEvents();
    bool SendIdleEvents( wxWindow* win );

    static bool Initialize();
    static bool InitialzeVisual();
    static void CleanUp();

    bool ProcessIdle();
#if wxUSE_THREADS
    void ProcessPendingEvents();
#endif
    void DeletePendingObjects();

    // This can be used to suppress the generation of Idle events.
    void SuppressIdleEvents(bool arg = TRUE) { m_suppressIdleEvents = arg; }
    bool GetSuppressIdleEvents() const { return m_suppressIdleEvents; }

    bool            m_initialized;

    gint            m_idleTag;
#if wxUSE_THREADS
    gint            m_wakeUpTimerTag;
#endif
    unsigned char  *m_colorCube;

private:
    /// Set to TRUE while we are in wxYield().
    bool m_suppressIdleEvents;

    DECLARE_DYNAMIC_CLASS(wxApp)
    DECLARE_EVENT_TABLE()
};

#endif // __GTKAPPH__
