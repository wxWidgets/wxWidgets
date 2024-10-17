/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/glcanvas.h
// Purpose:     wxGLCanvas, for using OpenGL/Mesa with wxWidgets and GTK
// Author:      Robert Roebling
// Created:     17/8/98
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GLCANVAS_H_
#define _WX_GLCANVAS_H_

#include "wx/setup.h"

#if wxUSE_GLCANVAS_EGL
    #include "wx/unix/glegl.h"
    typedef wxGLCanvasEGL wxGLCanvasImpl;
#else
    #include "wx/unix/glx11.h"
    typedef wxGLCanvasX11 wxGLCanvasImpl;
#endif

//---------------------------------------------------------------------------
// wxGLCanvas
//---------------------------------------------------------------------------

class WXDLLIMPEXP_GL wxGLCanvas : public wxGLCanvasImpl
{
    typedef wxGLCanvasImpl BaseType;
public:
    wxGLCanvas() = default;

    wxGLCanvas(wxWindow *parent,
               const wxGLAttributes& dispAttrs,
               wxWindowID id = wxID_ANY,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxGLCanvasName,
               const wxPalette& palette = wxNullPalette);

    explicit // avoid implicitly converting a wxWindow* to wxGLCanvas
    wxGLCanvas(wxWindow *parent,
               wxWindowID id = wxID_ANY,
               const int *attribList = nullptr,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxGLCanvasName,
               const wxPalette& palette = wxNullPalette);

    bool Create(wxWindow *parent,
                const wxGLAttributes& dispAttrs,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxGLCanvasName,
                const wxPalette& palette = wxNullPalette);

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxGLCanvasName,
                const int *attribList = nullptr,
                const wxPalette& palette = wxNullPalette);

    virtual bool SetBackgroundStyle(wxBackgroundStyle style) override;

    // implement wxGLCanvasX11 methods
    // --------------------------------

    virtual unsigned long GetXWindow() const override;
    void* GetNativeWindow() const;

    // implementation from now on
    virtual void GTKHandleRealized() override;

#ifdef __WXGTK3__
    wxSize m_size;
#endif

private:
    wxDECLARE_CLASS(wxGLCanvas);
};

#endif // _WX_GLCANVAS_H_

