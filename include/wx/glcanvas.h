#ifndef _WX_GLCANVAS_H_BASE_
#define _WX_GLCANVAS_H_BASE_

#include "wx/defs.h"

#if wxUSE_GLCANVAS

#define wxGLCanvasName _T("GLCanvas")

#if defined(__WXMSW__)
#include "wx/msw/glcanvas.h"
#elif defined(__WXMOTIF__)
#include "wx/motif/glcanvas.h"
#elif defined(__WXGTK__)
#include "wx/gtk/glcanvas.h"
#elif defined(__WXX11__)
#include "wx/x11/glcanvas.h"
#elif defined(__WXMAC__)
#include "wx/mac/glcanvas.h"
#elif defined(__WXPM__)
#include "wx/os2/glcanvas.h"
#elif defined(__WXSTUBS__)
#include "wx/stubs/glcanvas.h"
#endif

#include "wx/app.h"
class WXDLLEXPORT wxGLApp : public wxApp
{
public:
    wxGLApp() : wxApp() { }
    virtual ~wxGLApp();

    // use this in the constructor of the user-derived wxGLApp class to
    // determine if an OpenGL rendering context with these attributes
    // is available - returns TRUE if so, FALSE if not.
    bool InitGLVisual(int *attribList);

private:
    DECLARE_DYNAMIC_CLASS(wxGLApp)
};

#endif
    // wxUSE_GLCANVAS
#endif
    // _WX_GLCANVAS_H_BASE_
