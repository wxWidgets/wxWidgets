///////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/glegl.cpp
// Purpose:     EGL-based wxGLCanvas implementation for wxGTK (this file is
//              in src/unix subdirectory only for historical reasons)
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

#if wxUSE_GLCANVAS && defined(wxHAS_EGL)

#include "wx/glcanvas.h"
#include "wx/unix/private/glegl.h"

#include "wx/private/make_unique.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
#endif //WX_PRECOMP

#include "wx/gtk/private/wrapgdk.h"
#include "wx/gtk/private/backend.h"
#ifdef GDK_WINDOWING_WAYLAND
#include <wayland-egl.h>
#endif

#include <memory>

namespace
{

constexpr const char* TRACE_EGL = "glegl";

// It's ok to have this global variable because its ctor and dtor are trivial.
wxGLBackendEGL wxGLBackendEGL_instance;

// EGL version initialized by InitConfig().
EGLint gs_eglMajor = 0;
EGLint gs_eglMinor = 0;

} // anonymous namespace

// ----------------------------------------------------------------------------
// wxGLContextAttrs: OpenGL rendering context attributes
// ----------------------------------------------------------------------------
// EGL specific values

wxGLContextAttrs& wxGLBackendEGL::CoreProfile(wxGLContextAttrs& attrs)
{
    attrs.AddAttribBits(EGL_CONTEXT_OPENGL_PROFILE_MASK,
                  EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT);
    return attrs;
}

wxGLContextAttrs& wxGLBackendEGL::MajorVersion(wxGLContextAttrs& attrs, int val)
{
    if ( val > 0 )
    {
        attrs.AddAttribute(EGL_CONTEXT_MAJOR_VERSION);
        attrs.AddAttribute(val);
    }
    return attrs;
}

wxGLContextAttrs& wxGLBackendEGL::MinorVersion(wxGLContextAttrs& attrs, int val)
{
    if ( val >= 0 )
    {
        attrs.AddAttribute(EGL_CONTEXT_MINOR_VERSION);
        attrs.AddAttribute(val);
    }
    return attrs;
}

wxGLContextAttrs& wxGLBackendEGL::CompatibilityProfile(wxGLContextAttrs& attrs)
{
    attrs.AddAttribBits(EGL_CONTEXT_OPENGL_PROFILE_MASK,
                  EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT);
    return attrs;
}

wxGLContextAttrs& wxGLBackendEGL::ForwardCompatible(wxGLContextAttrs& attrs)
{
    attrs.AddAttribute(EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE);
    attrs.AddAttribute(EGL_TRUE);
    return attrs;
}

wxGLContextAttrs& wxGLBackendEGL::ES2(wxGLContextAttrs& attrs)
{
    attrs.useES = true;
    return attrs;
}

wxGLContextAttrs& wxGLBackendEGL::DebugCtx(wxGLContextAttrs& attrs)
{
    attrs.AddAttribute(EGL_CONTEXT_OPENGL_DEBUG);
    attrs.AddAttribute(EGL_TRUE);
    return attrs;
}

wxGLContextAttrs& wxGLBackendEGL::Robust(wxGLContextAttrs& attrs)
{
    attrs.AddAttribute(EGL_CONTEXT_OPENGL_ROBUST_ACCESS);
    attrs.AddAttribute(EGL_TRUE);
    return attrs;
}

wxGLContextAttrs& wxGLBackendEGL::NoResetNotify(wxGLContextAttrs& attrs)
{
    attrs.AddAttribute(EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY);
    attrs.AddAttribute(EGL_NO_RESET_NOTIFICATION);
    return attrs;
}

wxGLContextAttrs& wxGLBackendEGL::LoseOnReset(wxGLContextAttrs& attrs)
{
    attrs.AddAttribute(EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY);
    attrs.AddAttribute(EGL_LOSE_CONTEXT_ON_RESET);
    return attrs;
}

wxGLContextAttrs& wxGLBackendEGL::ResetIsolation(wxGLContextAttrs& attrs)
{
    return attrs;
}

