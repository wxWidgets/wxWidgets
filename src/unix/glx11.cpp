///////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/glx11.cpp
// Purpose:     code common to all X11-based wxGLCanvas implementations
// Author:      Vadim Zeitlin
// Created:     2007-04-15
// Copyright:   (c) 2007 Vadim Zeitlin <vadim@wxwidgets.org>
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

#if wxUSE_GLCANVAS && defined(wxHAS_GLX)

#ifndef WX_PRECOMP
    #include "wx/log.h"
#endif //WX_PRECOMP

#include "wx/glcanvas.h"

#include "wx/unix/private/glx11.h"
#include "wx/unix/private/x11ptr.h"

#include "wx/private/make_unique.h"

// IRIX headers call this differently
#ifdef __SGI__
    #ifndef GLX_SAMPLE_BUFFERS_ARB
        #define GLX_SAMPLE_BUFFERS_ARB GLX_SAMPLE_BUFFERS_SGIS
    #endif
    #ifndef GLX_SAMPLES_ARB
        #define GLX_SAMPLES_ARB GLX_SAMPLES_SGIS
    #endif
#endif // __SGI__

// ----------------------------------------------------------------------------
// define possibly missing XGL constants and types
// ----------------------------------------------------------------------------

#ifndef GLX_NONE_EXT
#define GLX_NONE_EXT                       0x8000
#endif

#ifndef GLX_ARB_multisample
#define GLX_ARB_multisample
#define GLX_SAMPLE_BUFFERS_ARB             100000
#define GLX_SAMPLES_ARB                    100001
#endif

#ifndef GLX_EXT_visual_rating
#define GLX_EXT_visual_rating
#define GLX_VISUAL_CAVEAT_EXT              0x20
#define GLX_NONE_EXT                       0x8000
#define GLX_SLOW_VISUAL_EXT                0x8001
#define GLX_NON_CONFORMANT_VISUAL_EXT      0x800D
#endif

#ifndef GLX_EXT_visual_info
#define GLX_EXT_visual_info
#define GLX_X_VISUAL_TYPE_EXT              0x22
#define GLX_DIRECT_COLOR_EXT               0x8003
#endif

#ifndef GLX_ARB_framebuffer_sRGB
#define GLX_ARB_framebuffer_sRGB
#define GLX_FRAMEBUFFER_SRGB_CAPABLE_ARB   0x20B2
#endif

#ifndef GLX_ARB_create_context
#define GLX_ARB_create_context
#define GLX_CONTEXT_MAJOR_VERSION_ARB      0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB      0x2092
#define GLX_CONTEXT_FLAGS_ARB              0x2094
#define GLX_CONTEXT_DEBUG_BIT_ARB          0x0001
#define GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x0002

/* Typedef for the GL 3.0 context creation function */
typedef GLXContext(*PFNGLXCREATECONTEXTATTRIBSARBPROC)
    (Display * dpy, GLXFBConfig config, GLXContext share_context,
    Bool direct, const int *attrib_list);
#endif

#ifndef GLX_ARB_create_context_profile
#define GLX_ARB_create_context_profile
#define GLX_CONTEXT_PROFILE_MASK_ARB       0x9126
#define GLX_CONTEXT_CORE_PROFILE_BIT_ARB   0x00000001
#define GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#endif

#ifndef GLX_ARB_create_context_robustness
#define GLX_ARB_create_context_robustness
#define GLX_CONTEXT_ROBUST_ACCESS_BIT_ARB  0x00000004
#define GLX_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB     0x8256
#define GLX_NO_RESET_NOTIFICATION_ARB                   0x8261
#define GLX_LOSE_CONTEXT_ON_RESET_ARB                   0x8252
#endif

#ifndef GLX_ARB_robustness_application_isolation
#define GLX_ARB_robustness_application_isolation
#define GLX_CONTEXT_RESET_ISOLATION_BIT_ARB             0x00000008
#endif
#ifndef GLX_ARB_robustness_share_group_isolation
#define GLX_ARB_robustness_share_group_isolation
#endif

#ifndef GLX_EXT_swap_control
#define GLX_SWAP_INTERVAL_EXT 0x20F1
#endif

