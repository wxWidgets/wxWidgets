///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/glcanvas.mm
// Purpose:     wxGLCanvas, for using OpenGL with wxWidgets under Macintosh
// Author:      Stefan Csomor
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
#include "wx/osx/private/available.h"

WXGLContext WXGLCreateContext( WXGLPixelFormat pixelFormat, WXGLContext shareContext )
{
    WXGLContext context = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext: shareContext];
    return context ;
}

void WXGLDestroyContext( WXGLContext context )
{
    if ( context )
    {
        [context release];
    }
}

void WXGLSwapBuffers( WXGLContext context )
{
    [context flushBuffer];
}

WXGLContext WXGLGetCurrentContext()
{
    return [NSOpenGLContext currentContext];
}

bool WXGLSetCurrentContext(WXGLContext context)
{
    [context makeCurrentContext];

    return true;
}

void WXGLDestroyPixelFormat( WXGLPixelFormat pixelFormat )
{
    if ( pixelFormat )
    {
        [pixelFormat release];
    }
}

// Form a list of attributes by joining canvas attributes and context attributes.
// OS X uses just one list to find a suitable pixel format.
WXGLPixelFormat WXGLChoosePixelFormat(const int *GLAttrs,
                                      int n1,
                                      const int *ctxAttrs,
                                      int n2)
{
    NSOpenGLPixelFormatAttribute data[128];
    const NSOpenGLPixelFormatAttribute *attribs;
    unsigned p = 0;

    // The list should have at least one value and the '0' at end. So the
    // minimum size is 2.
    if ( GLAttrs && n1 > 1 )
    {
        n1--; // skip the ending '0'
        while ( p < (unsigned)n1 )
        {
            data[p] = (NSOpenGLPixelFormatAttribute) GLAttrs[p];
            p++;
        }
    }

    if ( ctxAttrs && n2 > 1 )
    {
        n2--; // skip the ending '0'
        unsigned p2 = 0;
        while ( p2 < (unsigned)n2 )
            data[p++] = (NSOpenGLPixelFormatAttribute) ctxAttrs[p2++];
    }

    // End the list
    data[p] = (NSOpenGLPixelFormatAttribute) 0;

    attribs = data;

    return [[NSOpenGLPixelFormat alloc] initWithAttributes:(NSOpenGLPixelFormatAttribute*) attribs];
}

@interface wxNSCustomOpenGLView : NSOpenGLView
{
}

@end

@implementation wxNSCustomOpenGLView

+ (void)initialize
{
    static BOOL initialized = NO;
    if (!initialized)
    {
        initialized = YES;
        wxOSXCocoaClassAddWXMethods( self );
    }
}

- (BOOL)isOpaque
{
    return YES;
}

- (BOOL) acceptsFirstResponder
{
    return YES;
}

// for special keys

- (void)doCommandBySelector:(SEL)aSelector
{
    wxWidgetCocoaImpl* impl = (wxWidgetCocoaImpl* ) wxWidgetImpl::FindFromWXWidget( self );
    if (impl)
        impl->doCommandBySelector(aSelector, self, _cmd);
}

- (NSOpenGLContext *) openGLContext
{
    // Prevent the NSOpenGLView from making it's own context
    // We want to force using wxGLContexts
    return nullptr;
}

@end

bool wxGLCanvas::DoCreate(wxWindow *parent,
                          wxWindowID id,
                          const wxPoint& pos,
                          const wxSize& size,
                          long style,
                          const wxString& name)
{
    DontCreatePeer();
    
    if ( !wxWindow::Create(parent, id, pos, size, style, name) )
        return false;

    
    NSRect r = wxOSXGetFrameForControl( this, pos , size ) ;
    wxNSCustomOpenGLView* v = [[wxNSCustomOpenGLView alloc] initWithFrame:r];
    [v setWantsBestResolutionOpenGLSurface:YES];
    
    wxWidgetCocoaImpl* c = new wxWidgetCocoaImpl( this, v, wxWidgetImpl::Widget_UserKeyEvents | wxWidgetImpl::Widget_UserMouseEvents );
    SetPeer(c);
    MacPostControlCreate(pos, size) ;
    return true;
}

wxGLCanvas::~wxGLCanvas()
{
    if ( m_glFormat )
        WXGLDestroyPixelFormat(m_glFormat);
}

bool wxGLCanvas::SwapBuffers()
{
    WXGLContext context = WXGLGetCurrentContext();
    wxCHECK_MSG(context, false, wxT("should have current context"));

    [context flushBuffer];

    return true;
}

bool wxGLContext::SetCurrent(const wxGLCanvas& win) const
{
    if ( !m_glContext )
        return false;

    [m_glContext setView: win.GetHandle() ];
    [m_glContext update];

    [m_glContext makeCurrentContext];

    return true;
}

#endif // wxUSE_GLCANVAS