wxGLContextAttrs& wxGLBackendEGL::ReleaseFlush(wxGLContextAttrs& attrs, int)
{
    return attrs;
}

wxGLContextAttrs& wxGLBackendEGL::PlatformDefaults(wxGLContextAttrs& attrs)
{
    return attrs;
}

// ----------------------------------------------------------------------------
// wxGLAttributes: Visual/FBconfig attributes
// ----------------------------------------------------------------------------

wxGLAttributes& wxGLBackendEGL::RGBA(wxGLAttributes& attrs)
{
    return attrs;
}

wxGLAttributes& wxGLBackendEGL::BufferSize(wxGLAttributes& attrs, int val)
{
    if ( val >= 0 )
    {
        attrs.AddAttribute(EGL_BUFFER_SIZE);
        attrs.AddAttribute(val);
    }
    return attrs;
}

wxGLAttributes& wxGLBackendEGL::Level(wxGLAttributes& attrs, int val)
{
    attrs.AddAttribute(EGL_LEVEL);
    attrs.AddAttribute(val);
    return attrs;
}

wxGLAttributes& wxGLBackendEGL::DoubleBuffer(wxGLAttributes& attrs)
{
    return attrs;
}

wxGLAttributes& wxGLBackendEGL::Stereo(wxGLAttributes& attrs)
{
    return attrs;
}

wxGLAttributes& wxGLBackendEGL::AuxBuffers(wxGLAttributes& attrs, int)
{
    return attrs;
}

wxGLAttributes& wxGLBackendEGL::MinRGBA(wxGLAttributes& attrs, int mRed, int mGreen, int mBlue, int mAlpha)
{
    if ( mRed >= 0)
    {
        attrs.AddAttribute(EGL_RED_SIZE);
        attrs.AddAttribute(mRed);
    }
    if ( mGreen >= 0)
    {
        attrs.AddAttribute(EGL_GREEN_SIZE);
        attrs.AddAttribute(mGreen);
    }
    if ( mBlue >= 0)
    {
        attrs.AddAttribute(EGL_BLUE_SIZE);
        attrs.AddAttribute(mBlue);
    }
    if ( mAlpha >= 0)
    {
        attrs.AddAttribute(EGL_ALPHA_SIZE);
        attrs.AddAttribute(mAlpha);
    }
    return attrs;
}

wxGLAttributes& wxGLBackendEGL::Depth(wxGLAttributes& attrs, int val)
{
    if ( val >= 0 )
    {
        attrs.AddAttribute(EGL_DEPTH_SIZE);
        attrs.AddAttribute(val);
    }
    return attrs;
}

wxGLAttributes& wxGLBackendEGL::Stencil(wxGLAttributes& attrs, int val)
{
    if ( val >= 0 )
    {
        attrs.AddAttribute(EGL_STENCIL_SIZE);
        attrs.AddAttribute(val);
    }
    return attrs;
}

wxGLAttributes& wxGLBackendEGL::MinAcumRGBA(wxGLAttributes& attrs, int, int, int, int)
{
    return attrs;
}

wxGLAttributes& wxGLBackendEGL::SampleBuffers(wxGLAttributes& attrs, int val)
{
    if ( val >= 0 )
    {
        attrs.AddAttribute(EGL_SAMPLE_BUFFERS);
        attrs.AddAttribute(val);
    }
    return attrs;
}

wxGLAttributes& wxGLBackendEGL::Samplers(wxGLAttributes& attrs, int val)
{
    if ( val >= 0 )
    {
        attrs.AddAttribute(EGL_SAMPLES);
        attrs.AddAttribute(val);
    }
    return attrs;
}

wxGLAttributes& wxGLBackendEGL::FrameBuffersRGB(wxGLAttributes& attrs)
{
    return attrs;
}

wxGLAttributes& wxGLBackendEGL::PlatformDefaults(wxGLAttributes& attrs)
{
    // No EGL specific values
    return attrs;
}

void wxGLBackendEGL::EndList(wxGLAttribsBase& attrs)
{
    attrs.AddAttribute(EGL_NONE);
}