#ifndef GLX_ARB_context_flush_control
#define GLX_ARB_context_flush_control
#define GLX_CONTEXT_RELEASE_BEHAVIOR_ARB            0x2097
#define GLX_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB       0
#define GLX_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB      0x2098
#endif

#ifndef GLX_EXT_create_context_es2_profile
#define GLX_EXT_create_context_es2_profile
#define GLX_CONTEXT_ES2_PROFILE_BIT_EXT    0x00000004
#endif

#ifndef GLX_EXT_create_context_es_profile
#define GLX_EXT_create_context_es_profile
#define GLX_CONTEXT_ES2_PROFILE_BIT_EXT    0x00000004
#endif

namespace
{

constexpr const char* TRACE_GLX = "glx";

// It's ok to have this global variable because its ctor and dtor are trivial.
wxGLBackendX11 wxGLBackendX11_instance;

} // anonymous namespace

// ----------------------------------------------------------------------------
// wxGLContextAttrs: OpenGL rendering context attributes
// ----------------------------------------------------------------------------
// GLX specific values

wxGLContextAttrs& wxGLBackendX11::CoreProfile(wxGLContextAttrs& attrs)
{
    attrs.AddAttribBits(GLX_CONTEXT_PROFILE_MASK_ARB,
                  GLX_CONTEXT_CORE_PROFILE_BIT_ARB);
    attrs.SetNeedsARB();
    return attrs;
}

wxGLContextAttrs& wxGLBackendX11::MajorVersion(wxGLContextAttrs& attrs, int val)
{
    if ( val > 0 )
    {
        attrs.AddAttribute(GLX_CONTEXT_MAJOR_VERSION_ARB);
        attrs.AddAttribute(val);
        if ( val >= 3 )
            attrs.SetNeedsARB();
    }
    return attrs;
}

wxGLContextAttrs& wxGLBackendX11::MinorVersion(wxGLContextAttrs& attrs, int val)
{
    if ( val >= 0 )
    {
        attrs.AddAttribute(GLX_CONTEXT_MINOR_VERSION_ARB);
        attrs.AddAttribute(val);
    }
    return attrs;
}

wxGLContextAttrs& wxGLBackendX11::CompatibilityProfile(wxGLContextAttrs& attrs)
{
    attrs.AddAttribBits(GLX_CONTEXT_PROFILE_MASK_ARB,
                  GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB);
    attrs.SetNeedsARB();
    return attrs;
}

wxGLContextAttrs& wxGLBackendX11::ForwardCompatible(wxGLContextAttrs& attrs)
{
    attrs.AddAttribBits(GLX_CONTEXT_FLAGS_ARB,
                  GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB);
    attrs.SetNeedsARB();
    return attrs;
}

wxGLContextAttrs& wxGLBackendX11::ES2(wxGLContextAttrs& attrs)
{
    attrs.AddAttribBits(GLX_CONTEXT_PROFILE_MASK_ARB,
                  GLX_CONTEXT_ES2_PROFILE_BIT_EXT);
    attrs.SetNeedsARB();
    return attrs;
}

wxGLContextAttrs& wxGLBackendX11::DebugCtx(wxGLContextAttrs& attrs)
{
    attrs.AddAttribBits(GLX_CONTEXT_FLAGS_ARB,
                  GLX_CONTEXT_DEBUG_BIT_ARB);
    attrs.SetNeedsARB();
    return attrs;
}

wxGLContextAttrs& wxGLBackendX11::Robust(wxGLContextAttrs& attrs)
{
    attrs.AddAttribBits(GLX_CONTEXT_FLAGS_ARB,
                  GLX_CONTEXT_ROBUST_ACCESS_BIT_ARB);
    attrs.SetNeedsARB();
    return attrs;
}

wxGLContextAttrs& wxGLBackendX11::NoResetNotify(wxGLContextAttrs& attrs)
{
    attrs.AddAttribute(GLX_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB);
    attrs.AddAttribute(GLX_NO_RESET_NOTIFICATION_ARB);
    attrs.SetNeedsARB();
    return attrs;
}

wxGLContextAttrs& wxGLBackendX11::LoseOnReset(wxGLContextAttrs& attrs)
{
    attrs.AddAttribute(GLX_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB);
    attrs.AddAttribute(GLX_LOSE_CONTEXT_ON_RESET_ARB);
    attrs.SetNeedsARB();
    return attrs;
}

