///////////////////////////////////////////////////////////////////////////////
// Name:        wx/unix/private/glegl.h
// Purpose:     wxGLContext EGL-based implementation
// Author:      Vadim Zeitlin
// Created:     2025-12-07
// Copyright:   (c) 2025 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIX_PRIVATE_GLEGL_H_
#define _WX_UNIX_PRIVATE_GLEGL_H_

#include "wx/unix/private/glcanvas.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>

struct wl_compositor;
struct wl_subcompositor;
struct wl_callback;
struct wl_egl_window;
struct wl_surface;
struct wl_region;
struct wl_subsurface;

// ----------------------------------------------------------------------------
// wxGLContextEGL
// ----------------------------------------------------------------------------

class wxGLContextEGL : public wxGLContextImpl
{
public:
    wxGLContextEGL(wxGLCanvas *win,
                   const wxGLContext *other = nullptr,
                   const wxGLContextAttrs *ctxAttrs = nullptr);
    virtual ~wxGLContextEGL();

    virtual bool SetCurrent(const wxGLCanvas& win) const override;

private:
    EGLContext m_glContext;
};

// ----------------------------------------------------------------------------
// wxGLCanvasEGL
// ----------------------------------------------------------------------------

class wxGLCanvasEGL : public wxGLCanvasUnixImpl
{
public:
    explicit wxGLCanvasEGL(wxGLCanvasUnix* canvas)
        : wxGLCanvasUnixImpl(canvas)
    {
    }

    virtual ~wxGLCanvasEGL() override;

    virtual bool InitVisual(const wxGLAttributes& dispAttrs) override;

    virtual bool SwapBuffers() override;

    virtual bool DoSetSwapInterval(int interval) override;
    virtual int GetSwapInterval() const override;

    virtual void OnRealized() override;

    virtual bool HasWindow() const override;

    virtual void* GetXVisualInfo() const override { return nullptr; }

    // implementation only from now on
    // -------------------------------

    void CreateWaylandSubsurface();
    void DestroyWaylandSubsurface();

    EGLConfig GetEGLConfig() const { return m_config; }
    EGLDisplay GetEGLDisplay() const { return m_display; }
    EGLSurface GetEGLSurface() const { return m_surface; }

    static EGLDisplay GetDisplay();

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

    // Called from GTK callbacks and needs access to private members.
    friend void wxEGLUpdateGeometry(GtkWidget* widget, wxGLCanvasEGL* win);
};

// ----------------------------------------------------------------------------
// wxGLBackendEGL
// ----------------------------------------------------------------------------

class wxGLBackendEGL : public wxGLBackend
{
public:
    static wxGLBackend& Get();

    wxGLBackendEGL() = default;

    std::unique_ptr<wxGLContextImpl>
    CreateContextImpl(wxGLCanvas* win,
                      const wxGLContext* other,
                      const wxGLContextAttrs* ctxAttrs) override;

    std::unique_ptr<wxGLCanvasUnixImpl>
    CreateCanvasImpl(wxGLCanvasUnix* canvas) override;

    void ClearCurrentContext() override;

    bool IsExtensionSupported(const char* extension) override;
    bool IsDisplaySupported(const wxGLAttributes& dispAttrs) override;
    bool GetEGLVersion(int* major, int* minor) override;
    int GetGLXVersion() override;
    bool IsGLXMultiSampleAvailable() override;

    bool InitDefaultVisualInfo(const int* attribList) override;
    void* GetDefaultVisualInfo() override;
    void FreeDefaultVisualInfo() override;

    wxGLContextAttrs& CoreProfile(wxGLContextAttrs& attrs) override;
    wxGLContextAttrs& MajorVersion(wxGLContextAttrs& attrs, int val) override;
    wxGLContextAttrs& MinorVersion(wxGLContextAttrs& attrs, int val) override;
    wxGLContextAttrs& CompatibilityProfile(wxGLContextAttrs& attrs) override;
    wxGLContextAttrs& ForwardCompatible(wxGLContextAttrs& attrs) override;
    wxGLContextAttrs& ES2(wxGLContextAttrs& attrs) override;
    wxGLContextAttrs& DebugCtx(wxGLContextAttrs& attrs) override;
    wxGLContextAttrs& Robust(wxGLContextAttrs& attrs) override;
    wxGLContextAttrs& NoResetNotify(wxGLContextAttrs& attrs) override;
    wxGLContextAttrs& LoseOnReset(wxGLContextAttrs& attrs) override;
    wxGLContextAttrs& ResetIsolation(wxGLContextAttrs& attrs) override;
    wxGLContextAttrs& ReleaseFlush(wxGLContextAttrs& attrs, int val) override;
    wxGLContextAttrs& PlatformDefaults(wxGLContextAttrs& attrs) override;

    wxGLAttributes& RGBA(wxGLAttributes& attrs) override;
    wxGLAttributes& BufferSize(wxGLAttributes& attrs, int val) override;
    wxGLAttributes& Level(wxGLAttributes& attrs, int val) override;
    wxGLAttributes& DoubleBuffer(wxGLAttributes& attrs) override;
    wxGLAttributes& Stereo(wxGLAttributes& attrs) override;
    wxGLAttributes& AuxBuffers(wxGLAttributes& attrs, int val) override;
    wxGLAttributes& MinRGBA(wxGLAttributes& attrs, int mRed, int mGreen, int mBlue, int mAlpha) override;
    wxGLAttributes& Depth(wxGLAttributes& attrs, int val) override;
    wxGLAttributes& Stencil(wxGLAttributes& attrs, int val) override;
    wxGLAttributes& MinAcumRGBA(wxGLAttributes& attrs, int mRed, int mGreen, int mBlue, int mAlpha) override;
    wxGLAttributes& PlatformDefaults(wxGLAttributes& attrs) override;
    wxGLAttributes& SampleBuffers(wxGLAttributes& attrs, int val) override;
    wxGLAttributes& Samplers(wxGLAttributes& attrs, int val) override;
    wxGLAttributes& FrameBuffersRGB(wxGLAttributes& attrs) override;

    void EndList(wxGLAttribsBase& attrs) override;
};

#endif // _WX_UNIX_PRIVATE_GLEGL_H_