// ============================================================================
// wxGLContextEGL implementation
// ============================================================================

wxGLContextEGL::wxGLContextEGL(wxGLCanvas *win,
                               const wxGLContext *other,
                               const wxGLContextAttrs *ctxAttrs)
    : m_glContext(nullptr)
{
    // Fall back to OpenGL context parameters set at wxGLCanvas ctor if any.
    const wxGLContextAttrs& attrs = ctxAttrs ? *ctxAttrs
                                             : win->GetGLCTXAttrs();

    m_isOk = false;

    if ( !eglBindAPI(attrs.useES ? EGL_OPENGL_ES_API : EGL_OPENGL_API) )
    {
        // This is really not supposed to happen, so don't use wxLogError()
        // here, there is nothing the user can do about it if it does happen.
        wxFAIL_MSG("eglBindAPI() failed unexpectedly");
        return;
    }

    EGLConfig fbc = static_cast<wxGLCanvasEGL*>(win->GetImpl())->GetEGLConfig();
    wxCHECK_RET( fbc, "Invalid EGLConfig for OpenGL" );

    EGLContext otherCtx = EGL_NO_CONTEXT;
    if ( other )
        otherCtx = static_cast<wxGLContextEGL*>(other->GetImpl())->m_glContext;
    m_glContext = eglCreateContext(wxGLCanvasEGL::GetDisplay(), fbc,
                                   otherCtx,
                                   attrs.GetGLAttrs());

    if ( !m_glContext )
        wxLogMessage(_("Couldn't create OpenGL context"));
    else
        m_isOk = true;
}

wxGLContextEGL::~wxGLContextEGL()
{
    if ( !m_glContext )
        return;

    if ( m_glContext == eglGetCurrentContext() )
        wxGLBackendEGL_instance.ClearCurrentContext();

    eglDestroyContext(wxGLCanvasEGL::GetDisplay(), m_glContext);
}

bool wxGLContextEGL::SetCurrent(const wxGLCanvas& win) const
{
    if ( !m_glContext )
        return false;

    auto const winEGL = static_cast<wxGLCanvasEGL*>(win.GetImpl());

    return eglMakeCurrent(winEGL->GetEGLDisplay(), winEGL->GetEGLSurface(),
                          winEGL->GetEGLSurface(), m_glContext);
}

void wxGLBackendEGL::ClearCurrentContext()
{
    eglMakeCurrent(wxGLCanvasEGL::GetDisplay(), EGL_NO_SURFACE,
                   EGL_NO_SURFACE, EGL_NO_CONTEXT);
}

// ============================================================================
// wxGLCanvasEGL implementation
// ============================================================================

// ----------------------------------------------------------------------------
// initialization methods and dtor
// ----------------------------------------------------------------------------

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
    typedef EGLDisplay (*GetPlatformDisplayFunc)(EGLenum platform,
                                                 void* native_display,
                                                 const EGLAttrib* attrib_list);

    // Try loading the appropriate EGL function on first use.
    static GetPlatformDisplayFunc s_eglGetPlatformDisplay = nullptr;
    static bool s_eglGetPlatformDisplayInitialized = false;
    if ( !s_eglGetPlatformDisplayInitialized )
    {
        s_eglGetPlatformDisplayInitialized = true;

        if ( wxGLCanvasBase::IsExtensionInList(
                eglQueryString(nullptr, EGL_EXTENSIONS),
                "EGL_EXT_platform_base") )
        {
            s_eglGetPlatformDisplay = reinterpret_cast<GetPlatformDisplayFunc>(
                    eglGetProcAddress("eglGetPlatformDisplay"));
            if ( !s_eglGetPlatformDisplay )
            {
                // Try the fallback if not available.
                s_eglGetPlatformDisplay = reinterpret_cast<GetPlatformDisplayFunc>(
                    eglGetProcAddress("eglGetPlatformDisplayEXT"));
            }
        }
    }

    const wxDisplayInfo info = wxGetDisplayInfo();

    if ( !s_eglGetPlatformDisplay )
    {
        // Use the last fallback for backward compatibility.
        return eglGetDisplay(static_cast<EGLNativeDisplayType>(info.dpy));
    }

    EGLenum platform = 0;
    switch ( info.type )
    {
        case wxDisplayX11:
            platform = EGL_PLATFORM_X11_EXT;
            break;
        case wxDisplayWayland:
            platform = EGL_PLATFORM_WAYLAND_EXT;
            break;
        case wxDisplayNone:
            break;
    }

    wxCHECK_MSG( platform, EGL_NO_DISPLAY, "unknown display type" );

    return s_eglGetPlatformDisplay(platform, info.dpy, nullptr);
}