wxGLContextAttrs& wxGLBackendX11::ResetIsolation(wxGLContextAttrs& attrs)
{
    attrs.AddAttribBits(GLX_CONTEXT_FLAGS_ARB,
                  GLX_CONTEXT_RESET_ISOLATION_BIT_ARB);
    attrs.SetNeedsARB();
    return attrs;
}

wxGLContextAttrs& wxGLBackendX11::ReleaseFlush(wxGLContextAttrs& attrs, int val)
{
    attrs.AddAttribute(GLX_CONTEXT_RELEASE_BEHAVIOR_ARB);
    if ( val == 1 )
        attrs.AddAttribute(GLX_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB);
    else
        attrs.AddAttribute(GLX_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB);
    attrs.SetNeedsARB();
    return attrs;
}

wxGLContextAttrs& wxGLBackendX11::PlatformDefaults(wxGLContextAttrs& attrs)
{
    attrs.renderTypeRGBA = true;
    attrs.x11Direct = true;
    return attrs;
}

// ----------------------------------------------------------------------------
// wxGLAttributes: Visual/FBconfig attributes
// ----------------------------------------------------------------------------
// GLX specific values

//   Different versions of GLX API use rather different attributes lists, see
//   the following URLs:
//
//   - <= 1.2: http://www.opengl.org/sdk/docs/man/xhtml/glXChooseVisual.xml
//   - >= 1.3: http://www.opengl.org/sdk/docs/man/xhtml/glXChooseFBConfig.xml
//
//   Notice in particular that
//   - GLX_RGBA is boolean attribute in the old version of the API but a
//     value of GLX_RENDER_TYPE in the new one
//   - Boolean attributes such as GLX_DOUBLEBUFFER don't take values in the
//     old version but must be followed by True or False in the new one.

wxGLAttributes& wxGLBackendX11::RGBA(wxGLAttributes& attrs)
{
    if ( GetGLXVersion() >= 13 )
        attrs.AddAttribBits(GLX_RENDER_TYPE, GLX_RGBA_BIT);
    else
        attrs.AddAttribute(GLX_RGBA);
    return attrs;
}

wxGLAttributes& wxGLBackendX11::BufferSize(wxGLAttributes& attrs, int val)
{
    if ( val >= 0 )
    {
        attrs.AddAttribute(GLX_BUFFER_SIZE);
        attrs.AddAttribute(val);
    }
    return attrs;
}

wxGLAttributes& wxGLBackendX11::Level(wxGLAttributes& attrs, int val)
{
    attrs.AddAttribute(GLX_LEVEL);
    attrs.AddAttribute(val);
    return attrs;
}

wxGLAttributes& wxGLBackendX11::DoubleBuffer(wxGLAttributes& attrs)
{
    attrs.AddAttribute(GLX_DOUBLEBUFFER);
    if ( GetGLXVersion() >= 13 )
        attrs.AddAttribute(True);
    return attrs;
}

wxGLAttributes& wxGLBackendX11::Stereo(wxGLAttributes& attrs)
{
    attrs.AddAttribute(GLX_STEREO);
    if ( GetGLXVersion() >= 13 )
        attrs.AddAttribute(True);
    return attrs;
}

wxGLAttributes& wxGLBackendX11::AuxBuffers(wxGLAttributes& attrs, int val)
{
    if ( val >= 0 )
    {
        attrs.AddAttribute(GLX_AUX_BUFFERS);
        attrs.AddAttribute(val);
    }
    return attrs;
}

wxGLAttributes& wxGLBackendX11::MinRGBA(wxGLAttributes& attrs, int mRed, int mGreen, int mBlue, int mAlpha)
{
    if ( mRed >= 0)
    {
        attrs.AddAttribute(GLX_RED_SIZE);
        attrs.AddAttribute(mRed);
    }
    if ( mGreen >= 0)
    {
        attrs.AddAttribute(GLX_GREEN_SIZE);
        attrs.AddAttribute(mGreen);
    }
    if ( mBlue >= 0)
    {
        attrs.AddAttribute(GLX_BLUE_SIZE);
        attrs.AddAttribute(mBlue);
    }
    if ( mAlpha >= 0)
    {
        attrs.AddAttribute(GLX_ALPHA_SIZE);
        attrs.AddAttribute(mAlpha);
    }
    return attrs;
}

