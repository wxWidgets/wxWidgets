///////////////////////////////////////////////////////////////////////////////
// Name:        wx/unix/glcanvas.h
// Purpose:     class common for wxGLCanvas implementations in wxGTK and wxX11
// Author:      Vadim Zeitlin
// Created:     2007-04-15
// Copyright:   (c) 2007 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIX_GLCANVAS_H_
#define _WX_UNIX_GLCANVAS_H_

#include <memory>

#include <GL/gl.h>

// Forward declare struct from GL/glx.h
typedef struct __GLXFBConfigRec* GLXFBConfig;

class wxGLContextImpl;
class wxGLCanvasUnixImpl;

// ----------------------------------------------------------------------------
// wxGLContext: used as is in all ports
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_GL wxGLContext : public wxGLContextBase
{
public:
    wxGLContext(wxGLCanvas *win,
                const wxGLContext *other = nullptr,
                const wxGLContextAttrs *ctxAttrs = nullptr);
    virtual ~wxGLContext();

    virtual bool SetCurrent(const wxGLCanvas& win) const override;

    // Implementation only.
    wxGLContextImpl* GetImpl() const { return m_impl.get(); }

private:
    std::unique_ptr<wxGLContextImpl> m_impl;

    wxDECLARE_CLASS(wxGLContext);
};

// ----------------------------------------------------------------------------
// wxGLCanvasUnix: base class for port-specific wxGLCanvas
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_GL wxGLCanvasUnix : public wxGLCanvasBase
{
public:
    virtual ~wxGLCanvasUnix();

    virtual bool SwapBuffers() override;
    virtual SwapInterval SetSwapInterval(int interval) override;
    virtual int GetSwapInterval() const override;

    virtual bool IsShownOnScreen() const override;

    // EGL-specific methods
    // --------------------

    // return true when using EGL, filling major and minor with EGL version
    static bool GetEGLVersion(int* major, int* minor);


    // GLX-specific methods
    // --------------------

    // if both GLX and EGL are available, prefer using GLX instead of EGL used
    // by default
    static void PreferGLX();

    // return GLX version: 13 means 1.3 &c
    static int GetGLXVersion();

    // return true if multisample extension is available
    static bool IsGLXMultiSampleAvailable();

    // get the X11 handle of this window
    virtual unsigned long GetXWindow() const = 0;


    // Implementation only.
    wxGLCanvasUnixImpl* GetImpl() const { return m_impl.get(); }

    void* GetXVisualInfo() const;

protected:
    wxGLCanvasUnix();

    bool InitVisual(const wxGLAttributes& dispAttrs);

    // This is only called by wxGTK but defined in any case.
    void CallOnRealized();

private:
    std::unique_ptr<wxGLCanvasUnixImpl> m_impl;
};

// ----------------------------------------------------------------------------
// wxGLApp
// ----------------------------------------------------------------------------

// this is used in wx/glcanvas.h, prevent it from defining a generic wxGLApp
#define wxGL_APP_DEFINED

class WXDLLIMPEXP_GL wxGLApp : public wxGLAppBase
{
public:
    wxGLApp() = default;

    virtual bool InitGLVisual(const int *attribList) override;

    virtual void* GetXVisualInfo();

    // and override this wxApp method to clean up
    virtual int OnExit() override;

private:
    wxDECLARE_DYNAMIC_CLASS(wxGLApp);
};

#endif // _WX_UNIX_GLCANVAS_H_
