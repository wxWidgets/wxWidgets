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

#include "wx/private/elfversion.h"

#include "wx/gtk/private/wrapgdk.h"
#include "wx/gtk/private/backend.h"
#ifdef GDK_WINDOWING_WAYLAND
#include <wayland-egl.h>
#endif

#include <EGL/egl.h>
#include <EGL/eglext.h>

static const char* TRACE_EGL = "glegl";

// We can't add a member variable to wxGLCanvasEGL in 3.2 branch, so emulate it
// by encoding the corresponding boolean value via the presence of "this"
// pointer in the given hash set.
#include "wx/hashset.h"

namespace
{

// Define the equivalent of unordered_set<wxGLCanvasEGL*>.
WX_DECLARE_HASH_SET(wxGLCanvasEGL*, wxPointerHash, wxPointerEqual, wxGLCanvasSet);

// And use it to remember which objects already called eglSwapInterval().
wxGLCanvasSet gs_alreadySetSwapInterval;

} // anonymous namespace

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

void wxGLAttributes::AddDefaultsForWXBefore31()
{
    // ParseAttribList() will add EndList(), don't do it now
    DoubleBuffer();
}


// ============================================================================
// wxGLContext implementation
// ============================================================================

wxIMPLEMENT_CLASS(wxGLContext, wxObject);

wxGLContext::wxGLContext(wxGLCanvas *win,
                         const wxGLContext *other,
                         const wxGLContextAttrs *ctxAttrs)
    : m_glContext(NULL)
{
    const int* contextAttribs = NULL;

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
    m_config = NULL;
    m_display = NULL;
    m_surface = EGL_NO_SURFACE;
    m_wlCompositor = NULL;
    m_wlSubcompositor = NULL;
    m_wlFrameCallbackHandler = NULL;
    m_wlEGLWindow = NULL;
    m_wlSurface = NULL;
    m_wlRegion = NULL;
    m_wlSubsurface = NULL;
    m_readyToDraw = false;
}

bool wxGLCanvasEGL::InitVisual(const wxGLAttributes& dispAttrs)
{
    m_config = InitConfig(dispAttrs);
    if ( !m_config )
    {
        wxFAIL_MSG("Failed to get an EGLConfig for the requested attributes.");
    }
    return m_config != NULL;
}

