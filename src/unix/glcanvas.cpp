///////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/glcanvas.cpp
// Purpose:     wxGLCanvas implementation common to wxGTK and wxX11
// Author:      Vadim Zeitlin
// Created:     2025-12-07
// Copyright:   (c) 2025 Vadim Zeitlin <vadim@wxwidgets.org>
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

#include "wx/sysopt.h"

#include "wx/glcanvas.h"

#include "wx/unix/private/glcanvas.h"
#ifdef wxHAS_GLX
#include "wx/unix/private/glx11.h"
#endif // wxHAS_GLX
#ifdef wxHAS_EGL
    #include "wx/unix/private/glegl.h"

    #ifdef __WXGTK3__
        #include "wx/gtk/private/wrapgdk.h"
        #include "wx/gtk/private/backend.h"
    #endif // __WXGTK3__
#endif // wxHAS_EGL

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxGLContextAttrs: OpenGL rendering context attributes
// ----------------------------------------------------------------------------

wxGLContextAttrs& wxGLContextAttrs::CoreProfile()
{
    return wxGLBackend::Get().CoreProfile(*this);
}

wxGLContextAttrs& wxGLContextAttrs::MajorVersion(int val)
{
    return wxGLBackend::Get().MajorVersion(*this, val);
}

wxGLContextAttrs& wxGLContextAttrs::MinorVersion(int val)
{
    return wxGLBackend::Get().MinorVersion(*this, val);
}

wxGLContextAttrs& wxGLContextAttrs::CompatibilityProfile()
{
    return wxGLBackend::Get().CompatibilityProfile(*this);
}

wxGLContextAttrs& wxGLContextAttrs::ForwardCompatible()
{
    return wxGLBackend::Get().ForwardCompatible(*this);
}

wxGLContextAttrs& wxGLContextAttrs::ES2()
{
    return wxGLBackend::Get().ES2(*this);
}

wxGLContextAttrs& wxGLContextAttrs::DebugCtx()
{
    return wxGLBackend::Get().DebugCtx(*this);
}

wxGLContextAttrs& wxGLContextAttrs::Robust()
{
    return wxGLBackend::Get().Robust(*this);
}

wxGLContextAttrs& wxGLContextAttrs::NoResetNotify()
{
    return wxGLBackend::Get().NoResetNotify(*this);
}

wxGLContextAttrs& wxGLContextAttrs::LoseOnReset()
{
    return wxGLBackend::Get().LoseOnReset(*this);
}

wxGLContextAttrs& wxGLContextAttrs::ResetIsolation()
{
    return wxGLBackend::Get().ResetIsolation(*this);
}

wxGLContextAttrs& wxGLContextAttrs::ReleaseFlush(int val)
{
    return wxGLBackend::Get().ReleaseFlush(*this, val);
}

wxGLContextAttrs& wxGLContextAttrs::PlatformDefaults()
{
    return wxGLBackend::Get().PlatformDefaults(*this);
}

void wxGLContextAttrs::EndList()
{
    wxGLBackend::Get().EndList(*this);
}

// ----------------------------------------------------------------------------
// wxGLAttributes: Visual/FBconfig attributes
// ----------------------------------------------------------------------------

wxGLAttributes& wxGLAttributes::RGBA()
{
    return wxGLBackend::Get().RGBA(*this);
}

wxGLAttributes& wxGLAttributes::BufferSize(int val)
{
    return wxGLBackend::Get().BufferSize(*this, val);
}

wxGLAttributes& wxGLAttributes::Level(int val)
{
    return wxGLBackend::Get().Level(*this, val);
}

wxGLAttributes& wxGLAttributes::DoubleBuffer()
{
    return wxGLBackend::Get().DoubleBuffer(*this);
}

wxGLAttributes& wxGLAttributes::Stereo()
{
    return wxGLBackend::Get().Stereo(*this);
}

wxGLAttributes& wxGLAttributes::AuxBuffers(int val)
{
    return wxGLBackend::Get().AuxBuffers(*this, val);
}

wxGLAttributes& wxGLAttributes::MinRGBA(int mRed, int mGreen, int mBlue, int mAlpha)
{
    return wxGLBackend::Get().MinRGBA(*this, mRed, mGreen, mBlue, mAlpha);
}

wxGLAttributes& wxGLAttributes::Depth(int val)
{
    return wxGLBackend::Get().Depth(*this, val);
}

wxGLAttributes& wxGLAttributes::Stencil(int val)
{
    return wxGLBackend::Get().Stencil(*this, val);
}

wxGLAttributes& wxGLAttributes::MinAcumRGBA(int mRed, int mGreen, int mBlue, int mAlpha)
{
    return wxGLBackend::Get().MinAcumRGBA(*this, mRed, mGreen, mBlue, mAlpha);
}

wxGLAttributes& wxGLAttributes::SampleBuffers(int val)
{
    return wxGLBackend::Get().SampleBuffers(*this, val);
}

wxGLAttributes& wxGLAttributes::Samplers(int val)
{
    return wxGLBackend::Get().Samplers(*this, val);
}

wxGLAttributes& wxGLAttributes::FrameBuffersRGB()
{
    return wxGLBackend::Get().FrameBuffersRGB(*this);
}

void wxGLAttributes::EndList()
{
    wxGLBackend::Get().EndList(*this);
}

wxGLAttributes& wxGLAttributes::PlatformDefaults()
{
    return wxGLBackend::Get().PlatformDefaults(*this);
}

