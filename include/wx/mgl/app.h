/////////////////////////////////////////////////////////////////////////////
// Name:        app.h
// Purpose:
// Author:      Vaclav Slavik
// Id:          $Id$
// Copyright:   (c) 2001 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_APP_H__
#define __WX_APP_H__

#ifdef __GNUG__
#pragma interface "app.h"
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

class WXDLLEXPORT wxApp: public wxAppBase
{
public:
    wxApp() {}
    ~wxApp() {}

    /* override for altering the way wxGTK intializes the GUI
     * (palette/visual/colorcube). under wxMSW, OnInitGui() does nothing by
     * default. when overriding this method, the code in it is likely to be
     * platform dependent, otherwise use OnInit(). */
    virtual bool OnInitGui() {return 0;}

    // override base class (pure) virtuals
    virtual int MainLoop() {return 0;}
    virtual void ExitMainLoop() {}
    virtual bool Initialized() {return 0;}
    virtual bool Pending() {return 0;}
    virtual void Dispatch() {}

    virtual wxIcon GetStdIcon(int which) const {return wxNullIcon;}

    // implementation only from now on
    void OnIdle( wxIdleEvent &event ) {}
    bool SendIdleEvents() {return 0;}
    bool SendIdleEvents( wxWindow* win ) {return 0;}

    static bool Initialize() {return 0;}
    static bool InitialzeVisual() {return 0;}
    static void CleanUp() {}

    bool ProcessIdle() {return 0;}
    void DeletePendingObjects() {}

    // This can be used to suppress the generation of Idle events.
    void SuppressIdleEvents(bool arg = TRUE) { m_suppressIdleEvents = arg; }
    bool GetSuppressIdleEvents() const { return m_suppressIdleEvents; }

#if 0 //FIXME MGL
    bool            m_initialized;

    gint            m_idleTag;
#if wxUSE_THREADS
    gint            m_wakeUpTimerTag;
#endif
    unsigned char  *m_colorCube;
#endif

private:
    /// Set to TRUE while we are in wxYield().
    bool m_suppressIdleEvents;

private:
    DECLARE_DYNAMIC_CLASS(wxApp)
    DECLARE_EVENT_TABLE()
};

int WXDLLEXPORT wxEntry( int argc, char *argv[] );

#endif // __WX_APP_H__
