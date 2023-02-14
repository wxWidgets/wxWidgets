///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/glcanvas_osx.cpp
// Purpose:     wxGLCanvas, for using OpenGL with wxWidgets under Macintosh
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"


#if wxUSE_GLCANVAS

#include "wx/glcanvas.h"

#ifndef WX_PRECOMP
    #include "wx/frame.h"
    #include "wx/log.h"
    #include "wx/settings.h"
#endif

#include "wx/osx/private.h"

// These 'WX' values are the same as 'NS' ones
// Source: https://developer.apple.com/library/mac/documentation/
//  Cocoa/Reference/ApplicationKit/Classes/NSOpenGLPixelFormat_Class/index.html
#define WXOpenGLPFAAllRenderers            1
#define WXOpenGLPFATripleBuffer            3
#define WXOpenGLPFADoubleBuffer            5
#define WXOpenGLPFAStereo                  6
#define WXOpenGLPFAAuxBuffers              7
#define WXOpenGLPFAColorSize               8
#define WXOpenGLPFAAlphaSize              11
#define WXOpenGLPFADepthSize              12
#define WXOpenGLPFAStencilSize            13
#define WXOpenGLPFAAccumSize              14
#define WXOpenGLPFAMinimumPolicy          51
#define WXOpenGLPFAMaximumPolicy          52
#define WXOpenGLPFAOffScreen              53
#define WXOpenGLPFAFullScreen             54
#define WXOpenGLPFASampleBuffers          55
#define WXOpenGLPFASamples                56
#define WXOpenGLPFAAuxDepthStencil        57
#define WXOpenGLPFAColorFloat             58
#define WXOpenGLPFAMultisample            59
#define WXOpenGLPFASupersample            60
#define WXOpenGLPFASampleAlpha            61
#define WXOpenGLPFARendererID             70
#define WXOpenGLPFASingleRenderer         71
#define WXOpenGLPFANoRecovery             72
#define WXOpenGLPFAAccelerated            73
#define WXOpenGLPFAClosestPolicy          74
#define WXOpenGLPFARobust                 75
#define WXOpenGLPFABackingStore           76
#define WXOpenGLPFAMPSafe                 78
#define WXOpenGLPFAWindow                 80
#define WXOpenGLPFAMultiScreen            81
#define WXOpenGLPFACompliant              83
#define WXOpenGLPFAScreenMask             84
#define WXOpenGLPFAPixelBuffer            90
#define WXOpenGLPFARemotePixelBuffer      91
#define WXOpenGLPFAAllowOfflineRenderers  96
#define WXOpenGLPFAAcceleratedCompute     97
#define WXOpenGLPFAOpenGLProfile          99
#define WXOpenGLPFAVirtualScreenCount    128

#define WXOpenGLProfileVersionLegacy    0x1000
#define WXOpenGLProfileVersion3_2Core   0x3200

// ----------------------------------------------------------------------------
// wxGLContextAttrs: OpenGL rendering context attributes
// ----------------------------------------------------------------------------
// OSX specific values

