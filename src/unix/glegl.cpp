///////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/glegl.cpp
// Purpose:     code common to all EGL-based wxGLCanvas implementations
// Author:      Scott Talbert
// Created:     2017-12-26
// Copyright:   (c) 2017 Scott Talbert <swt@techie.net>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_GLCANVAS && wxUSE_GLCANVAS_EGL

#include "wx/glcanvas.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
#endif //WX_PRECOMP

#include "wx/scopedptr.h"

#include "wx/gtk/private/wrapgtk.h"
#include "wx/gtk/private/backend.h"
#ifdef GDK_WINDOWING_WAYLAND
#include <gdk/gdkwayland.h>
#include <wayland-egl.h>
#endif
#ifdef GDK_WINDOWING_X11
#include <gdk/gdkx.h>
#endif

#include <EGL/egl.h>
#include <EGL/eglext.h>

// ----------------------------------------------------------------------------
// wxGLContextAttrs: OpenGL rendering context attributes
// ----------------------------------------------------------------------------
// EGL specific values

wxGLContextAttrs& wxGLContextAttrs::CoreProfile()
{
    AddAttribBits(EGL_CONTEXT_OPENGL_PROFILE_MASK,
                  EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT);
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::MajorVersion(int val)
{
    if ( val > 0 )
    {
        AddAttribute(EGL_CONTEXT_MAJOR_VERSION);
        AddAttribute(val);
    }
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::MinorVersion(int val)
{
    if ( val >= 0 )
    {
        AddAttribute(EGL_CONTEXT_MINOR_VERSION);
        AddAttribute(val);
    }
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::CompatibilityProfile()
{
    AddAttribBits(EGL_CONTEXT_OPENGL_PROFILE_MASK,
                  EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT);
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::ForwardCompatible()
{
    AddAttribute(EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE);
    AddAttribute(EGL_TRUE);
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::ES2()
{
    AddAttribBits(EGL_RENDERABLE_TYPE,
                  EGL_OPENGL_ES2_BIT);
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::DebugCtx()
{
    AddAttribute(EGL_CONTEXT_OPENGL_DEBUG);
    AddAttribute(EGL_TRUE);
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::Robust()
{
    AddAttribute(EGL_CONTEXT_OPENGL_ROBUST_ACCESS);
    AddAttribute(EGL_TRUE);
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::NoResetNotify()
{
    AddAttribute(EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY);
    AddAttribute(EGL_NO_RESET_NOTIFICATION);
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::LoseOnReset()
{
    AddAttribute(EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY);
    AddAttribute(EGL_LOSE_CONTEXT_ON_RESET);
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::ResetIsolation()
{
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::ReleaseFlush(int)
{
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::PlatformDefaults()
{
    return *this;
}

void wxGLContextAttrs::EndList()
{
    AddAttribute(EGL_NONE);
}

// ----------------------------------------------------------------------------
// wxGLAttributes: Visual/FBconfig attributes
// ----------------------------------------------------------------------------

wxGLAttributes& wxGLAttributes::RGBA()
{
    return *this;
}

wxGLAttributes& wxGLAttributes::BufferSize(int val)
{
    if ( val >= 0 )
    {
        AddAttribute(EGL_BUFFER_SIZE);
        AddAttribute(val);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::Level(int val)
{
    AddAttribute(EGL_LEVEL);
    AddAttribute(val);
    return *this;
}

wxGLAttributes& wxGLAttributes::DoubleBuffer()
{
    return *this;
}

wxGLAttributes& wxGLAttributes::Stereo()
{
    return *this;
}

wxGLAttributes& wxGLAttributes::AuxBuffers(int)
{
    return *this;
}

wxGLAttributes& wxGLAttributes::MinRGBA(int mRed, int mGreen, int mBlue, int mAlpha)
{
    if ( mRed >= 0)
    {
        AddAttribute(EGL_RED_SIZE);
        AddAttribute(mRed);
    }
    if ( mGreen >= 0)
    {
        AddAttribute(EGL_GREEN_SIZE);
        AddAttribute(mGreen);
    }
    if ( mBlue >= 0)
    {
        AddAttribute(EGL_BLUE_SIZE);
        AddAttribute(mBlue);
    }
    if ( mAlpha >= 0)
    {
        AddAttribute(EGL_ALPHA_SIZE);
        AddAttribute(mAlpha);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::Depth(int val)
{
    if ( val >= 0 )
    {
        AddAttribute(EGL_DEPTH_SIZE);
        AddAttribute(val);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::Stencil(int val)
{
    if ( val >= 0 )
    {
        AddAttribute(EGL_STENCIL_SIZE);
        AddAttribute(val);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::MinAcumRGBA(int, int, int, int)
{
    return *this;
}

wxGLAttributes& wxGLAttributes::SampleBuffers(int val)
{
    if ( val >= 0 )
    {
        AddAttribute(EGL_SAMPLE_BUFFERS);
        AddAttribute(val);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::Samplers(int val)
{
    if ( val >= 0 )
    {
        AddAttribute(EGL_SAMPLES);
        AddAttribute(val);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::FrameBuffersRGB()
{
    return *this;
}

void wxGLAttributes::EndList()
{
    AddAttribute(EGL_NONE);
}

wxGLAttributes& wxGLAttributes::PlatformDefaults()
{
    // No EGL specific values
    return *this;
}

wxGLAttributes& wxGLAttributes::Defaults()
{
    RGBA().DoubleBuffer().Depth(16).SampleBuffers(1).Samplers(4);
    return *this;
}


// ============================================================================
// wxGLContext implementation
// ============================================================================

wxIMPLEMENT_CLASS(wxGLContext, wxObject);

wxGLContext::wxGLContext(wxGLCanvas *win,
                         const wxGLContext *other,
                         const wxGLContextAttrs *ctxAttrs)
    : m_glContext(nullptr)
{
    const int* contextAttribs = nullptr;

    if ( ctxAttrs )
    {
        contextAttribs = ctxAttrs->GetGLAttrs();
    }
    else if ( win->GetGLCTXAttrs().GetGLAttrs() )
    {
        // If OpenGL context parameters were set at wxGLCanvas ctor, get them now
        contextAttribs = win->GetGLCTXAttrs().GetGLAttrs();
    }

    m_isOk = false;

    EGLConfig *fbc = win->GetEGLConfig();
    wxCHECK_RET( fbc, "Invalid EGLConfig for OpenGL" );

    m_glContext = eglCreateContext(wxGLCanvasEGL::GetDisplay(), fbc[0],
                                   other ? other->m_glContext : EGL_NO_CONTEXT,
                                   contextAttribs);

    if ( !m_glContext )
        wxLogMessage(_("Couldn't create OpenGL context"));
    else
        m_isOk = true;
}

wxGLContext::~wxGLContext()
{
    if ( !m_glContext )
        return;

    if ( m_glContext == eglGetCurrentContext() )
        eglMakeCurrent(wxGLCanvasEGL::GetDisplay(), EGL_NO_SURFACE,
                       EGL_NO_SURFACE, EGL_NO_CONTEXT);

    eglDestroyContext(wxGLCanvasEGL::GetDisplay(), m_glContext);
}

bool wxGLContext::SetCurrent(const wxGLCanvas& win) const
{
    if ( !m_glContext )
        return false;

    return eglMakeCurrent(win.GetEGLDisplay(), win.GetEGLSurface(),
                          win.GetEGLSurface(), m_glContext);
}

// ============================================================================
// wxGLCanvasEGL implementation
// ============================================================================

// ----------------------------------------------------------------------------
// initialization methods and dtor
// ----------------------------------------------------------------------------

wxGLCanvasEGL::wxGLCanvasEGL()
{
    m_config = nullptr;
    m_display = nullptr;
    m_surface = EGL_NO_SURFACE;
    m_wlCompositor = nullptr;
    m_wlSubcompositor = nullptr;
    m_wlFrameCallbackHandler = nullptr;
    m_wlEGLWindow = nullptr;
    m_wlSurface = nullptr;
    m_wlRegion = nullptr;
    m_wlSubsurface = nullptr;
    m_readyToDraw = false;
}

bool wxGLCanvasEGL::InitVisual(const wxGLAttributes& dispAttrs)
{
    m_config = InitConfig(dispAttrs);
    if ( !m_config )
    {
        wxFAIL_MSG("Failed to get an EGLConfig for the requested attributes.");
    }
    return m_config != nullptr;
}

/* static */
EGLDisplay wxGLCanvasEGL::GetDisplay()
{
    wxDisplayInfo info = wxGetDisplayInfo();
    EGLenum platform;
    switch ( info.type )
    {
        case wxDisplayX11:
            platform = EGL_PLATFORM_X11_EXT;
            break;
        case wxDisplayWayland:
            platform = EGL_PLATFORM_WAYLAND_EXT;
            break;
        default:
            return EGL_NO_DISPLAY;
    }

    return eglGetPlatformDisplay(platform, info.dpy, nullptr);
}

#ifdef GDK_WINDOWING_WAYLAND

// Helper declared as friend in the header and so can access m_wlSubsurface.
void wxEGLUpdatePosition(wxGLCanvasEGL* win)
{
    int x, y;
    gdk_window_get_origin(win->GTKGetDrawingWindow(), &x, &y);
    wl_subsurface_set_position(win->m_wlSubsurface, x, y);
}

extern "C"
{

static void wl_global (void *data,
                       struct wl_registry *wl_registry,
                       uint32_t name,
                       const char *interface,
                       uint32_t)
{
    wxGLCanvasEGL *glc = static_cast<wxGLCanvasEGL *>(data);

    if ( !strcmp (interface, "wl_compositor") )
        glc->m_wlCompositor = (struct wl_compositor *) wl_registry_bind (wl_registry, name, &wl_compositor_interface, 3);
    else if ( !strcmp (interface, "wl_subcompositor") )
        glc->m_wlSubcompositor = (struct wl_subcompositor *) wl_registry_bind (wl_registry, name, &wl_subcompositor_interface, 1);
}

static void wl_global_remove (void *,
                              struct wl_registry *,
                              uint32_t)
{
}

static const struct wl_registry_listener wl_registry_listener = {
    wl_global,
    wl_global_remove
};

static void wl_frame_callback_handler(void* data,
                                      struct wl_callback *,
                                      uint32_t)
{
    wxGLCanvasEGL *glc = static_cast<wxGLCanvasEGL *>(data);
    glc->m_readyToDraw = true;
    g_clear_pointer(&glc->m_wlFrameCallbackHandler, wl_callback_destroy);
    glc->SendSizeEvent();
    gtk_widget_queue_draw(glc->m_wxwindow);
}

static const struct wl_callback_listener wl_frame_listener = {
    wl_frame_callback_handler
};

static void gtk_glcanvas_size_callback(GtkWidget *widget,
                                       GtkAllocation *,
                                       wxGLCanvasEGL *win)
{
    int scale = gtk_widget_get_scale_factor(widget);
    wl_egl_window_resize(win->m_wlEGLWindow, win->m_width * scale,
                         win->m_height * scale, 0, 0);

    wxEGLUpdatePosition(win);
}

} // extern "C"
#endif // GDK_WINDOWING_WAYLAND

bool wxGLCanvasEGL::CreateSurface()
{
    m_display = GetDisplay();
    if ( m_display == EGL_NO_DISPLAY )
    {
        wxFAIL_MSG("Unable to get EGL Display");
        return false;
    }

    GdkWindow *window = GTKGetDrawingWindow();
#ifdef GDK_WINDOWING_X11
    if (wxGTKImpl::IsX11(window))
    {
        m_xwindow = GDK_WINDOW_XID(window);
        m_surface = eglCreatePlatformWindowSurface(m_display, *m_config,
                                                   &m_xwindow, nullptr);
        m_readyToDraw = true;
    }
#endif
#ifdef GDK_WINDOWING_WAYLAND
    if (wxGTKImpl::IsWayland(window))
    {
        int w = gdk_window_get_width(window);
        int h = gdk_window_get_height(window);
        struct wl_display *display = gdk_wayland_display_get_wl_display(gdk_window_get_display(window));
        struct wl_surface *surface = gdk_wayland_window_get_wl_surface(window);
        struct wl_registry *registry = wl_display_get_registry(display);
        wl_registry_add_listener(registry, &wl_registry_listener, this);
        wl_display_roundtrip(display);
        if ( !m_wlCompositor || !m_wlSubcompositor )
        {
            wxFAIL_MSG("Invalid Wayland compositor or subcompositor");
            return false;
        }
        m_wlSurface = wl_compositor_create_surface(m_wlCompositor);
        m_wlRegion = wl_compositor_create_region(m_wlCompositor);
        m_wlSubsurface = wl_subcompositor_get_subsurface(m_wlSubcompositor,
                                                         m_wlSurface,
                                                         surface);
        wl_surface_set_input_region(m_wlSurface, m_wlRegion);
        wl_subsurface_set_desync(m_wlSubsurface);
        wxEGLUpdatePosition(this);
        int scale = gdk_window_get_scale_factor(window);
        wl_surface_set_buffer_scale(m_wlSurface, scale);
        m_wlEGLWindow = wl_egl_window_create(m_wlSurface, w * scale,
                                             h * scale);
        m_surface = eglCreatePlatformWindowSurface(m_display, *m_config,
                                                   m_wlEGLWindow, nullptr);
        m_wlFrameCallbackHandler = wl_surface_frame(surface);
        wl_callback_add_listener(m_wlFrameCallbackHandler,
                                 &wl_frame_listener, this);
        g_signal_connect(m_widget, "size-allocate",
                         G_CALLBACK(gtk_glcanvas_size_callback), this);
    }
#endif

    if ( m_surface == EGL_NO_SURFACE )
    {
        wxFAIL_MSG("Unable to create EGL surface");
        return false;
    }

    return true;
}

wxGLCanvasEGL::~wxGLCanvasEGL()
{
    if ( m_config && m_config != ms_glEGLConfig )
        delete m_config;
    if ( m_surface )
        eglDestroySurface(m_display, m_surface);
#ifdef GDK_WINDOWING_WAYLAND
    g_clear_pointer(&m_wlEGLWindow, wl_egl_window_destroy);
    g_clear_pointer(&m_wlSubsurface, wl_subsurface_destroy);
    g_clear_pointer(&m_wlSurface, wl_surface_destroy);
    g_clear_pointer(&m_wlFrameCallbackHandler, wl_callback_destroy);
#endif
}

// ----------------------------------------------------------------------------
// working with GL attributes
// ----------------------------------------------------------------------------

/* static */
bool wxGLCanvasBase::IsExtensionSupported(const char *extension)
{
    EGLDisplay dpy = eglGetDisplay(static_cast<EGLNativeDisplayType>(wxGetDisplay()));

    return IsExtensionInList(eglQueryString(dpy, EGL_EXTENSIONS), extension);
}


/* static */
EGLConfig *wxGLCanvasEGL::InitConfig(const wxGLAttributes& dispAttrs)
{
    const int* attrsList = dispAttrs.GetGLAttrs();
    if ( !attrsList )
    {
        wxFAIL_MSG("wxGLAttributes object is empty.");
        return nullptr;
    }

    EGLDisplay dpy = GetDisplay();
    if ( dpy == EGL_NO_DISPLAY ) {
        wxFAIL_MSG("Unable to get EGL Display");
        return nullptr;
    }
    if ( !eglInitialize(dpy, nullptr, nullptr) )
    {
        wxFAIL_MSG("eglInitialize failed");
        return nullptr;
    }
    if ( !eglBindAPI(EGL_OPENGL_API) ) {
        wxFAIL_MSG("eglBindAPI failed");
        return nullptr;
    }

    EGLConfig *config = new EGLConfig;
    int returned;
    // Use the first good match
    if ( eglChooseConfig(dpy, attrsList, config, 1, &returned) && returned == 1 )
    {
        return config;
    }
    else
    {
        delete config;
        return nullptr;
    }
}

/* static */
bool wxGLCanvasBase::IsDisplaySupported(const wxGLAttributes& dispAttrs)
{
    wxScopedPtr<EGLConfig> config(wxGLCanvasEGL::InitConfig(dispAttrs));
    return config != nullptr;
}

/* static */
bool wxGLCanvasBase::IsDisplaySupported(const int *attribList)
{
    wxGLAttributes dispAttrs;
    ParseAttribList(attribList, dispAttrs);

    return IsDisplaySupported(dispAttrs);
}

// ----------------------------------------------------------------------------
// default visual management
// ----------------------------------------------------------------------------

EGLConfig *wxGLCanvasEGL::ms_glEGLConfig = nullptr;

/* static */
bool wxGLCanvasEGL::InitDefaultConfig(const int *attribList)
{
    FreeDefaultConfig();
    wxGLAttributes dispAttrs;
    ParseAttribList(attribList, dispAttrs);

    ms_glEGLConfig = InitConfig(dispAttrs);
    return ms_glEGLConfig != nullptr;
}

/* static */
void wxGLCanvasEGL::FreeDefaultConfig()
{
    if ( ms_glEGLConfig )
    {
        delete ms_glEGLConfig;
        ms_glEGLConfig = nullptr;
    }
}

// ----------------------------------------------------------------------------
// other GL methods
// ----------------------------------------------------------------------------

bool wxGLCanvasEGL::SwapBuffers()
{
    // Under Wayland, if eglSwapBuffers() is called before the wl_surface has
    // been realized, it will deadlock.  Thus, we need to avoid swapping before
    // this has happened.
    if ( !m_readyToDraw )
        return false;

    return eglSwapBuffers(m_display, m_surface);
}

bool wxGLCanvasEGL::IsShownOnScreen() const
{
    wxDisplayInfo info = wxGetDisplayInfo();
    switch ( info.type )
    {
        case wxDisplayX11:
            return GetXWindow() && wxGLCanvasBase::IsShownOnScreen();
        case wxDisplayWayland:
            return m_readyToDraw && wxGLCanvasBase::IsShownOnScreen();
        default:
            return false;
    }
}

#endif // wxUSE_GLCANVAS && wxUSE_GLCANVAS_EGL

