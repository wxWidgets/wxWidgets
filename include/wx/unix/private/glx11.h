///////////////////////////////////////////////////////////////////////////////
// Name:        wx/unix/private/glx11.h
// Purpose:     wxGLContext GLX-based implementation for X11
// Author:      Vadim Zeitlin
// Created:     2025-12-07
// Copyright:   (c) 2025 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIX_PRIVATE_GLX11_H_
#define _WX_UNIX_PRIVATE_GLX11_H_

#include "wx/unix/private/glcanvas.h"

#include <GL/glx.h>

// ----------------------------------------------------------------------------
// wxGLContextX11
// ----------------------------------------------------------------------------

class wxGLContextX11 : public wxGLContextImpl
{
public:
    wxGLContextX11(wxGLCanvas *win,
                   const wxGLContext *other = nullptr,
                   const wxGLContextAttrs *ctxAttrs = nullptr);
    virtual ~wxGLContextX11();

    virtual bool SetCurrent(const wxGLCanvas& win) const override;

private:
    GLXContext m_glContext;
};

// ----------------------------------------------------------------------------
// wxGLCanvasX11
// ----------------------------------------------------------------------------

class wxGLCanvasX11 : public wxGLCanvasUnixImpl
{
public:
    explicit wxGLCanvasX11(wxGLCanvasUnix* canvas);

    virtual ~wxGLCanvasX11() override;

    virtual bool InitVisual(const wxGLAttributes& dispAttrs) override;

    virtual bool SwapBuffers() override;

    virtual bool DoSetSwapInterval(int interval) override;
    virtual int GetSwapInterval() const override;

    virtual void OnRealized() override { /* nothing to do here for GLX */ }

    virtual bool HasWindow() const override;

    virtual void* GetXVisualInfo() const override { return m_vi; }

    GLXFBConfig *GetGLXFBConfig() const { return m_fbc; }

    // initialize the global default GL visual, return false if matching visual
    // not found
    static bool InitDefaultVisualInfo(const int *attribList);

private:
    GLXFBConfig *m_fbc;
    XVisualInfo* m_vi;
};

// ----------------------------------------------------------------------------
// wxGLBackendX11
// ----------------------------------------------------------------------------

class wxGLBackendX11 : public wxGLBackend
{
public:
    static wxGLBackend& Get();

    wxGLBackendX11() = default;

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

#endif // _WX_UNIX_PRIVATE_GLX11_H_
