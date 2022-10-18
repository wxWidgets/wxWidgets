///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/glcanvas.mm
// Purpose:     wxGLCanvas, for using OpenGL with wxWidgets under iPhone
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

#import <GLKit/GLKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>
#import <QuartzCore/QuartzCore.h>

@interface wxUICustomOpenGLView : GLKView
{
}

@property (retain) GLKViewController *viewController;

@end

@implementation wxUICustomOpenGLView

+ (void)initialize
{
    static BOOL initialized = NO;
    if (!initialized) 
    {
        initialized = YES;
        wxOSXIPhoneClassAddWXMethods( self );
    }
}

- (void) swapBuffers {
    [self.context presentRenderbuffer:GL_RENDERBUFFER_OES];
}

@end


WXGLContext WXGLCreateContext( WXGLPixelFormat pixelFormat, WXGLContext shareContext )
{
    WXGLContext context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
    
    if ( !context )
        wxFAIL_MSG("NSOpenGLContext creation failed");
    return context ;
}

void WXGLDestroyContext( WXGLContext context )
{
    if ( context )
    {
        [context release];
    }
}

WXGLContext WXGLGetCurrentContext()
{
    return [EAGLContext currentContext];
}

bool WXGLSetCurrentContext(WXGLContext context)
{
    [EAGLContext setCurrentContext:context];
    return true;
}

void WXGLDestroyPixelFormat( WXGLPixelFormat pixelFormat )
{
}


WXGLPixelFormat WXGLChoosePixelFormat(const int *GLAttrs,
                                      int n1,
                                      const int *ctxAttrs,
                                      int n2)
{
    return @"dummy";
}

bool wxGLContext::SetCurrent(const wxGLCanvas& win) const
{
    if ( !m_glContext )
        return false;  

    wxUICustomOpenGLView* v = (wxUICustomOpenGLView*) win.GetPeer()->GetWXWidget();
    if ( v.context != m_glContext ) {
        [v setContext:m_glContext];
        [v bindDrawable];
    }
    WXGLSetCurrentContext(m_glContext);
    
    return true;
}

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

    CGRect r = wxOSXGetFrameForControl( this, pos , size ) ;

    // We need a context, otherwise the view will not get an initial
    // Paint Event, making our own samples fail ...
    WXGLPixelFormat pf = WXGLChoosePixelFormat(nullptr, 0, nullptr, 0);
    WXGLContext context = WXGLCreateContext(pf, nullptr);

    wxUICustomOpenGLView* v = [[wxUICustomOpenGLView alloc] initWithFrame:r context: context ];

    WXGLDestroyContext(context);
    WXGLDestroyPixelFormat(pf);

    wxWidgetIPhoneImpl* c = new wxWidgetIPhoneImpl( this, v, wxWidgetImpl::Widget_UserKeyEvents | wxWidgetImpl::Widget_UserMouseEvents );
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

    wxUICustomOpenGLView* v = (wxUICustomOpenGLView*) GetPeer()->GetWXWidget();
    if ( v != nil )
        [v swapBuffers];
    return true;
}


#endif // wxUSE_GLCANVAS
