/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/app.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_APP_H_
#define _WX_GTK_APP_H_

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

#if wxUSE_THREADS
class WXDLLIMPEXP_BASE wxMutex;
#endif

//-----------------------------------------------------------------------------
// wxApp
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxApp: public wxAppBase
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

#ifdef __WXDEBUG__
    virtual void OnAssertFailure(const wxChar *file,
                                 int line,
                                 const wxChar *func,
                                 const wxChar *cond,
                                 const wxChar *msg);
#endif // __WXDEBUG__

    // GTK-specific methods
    // -------------------

    // this can be overridden to return a specific visual to be used for GTK+
    // instead of the default one (it's used by wxGLApp)
    //
    // must return XVisualInfo pointer (it is not freed by caller)
    virtual void *GetXVisualInfo() { return NULL; }


    // implementation only from now on
    // -------------------------------

    // This returns the current visual: either that used by wxRootWindow
    // or the XVisualInfo* for SGI.
    GdkVisual      *GetGdkVisual();

    // check for pending events, without interference from our idle source
    bool EventsPending();
    bool DoIdle();

private:
    // true if we're inside an assert modal dialog
#ifdef __WXDEBUG__
    bool m_isInAssert;
#endif // __WXDEBUG__
#if wxUSE_THREADS
    wxMutex* m_idleMutex;
#endif
    guint m_idleSourceId;

    DECLARE_DYNAMIC_CLASS(wxApp)
    DECLARE_EVENT_TABLE()
};

#endif // _WX_GTK_APP_H_