// ----------------------------------------------------------------------------
// wxGLContext
// ----------------------------------------------------------------------------

wxIMPLEMENT_CLASS(wxGLContext, wxObject);

wxGLBackend* wxGLBackend::ms_instance = nullptr;

#if defined(wxHAS_EGL) && defined(wxHAS_GLX)

static bool wxGLBackendPreferGLX = false;

/* static */
void wxGLBackend::PreferGLX()
{
    wxASSERT_MSG( !ms_instance,
                  "PreferGLX() called too late and will have no effect" );

    wxGLBackendPreferGLX = true;
}
#endif // wxHAS_EGL && wxHAS_GLX

/* static */
wxGLBackend* wxGLBackend::Init()
{
#ifdef wxHAS_EGL
    // Only EGL can be used when using Wayland, so ignore calls to PreferGLX()
    // and system option in that case.
    //
    // It is unfortunate that we need to call wxGTK-specific functions from
    // this code which is shared by wxGTK and wxX11, but there is no way to
    // virtualize it, notably we can't do it at wxGLCanvas level because it may
    // be already too late: for example, the backend has to be already
    // available for attributes creation.
#if defined(__WXGTK3__) && defined(GDK_WINDOWING_WAYLAND)
    if ( wxGTKImpl::IsWayland(gdk_display_get_default()) )
        return &wxGLBackendEGL::Get();
#endif // GTK 3 with Wayland

#ifdef wxHAS_GLX
    if ( !(wxGLBackendPreferGLX || wxSystemOptions::IsFalse("opengl.egl")) )
        return &wxGLBackendEGL::Get();
#else
    return &wxGLBackendEGL::Get();
#endif // wxHAS_GLX

#endif // wxHAS_EGL

#ifdef wxHAS_GLX
    return &wxGLBackendX11::Get();
#endif // wxHAS_GLX
}

wxGLContext::wxGLContext(wxGLCanvas *win,
                         const wxGLContext *other,
                         const wxGLContextAttrs *ctxAttrs)
    : m_impl(wxGLBackend::Get().CreateContextImpl(win, other, ctxAttrs))
{
    m_isOk = m_impl && m_impl->IsOK();
}

wxGLContext::~wxGLContext() = default;

bool wxGLContext::SetCurrent(const wxGLCanvas& win) const
{
    return IsOK() && m_impl->SetCurrent(win);
}

/* static */
void wxGLContextBase::ClearCurrent()
{
    wxGLBackend::Get().ClearCurrentContext();
}

// ----------------------------------------------------------------------------
// wxGLCanvasUnix
// ----------------------------------------------------------------------------

wxGLCanvasUnix::wxGLCanvasUnix()
    : m_impl(wxGLBackend::Get().CreateCanvasImpl(this))
{
}

wxGLCanvasUnix::~wxGLCanvasUnix() = default;

bool wxGLCanvasUnix::InitVisual(const wxGLAttributes& dispAttrs)
{
    return m_impl->InitVisual(dispAttrs);
}

bool wxGLCanvasUnix::SwapBuffers()
{
    return m_impl->SwapBuffers();
}

bool wxGLCanvasUnix::IsShownOnScreen() const
{
    return m_impl->HasWindow() && wxGLCanvasBase::IsShownOnScreen();
}

void* wxGLCanvasUnix::GetXVisualInfo() const
{
    return m_impl->GetXVisualInfo();
}

void wxGLCanvasUnix::CallOnRealized()
{
    m_impl->OnRealized();
}

/* static */
void wxGLCanvasUnix::PreferGLX()
{
#if defined(wxHAS_EGL) && defined(wxHAS_GLX)
    wxGLBackend::PreferGLX();
#endif // wxHAS_EGL && wxHAS_GLX
}

/* static */
bool wxGLCanvasUnix::GetEGLVersion(int* major, int* minor)
{
    return wxGLBackend::Get().GetEGLVersion(major, minor);
}

/* static */
int wxGLCanvasUnix::GetGLXVersion()
{
    return wxGLBackend::Get().GetGLXVersion();
}

/* static */
bool wxGLCanvasUnix::IsGLXMultiSampleAvailable()
{
    return wxGLBackend::Get().IsGLXMultiSampleAvailable();
}

/* static */
bool wxGLCanvasBase::IsExtensionSupported(const char *extension)
{
    return wxGLBackend::Get().IsExtensionSupported(extension);
}

/* static */
bool wxGLCanvasBase::IsDisplaySupported(const wxGLAttributes& dispAttrs)
{
    return wxGLBackend::Get().IsDisplaySupported(dispAttrs);
}

/* static */
bool wxGLCanvasBase::IsDisplaySupported(const int *attribList)
{
    wxGLAttributes dispAttrs;
    ParseAttribList(attribList, dispAttrs);

    return IsDisplaySupported(dispAttrs);
}

// ----------------------------------------------------------------------------
// wxGLApp
// ----------------------------------------------------------------------------

bool wxGLApp::InitGLVisual(const int* attribList)
{
    return wxGLBackend::Get().InitDefaultVisualInfo(attribList);
}

void* wxGLApp::GetXVisualInfo()
{
    return wxGLBackend::Get().GetDefaultVisualInfo();
}

int wxGLApp::OnExit()
{
    wxGLBackend::Get().FreeDefaultVisualInfo();

    return wxGLAppBase::OnExit();
}