wxGLAttributes& wxGLBackendX11::Depth(wxGLAttributes& attrs, int val)
{
    if ( val >= 0 )
    {
        attrs.AddAttribute(GLX_DEPTH_SIZE);
        attrs.AddAttribute(val);
    }
    return attrs;
}

wxGLAttributes& wxGLBackendX11::Stencil(wxGLAttributes& attrs, int val)
{
    if ( val >= 0 )
    {
        attrs.AddAttribute(GLX_STENCIL_SIZE);
        attrs.AddAttribute(val);
    }
    return attrs;
}

wxGLAttributes& wxGLBackendX11::MinAcumRGBA(wxGLAttributes& attrs, int mRed, int mGreen, int mBlue, int mAlpha)
{
    if ( mRed >= 0)
    {
        attrs.AddAttribute(GLX_ACCUM_RED_SIZE);
        attrs.AddAttribute(mRed);
    }
    if ( mGreen >= 0)
    {
        attrs.AddAttribute(GLX_ACCUM_GREEN_SIZE);
        attrs.AddAttribute(mGreen);
    }
    if ( mBlue >= 0)
    {
        attrs.AddAttribute(GLX_ACCUM_BLUE_SIZE);
        attrs.AddAttribute(mBlue);
    }
    if ( mAlpha >= 0)
    {
        attrs.AddAttribute(GLX_ACCUM_ALPHA_SIZE);
        attrs.AddAttribute(mAlpha);
    }
    return attrs;
}

wxGLAttributes& wxGLBackendX11::SampleBuffers(wxGLAttributes& attrs, int val)
{
#ifdef GLX_SAMPLE_BUFFERS_ARB
    if ( val >= 0 && IsGLXMultiSampleAvailable() )
    {
        attrs.AddAttribute(GLX_SAMPLE_BUFFERS_ARB);
        attrs.AddAttribute(val);
    }
#endif
    return attrs;
}

wxGLAttributes& wxGLBackendX11::Samplers(wxGLAttributes& attrs, int val)
{
#ifdef GLX_SAMPLES_ARB
    if ( val >= 0 && IsGLXMultiSampleAvailable() )
    {
        attrs.AddAttribute(GLX_SAMPLES_ARB);
        attrs.AddAttribute(val);
    }
#endif
    return attrs;
}

wxGLAttributes& wxGLBackendX11::FrameBuffersRGB(wxGLAttributes& attrs)
{
    attrs.AddAttribute(GLX_FRAMEBUFFER_SRGB_CAPABLE_ARB);
    attrs.AddAttribute(True);
    return attrs;
}

wxGLAttributes& wxGLBackendX11::PlatformDefaults(wxGLAttributes& attrs)
{
    // No GLX specific values
    return attrs;
}

void wxGLBackendX11::EndList(wxGLAttribsBase& attrs)
{
    attrs.AddAttribute(None);
}


// ============================================================================
// wxGLContextX11 implementation
// ============================================================================

static bool MakeCurrent(GLXDrawable drawable, GLXContext context);

// Need this X error handler for the case context creation fails
static bool g_ctxErrorOccurred = false;
static int CTXErrorHandler( Display* WXUNUSED(dpy), XErrorEvent* WXUNUSED(ev) )
{
    g_ctxErrorOccurred = true;
    return 0;
}

