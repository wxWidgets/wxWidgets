/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/app.h
// Purpose:     wxApp class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_APP_H_
#define _WX_APP_H_

#include "wx/event.h"
#include "wx/icon.h"

class WXDLLIMPEXP_FWD_CORE wxFrame;
class WXDLLIMPEXP_FWD_CORE wxWindow;
class WXDLLIMPEXP_FWD_CORE wxApp;
class WXDLLIMPEXP_FWD_CORE wxKeyEvent;
class WXDLLIMPEXP_FWD_BASE wxLog;

// Represents the application. Derive OnInit and declare
// a new App object to start application
class WXDLLIMPEXP_CORE wxApp : public wxAppBase
{
public:
    wxApp();
    virtual ~wxApp();

    // override base class (pure) virtuals
    virtual bool Initialize(int& argc, wxChar **argv) wxOVERRIDE;
    virtual void CleanUp() wxOVERRIDE;

    virtual void WakeUpIdle() wxOVERRIDE;

    virtual void SetPrintMode(int mode) wxOVERRIDE { m_printMode = mode; }
    virtual int GetPrintMode() const { return m_printMode; }

    // implementation only
    void OnIdle(wxIdleEvent& event);
    void OnEndSession(wxCloseEvent& event);
    void OnQueryEndSession(wxCloseEvent& event);

#if wxUSE_EXCEPTIONS
    virtual bool OnExceptionInMainLoop() wxOVERRIDE;
#endif // wxUSE_EXCEPTIONS

    // MSW-specific from now on
    // ------------------------

    // this suffix should be appended to all our Win32 class names to obtain a
    // variant registered without CS_[HV]REDRAW styles
    static const wxChar *GetNoRedrawClassSuffix() { return wxT("NR"); }

    // Flags for GetRegisteredClassName()
    enum
    {
        // Just a symbolic name indicating absence of any special flags.
        RegClass_Default = 0,

        // Return the name with the GetNoRedrawClassSuffix() appended to it.
        RegClass_ReturnNR = 1,

        // Don't register the class with CS_[HV]REDRAW styles. This is useful
        // for internal windows for which we can guarantee that they will be
        // never created with wxFULL_REPAINT_ON_RESIZE flag.
        //
        // Notice that this implies RegClass_ReturnNR.
        RegClass_OnlyNR = 3
    };

    // get the name of the registered Win32 class with the given (unique) base
    // name: this function constructs the unique class name using this name as
    // prefix, checks if the class is already registered and registers it if it
    // isn't and returns the name it was registered under (or NULL if it failed)
    //
    // the registered class will always have CS_[HV]REDRAW and CS_DBLCLKS
    // styles as well as any additional styles specified as arguments here; and
    // there will be also a companion registered class identical to this one
    // but without CS_[HV]REDRAW whose name will be the same one but with
    // GetNoRedrawClassSuffix()
    //
    // the background brush argument must be either a COLOR_XXX standard value
    // or (default) -1 meaning that the class paints its background itself
    static const wxChar *GetRegisteredClassName(const wxChar *name,
                                                int bgBrushCol = -1,
                                                int extraStyles = 0,
                                                int flags = RegClass_Default);

    // return true if this name corresponds to one of the classes we registered
    // in the previous GetRegisteredClassName() calls
    static bool IsRegisteredClassName(const wxString& name);

    // Return the layout direction to use for a window by default.
    //
    // If the parent is specified, use the same layout direction as it uses.
    // Otherwise use the default global layout, either from wxTheApp, if it
    // exists, or Windows itself.
    //
    // Notice that this normally should not be used for the child windows as
    // they already inherit, just dialogs such as wxMessageDialog may want to
    // use it.
    static wxLayoutDirection MSWGetDefaultLayout(wxWindow* parent = NULL);

    // Call ProcessPendingEvents() but only if we need to do it, i.e. there was
    // a recent call to WakeUpIdle().
    void MSWProcessPendingEventsIfNeeded();

protected:
    int    m_printMode; // wxPRINT_WINDOWS, wxPRINT_POSTSCRIPT

public:
    // unregister any window classes registered by GetRegisteredClassName()
    static void UnregisterWindowClasses();

#if wxUSE_RICHEDIT
    // initialize the richedit DLL of (at least) given version, return true if
    // ok
    static bool InitRichEdit(int version = 2);
#endif // wxUSE_RICHEDIT

    // returns 400, 470, 471 for comctl32.dll 4.00, 4.70, 4.71 or 0 if it
    // wasn't found at all
    static int GetComCtl32Version();

    // the SW_XXX value to be used for the frames opened by the application
    // (currently seems unused which is a bug -- TODO)
    static int m_nCmdShow;

protected:
    wxDECLARE_EVENT_TABLE();
    wxDECLARE_NO_COPY_CLASS(wxApp);
    wxDECLARE_DYNAMIC_CLASS(wxApp);
};

#endif // _WX_APP_H_

