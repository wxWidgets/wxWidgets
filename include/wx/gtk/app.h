/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/app.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKAPPH__
#define __GTKAPPH__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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
    virtual ~wxApp();

    /* override for altering the way wxGTK intializes the GUI
     * (palette/visual/colorcube). under wxMSW, OnInitGui() does nothing by
     * default. when overriding this method, the code in it is likely to be
     * platform dependent, otherwise use OnInit(). */
    virtual bool OnInitGui();

    // override base class (pure) virtuals
    virtual bool Yield(bool onlyIfNeeded = FALSE);
    virtual void WakeUpIdle();

    virtual bool Initialize(int& argc, wxChar **argv);
    virtual void CleanUp();

    static bool InitialzeVisual();

#ifdef __WXDEBUG__
    virtual void OnAssert(const wxChar *file, int line, const wxChar *cond, const wxChar *msg);

    bool IsInAssert() const { return m_isInAssert; }
#endif // __WXDEBUG__

    gint            m_idleTag;
    void RemoveIdleTag();
    
    unsigned char  *m_colorCube;

    // Used by the the wxGLApp and wxGLCanvas class for GL-based X visual
    // selection; this is actually an XVisualInfo*
    void           *m_glVisualInfo;
    // This returns the current visual: either that used by wxRootWindow
    // or the XVisualInfo* for SGI.
    GdkVisual      *GetGdkVisual();

private:
    // true if we're inside an assert modal dialog
#ifdef __WXDEBUG__
    bool m_isInAssert;
#endif // __WXDEBUG__

    DECLARE_DYNAMIC_CLASS(wxApp)
    DECLARE_EVENT_TABLE()
};

#endif // __GTKAPPH__