wxGLContextX11::wxGLContextX11(wxGLCanvas *win,
                               const wxGLContext *otherCtx,
                               const wxGLContextAttrs *ctxAttrs)
    : m_glContext(nullptr)
{
    // We assume that all contexts in the program are of the same type, so the
    // other context passed in must also be using wxGLContextX11.
    auto const otherGLXContext =
        otherCtx ? static_cast<const wxGLContextX11*>(otherCtx->GetImpl())->m_glContext
                 : None;

    // The window must also be of the appropriate type.
    auto const winX11 = static_cast<wxGLCanvasX11*>(win->GetImpl());

    // Fall back to OpenGL context parameters set at wxGLCanvas ctor if any.
    const wxGLContextAttrs& attrs = ctxAttrs ? *ctxAttrs
                                             : win->GetGLCTXAttrs();

    const int* const contextAttribs = attrs.GetGLAttrs();
    Bool x11Direct = True;
    int renderType = GLX_RGBA_TYPE;
    bool needsARB = false;

    // Note that we don't use default values from GetGLCTXAttrs() here unless
    // we have some explicit attributes specified, as the default values differ
    // from PlatformDefaults() ones, which are used just above.
    if ( contextAttribs )
    {
        renderType = attrs.renderTypeRGBA ? GLX_RGBA_TYPE : GLX_COLOR_INDEX_TYPE;
        x11Direct = attrs.x11Direct ? True : False;
        needsARB = attrs.NeedsARB();
    }

    m_isOk = false;

    Display* dpy = wxGetX11Display();
    XVisualInfo* vi = static_cast<XVisualInfo*>(winX11->GetXVisualInfo());
    wxCHECK_RET( vi, "invalid visual for OpenGL" );

    // We need to create a temporary context to get the
    // glXCreateContextAttribsARB function
    GLXContext tempContext = glXCreateContext(dpy, vi, nullptr, x11Direct);
    wxCHECK_RET(tempContext, "glXCreateContext failed" );

    GLXFBConfig* const fbc = winX11->GetGLXFBConfig();
    PFNGLXCREATECONTEXTATTRIBSARBPROC wx_glXCreateContextAttribsARB = 0;
    if (fbc)
    {
        wx_glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC)
            glXGetProcAddress(reinterpret_cast<const GLubyte*>("glXCreateContextAttribsARB"));
    }

    glXDestroyContext( dpy, tempContext );

    // The preferred way is using glXCreateContextAttribsARB, even for old context
    if ( !wx_glXCreateContextAttribsARB && needsARB ) // OpenGL 3 context creation
    {
        wxLogMessage(_("OpenGL 3.0 or later is not supported by the OpenGL driver."));
        return;
    }

    // Install a X error handler, so as to the app doesn't exit (without
    // even a warning) if GL >= 3.0 context creation fails
    g_ctxErrorOccurred = false;
    int (*oldHandler)(Display*, XErrorEvent*) = XSetErrorHandler(&CTXErrorHandler);

    if ( wx_glXCreateContextAttribsARB )
    {
        m_glContext = wx_glXCreateContextAttribsARB( dpy, fbc[0],
                                otherGLXContext,
                                x11Direct, contextAttribs );

        // Some old hardware may accept the use of this ARB, but may fail.
        // In case of null attributes we'll try creating the context old-way.
        XSync( dpy, False );
        if ( g_ctxErrorOccurred && (!contextAttribs || !needsARB) )
        {
            g_ctxErrorOccurred = false; //Reset
            m_glContext = nullptr;
        }
    }

    if ( !g_ctxErrorOccurred && !m_glContext )
    {
        // Old-way, without context atributes. Up to GL 2.1
        if (fbc)
        {
            m_glContext = glXCreateNewContext( dpy, fbc[0], renderType,
                                               otherGLXContext,
                                               x11Direct );
        }
        else // GLX <= 1.2
        {
            m_glContext = glXCreateContext( dpy, vi,
                                            otherGLXContext,
                                            x11Direct );
        }
    }

    // Sync to ensure any errors generated are processed.
    XSync( dpy, False );

    if ( g_ctxErrorOccurred || !m_glContext )
        wxLogMessage(_("Couldn't create OpenGL context"));
    else
        m_isOk = true;

    // Restore old error handler
    XSetErrorHandler( oldHandler );
}

wxGLContextX11::~wxGLContextX11()
{
    if ( !m_glContext )
        return;

    if ( m_glContext == glXGetCurrentContext() )
        wxGLBackendX11_instance.ClearCurrentContext();

    glXDestroyContext( wxGetX11Display(), m_glContext );
}

bool wxGLContextX11::SetCurrent(const wxGLCanvas& win) const
{
    if ( !m_glContext )
        return false;

    const Window xid = win.GetXWindow();
    wxCHECK2_MSG( xid, return false, wxT("window must be shown") );

    return MakeCurrent(xid, m_glContext);
}