void wxGLCanvasEGL::OnWLFrameCallback()
{
#ifdef GDK_WINDOWING_WAYLAND
    wxLogTrace(TRACE_EGL, "In frame callback handler for %p", this);

    m_readyToDraw = true;
    g_clear_pointer(&m_wlFrameCallbackHandler, wl_callback_destroy);
    m_canvas->SendSizeEvent();
    gtk_widget_queue_draw(m_canvas->m_wxwindow);
#endif // GDK_WINDOWING_WAYLAND
}

#ifdef GDK_WINDOWING_WAYLAND

void wxGLCanvasEGL::UpdateSubsurfacePosition()
{
    if ( !m_wlSubsurface )
    {
        // In some circumstances such as when reparenting a canvas between two hidden
        // toplevel windows, GTK will call size-allocate before mapping the canvas
        // Ignore the call, the position will be fixed when it is mapped
        return;
    }

    int x, y;
    gdk_window_get_origin(m_canvas->GTKGetDrawingWindow(), &x, &y);
    wl_subsurface_set_position(m_wlSubsurface, x, y);
}

// Helper declared as friend in the header and so can access member variables.
//
// Used when size or scale factor changes
void wxEGLUpdateGeometry(GtkWidget* widget, wxGLCanvasEGL* win)
{
    int scale = gtk_widget_get_scale_factor(widget);
    wl_egl_window_resize(win->m_wlEGLWindow, win->m_canvas->m_width * scale,
                         win->m_canvas->m_height * scale, 0, 0);

    win->UpdateSubsurfacePosition();

    wl_surface_set_buffer_scale(win->m_wlSurface, scale);
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
    glc->OnWLFrameCallback();
}

static const struct wl_callback_listener wl_frame_listener = {
    wl_frame_callback_handler
};

static gboolean gtk_glcanvas_map_callback(GtkWidget *, GdkEventAny *, wxGLCanvasEGL *win)
{
    win->CreateWaylandSubsurface();
    return FALSE;
}

static void gtk_glcanvas_unmap_callback(GtkWidget *, wxGLCanvasEGL *win)
{
    win->DestroyWaylandSubsurface();
}

static void gtk_glcanvas_size_callback(GtkWidget *widget,
                                       GtkAllocation *,
                                       wxGLCanvasEGL *win)
{
    wxEGLUpdateGeometry(widget, win);
}

static void gtk_glcanvas_scale_factor_notify(GtkWidget* widget,
                                             GParamSpec*,
                                             wxGLCanvasEGL *win)
{
    wxEGLUpdateGeometry(widget, win);
}

} // extern "C"
#endif // GDK_WINDOWING_WAYLAND

