///////////////////////////////////////////////////////////////////////////////
// Name:        wx/unix/glegl.h
// Purpose:     class common for all EGL-based wxGLCanvas implementations
// Author:      Scott Talbert
// Created:     2017-12-26
// Copyright:   (c) 2017 Scott Talbert <swt@techie.net>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIX_GLEGL_H_
#define _WX_UNIX_GLEGL_H_

#include <GL/gl.h>

// This is to avoid including Wayland & EGL headers here to pollute namespace
struct wl_compositor;
struct wl_subcompositor;
struct wl_callback;
struct wl_egl_window;
struct wl_surface;
struct wl_region;
struct wl_subsurface;
typedef void *EGLDisplay;
typedef void *EGLConfig;
typedef void *EGLSurface;
typedef void *EGLContext;

class wxGLContextAttrs;
class wxGLAttributes;

// ----------------------------------------------------------------------------
// wxGLContext
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_GL wxGLContext : public wxGLContextBase
{
public:
    wxGLContext(wxGLCanvas *win,
                const wxGLContext *other = nullptr,
                const wxGLContextAttrs *ctxAttrs = nullptr);
    virtual ~wxGLContext();

    virtual bool SetCurrent(const wxGLCanvas& win) const override;

private:
    EGLContext m_glContext;

    wxDECLARE_CLASS(wxGLContext);
};

// ----------------------------------------------------------------------------
// wxGLCanvasEGL
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_GL wxGLCanvasEGL : public wxGLCanvasBase
{
public:
    // initialization and dtor
    // -----------------------

    // default ctor doesn't do anything, InitConfig() must be called
    wxGLCanvasEGL() = default;

    // initializes EGLConfig corresponding to the given attributes
    bool InitVisual(const wxGLAttributes& dispAttrs);

    // creates EGLSurface
    bool CreateSurface();

    virtual ~wxGLCanvasEGL();

    // Wayland-specific callbacks
    // --------------------------

    void CreateWaylandSubsurface();
    void DestroyWaylandSubsurface();

    // implement wxGLCanvasBase methods
    // --------------------------------

    virtual bool SwapBuffers() override;


    // X11-specific methods
    // --------------------

    // get the X11 handle of this window
    virtual unsigned long GetXWindow() const = 0;


    // override some wxWindow methods
    // ------------------------------

    // return true only if the window is realized: OpenGL context can't be
    // created until we are
    virtual bool IsShownOnScreen() const override;


    // implementation only from now on
    // -------------------------------

    // get the EGLConfig we use
    EGLConfig GetEGLConfig() const { return m_config; }
    EGLDisplay GetEGLDisplay() const { return m_display; }
    EGLSurface GetEGLSurface() const { return m_surface; }

    static EGLDisplay GetDisplay();

    // initialize the global default GL config, return false if matching config
    // not found
    static bool InitDefaultConfig(const int *attribList);

    // get the default EGL Config (may be null, shouldn't be freed by caller)
    static EGLConfig GetDefaultConfig() { return ms_glEGLConfig; }

    // free the global GL visual, called by wxGLApp
    static void FreeDefaultConfig();

    // initializes EGLConfig
    //
    // returns nullptr if EGLConfig couldn't be initialized, otherwise caller
    // is responsible for freeing the pointer
    static EGLConfig InitConfig(const wxGLAttributes& dispAttrs);

    // Only called when using Wayland to indicate that we should be redrawn.
    void OnWLFrameCallback();

    wl_compositor *m_wlCompositor = nullptr;
    wl_subcompositor *m_wlSubcompositor = nullptr;
    wl_callback *m_wlFrameCallbackHandler = nullptr;
    wl_egl_window *m_wlEGLWindow = nullptr;

private:
    // Set correct m_wlSubsurface position.
    //
    // This is defined only when using Wayland.
    void UpdateSubsurfacePosition();

    // Call eglCreatePlatformWindowSurface() when using EGL 1.5 or later,
    // otherwise try eglCreatePlatformWindowSurfaceEXT() if it's available and
    // fall back on eglCreateWindowSurface() otherwise.
    //
    // This function uses m_display and m_config which must be initialized
    // before using it and should be passed either m_xwindow or m_wlEGLWindow
    // depending on whether we are using X11 or Wayland.
    EGLSurface CallCreatePlatformWindowSurface(void *window) const;


    EGLConfig m_config = nullptr;
    EGLDisplay m_display = nullptr;
    EGLSurface m_surface = nullptr;

    unsigned long m_xwindow = 0;
    wl_surface *m_wlSurface = nullptr;
    wl_region *m_wlRegion = nullptr;
    wl_subsurface *m_wlSubsurface = nullptr;

    bool m_readyToDraw = false;
    bool m_swapIntervalSet = false;

    // the global/default versions of the above
    static EGLConfig ms_glEGLConfig;

    // Called from GTK callbacks and needs access to private members.
    friend void wxEGLUpdateGeometry(GtkWidget* widget, wxGLCanvasEGL* win);
};

// ----------------------------------------------------------------------------
// wxGLApp
// ----------------------------------------------------------------------------

// this is used in wx/glcanvas.h, prevent it from defining a generic wxGLApp
#define wxGL_APP_DEFINED

class WXDLLIMPEXP_GL wxGLApp : public wxGLAppBase
{
public:
    wxGLApp() : wxGLAppBase() { }

    // implement wxGLAppBase method
    virtual bool InitGLVisual(const int *attribList) override
    {
        return wxGLCanvasEGL::InitDefaultConfig(attribList);
    }

#ifndef __WXGTK3__
    virtual void* GetXVisualInfo() override
    {
        return wxGLCanvasEGL::GetDefaultConfig();
    }
#endif // GTK < 3

    // and override this wxApp method to clean up
    virtual int OnExit() override
    {
        wxGLCanvasEGL::FreeDefaultConfig();

        return wxGLAppBase::OnExit();
    }

private:
    wxDECLARE_DYNAMIC_CLASS(wxGLApp);
};

#endif // _WX_UNIX_GLEGL_H_