// wrapper around glXMakeContextCurrent/glXMakeCurrent depending on GLX
// version
static bool MakeCurrent(GLXDrawable drawable, GLXContext context)
{
    if (wxGLBackendX11_instance.GetGLXVersion() >= 13)
        return glXMakeContextCurrent( wxGetX11Display(), drawable, drawable, context);
    else // GLX <= 1.2 doesn't have glXMakeContextCurrent()
        return glXMakeCurrent( wxGetX11Display(), drawable, context);
}

// ============================================================================
// wxGLCanvasX11 implementation
// ============================================================================

static GLXFBConfig* gs_glFBCInfo;
static XVisualInfo* gs_glVisualInfo;

static bool InitXVisualInfo(
    const wxGLAttributes& dispAttrs, GLXFBConfig** pFBC, XVisualInfo** pXVisual);

// ----------------------------------------------------------------------------
// initialization methods and dtor
// ----------------------------------------------------------------------------

wxGLCanvasX11::wxGLCanvasX11(wxGLCanvasUnix* canvas)
    : wxGLCanvasUnixImpl(canvas)
{
    m_fbc = nullptr;
    m_vi = nullptr;
}

bool wxGLCanvasX11::InitVisual(const wxGLAttributes& dispAttrs)
{
    XVisualInfo* vi = nullptr;
    bool ret = InitXVisualInfo(dispAttrs, &m_fbc, &vi);
    m_vi = vi;
    if ( !ret )
    {
        wxFAIL_MSG("Failed to get a XVisualInfo for the requested attributes.");
    }
    return ret;
}

wxGLCanvasX11::~wxGLCanvasX11()
{
    if (m_fbc && m_fbc != gs_glFBCInfo)
        XFree(m_fbc);

    if (m_vi && m_vi != gs_glVisualInfo)
        XFree(m_vi);
}

// ----------------------------------------------------------------------------
// working with GL attributes
// ----------------------------------------------------------------------------

bool wxGLBackendX11::IsExtensionSupported(const char *extension)
{
    Display * const dpy = wxGetX11Display();

    return wxGLCanvasBase::IsExtensionInList
           (
                glXQueryExtensionsString(dpy, DefaultScreen(dpy)),
                extension
           );
}


bool wxGLBackendX11::IsGLXMultiSampleAvailable()
{
    static int s_isMultiSampleAvailable = -1;
    if ( s_isMultiSampleAvailable == -1 )
        s_isMultiSampleAvailable = IsExtensionSupported("GLX_ARB_multisample");

    return s_isMultiSampleAvailable != 0;
}

static bool InitXVisualInfo(const wxGLAttributes& dispAttrs,
                                    GLXFBConfig** pFBC,
                                    XVisualInfo** pXVisual)
{
    // GLX_XX attributes
    const int* attrsListGLX = dispAttrs.GetGLAttrs();
    if ( !attrsListGLX )
    {
        wxFAIL_MSG("wxGLAttributes object is empty.");
        return false;
    }

    Display* dpy = wxGetX11Display();

    if (wxGLBackendX11_instance.GetGLXVersion() >= 13)
    {
        int returned;
        *pFBC = glXChooseFBConfig(dpy, DefaultScreen(dpy), attrsListGLX, &returned);

        if ( *pFBC )
        {
            // Use the first good match
            *pXVisual = glXGetVisualFromFBConfig(wxGetX11Display(), **pFBC);
            if ( !*pXVisual )
            {
                XFree(*pFBC);
                *pFBC = nullptr;
            }
        }
    }
    else // GLX <= 1.2
    {
        *pFBC = nullptr;
        *pXVisual = glXChooseVisual(dpy, DefaultScreen(dpy),
                                    const_cast<int*>(attrsListGLX) );
    }

    return *pXVisual != nullptr;
}

bool wxGLBackendX11::IsDisplaySupported(const wxGLAttributes& dispAttrs)
{
    wxX11Ptr<GLXFBConfig> fbc;
    wxX11Ptr<XVisualInfo> vi;

    return InitXVisualInfo(dispAttrs, fbc.Out(), vi.Out());
}

// ----------------------------------------------------------------------------
// default visual management
// ----------------------------------------------------------------------------