EGLSurface wxGLCanvasEGL::CallCreatePlatformWindowSurface(void *window) const
{
    // Type of eglCreatePlatformWindowSurface[EXT]().
    typedef EGLSurface (*CreatePlatformWindowSurface)(EGLDisplay display,
                                                      EGLConfig config,
                                                      void* window,
                                                      EGLAttrib const* attrib_list);

    if ( gs_eglMajor > 1 || (gs_eglMajor == 1 && gs_eglMinor >= 5) )
    {
        // EGL 1.5 or later: use eglCreatePlatformWindowSurface() which must be
        // available.
        static CreatePlatformWindowSurface s_eglCreatePlatformWindowSurface = nullptr;
        if ( !s_eglCreatePlatformWindowSurface )
        {
            s_eglCreatePlatformWindowSurface = reinterpret_cast<CreatePlatformWindowSurface>(
                eglGetProcAddress("eglCreatePlatformWindowSurface"));
        }

        // This check is normally superfluous but avoid crashing just in case
        // it isn't.
        if ( s_eglCreatePlatformWindowSurface )
        {
            return s_eglCreatePlatformWindowSurface(m_display, m_config,
                                                    window,
                                                    nullptr);
        }
    }

    // Try loading the appropriate EGL function on first use.
    static CreatePlatformWindowSurface s_eglCreatePlatformWindowSurfaceEXT = nullptr;
    static bool s_extFuncInitialized = false;
    if ( !s_extFuncInitialized )
    {
        s_extFuncInitialized = true;

        if ( wxGLBackendEGL_instance.IsExtensionSupported("EGL_EXT_platform_base") )
        {
            s_eglCreatePlatformWindowSurfaceEXT = reinterpret_cast<CreatePlatformWindowSurface>(
                eglGetProcAddress("eglCreatePlatformWindowSurfaceEXT"));
        }
    }

    if ( s_eglCreatePlatformWindowSurfaceEXT )
    {
        return s_eglCreatePlatformWindowSurfaceEXT(m_display, m_config,
                                                   window,
                                                   nullptr);
    }
    else
    {
        return eglCreateWindowSurface(m_display, m_config,
                                      reinterpret_cast<EGLNativeWindowType>(window),
                                      nullptr);
    }
}

void wxGLCanvasEGL::OnRealized()
{
    m_display = GetDisplay();
    if ( m_display == EGL_NO_DISPLAY )
    {
        wxFAIL_MSG("Unable to get EGL Display");
        return;
    }

    GdkWindow *window = m_canvas->GTKGetDrawingWindow();
#ifdef GDK_WINDOWING_X11
    if (wxGTKImpl::IsX11(window))
    {
        if ( m_surface != EGL_NO_SURFACE )
        {
            eglDestroySurface(m_surface, m_display);
            m_surface = EGL_NO_SURFACE;
        }

        m_xwindow = GDK_WINDOW_XID(window);
        m_surface = CallCreatePlatformWindowSurface(&m_xwindow);
    }
#endif
#ifdef GDK_WINDOWING_WAYLAND
    if (wxGTKImpl::IsWayland(window))
    {
        if ( m_wlSurface )
        {
            // Already created (can happen when the canvas is un-realized then
            // re-realized, for example, when the canvas is re-parented)
            return;
        }

        int w = gdk_window_get_width(window);
        int h = gdk_window_get_height(window);
        struct wl_display *display = gdk_wayland_display_get_wl_display(gdk_window_get_display(window));
        struct wl_registry *registry = wl_display_get_registry(display);
        wl_registry_add_listener(registry, &wl_registry_listener, this);
        wl_display_roundtrip(display);
        if ( !m_wlCompositor || !m_wlSubcompositor )
        {
            wxFAIL_MSG("Invalid Wayland compositor or subcompositor");
            return;
        }
        m_wlSurface = wl_compositor_create_surface(m_wlCompositor);
        m_wlRegion = wl_compositor_create_region(m_wlCompositor);
        wl_surface_set_input_region(m_wlSurface, m_wlRegion);
        int scale = gdk_window_get_scale_factor(window);
        wl_surface_set_buffer_scale(m_wlSurface, scale);
        m_wlEGLWindow = wl_egl_window_create(m_wlSurface, w * scale,
                                             h * scale);
        m_surface = CallCreatePlatformWindowSurface(m_wlEGLWindow);

        GtkWidget* const widget = m_canvas->m_widget;

        // We need to use "map-event" instead of "map" to ensure that the
        // widget's underlying Wayland surface has been created.
        // Otherwise, gdk_wayland_window_get_wl_surface may return nullptr,
        // for example when hiding then showing a window containing a canvas.
        gtk_widget_add_events(widget, GDK_STRUCTURE_MASK);
        g_signal_connect(widget, "map-event",
                         G_CALLBACK(gtk_glcanvas_map_callback), this);
        // However, note the use of "unmap" instead of the later "unmap-event"
        // Not unmapping the canvas as soon as possible causes problems when reparenting
        g_signal_connect(widget, "unmap",
                         G_CALLBACK(gtk_glcanvas_unmap_callback), this);

        // We connect to "size-allocate" to update the position of the
        // subsurface when the toplevel window is moved, which also updates the
        // scale as a side effect, but we need to also separately connect to
        // "notify::scale-factor" to catch scale changes, which is especially
        // important initially, as we don't get a "size-allocate" with the
        // correct scale when the window is created.
        g_signal_connect(widget, "size-allocate",
                         G_CALLBACK(gtk_glcanvas_size_callback), this);
        g_signal_connect(widget, "notify::scale-factor",
                         G_CALLBACK (gtk_glcanvas_scale_factor_notify), this);
    }
#endif

    if ( m_surface == EGL_NO_SURFACE )
    {
        wxFAIL_MSG("Unable to create EGL surface");
        return;
    }
}