/* static */
EGLDisplay wxGLCanvasEGL::GetDisplay()
{
    typedef EGLDisplay (*GetPlatformDisplayFunc)(EGLenum platform,
                                                 void* native_display,
                                                 const EGLAttrib* attrib_list);

    // Try loading the appropriate EGL function on first use.
    static GetPlatformDisplayFunc s_eglGetPlatformDisplay = NULL;
    static bool s_eglGetPlatformDisplayInitialized = false;
    if ( !s_eglGetPlatformDisplayInitialized )
    {
        s_eglGetPlatformDisplayInitialized = true;

        if ( wxGLCanvasBase::IsExtensionInList(
                eglQueryString(NULL, EGL_EXTENSIONS),
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

    return s_eglGetPlatformDisplay(platform, info.dpy, NULL);
}

void wxGLCanvasEGL::OnWLFrameCallback()
{
#ifdef GDK_WINDOWING_WAYLAND
    wxLogTrace(TRACE_EGL, "In frame callback handler for %p", this);

    m_readyToDraw = true;
    g_clear_pointer(&m_wlFrameCallbackHandler, wl_callback_destroy);
    SendSizeEvent();
    gtk_widget_queue_draw(m_wxwindow);
#endif // GDK_WINDOWING_WAYLAND
}

#ifdef GDK_WINDOWING_WAYLAND

// Helper declared as friend in the header and so can access m_wlSubsurface.
void wxEGLUpdatePosition(wxGLCanvasEGL* win)
{
    if ( !win->m_wlSubsurface )
    {
        // In some circumstances such as when reparenting a canvas between two hidden
        // toplevel windows, GTK will call size-allocate before mapping the canvas
        // Ignore the call, the position will be fixed when it is mapped
        return;
    }

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
        if ( m_surface != EGL_NO_SURFACE )
        {
            eglDestroySurface(m_surface, m_display);
            m_surface = EGL_NO_SURFACE;
        }

        m_xwindow = GDK_WINDOW_XID(window);
        m_surface = eglCreatePlatformWindowSurface(m_display, *m_config,
                                                   &m_xwindow, NULL);
    }
#endif
#ifdef GDK_WINDOWING_WAYLAND
    if (wxGTKImpl::IsWayland(window))
    {
        if ( m_wlSurface )
        {
            // Already created (can happen when the canvas is un-realized then
            // re-realized, for example, when the canvas is re-parented)
            return true;
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
            return false;
        }
        m_wlSurface = wl_compositor_create_surface(m_wlCompositor);
        m_wlRegion = wl_compositor_create_region(m_wlCompositor);
        wl_surface_set_input_region(m_wlSurface, m_wlRegion);
        int scale = gdk_window_get_scale_factor(window);
        wl_surface_set_buffer_scale(m_wlSurface, scale);
        m_wlEGLWindow = wl_egl_window_create(m_wlSurface, w * scale,
                                             h * scale);
        m_surface = eglCreatePlatformWindowSurface(m_display, *m_config,
                                                   m_wlEGLWindow, NULL);

        // We need to use "map-event" instead of "map" to ensure that the
        // widget's underlying Wayland surface has been created.
        // Otherwise, gdk_wayland_window_get_wl_surface may return NULL,
        // for example when hiding then showing a window containing a canvas.
        gtk_widget_add_events(m_widget, GDK_STRUCTURE_MASK);
        g_signal_connect(m_widget, "map-event",
                         G_CALLBACK(gtk_glcanvas_map_callback), this);
        // However, note the use of "unmap" instead of the later "unmap-event"
        // Not unmapping the canvas as soon as possible causes problems when reparenting
        g_signal_connect(m_widget, "unmap",
                         G_CALLBACK(gtk_glcanvas_unmap_callback), this);
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
    DestroyWaylandSubsurface();
    g_clear_pointer(&m_wlEGLWindow, wl_egl_window_destroy);
    g_clear_pointer(&m_wlSurface, wl_surface_destroy);
#endif

    gs_alreadySetSwapInterval.erase(this);
}

wxELF_VERSION_COMPAT("_ZN13wxGLCanvasEGL23CreateWaylandSubsurfaceEv", "3.2.3")
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
    if ( !gtk_widget_get_mapped(m_widget) || m_wlSubsurface )
    {
        return;
    }

    GdkWindow *window = GTKGetDrawingWindow();
    struct wl_surface *surface = gdk_wayland_window_get_wl_surface(window);

    m_wlSubsurface = wl_subcompositor_get_subsurface(m_wlSubcompositor,
                                                     m_wlSurface,
                                                     surface);
    wl_subsurface_set_desync(m_wlSubsurface);
    wxEGLUpdatePosition(this);
    m_wlFrameCallbackHandler = wl_surface_frame(surface);
    wl_callback_add_listener(m_wlFrameCallbackHandler,
                             &wl_frame_listener, this);

    if ( m_surface == EGL_NO_SURFACE )
    {
        wxFAIL_MSG("Unable to create EGL surface");
        return;
    }
#endif
}

wxELF_VERSION_COMPAT("_ZN13wxGLCanvasEGL24DestroyWaylandSubsurfaceEv", "3.2.3")
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

/* static */
bool wxGLCanvasBase::IsExtensionSupported(const char *extension)
{
    EGLDisplay dpy = wxGLCanvasEGL::GetDisplay();

    return IsExtensionInList(eglQueryString(dpy, EGL_EXTENSIONS), extension);
}


/* static */
EGLConfig *wxGLCanvasEGL::InitConfig(const wxGLAttributes& dispAttrs)
{
    const int* attrsList = dispAttrs.GetGLAttrs();
    if ( !attrsList )
    {
        wxFAIL_MSG("wxGLAttributes object is empty.");
        return NULL;
    }

    EGLDisplay dpy = GetDisplay();
    if ( dpy == EGL_NO_DISPLAY ) {
        wxFAIL_MSG("Unable to get EGL Display");
        return NULL;
    }

    EGLint eglMajor = 0;
    EGLint eglMinor = 0;
    if ( !eglInitialize(dpy, &eglMajor, &eglMinor) )
    {
        wxFAIL_MSG("eglInitialize failed");
        return NULL;
    }

    // The runtime EGL version cannot be known until EGL has been initialized.
    if ( eglMajor < 1 || (eglMajor == 1 && eglMinor < 5) )
    {
        // Ignore the return value here, we cannot recover at this point.
        eglTerminate(dpy);
        wxLogError(wxString::Format(
            "EGL version is %d.%d. EGL version 1.5 or greater is required.",
            eglMajor, eglMinor));
        return NULL;
    }

    if ( !eglBindAPI(EGL_OPENGL_API) ) {
        wxFAIL_MSG("eglBindAPI failed");
        return NULL;
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
                wxScopedPtr<EGLConfig> config(new EGLConfig);

                if ( !eglChooseConfig(dpy, attrsList, config.get(), 1, &numConfigs)
                        || numConfigs != 1 )
                {
                    // This is not necessarily an error, there may just be no
                    // matches.
                    return NULL;
                }

                return config.release();
            }
        }
    }

    // We get here only if alpha was not requested or is zero and we want to
    // ensure that we really return a config not using alpha in this case, so
    // get all of them and try to find the first one without alpha.
    if ( !eglChooseConfig(dpy, attrsList, NULL, 0, &numConfigs) || !numConfigs )
        return NULL;

    wxLogTrace(TRACE_EGL, "Enumerated %d matching EGL configs", numConfigs);

    wxVector<EGLConfig> configs(numConfigs);
    if ( !eglChooseConfig(dpy, attrsList, &configs[0], configs.size(), &numConfigs) )
    {
        wxLogTrace(TRACE_EGL, "Failed to get all EGL configs");
        return NULL;
    }

    for ( wxVector<EGLConfig>::iterator it = configs.begin(); it != configs.end(); ++it )
    {
        EGLint alpha = 0;
        if ( !eglGetConfigAttrib(dpy, *it, EGL_ALPHA_SIZE, &alpha) )
        {
            wxLogTrace(TRACE_EGL, "Failed to get EGL_ALPHA_SIZE for config");
            continue;
        }

        if ( alpha == 0 )
        {
            // We can use this one.
            return new EGLConfig(*it);
        }
    }

    // Choose the first config, it's better to return something using alpha
    // than nothing at all.
    return new EGLConfig(configs.front());
}