void wxGLBackendX11::FreeDefaultVisualInfo()
{
    if (gs_glFBCInfo)
    {
        XFree(gs_glFBCInfo);
        gs_glFBCInfo = nullptr;
    }
    if (gs_glVisualInfo)
    {
        XFree(gs_glVisualInfo);
        gs_glVisualInfo = nullptr;
    }
}

bool wxGLBackendX11::InitDefaultVisualInfo(const int* attribList)
{
    FreeDefaultVisualInfo();
    wxGLAttributes dispAttrs;
    wxGLCanvasBase::ParseAttribList(attribList, dispAttrs);

    return InitXVisualInfo(dispAttrs, &gs_glFBCInfo, &gs_glVisualInfo);
}

void* wxGLBackendX11::GetDefaultVisualInfo()
{
    return gs_glVisualInfo;
}

// ----------------------------------------------------------------------------
// other GL methods
// ----------------------------------------------------------------------------

bool wxGLBackendX11::GetEGLVersion(int* WXUNUSED(major), int* WXUNUSED(minor))
{
    return false;
}

int wxGLBackendX11::GetGLXVersion()
{
    static int s_glxVersion = 0;
    if ( s_glxVersion == 0 )
    {
        // check the GLX version
        int glxMajorVer, glxMinorVer;
        bool ok = glXQueryVersion(wxGetX11Display(), &glxMajorVer, &glxMinorVer);
        wxASSERT_MSG( ok, wxT("GLX version not found") );
        if (!ok)
            s_glxVersion = 10; // 1.0 by default
        else
            s_glxVersion = glxMajorVer*10 + glxMinorVer;
    }

    return s_glxVersion;
}

namespace
{

bool IsSwapControlExtensionSupported()
{
    return wxGLBackendX11_instance.IsExtensionSupported("GLX_EXT_swap_control");
}

bool IsMesaSwapControlExtensionSupported()
{
    return wxGLBackendX11_instance.IsExtensionSupported("GLX_MESA_swap_control");
}

// Call glXSwapIntervalEXT() if present.
//
// For now just try using EXT_swap_control extension, in principle there is
// also a MESA one, but it's not clear if it's worth falling back on it (or
// preferring to use it?).
bool wxGLSetSwapInterval(Display* dpy, GLXDrawable drawable, int interval)
{
    typedef void (*PFNGLXSWAPINTERVALEXTPROC)(Display *dpy,
                                              GLXDrawable drawable,
                                              int interval);
    static PFNGLXSWAPINTERVALEXTPROC s_glXSwapIntervalEXT = nullptr;

    typedef void (*PFNGLXSWAPINTERVALMESAPROC)(int interval);
    static PFNGLXSWAPINTERVALMESAPROC s_glXSwapIntervalMESA = nullptr;

    if ( IsSwapControlExtensionSupported() )
    {
        static bool s_glXSwapIntervalEXTInit = false;
        if ( !s_glXSwapIntervalEXTInit )
        {
            s_glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC)
                glXGetProcAddress((const GLubyte*)"glXSwapIntervalEXT");

            s_glXSwapIntervalEXTInit = true;

            if ( !s_glXSwapIntervalEXT )
            {
                wxLogTrace(TRACE_GLX, "GLX_EXT_swap_control supported but "
                           "glXSwapIntervalEXT() unexpectedly not found");
            }
        }
    }
    else if ( IsMesaSwapControlExtensionSupported() )
    {
        static bool s_glXSwapIntervalMESAInit = false;
        if ( !s_glXSwapIntervalMESAInit )
        {
            s_glXSwapIntervalMESA = (PFNGLXSWAPINTERVALMESAPROC)
                glXGetProcAddress((const GLubyte*)"glXSwapIntervalMESA");

            s_glXSwapIntervalMESAInit = true;

            if ( !s_glXSwapIntervalMESA )
            {
                wxLogTrace(TRACE_GLX, "GLX_MESA_swap_control supported but "
                           "glXSwapIntervalMESA() unexpectedly not found");
            }
        }
    }

