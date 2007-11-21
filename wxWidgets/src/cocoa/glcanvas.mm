/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/glcanvas.mm
// Purpose:     wxGLContext, wxGLCanvas
// Author:      David Elliott
// Modified by:
// Created:     2004/09/29
// RCS-ID:      $Id$
// Copyright:   (c) 2004 David Elliott
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_GLCANVAS

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif //WX_PRECOMP
#include "wx/glcanvas.h"

#include "wx/cocoa/autorelease.h"

#import <AppKit/NSOpenGL.h>
#import <AppKit/NSOpenGLView.h>

IMPLEMENT_DYNAMIC_CLASS(wxGLCanvas, wxWindow)
BEGIN_EVENT_TABLE(wxGLCanvas, wxWindow)
END_EVENT_TABLE()
// WX_IMPLEMENT_COCOA_OWNER(wxGLCanvas,NSOpenGLView,NSView,NSView)

wxGLCanvas::wxGLCanvas(wxWindow *parent,
            wxWindowID winid, const wxPoint& pos, const wxSize& size,
            long style, const wxString& name,
            int *attribList, const wxPalette& palette)
{
    Create(parent,winid,pos,size,style,name);
}

wxGLCanvas::wxGLCanvas(wxWindow *parent,
            const wxGLContext *shared,
            wxWindowID winid, const wxPoint& pos, const wxSize& size,
            long style, const wxString& name,
            int *attribList, const wxPalette& palette)
{
    Create(parent,winid,pos,size,style,name);
}

wxGLCanvas::wxGLCanvas(wxWindow *parent,
            const wxGLCanvas *shared,
            wxWindowID winid, const wxPoint& pos, const wxSize& size,
            long style, const wxString& name,
            int *attribList, const wxPalette& palette)
{
    Create(parent,winid,pos,size,style,name);
}

bool wxGLCanvas::Create(wxWindow *parent, wxWindowID winid,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    wxAutoNSAutoreleasePool pool;
    if(!CreateBase(parent,winid,pos,size,style,wxDefaultValidator,name))
        return false;
    SetNSView([[NSOpenGLView alloc] initWithFrame: MakeDefaultNSRect(size)
                pixelFormat:[NSOpenGLView defaultPixelFormat]]);
    [m_cocoaNSView release];
    if(m_parent)
        m_parent->CocoaAddChild(this);
    SetInitialFrameRect(pos,size);

    return true;
}

wxGLCanvas::~wxGLCanvas()
{
}

void wxGLCanvas::SetCurrent()
{
    [[(NSOpenGLView*)m_cocoaNSView openGLContext] makeCurrentContext];
}

void wxGLCanvas::SwapBuffers()
{
    [[(NSOpenGLView*)m_cocoaNSView openGLContext] flushBuffer];
}

#endif // wxUSE_GLCANVAS
