/////////////////////////////////////////////////////////////////////////////
// Name:        glcanvas.cpp
// Purpose:     wxGLCanvas, for using OpenGL with wxWindows 2.0 for Motif.
//              Uses the GLX extension.
// Author:      Julian Smart and Wolfram Gloger
// Modified by:
// Created:     1995, 1999
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Wolfram Gloger
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "glcanvas.h"
#endif

#include "glcanvas.h"
#include "wx/utils.h"
#include "wx/app.h"

#include <Xm/Xm.h>
#include "wx/motif/private.h"

#ifdef OLD_MESA
// workaround for bug in Mesa's glx.c
static int bitcount( unsigned long n )
{
    int bits;
    for (bits=0; n>0;) {
	if(n & 1) bits++;
	n = n >> 1;
    }
    return bits;
}
#endif

/*
 * GLCanvas implementation
 */

IMPLEMENT_CLASS(wxGLCanvas, wxScrolledWindow)

wxGLCanvas::wxGLCanvas(wxWindow *parent, wxWindowID id = -1, const wxPoint& pos,
  const wxSize& size, long style, 
  const wxString& name, int *attrib_list, const wxPalette& palette):
    wxScrolledWindow(parent, id, pos, size, style, name)
{
    XVisualInfo *vi, vi_templ;
    XWindowAttributes xwa;
    int val, n;

    Display* display = (Display*) GetXDisplay();

    glx_cx = 0;
    // Check for the presence of the GLX extension
    if(!glXQueryExtension(display, NULL, NULL)) {
	wxDebugMsg("wxGLCanvas: GLX extension is missing\n");
	return;
    }

    if(attrib_list) {
	// Get an appropriate visual
	vi = glXChooseVisual(display, DefaultScreen(display), attrib_list);
	if(!vi) return;

	// Here we should make sure that vi is the same visual as the
	// one used by the xwindow drawable in wxCanvas.  However,
	// there is currently no mechanism for this in wx_canvs.cc.
    } else {
	// By default, we use the visual of xwindow
	XGetWindowAttributes(display, (Window) GetXWindow(), &xwa);
	vi_templ.visualid = XVisualIDFromVisual(xwa.visual);
	vi = XGetVisualInfo(display, VisualIDMask, &vi_templ, &n);
	if(!vi) return;
	glXGetConfig(display, vi, GLX_USE_GL, &val);
	if(!val) return;
	// Basically, this is it.  It should be possible to use vi
	// in glXCreateContext() below.  But this fails with Mesa.
	// I notified the Mesa author about it; there may be a fix.
#ifdef OLD_MESA
	// Construct an attribute list matching the visual
	int a_list[32];
	n = 0;
	if(vi->c_class==TrueColor || vi->c_class==DirectColor) { // RGBA visual
	    a_list[n++] = GLX_RGBA;
	    a_list[n++] = GLX_RED_SIZE;
	    a_list[n++] = bitcount(vi->red_mask);
	    a_list[n++] = GLX_GREEN_SIZE;
	    a_list[n++] = bitcount(vi->green_mask);
	    a_list[n++] = GLX_BLUE_SIZE;
	    a_list[n++] = bitcount(vi->blue_mask);
	    glXGetConfig(display, vi, GLX_ALPHA_SIZE, &val);
	    a_list[n++] = GLX_ALPHA_SIZE;
	    a_list[n++] = val;
	} else { // Color index visual
	    glXGetConfig(display, vi, GLX_BUFFER_SIZE, &val);
	    a_list[n++] = GLX_BUFFER_SIZE;
	    a_list[n++] = val;
	}
	a_list[n] = None;
	XFree(vi);
	vi = glXChooseVisual(display, DefaultScreen(display), a_list);
	if(!vi) return;
#endif /* OLD_MESA */
    }

    // Create the GLX context and make it current
    glx_cx = glXCreateContext(display, vi, 0, GL_TRUE);
#ifndef OLD_MESA
    XFree(vi);
#endif
    SetCurrent();
}

wxGLCanvas::~wxGLCanvas(void)
{
    Display* display = (Display*) GetXDisplay();
    if(glx_cx) glXDestroyContext(display, glx_cx);
}

void wxGLCanvas::SwapBuffers()
{
    Display* display = (Display*) GetXDisplay();
    if(glx_cx) glXSwapBuffers(display, (Window) GetXWindow());
}

void wxGLCanvas::SetCurrent()
{
    Display* display = (Display*) GetXDisplay();
    if(glx_cx) glXMakeCurrent(display, (Window) GetXWindow(), glx_cx);
}

void wxGLCanvas::SetColour(const char *col)
{
    wxColour *the_colour = wxTheColourDatabase->FindColour(col);
    if(the_colour) {
	GLboolean b;
	glGetBooleanv(GL_RGBA_MODE, &b);
	if(b) {
	    glColor3ub(the_colour->Red(),
		       the_colour->Green(),
		       the_colour->Blue());
	} else {
	    GLint pix = (GLint)the_colour->m_pixel;
	    if(pix == -1) {
		XColor exact_def;
		exact_def.red = (unsigned short)the_colour->Red() << 8;
		exact_def.green = (unsigned short)the_colour->Green() << 8;
		exact_def.blue = (unsigned short)the_colour->Blue() << 8;
		exact_def.flags = DoRed | DoGreen | DoBlue;
		if(!XAllocColor((Display*) GetXDisplay(), (Colormap) wxTheApp->GetMainColormap(GetXDisplay()), &exact_def)) {
		    wxDebugMsg("wxGLCanvas: cannot allocate color\n");
		    return;
		}
		pix = the_colour->m_pixel = exact_def.pixel;
	    }
	    glIndexi(pix);
	}
    }
}