wxGLCanvasEGL::~wxGLCanvasEGL()
{
    if ( m_surface )
        eglDestroySurface(m_display, m_surface);
#ifdef GDK_WINDOWING_WAYLAND
    DestroyWaylandSubsurface();
    g_clear_pointer(&m_wlEGLWindow, wl_egl_window_destroy);
    g_clear_pointer(&m_wlSurface, wl_surface_destroy);
#endif
}

void wxGLCanvasEGL::CreateWaylandSubsurface()
{
#ifdef GDK_WINDOWING_WAYLAND
    // It's possible that we get in here unnecessarily in two ways:
    // (1) If the canvas widget is shown, and then immediately hidden, we will
    //     still receive a map-event signal, but by that point, the subsurface
    //     does not need to be created anymore as the canvas is hidden
    // (2) If the canvas widget is shown, and then immediately hidden, and then
    //     immediately shown again, we will receive two map-event signals.
    //     By the second time we get it, the subsurface will already be created
    // Not ignoring either of the two scenarios will likely cause the subsurface
    // to be created twice, leading to a crash due to a Wayland protocol error
    // See https://github.com/wxWidgets/wxWidgets/issues/23961
    if ( !gtk_widget_get_mapped(m_canvas->m_widget) || m_wlSubsurface )
    {
        return;
    }

    GdkWindow *window = m_canvas->GTKGetDrawingWindow();
    struct wl_surface *surface = gdk_wayland_window_get_wl_surface(window);

    m_wlSubsurface = wl_subcompositor_get_subsurface(m_wlSubcompositor,
                                                     m_wlSurface,
                                                     surface);
    wxCHECK_RET( m_wlSubsurface, "Unable to get EGL subsurface" );

    wl_subsurface_set_desync(m_wlSubsurface);
    UpdateSubsurfacePosition();
    m_wlFrameCallbackHandler = wl_surface_frame(surface);
    wl_callback_add_listener(m_wlFrameCallbackHandler,
                             &wl_frame_listener, this);
#endif
}

void wxGLCanvasEGL::DestroyWaylandSubsurface()
{
#ifdef GDK_WINDOWING_WAYLAND
    g_clear_pointer(&m_wlSubsurface, wl_subsurface_destroy);
    g_clear_pointer(&m_wlFrameCallbackHandler, wl_callback_destroy);
    m_readyToDraw = false;
#endif
}

// ----------------------------------------------------------------------------
// working with GL attributes
// ----------------------------------------------------------------------------

bool wxGLBackendEGL::IsExtensionSupported(const char *extension)
{
    EGLDisplay dpy = wxGLCanvasEGL::GetDisplay();

    return wxGLCanvasBase::IsExtensionInList(eglQueryString(dpy, EGL_EXTENSIONS),
                                             extension);
}


