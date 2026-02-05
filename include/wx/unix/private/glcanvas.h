///////////////////////////////////////////////////////////////////////////////
// Name:        wx/unix/private/glcontext.h
// Purpose:     Base class for Unix-specific wxGLContext implementation
// Author:      Vadim Zeitlin
// Created:     2025-12-07
// Copyright:   (c) 2025 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIX_PRIVATE_GLCANVAS_H_
#define _WX_UNIX_PRIVATE_GLCANVAS_H_

// ----------------------------------------------------------------------------
// wxGLContextImpl is used by wxGLContext to implement its functionality
// ----------------------------------------------------------------------------

class wxGLContextImpl
{
public:
    virtual ~wxGLContextImpl() = default;

    virtual bool SetCurrent(const wxGLCanvas& win) const = 0;

    bool IsOK() const { return m_isOk; }

protected:
    wxGLContextImpl() = default;

    // This is similar to wxGLContextBase::m_isOk and allows to keep the
    // code from before the refactoring which introduced this class unchanged.
    bool m_isOk = false;

    wxDECLARE_NO_COPY_CLASS(wxGLContextImpl);
};

// ----------------------------------------------------------------------------
// wxGLCanvasUnixImpl: note that this is not a wxWindow-derived class
// ----------------------------------------------------------------------------

class wxGLCanvasUnixImpl
{
public:
    virtual ~wxGLCanvasUnixImpl() = default;

    virtual bool InitVisual(const wxGLAttributes& dispAttrs) = 0;

    virtual bool SwapBuffers() = 0;

    virtual void OnRealized() = 0;

    virtual bool HasWindow() const = 0;

    virtual void* GetXVisualInfo() const = 0;

protected:
    explicit wxGLCanvasUnixImpl(wxGLCanvasUnix* canvas)
        : m_canvas(canvas)
    {
    }

    wxGLCanvasUnix* const m_canvas;

    wxDECLARE_NO_COPY_CLASS(wxGLCanvasUnixImpl);
};

// ----------------------------------------------------------------------------
// wxGLBackend: used for creating implementation objects and implementing other
// non-member functions.
// ----------------------------------------------------------------------------

class wxGLBackend
{
public:
#if defined(wxHAS_EGL) && defined(wxHAS_GLX)
    // This can be called only before calling Get() for the first time.
    static void PreferGLX();
#endif // wxHAS_EGL && wxHAS_GLX

    // Get the (sole) instance of this class.
    static wxGLBackend& Get()
    {
        if ( !ms_instance )
            ms_instance = Init();

        return *ms_instance;
    }

    wxGLBackend(const wxGLBackend&) = delete;
    wxGLBackend& operator=(const wxGLBackend&) = delete;

    virtual ~wxGLBackend() = default;

    // Factory functions for creating the implementation objects.

    virtual std::unique_ptr<wxGLContextImpl>
    CreateContextImpl(wxGLCanvas* win,
                      const wxGLContext* other,
                      const wxGLContextAttrs* ctxAttrs) = 0;

    virtual std::unique_ptr<wxGLCanvasUnixImpl>
    CreateCanvasImpl(wxGLCanvasUnix* canvas) = 0;

    // Static functions of wxGLContext and wxGLCanvas.
    virtual void ClearCurrentContext() = 0;

    virtual bool IsExtensionSupported(const char* extension) = 0;

    virtual bool IsDisplaySupported(const wxGLAttributes& dispAttrs) = 0;

    virtual bool GetEGLVersion(int* major, int* minor) = 0;
    virtual int GetGLXVersion() = 0;

    virtual bool IsGLXMultiSampleAvailable() = 0;

    // wxGLApp functions implementation.
    virtual bool InitDefaultVisualInfo(const int* attribList) = 0;
    virtual void* GetDefaultVisualInfo() = 0;
    virtual void FreeDefaultVisualInfo() = 0;

    // wxGLContextAttrs functions implementation.
    virtual wxGLContextAttrs& CoreProfile(wxGLContextAttrs& attrs) = 0;
    virtual wxGLContextAttrs& MajorVersion(wxGLContextAttrs& attrs, int val) = 0;
    virtual wxGLContextAttrs& MinorVersion(wxGLContextAttrs& attrs, int val) = 0;
    virtual wxGLContextAttrs& CompatibilityProfile(wxGLContextAttrs& attrs) = 0;
    virtual wxGLContextAttrs& ForwardCompatible(wxGLContextAttrs& attrs) = 0;
    virtual wxGLContextAttrs& ES2(wxGLContextAttrs& attrs) = 0;
    virtual wxGLContextAttrs& DebugCtx(wxGLContextAttrs& attrs) = 0;
    virtual wxGLContextAttrs& Robust(wxGLContextAttrs& attrs) = 0;
    virtual wxGLContextAttrs& NoResetNotify(wxGLContextAttrs& attrs) = 0;
    virtual wxGLContextAttrs& LoseOnReset(wxGLContextAttrs& attrs) = 0;
    virtual wxGLContextAttrs& ResetIsolation(wxGLContextAttrs& attrs) = 0;
    virtual wxGLContextAttrs& ReleaseFlush(wxGLContextAttrs& attrs, int val) = 0;
    virtual wxGLContextAttrs& PlatformDefaults(wxGLContextAttrs& attrs) = 0;

    // wxGLAttributes functions implementation.
    virtual wxGLAttributes& RGBA(wxGLAttributes& attrs) = 0;
    virtual wxGLAttributes& BufferSize(wxGLAttributes& attrs, int val) = 0;
    virtual wxGLAttributes& Level(wxGLAttributes& attrs, int val) = 0;
    virtual wxGLAttributes& DoubleBuffer(wxGLAttributes& attrs) = 0;
    virtual wxGLAttributes& Stereo(wxGLAttributes& attrs) = 0;
    virtual wxGLAttributes& AuxBuffers(wxGLAttributes& attrs, int val) = 0;
    virtual wxGLAttributes& MinRGBA(wxGLAttributes& attrs, int mRed, int mGreen, int mBlue, int mAlpha) = 0;
    virtual wxGLAttributes& Depth(wxGLAttributes& attrs, int val) = 0;
    virtual wxGLAttributes& Stencil(wxGLAttributes& attrs, int val) = 0;
    virtual wxGLAttributes& MinAcumRGBA(wxGLAttributes& attrs, int mRed, int mGreen, int mBlue, int mAlpha) = 0;
    virtual wxGLAttributes& PlatformDefaults(wxGLAttributes& attrs) = 0;
    virtual wxGLAttributes& SampleBuffers(wxGLAttributes& attrs, int val) = 0;
    virtual wxGLAttributes& Samplers(wxGLAttributes& attrs, int val) = 0;
    virtual wxGLAttributes& FrameBuffersRGB(wxGLAttributes& attrs) = 0;

    // Both kinds of attributes are terminated in the same way.
    virtual void EndList(wxGLAttribsBase& attrs) = 0;

protected:
    wxGLBackend() = default;

private:
    static wxGLBackend* Init();

    static wxGLBackend* ms_instance;
};

#endif // _WX_UNIX_PRIVATE_GLCANVAS_H_