    if ( s_glXSwapIntervalEXT )
    {
        wxLogTrace(TRACE_GLX, "Setting GLX swap interval to %d", interval);

        s_glXSwapIntervalEXT(dpy, drawable, interval);

        return true;
    }
    else if ( s_glXSwapIntervalMESA )
    {
        wxLogTrace(TRACE_GLX, "Setting GLX swap interval to %d (using MESA)",
                   interval);

        s_glXSwapIntervalMESA(interval);
    }

    return false;
}

int wxMESAGetSwapInterval()
{
    typedef int (*PFNGLXGETSWAPINTERVALMESAPROC)();
    static PFNGLXGETSWAPINTERVALMESAPROC s_glXGetSwapIntervalMESA = nullptr;

    if ( IsMesaSwapControlExtensionSupported() )
    {
        static bool s_glXGetSwapIntervalMESAInit = false;
        if ( !s_glXGetSwapIntervalMESAInit )
        {
            s_glXGetSwapIntervalMESA = (PFNGLXGETSWAPINTERVALMESAPROC)
                glXGetProcAddress((const GLubyte*)"glXGetSwapIntervalMESA");

            s_glXGetSwapIntervalMESAInit = true;

            if ( !s_glXGetSwapIntervalMESA )
            {
                wxLogTrace(TRACE_GLX, "GLX_MESA_swap_control supported but "
                           "glXGetSwapIntervalMESA() unexpectedly not found");
            }
        }
    }

    if ( s_glXGetSwapIntervalMESA )
    {
        return s_glXGetSwapIntervalMESA();
    }

    return wxGLCanvas::DefaultSwapInterval;
}

} // anonymous namespace

bool wxGLCanvasX11::SwapBuffers()
{
    const Window xid = m_canvas->GetXWindow();
    wxCHECK2_MSG( xid, return false, wxT("window must be shown") );

    const auto dpy = wxGetX11Display();

    // Disable blocking in glXSwapBuffers, as this is needed under XWayland for
    // the reasons explained in wxGLCanvasEGL::SwapBuffers().
    if ( m_swapIntervalToSet != wxGLCanvas::DefaultSwapInterval )
    {
        wxGLSetSwapInterval(dpy, xid, m_swapIntervalToSet);

        // Don't try again in any case, if we failed this time, we'll fail the
        // next one anyhow.
        m_swapIntervalToSet = wxGLCanvas::DefaultSwapInterval;
    }

    glXSwapBuffers(dpy, xid);
    return true;
}

bool wxGLCanvasX11::DoSetSwapInterval(int interval)
{
    const Window xid = m_canvas->GetXWindow();
    const auto dpy = wxGetX11Display();

    return wxGLSetSwapInterval(dpy, xid, interval);
}

int wxGLCanvasX11::GetSwapInterval() const
{
    int swapInterval = wxGLCanvas::DefaultSwapInterval;

    const Window xid = m_canvas->GetXWindow();
    if ( xid )
    {
        if ( IsSwapControlExtensionSupported() )
        {
            const auto dpy = wxGetX11Display();
            unsigned int value = 0;
            glXQueryDrawable(dpy, xid, GLX_SWAP_INTERVAL_EXT, &value);

            swapInterval = value;
        }
        else if ( IsMesaSwapControlExtensionSupported() )
        {
            swapInterval = wxMESAGetSwapInterval();
        }
    }

    return swapInterval;
}

bool wxGLCanvasX11::HasWindow() const
{
    return m_canvas->GetXWindow();
}

// ----------------------------------------------------------------------------
// wxGLBackendX11
// ----------------------------------------------------------------------------

/* static */
wxGLBackend& wxGLBackendX11::Get()
{
    return wxGLBackendX11_instance;
}

std::unique_ptr<wxGLContextImpl>
wxGLBackendX11::CreateContextImpl(wxGLCanvas* win,
                                  const wxGLContext* other,
                                  const wxGLContextAttrs* ctxAttrs)
{
    return std::make_unique<wxGLContextX11>(win, other, ctxAttrs);
}

std::unique_ptr<wxGLCanvasUnixImpl>
wxGLBackendX11::CreateCanvasImpl(wxGLCanvasUnix* canvas)
{
    return std::make_unique<wxGLCanvasX11>(canvas);
}

void wxGLBackendX11::ClearCurrentContext()
{
    MakeCurrent(None, nullptr);
}

#endif // wxUSE_GLCANVAS