/* static */
EGLConfig wxGLCanvasEGL::InitConfig(const wxGLAttributes& dispAttrs)
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

    if ( !eglInitialize(dpy, &gs_eglMajor, &gs_eglMinor) )
    {
        wxFAIL_MSG("eglInitialize failed");
        return nullptr;
    }

    // The runtime EGL version cannot be known until EGL has been initialized.
    if ( gs_eglMajor < 1 || (gs_eglMajor == 1 && gs_eglMinor < 4) )
    {
        // Ignore the return value here, we cannot recover at this point.
        eglTerminate(dpy);
        wxLogError(
            _("EGL version is %d.%d but version 1.4 or greater is required."),
            gs_eglMajor, gs_eglMinor);
        return nullptr;
    }

    EGLint numConfigs = 0;

    // Check if we need to filter out the configs using alpha, as getting one
    // is unexpected if it hasn't been explicitly requested by using MinRGBA().
    for ( int i = 0; attrsList[i] != EGL_NONE; i += 2 )
    {
        if ( attrsList[i] == EGL_ALPHA_SIZE )
        {
            if ( attrsList[i + 1] > 0 )
            {
                // We can just get the first config proposed by the driver in
                // this case.
                EGLConfig config;

                if ( !eglChooseConfig(dpy, attrsList, &config, 1, &numConfigs)
                        || numConfigs != 1 )
                {
                    // This is not necessarily an error, there may just be no
                    // matches.
                    return nullptr;
                }

                return config;
            }
        }
    }

    // We get here only if alpha was not requested or is zero and we want to
    // ensure that we really return a config not using alpha in this case, so
    // get all of them and try to find the first one without alpha.
    if ( !eglChooseConfig(dpy, attrsList, nullptr, 0, &numConfigs) || !numConfigs )
        return nullptr;

    wxLogTrace(TRACE_EGL, "Enumerated %d matching EGL configs", numConfigs);

    std::vector<EGLConfig> configs(numConfigs);
    if ( !eglChooseConfig(dpy, attrsList, &configs[0], configs.size(), &numConfigs) )
    {
        wxLogTrace(TRACE_EGL, "Failed to get all EGL configs");
        return nullptr;
    }

    for ( auto& config : configs )
    {
        EGLint alpha = 0;
        if ( !eglGetConfigAttrib(dpy, config, EGL_ALPHA_SIZE, &alpha) )
        {
            wxLogTrace(TRACE_EGL, "Failed to get EGL_ALPHA_SIZE for config");
            continue;
        }

        if ( alpha == 0 )
        {
            // We can use this one.
            return config;
        }
    }

    // Choose the first config, it's better to return something using alpha
    // than nothing at all.
    return configs.front();
}

bool wxGLBackendEGL::IsDisplaySupported(const wxGLAttributes& dispAttrs)
{
    return wxGLCanvasEGL::InitConfig(dispAttrs) != nullptr;
}

// ----------------------------------------------------------------------------
// default visual management
// ----------------------------------------------------------------------------

static EGLConfig wxDefaultEGLConfig = nullptr;

bool wxGLBackendEGL::InitDefaultVisualInfo(const int *attribList)
{
    wxGLAttributes dispAttrs;
    wxGLCanvasBase::ParseAttribList(attribList, dispAttrs);

    wxDefaultEGLConfig = wxGLCanvasEGL::InitConfig(dispAttrs);
    return wxDefaultEGLConfig != nullptr;
}

void wxGLBackendEGL::FreeDefaultVisualInfo()
{
    wxDefaultEGLConfig = nullptr;
}

void* wxGLBackendEGL::GetDefaultVisualInfo()
{
    return wxDefaultEGLConfig;
}


// ----------------------------------------------------------------------------
// other GL methods
// ----------------------------------------------------------------------------