wxGLContextAttrs& wxGLContextAttrs::CoreProfile()
{
    AddAttribute(WXOpenGLPFAOpenGLProfile);
    AddAttribute(WXOpenGLProfileVersion3_2Core);
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::MajorVersion(int val)
{
    // No effect
    wxUnusedVar(val);
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::MinorVersion(int val)
{
    // No effect
    wxUnusedVar(val);
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::CompatibilityProfile()
{
    AddAttribute(WXOpenGLPFAOpenGLProfile);
    AddAttribute(WXOpenGLProfileVersionLegacy);
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::ForwardCompatible()
{
    // No effect
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::ES2()
{
    // No effect
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::DebugCtx()
{
    // No effect
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::Robust()
{
    // No effect. Somehow similar flag (NSOpenGLPFARobust) is deprecated in OS X v10.5
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::NoResetNotify()
{
    // No effect
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::LoseOnReset()
{
    // No effect
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::ResetIsolation()
{
    // No effect
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::ReleaseFlush(int val)
{
    // No effect
    wxUnusedVar(val);
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::PlatformDefaults()
{
    // No OSX specific defaults
    return *this;
}

void wxGLContextAttrs::EndList()
{
    AddAttribute(0);
}

// ----------------------------------------------------------------------------
// wxGLAttributes: pixel format attributes
// ----------------------------------------------------------------------------
// OSX specific values

wxGLAttributes& wxGLAttributes::RGBA()
{
    // No effect
    return *this;
}

wxGLAttributes& wxGLAttributes::BufferSize(int val)
{
    if ( val >= 0 )
    {
        AddAttribute(WXOpenGLPFAColorSize);
        AddAttribute(val);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::Level(int val)
{
    // No effect
    wxUnusedVar(val);
    return *this;
}

wxGLAttributes& wxGLAttributes::DoubleBuffer()
{
    AddAttribute(WXOpenGLPFADoubleBuffer);
    return *this;
}

wxGLAttributes& wxGLAttributes::Stereo()
{
    AddAttribute(WXOpenGLPFAStereo);
    return *this;
}

wxGLAttributes& wxGLAttributes::AuxBuffers(int val)
{
    if ( val >= 0 )
    {
        AddAttribute(WXOpenGLPFAAuxBuffers);
        AddAttribute(val);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::MinRGBA(int mRed, int mGreen, int mBlue, int mAlpha)
{
    // NOTE: 'NSOpenGLPixelFormat' doesn't offer a value for each of the RGBA
    //   components, but just one for the size of the colour buffer. In order
    //   to make wx more effective, this function _does_ set the colour buffer.

    int minColorBits = 0;
    if ( mRed > minColorBits )
        minColorBits = mRed;
    if ( mGreen > minColorBits )
        minColorBits = mGreen;
    if ( mBlue > minColorBits )
        minColorBits = mBlue;
    // Now that we have the R/G/B maximum, obtain a minimun for the buffer.
    minColorBits *= 4; // 'alpha' included

    if ( minColorBits > 0 )
    {
        AddAttribute(WXOpenGLPFAColorSize);
        AddAttribute(minColorBits);
    }

    if ( mAlpha >= 0 )
    {
        AddAttribute(WXOpenGLPFAAlphaSize);
        AddAttribute(mAlpha);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::Depth(int val)
{
    if ( val >= 0 )
    {
        AddAttribute(WXOpenGLPFADepthSize);
        AddAttribute(val);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::Stencil(int val)
{
    if ( val >= 0 )
    {
        AddAttribute(WXOpenGLPFAStencilSize);
        AddAttribute(val);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::MinAcumRGBA(int mRed, int mGreen, int mBlue, int mAlpha)
{
    // See MinRGBA() for some explanation.
    int minAcumBits = 0;
    if ( mRed > minAcumBits )
        minAcumBits = mRed;
    if ( mGreen > minAcumBits )
        minAcumBits = mGreen;
    if ( mBlue > minAcumBits )
        minAcumBits = mBlue;
    if ( mAlpha > minAcumBits )
        minAcumBits = mAlpha;
    minAcumBits *= 4;
    if ( minAcumBits >= 0 )
    {
        AddAttribute(WXOpenGLPFAAccumSize);
        AddAttribute(minAcumBits);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::SampleBuffers(int val)
{
    if ( val >= 0 )
    {
        AddAttribute(WXOpenGLPFASampleBuffers);
        AddAttribute(val);
        // Don't use software fallback
        AddAttribute(WXOpenGLPFANoRecovery);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::Samplers(int val)
{
    if ( val >= 0 )
    {
        AddAttribute(WXOpenGLPFASamples);
        AddAttribute(val);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::FrameBuffersRGB()
{
    // No effect
    return *this;
}

void wxGLAttributes::EndList()
{
    AddAttribute(0);
}

wxGLAttributes& wxGLAttributes::PlatformDefaults()
{
    AddAttribute(WXOpenGLPFAMinimumPolicy); // use _SIZE tags as minimum sizes

    // Test if we support hardware acceleration, we always want to use it if it
    // is available and, apparently, in spite of the Apple docs explicitly
    // saying the contrary:
    //
    //  If present, this attribute indicates that only hardware-accelerated
    //  renderers are considered. If not present, accelerated renderers are
    //  still preferred.
    //
    // hardware acceleration is not always used without it, so we do need to
    // specify it. But we shouldn't do it if acceleration is really not
    // available.
    const int attrsAccel[] = { WXOpenGLPFAAccelerated, 0 };
    WXGLPixelFormat testFormat = WXGLChoosePixelFormat(attrsAccel, 2);
    if ( testFormat )
    {
        // Hardware acceleration is available, use it.
        AddAttribute(WXOpenGLPFAAccelerated);
        WXGLDestroyPixelFormat(testFormat);
    }
    return *this;
}

// ----------------------------------------------------------------------------
// wxGLContext
// ----------------------------------------------------------------------------

wxGLContext::wxGLContext(wxGLCanvas *win,
                         const wxGLContext *other,
                         const wxGLContextAttrs *ctxAttrs)
    : m_glContext(nullptr)
{
    const int* contextAttribs = nullptr;
    int ctxSize = 0;

    if ( ctxAttrs )
    {
        contextAttribs = ctxAttrs->GetGLAttrs();
        ctxSize = ctxAttrs->GetSize();
    }
    else if ( win->GetGLCTXAttrs().GetGLAttrs() )
    {
        // If OpenGL context parameters were set at wxGLCanvas ctor, get them now
        contextAttribs = win->GetGLCTXAttrs().GetGLAttrs();
        ctxSize = win->GetGLCTXAttrs().GetSize();
    }
    // else use GPU driver defaults

    // Join canvas attributes and context attributes to ask for a pixel format
    WXGLPixelFormat pf = WXGLChoosePixelFormat(win->GetGLDispAttrs().GetGLAttrs(),
                                               win->GetGLDispAttrs().GetSize(),
                                               contextAttribs, ctxSize);

    m_isOk = false;

    if ( pf )
    {
        m_glContext = WXGLCreateContext(pf, other ? other->m_glContext : nullptr);
        if ( m_glContext )
        {
            m_isOk = true;
        }

        WXGLDestroyPixelFormat(pf);
    }
    if ( !m_isOk )
        wxLogMessage(_("Couldn't create OpenGL context"));
}

wxGLContext::~wxGLContext()
{
    if ( m_glContext )
    {
        WXGLDestroyContext(m_glContext);
    }
}

// ----------------------------------------------------------------------------
// wxGLCanvas
// ----------------------------------------------------------------------------

wxIMPLEMENT_CLASS(wxGLCanvas, wxWindow);

wxGLCanvas::wxGLCanvas(wxWindow *parent,
                       const wxGLAttributes& dispAttrs,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name,
                       const wxPalette& palette)
{
    Create(parent, dispAttrs, id, pos, size, style, name, palette);
}

wxGLCanvas::wxGLCanvas(wxWindow *parent,
                       wxWindowID id,
                       const int *attribList,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name,
                       const wxPalette& palette)
{
    Create(parent, id, pos, size, style, name, attribList, palette);
}

bool wxGLCanvas::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name,
                        const int *attribList,
                        const wxPalette& palette)
{
    // Separate 'pixel format' attributes.
    // Also store context attributes for wxGLContext ctor
    // If 'attribList' is null, ParseAttribList() will set defaults.
    wxGLAttributes dispAttrs;
    if ( ! ParseAttribList(attribList, dispAttrs, &m_GLCTXAttrs) )
        return false;

    return Create(parent, dispAttrs, id, pos, size, style, name, palette);
}

bool wxGLCanvas::Create(wxWindow *parent,
                        const wxGLAttributes& dispAttrs,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name,
                        const wxPalette& WXUNUSED(palette))
{
    // Don't allow an empty list
    if ( !dispAttrs.GetGLAttrs() )
    {
        wxFAIL_MSG("wxGLAttributes object is empty.");
        return false;
    }

    // Return false if any attribute is unsupported
    if ( ! IsDisplaySupported(dispAttrs) )
    {
        wxFAIL_MSG("Can't find a pixel format for the requested attributes");
        return false;
    }

    // Make a copy of attributes. Will use at wxGLContext ctor
    m_GLAttrs = dispAttrs;

    if ( !wxGLCanvas::DoCreate(parent,id,pos,size,style,name) )
        return false;
    
    return true;
}

/* static */
bool wxGLCanvas::IsAGLMultiSampleAvailable()
{
    static int s_isMultiSampleAvailable = -1;
    if ( s_isMultiSampleAvailable == -1 )
        s_isMultiSampleAvailable = IsExtensionSupported("GL_ARB_multisample");

    return s_isMultiSampleAvailable != 0;
}

/* static */
bool wxGLCanvasBase::IsDisplaySupported(const wxGLAttributes& dispAttrs)
{
    WXGLPixelFormat testFormat = WXGLChoosePixelFormat(dispAttrs.GetGLAttrs(), dispAttrs.GetSize());
    if ( testFormat )
    {
        WXGLDestroyPixelFormat(testFormat);
    }
    else
    {
        return false;
    }
    return true;
}

/* static */
bool wxGLCanvasBase::IsDisplaySupported(const int *attribList)
{
    wxGLAttributes dispAttrs;
    ParseAttribList(attribList, dispAttrs);

    return IsDisplaySupported(dispAttrs);
}

bool wxGLCanvasBase::IsExtensionSupported(const char *extension)
{
    // We need a valid context to query for extensions. Use a default one.
    wxGLAttributes dispAttrs;
    ParseAttribList(nullptr, dispAttrs); // Sets defaults
    WXGLPixelFormat fmt = WXGLChoosePixelFormat(dispAttrs.GetGLAttrs(), dispAttrs.GetSize());
    WXGLContext ctx = WXGLCreateContext(fmt, nullptr);
    if ( !ctx )
        return false;

    WXGLContext ctxOld = WXGLGetCurrentContext();
    WXGLSetCurrentContext(ctx);

    wxString extensions = wxString::FromAscii(glGetString(GL_EXTENSIONS));

    WXGLSetCurrentContext(ctxOld);
    WXGLDestroyPixelFormat(fmt);
    WXGLDestroyContext(ctx);

    return IsExtensionInList(extensions.ToAscii(), extension);
}

// ----------------------------------------------------------------------------
// wxGLApp
// ----------------------------------------------------------------------------

bool wxGLApp::InitGLVisual(const int *attribList)
{
    WXGLPixelFormat fmt = WXGLChoosePixelFormat(attribList);
    if ( !fmt )
        return false;

    WXGLDestroyPixelFormat(fmt);
    return true;
}

#endif // wxUSE_GLCANVAS
