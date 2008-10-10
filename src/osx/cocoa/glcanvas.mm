///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/glcanvas.mm
// Purpose:     wxGLCanvas, for using OpenGL with wxWidgets under Macintosh
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: glcanvas.cpp 54129 2008-06-11 19:30:52Z SC $
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

#if defined(__BORLANDC__)
    #pragma hdrstop
#endif

#if wxUSE_GLCANVAS

#include "wx/glcanvas.h"

#ifndef WX_PRECOMP
    #include "wx/frame.h"
    #include "wx/log.h"
    #include "wx/settings.h"
#endif

#include "wx/osx/private.h"


WXGLContext WXGLCreateContext( WXGLPixelFormat pixelFormat, WXGLContext shareContext )
{
    WXGLContext context = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext: shareContext];
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

void WXGLSwapBuffers( WXGLContext context )
{
    [context flushBuffer];
}

WXGLContext WXGLGetCurrentContext()
{
    return [NSOpenGLContext currentContext];
}

void WXGLDestroyPixelFormat( WXGLPixelFormat pixelFormat )
{
    if ( pixelFormat )
    {
        [pixelFormat release];
    }
}


WXGLPixelFormat WXGLChoosePixelFormat(const int *attribList)
{
    NSOpenGLPixelFormatAttribute data[512];
    const NSOpenGLPixelFormatAttribute defaultAttribs[] =
    {
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAMinimumPolicy,
        NSOpenGLPFAColorSize,8,
        NSOpenGLPFAAlphaSize,0,
        NSOpenGLPFADepthSize,8,
        (NSOpenGLPixelFormatAttribute)nil
    };

    const NSOpenGLPixelFormatAttribute *attribs;
    if ( !attribList )
    {
        attribs = defaultAttribs;
    }
    else
    {
        unsigned p = 0;
        data[p++] = NSOpenGLPFAMinimumPolicy; // make _SIZE tags behave more like GLX

        for ( unsigned arg = 0; attribList[arg] !=0 && p < WXSIZEOF(data); )
        {
            switch ( attribList[arg++] )
            {
                case WX_GL_RGBA:
                    //data[p++] = AGL_RGBA;
                    break;

                case WX_GL_BUFFER_SIZE:
                    //data[p++] = AGL_BUFFER_SIZE;
                    //data[p++] = attribList[arg++];
                    break;

                case WX_GL_LEVEL:
                    //data[p++]=AGL_LEVEL;
                    //data[p++]=attribList[arg++];
                    break;

                case WX_GL_DOUBLEBUFFER:
                    data[p++] = NSOpenGLPFADoubleBuffer;
                    break;

                case WX_GL_STEREO:
                    data[p++] = NSOpenGLPFAStereo;
                    break;

                case WX_GL_AUX_BUFFERS:
                    data[p++] = NSOpenGLPFAAuxBuffers;
                    data[p++] = attribList[arg++];
                    break;

                case WX_GL_MIN_RED:
                    data[p++] = NSOpenGLPFAColorSize;
                    data[p++] = attribList[arg++];
                    break;

                case WX_GL_MIN_GREEN:
                    //data[p++] = AGL_GREEN_SIZE;
                    //data[p++] = attribList[arg++];
                    break;

                case WX_GL_MIN_BLUE:
                    //data[p++] = AGL_BLUE_SIZE;
                    //data[p++] = attribList[arg++];
                    break;

                case WX_GL_MIN_ALPHA:
                    data[p++] = NSOpenGLPFAAlphaSize;
                    data[p++] = attribList[arg++];
                    break;

                case WX_GL_DEPTH_SIZE:
                    data[p++] = NSOpenGLPFADepthSize;
                    data[p++] = attribList[arg++];
                    break;

                case WX_GL_STENCIL_SIZE:
                    data[p++] = NSOpenGLPFAStencilSize;
                    data[p++] = attribList[arg++];
                    break;

                case WX_GL_MIN_ACCUM_RED:
                    data[p++] = NSOpenGLPFAAccumSize;
                    data[p++] = attribList[arg++];
                    break;

                case WX_GL_MIN_ACCUM_GREEN:
                    //data[p++] = AGL_ACCUM_GREEN_SIZE;
                    //data[p++] = attribList[arg++];
                    break;

                case WX_GL_MIN_ACCUM_BLUE:
                    //data[p++] = AGL_ACCUM_BLUE_SIZE;
                    //data[p++] = attribList[arg++];
                    break;

                case WX_GL_MIN_ACCUM_ALPHA:
                    //data[p++] = AGL_ACCUM_ALPHA_SIZE;
                    //data[p++] = attribList[arg++];
                    break;

                case WX_GL_SAMPLE_BUFFERS:
                    if ( !wxGLCanvas::IsAGLMultiSampleAvailable() )
                    {
                        if ( !attribList[arg++] )
                            break;

                        return false;
                    }

                    data[p++] = NSOpenGLPFASampleBuffers;
                    if ( (data[p++] = attribList[arg++]) == true )
                    {
                        // don't use software fallback
                        data[p++] = NSOpenGLPFANoRecovery;
                    }
                    break;

                case WX_GL_SAMPLES:
                    if ( !wxGLCanvas::IsAGLMultiSampleAvailable() )
                    {
                        if ( !attribList[arg++] )
                            break;

                        return false;
                    }

                    data[p++] = NSOpenGLPFASamples;
                    data[p++] = attribList[arg++];
                    break;
            }
        }

        data[p] = (NSOpenGLPixelFormatAttribute)nil;

        attribs = data;
    }

    return [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];
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

@interface wxNSCustomOpenGLView : NSView
{
    wxWidgetImpl* impl;
    NSOpenGLContext* context;
}

- (id)initWithFrame:(NSRect)frame;
- (void)setImplementation: (wxWidgetImpl *) theImplementation;
- (wxWidgetImpl*) implementation;
- (BOOL) isFlipped;

@end

@implementation wxNSCustomOpenGLView

- (id)initWithFrame:(NSRect)frame
{
    [super initWithFrame:frame];
    impl = NULL;
    return self;
}

- (void)setImplementation: (wxWidgetImpl *) theImplementation
{
    impl = theImplementation;
}

- (wxWidgetImpl*) implementation
{
    return impl;
}

- (BOOL) isFlipped
{
    return YES;
}

- (BOOL)isOpaque
{
    return YES;
}

@end

bool wxGLCanvas::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name,
                        const int *attribList,
                        const wxPalette& WXUNUSED(palette))
{
    m_glFormat = WXGLChoosePixelFormat(attribList);
    if ( !m_glFormat )
        return false;

   // m_macIsUserPane = false ;

    if ( !wxWindow::Create(parent, id, pos, size, style, name) )
        return false;

/*
    NSRect r = wxOSXGetFrameForControl( this, pos , size ) ;
    wxNSCustomOpenGLView* v = [[wxNSCustomOpenGLView alloc] initWithFrame:r];
    m_peer = new wxWidgetCocoaImpl( this, v );
    [v setImplementation:m_peer];

    MacPostControlCreate(pos, size) ;
*/
    return true;
}

wxGLCanvas::~wxGLCanvas()
{
    if ( m_glFormat )
        WXGLDestroyPixelFormat(m_glFormat);
}


#endif // wxUSE_GLCANVAS