bool wxGLCanvasEGL::SwapBuffers()
{
    // Before doing anything else, ensure that eglSwapBuffers() doesn't block:
    // under Wayland we don't want it to because we use the surface callback to
    // know when we should draw anyhow and with X11 it blocks for up to a
    // second when the window is entirely occluded and because we can't detect
    // this currently (our IsShownOnScreen() doesn't account for all cases in
    // which this happens) we must prevent it from blocking to avoid making the
    // entire application completely unusable just because one of its windows
    // using wxGLCanvas got occluded or unmapped (e.g. due to a move to another
    // workspace).
    if ( m_swapIntervalToSet != wxGLCanvas::DefaultSwapInterval )
    {
        DoSetSwapInterval(m_swapIntervalToSet);

        // Don't set it again in any case, even if it failed, as it would just
        // fail again the next time anyhow.
        m_swapIntervalToSet = wxGLCanvas::DefaultSwapInterval;
    }

    GdkWindow* const window = m_canvas->GTKGetDrawingWindow();
#ifdef GDK_WINDOWING_X11
    if (wxGTKImpl::IsX11(window))
    {
        // TODO: We need to check if it's really shown on screen, i.e. if it's
        // not completely occluded even if it hadn't been explicitly hidden.
        if ( !m_canvas->IsShownOnScreen() )
        {
            // Trying to draw on a hidden window is useless.
            wxLogTrace(TRACE_EGL, "Window %p is hidden, not drawing", this);
            return false;
        }
    }
#endif // GDK_WINDOWING_X11
#ifdef GDK_WINDOWING_WAYLAND
    if (wxGTKImpl::IsWayland(window))
    {
        // Under Wayland, we must not draw before the window has been realized,
        // as this could result in a deadlock inside eglSwapBuffers()
        if ( !m_readyToDraw )
        {
            wxLogTrace(TRACE_EGL, "Window %p is not not ready to draw yet", this);
            return false;
        }
    }
#endif // GDK_WINDOWING_WAYLAND

    wxLogTrace(TRACE_EGL, "Swapping buffers for window %p", this);

    return eglSwapBuffers(m_display, m_surface);
}

bool wxGLCanvasEGL::DoSetSwapInterval(int interval)
{
    if ( !eglSwapInterval(m_display, interval) )
    {
        wxLogTrace(TRACE_EGL, "eglSwapInterval(%d) failed for %p: %#x",
                   interval, this, eglGetError());
        return false;
    }

    wxLogTrace(TRACE_EGL, "Set EGL swap interval to 0 for %p", this);

    return true;
}

int wxGLCanvasEGL::GetSwapInterval() const
{
    // There doesn't seem to be a way to query the current swap interval in
    // EGL, there are  EGL_MIN_SWAP_INTERVAL and EGL_MAX_SWAP_INTERVAL in
    // EGLConfig, but not the current value.
    return wxGLCanvas::DefaultSwapInterval;
}

bool wxGLCanvasEGL::HasWindow() const
{
    wxDisplayInfo info = wxGetDisplayInfo();
    switch ( info.type )
    {
        case wxDisplayX11:
            return m_canvas->GetXWindow();
        case wxDisplayWayland:
            return m_wlSubsurface;
        case wxDisplayNone:
            break;
    }

    return false;
}

// ----------------------------------------------------------------------------
// wxGLBackendEGL
// ----------------------------------------------------------------------------

/* static */
wxGLBackend& wxGLBackendEGL::Get()
{
    return wxGLBackendEGL_instance;
}

std::unique_ptr<wxGLContextImpl>
wxGLBackendEGL::CreateContextImpl(wxGLCanvas* win,
                                  const wxGLContext* other,
                                  const wxGLContextAttrs* ctxAttrs)
{
    return std::make_unique<wxGLContextEGL>(win, other, ctxAttrs);
}

std::unique_ptr<wxGLCanvasUnixImpl>
wxGLBackendEGL::CreateCanvasImpl(wxGLCanvasUnix* canvas)
{
    return std::make_unique<wxGLCanvasEGL>(canvas);
}

bool wxGLBackendEGL::GetEGLVersion(int* major, int* minor)
{
    if ( major )
        *major = gs_eglMajor;
    if ( minor )
        *minor = gs_eglMinor;

    return true;
}

int wxGLBackendEGL::GetGLXVersion()
{
    // EGL doesn't have GLX version.
    return 0;
}

bool wxGLBackendEGL::IsGLXMultiSampleAvailable()
{
    // EGL doesn't have GLX multisample extension.
    return false;
}

#endif // wxUSE_GLCANVAS && defined(wxHAS_EGL)