/* static */
bool wxGLCanvasBase::IsDisplaySupported(const wxGLAttributes& dispAttrs)
{
    wxScopedPtr<EGLConfig> config(wxGLCanvasEGL::InitConfig(dispAttrs));
    return config != NULL;
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

EGLConfig *wxGLCanvasEGL::ms_glEGLConfig = NULL;

/* static */
bool wxGLCanvasEGL::InitDefaultConfig(const int *attribList)
{
    FreeDefaultConfig();
    wxGLAttributes dispAttrs;
    ParseAttribList(attribList, dispAttrs);

    ms_glEGLConfig = InitConfig(dispAttrs);
    return ms_glEGLConfig != NULL;
}

/* static */
void wxGLCanvasEGL::FreeDefaultConfig()
{
    if ( ms_glEGLConfig )
    {
        delete ms_glEGLConfig;
        ms_glEGLConfig = NULL;
    }
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
    if ( !gs_alreadySetSwapInterval.count(this) )
    {
        // Ensure that eglSwapBuffers() doesn't block, as we use the surface
        // callback to know when we should draw ourselves already.
        if ( eglSwapInterval(m_display, 0) )
        {
            wxLogTrace(TRACE_EGL, "Set EGL swap interval to 0 for %p", this);

            // It shouldn't be necessary to set it again.
            gs_alreadySetSwapInterval.insert(this);
        }
        else
        {
            wxLogTrace(TRACE_EGL, "eglSwapInterval(0) failed for %p: %#x",
                       this, eglGetError());
        }
    }

    GdkWindow* const window = GTKGetDrawingWindow();
#ifdef GDK_WINDOWING_X11
    if (wxGTKImpl::IsX11(window))
    {
        if ( !IsShownOnScreen() )
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

bool wxGLCanvasEGL::IsShownOnScreen() const
{
    wxDisplayInfo info = wxGetDisplayInfo();
    switch ( info.type )
    {
        case wxDisplayX11:
            return GetXWindow() && wxGLCanvasBase::IsShownOnScreen();
        case wxDisplayWayland:
            return m_wlSubsurface && wxGLCanvasBase::IsShownOnScreen();
        case wxDisplayNone:
            break;
    }

    return false;
}

#endif // wxUSE_GLCANVAS && wxUSE_GLCANVAS_EGL

