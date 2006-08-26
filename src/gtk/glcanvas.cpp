/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/glcanvas.cpp
// Purpose:     wxGLCanvas, for using OpenGL/Mesa with wxWidgets and GTK
// Author:      Robert Roebling
// Modified by:
// Created:     17/08/98
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_GLCANVAS

#include "wx/glcanvas.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/colour.h"
#endif // WX_PRECOMP

#include "wx/module.h"

extern "C"
{
#include "gtk/gtk.h"
#include "gdk/gdk.h"
#include "gdk/gdkx.h"
}

#include "wx/gtk/win_gtk.h"
#include "wx/gtk/private.h"

// DLL options compatibility check:
#include "wx/build.h"
WX_CHECK_BUILD_OPTIONS("wxGL")


//---------------------------------------------------------------------------
// static variables
//---------------------------------------------------------------------------
int wxGLCanvas::m_glxVersion = 0;

//---------------------------------------------------------------------------
// global data
//---------------------------------------------------------------------------

XVisualInfo *g_vi = (XVisualInfo*) NULL;

//---------------------------------------------------------------------------
// wxGLContext
//---------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGLContext,wxObject)

wxGLContext::wxGLContext( bool WXUNUSED(isRGB), wxWindow *win, const wxPalette& WXUNUSED(palette) )
{
    m_window = win;
    m_widget = win->m_wxwindow;

    wxGLCanvas *gc = (wxGLCanvas*) win;

    if (wxGLCanvas::GetGLXVersion() >= 13)
    {
        // GLX >= 1.3
        GLXFBConfig *fbc = gc->m_fbc;
        wxCHECK_RET( fbc, _T("invalid GLXFBConfig for OpenGl") );
        m_glContext = glXCreateNewContext( GDK_DISPLAY(), fbc[0], GLX_RGBA_TYPE, None, GL_TRUE );
    }
    else
    {
        // GLX <= 1.2
        XVisualInfo *vi = (XVisualInfo *) gc->m_vi;
        wxCHECK_RET( vi, _T("invalid visual for OpenGl") );
        m_glContext = glXCreateContext( GDK_DISPLAY(), vi, None, GL_TRUE );
    }

    wxCHECK_RET( m_glContext, _T("Couldn't create OpenGl context") );
}

wxGLContext::wxGLContext(
               bool WXUNUSED(isRGB), wxWindow *win,
               const wxPalette& WXUNUSED(palette),
               const wxGLContext *other        /* for sharing display lists */
)
{
    m_window = win;
    m_widget = win->m_wxwindow;

    wxGLCanvas *gc = (wxGLCanvas*) win;

    if (wxGLCanvas::GetGLXVersion() >= 13)
    {
        // GLX >= 1.3
        GLXFBConfig *fbc = gc->m_fbc;
        wxCHECK_RET( fbc, _T("invalid GLXFBConfig for OpenGl") );
        m_glContext = glXCreateNewContext( GDK_DISPLAY(), fbc[0], GLX_RGBA_TYPE,
                                           other ? other->m_glContext : None,
                                           GL_TRUE );
    }
    else
    {
        // GLX <= 1.2
        XVisualInfo *vi = (XVisualInfo *) gc->m_vi;
        wxCHECK_RET( vi, _T("invalid visual for OpenGl") );
        m_glContext = glXCreateContext( GDK_DISPLAY(), vi,
                                        other ? other->m_glContext : None,
                                        GL_TRUE );
    }

    if ( !m_glContext )
    {
        wxFAIL_MSG( _T("Couldn't create OpenGl context") );
    }
}

wxGLContext::~wxGLContext()
{
    if (!m_glContext) return;

    if (m_glContext == glXGetCurrentContext())
    {
        if (wxGLCanvas::GetGLXVersion() >= 13)
            // GLX >= 1.3
            glXMakeContextCurrent( GDK_DISPLAY(), None, None, NULL);
        else
            // GLX <= 1.2
            glXMakeCurrent( GDK_DISPLAY(), None, NULL);
    }

    glXDestroyContext( GDK_DISPLAY(), m_glContext );
}

void wxGLContext::SwapBuffers()
{
    if (m_glContext)
    {
        GdkWindow *window = GTK_PIZZA(m_widget)->bin_window;
        glXSwapBuffers( GDK_DISPLAY(), GDK_WINDOW_XWINDOW( window ) );
    }
}

void wxGLContext::SetCurrent()
{
    if (m_glContext)
    {
        GdkWindow *window = GTK_PIZZA(m_widget)->bin_window;

        if (wxGLCanvas::GetGLXVersion() >= 13)
            // GLX >= 1.3
            glXMakeContextCurrent( GDK_DISPLAY(), GDK_WINDOW_XWINDOW(window),
                                   GDK_WINDOW_XWINDOW(window), m_glContext );
        else
            // GLX <= 1.2
            glXMakeCurrent( GDK_DISPLAY(), GDK_WINDOW_XWINDOW(window), m_glContext );
    }
}

void wxGLContext::SetColour(const wxChar *colour)
{
    wxColour col = wxTheColourDatabase->Find(colour);
    if (col.Ok())
    {
        float r = (float)(col.Red()/256.0);
        float g = (float)(col.Green()/256.0);
        float b = (float)(col.Blue()/256.0);
        glColor3f( r, g, b);
    }
}

void wxGLContext::SetupPixelFormat()
{
}

void wxGLContext::SetupPalette( const wxPalette& WXUNUSED(palette) )
{
}

wxPalette wxGLContext::CreateDefaultPalette()
{
    return wxNullPalette;
}

//-----------------------------------------------------------------------------
// "realize" from m_wxwindow
//-----------------------------------------------------------------------------

extern "C" {
static gint
gtk_glwindow_realized_callback( GtkWidget *WXUNUSED(widget), wxGLCanvas *win )
{
    if ( !win->m_glContext )
    {
        wxGLContext *share = win->m_sharedContext;
        if ( !share && win->m_sharedContextOf )
            share = win->m_sharedContextOf->GetContext();

        win->m_glContext = new wxGLContext( TRUE, win, wxNullPalette, share );
    }

    return FALSE;
}
}

//-----------------------------------------------------------------------------
// "map" from m_wxwindow
//-----------------------------------------------------------------------------

extern "C" {
static gint
gtk_glwindow_map_callback( GtkWidget * WXUNUSED(widget), wxGLCanvas *win )
{
    if (win->m_glContext/* && win->m_exposed*/)
    {
        wxPaintEvent event( win->GetId() );
        event.SetEventObject( win );
        win->GetEventHandler()->ProcessEvent( event );

        win->m_exposed = false;
        win->GetUpdateRegion().Clear();
    }

    return FALSE;
}
}

//-----------------------------------------------------------------------------
// "expose_event" of m_wxwindow
//-----------------------------------------------------------------------------

extern "C" {
static void
gtk_glwindow_expose_callback( GtkWidget *WXUNUSED(widget), GdkEventExpose *gdk_event, wxGLCanvas *win )
{
    // don't need to install idle handler, its done from "event" signal

    win->m_exposed = true;

    win->GetUpdateRegion().Union( gdk_event->area.x,
                                  gdk_event->area.y,
                                  gdk_event->area.width,
                                  gdk_event->area.height );
}
}

//-----------------------------------------------------------------------------
// "size_allocate" of m_wxwindow
//-----------------------------------------------------------------------------

extern "C" {
static void
gtk_glcanvas_size_callback( GtkWidget *WXUNUSED(widget), GtkAllocation* alloc, wxGLCanvas *win )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    if (!win->m_hasVMT)
        return;

    wxSizeEvent event( wxSize(win->m_width,win->m_height), win->GetId() );
    event.SetEventObject( win );
    win->GetEventHandler()->ProcessEvent( event );
}
}

//---------------------------------------------------------------------------
// wxGlCanvas
//---------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGLCanvas, wxWindow)

BEGIN_EVENT_TABLE(wxGLCanvas, wxWindow)
    EVT_SIZE(wxGLCanvas::OnSize)
END_EVENT_TABLE()

wxGLCanvas::wxGLCanvas( wxWindow *parent, wxWindowID id,
                        const wxPoint& pos, const wxSize& size,
                        long style, const wxString& name,
                        int *attribList,
                        const wxPalette& palette )
{
    Create( parent, NULL, NULL, id, pos, size, style, name, attribList, palette );
}

wxGLCanvas::wxGLCanvas( wxWindow *parent,
                        const wxGLContext *shared,
                        wxWindowID id,
                        const wxPoint& pos, const wxSize& size,
                        long style, const wxString& name,
                        int *attribList,
                        const wxPalette& palette )
{
    Create( parent, shared, NULL, id, pos, size, style, name, attribList, palette );
}

wxGLCanvas::wxGLCanvas( wxWindow *parent,
                        const wxGLCanvas *shared,
                        wxWindowID id,
                        const wxPoint& pos, const wxSize& size,
                        long style, const wxString& name,
                        int *attribList,
                        const wxPalette& palette )
{
    Create( parent, NULL, shared, id, pos, size, style, name, attribList, palette );
}

bool wxGLCanvas::Create( wxWindow *parent,
                         const wxGLContext *shared,
                         const wxGLCanvas *shared_context_of,
                         wxWindowID id,
                         const wxPoint& pos, const wxSize& size,
                         long style, const wxString& name,
                         int *attribList,
                         const wxPalette& palette)
{
    m_sharedContext = (wxGLContext*)shared;  // const_cast
    m_sharedContextOf = (wxGLCanvas*)shared_context_of;  // const_cast
    m_glContext = (wxGLContext*) NULL;

    m_exposed = false;
    m_noExpose = true;
    m_nativeSizeEvent = true;
    m_fbc = NULL;
    m_vi = NULL;

    // to be sure the glx version is known
    wxGLCanvas::QueryGLXVersion();

    if (wxGLCanvas::GetGLXVersion() >= 13)
    {
        // GLX >= 1.3 uses a GLXFBConfig
        GLXFBConfig * fbc = NULL;
        if (wxTheApp->m_glFBCInfo != NULL)
        {
            fbc = (GLXFBConfig *) wxTheApp->m_glFBCInfo;
            m_canFreeFBC = false; // owned by wxTheApp - don't free upon destruction
        }
        else
        {
            fbc = (GLXFBConfig *) wxGLCanvas::ChooseGLFBC(attribList);
            m_canFreeFBC = true;
        }
        m_fbc = fbc;  // save for later use
        wxCHECK_MSG( m_fbc, false, _T("required FBConfig couldn't be found") );
    }

    XVisualInfo *vi = NULL;
    if (wxTheApp->m_glVisualInfo != NULL)
    {
        vi = (XVisualInfo *)wxTheApp->m_glVisualInfo;
        m_canFreeVi = false; // owned by wxTheApp - don't free upon destruction
    }
    else
    {
        if (wxGLCanvas::GetGLXVersion() >= 13)
        // GLX >= 1.3
            vi = glXGetVisualFromFBConfig(GDK_DISPLAY(), m_fbc[0]);
        else
            // GLX <= 1.2
            vi = (XVisualInfo *) ChooseGLVisual(attribList);

        m_canFreeVi = true;
    }

    m_vi = vi;  // save for later use

    wxCHECK_MSG( m_vi, false, _T("required visual couldn't be found") );
    GdkVisual *visual;
    GdkColormap *colormap;

    // MR: This needs a fix for lower gtk+ versions too. Might need to rethink logic (FIXME)
#if defined(__WXGTK20__) && GTK_CHECK_VERSION(2,2,0)
    if (!gtk_check_version(2,2,0))
    {
        wxWindow::Create( parent, id, pos, size, style, name );

        m_glWidget = m_wxwindow;

        GdkScreen *screen = gtk_widget_get_screen( m_glWidget );
        colormap = gdk_screen_get_default_colormap(screen);
        visual = gdk_colormap_get_visual(colormap);

        if (GDK_VISUAL_XVISUAL(visual)->visualid != vi->visualid)
        {
            visual = gdk_x11_screen_lookup_visual( screen, vi->visualid );
            colormap = gdk_colormap_new(visual, FALSE);
        }

        gtk_widget_set_colormap( m_glWidget, colormap );
    }
    else
#endif
    {
        visual = gdkx_visual_get( vi->visualid );
        colormap = gdk_colormap_new( visual, TRUE );

        gtk_widget_push_colormap( colormap );

        wxWindow::Create( parent, id, pos, size, style, name );
        m_glWidget = m_wxwindow;
    }

    gtk_widget_set_double_buffered( m_glWidget, FALSE );

    g_signal_connect (m_wxwindow, "realize",
                      G_CALLBACK (gtk_glwindow_realized_callback),
                      this);
    g_signal_connect (m_wxwindow, "map",
                      G_CALLBACK (gtk_glwindow_map_callback),
                      this);
    g_signal_connect (m_wxwindow, "expose_event",
                      G_CALLBACK (gtk_glwindow_expose_callback),
                      this);
    g_signal_connect (m_widget, "size_allocate",
                      G_CALLBACK (gtk_glcanvas_size_callback),
                      this);

    if (gtk_check_version(2,2,0) != NULL)
    {
        gtk_widget_pop_colormap();
    }

    // if our parent window is already visible, we had been realized before we
    // connected to the "realize" signal and hence our m_glContext hasn't been
    // initialized yet and we have to do it now
    if (GTK_WIDGET_REALIZED(m_wxwindow))
        gtk_glwindow_realized_callback( m_wxwindow, this );

    if (GTK_WIDGET_MAPPED(m_wxwindow))
        gtk_glwindow_map_callback( m_wxwindow, this );

    return true;
}

wxGLCanvas::~wxGLCanvas()
{
    GLXFBConfig * fbc = (GLXFBConfig *) m_fbc;
    if (fbc && m_canFreeFBC)
        XFree( fbc );

    XVisualInfo *vi = (XVisualInfo *) m_vi;
    if (vi && m_canFreeVi)
        XFree( vi );

    delete m_glContext;
}

void* wxGLCanvas::ChooseGLVisual(int *attribList)
{
    int data[512];
    GetGLAttribListFromWX( attribList, data );
    attribList = (int*) data;

    Display *dpy = GDK_DISPLAY();

    return glXChooseVisual( dpy, DefaultScreen(dpy), attribList );
}

void* wxGLCanvas::ChooseGLFBC(int *attribList)
{
    int data[512];
    GetGLAttribListFromWX( attribList, data );
    attribList = (int*) data;

    int returned;
    return glXChooseFBConfig( GDK_DISPLAY(), DefaultScreen(GDK_DISPLAY()),
                              attribList, &returned );
}


void wxGLCanvas::GetGLAttribListFromWX(int *wx_attribList, int *gl_attribList )
{
    if (!wx_attribList)
    {
        if (wxGLCanvas::GetGLXVersion() >= 13)
        // leave GLX >= 1.3 choose the default attributes
            gl_attribList[0] = 0;
        else
        {
            int i = 0;
            // default settings if attriblist = 0
            gl_attribList[i++] = GLX_RGBA;
            gl_attribList[i++] = GLX_DOUBLEBUFFER;
            gl_attribList[i++] = GLX_DEPTH_SIZE;   gl_attribList[i++] = 1;
            gl_attribList[i++] = GLX_RED_SIZE;     gl_attribList[i++] = 1;
            gl_attribList[i++] = GLX_GREEN_SIZE;   gl_attribList[i++] = 1;
            gl_attribList[i++] = GLX_BLUE_SIZE;    gl_attribList[i++] = 1;
            gl_attribList[i++] = GLX_ALPHA_SIZE;   gl_attribList[i++] = 0;
            gl_attribList[i++] = None;
        }
    }
    else
    {
        int arg=0, p=0;
        while( (wx_attribList[arg]!=0) && (p<510) )
        {
            switch( wx_attribList[arg++] )
            {
                case WX_GL_RGBA:
                    if (wxGLCanvas::GetGLXVersion() <= 12)
                        // for GLX >= 1.3, GLX_RGBA is useless (setting this flags will crash on most opengl implm)
                        gl_attribList[p++] = GLX_RGBA;
                    break;
                case WX_GL_BUFFER_SIZE:
                    gl_attribList[p++] = GLX_BUFFER_SIZE;
                    gl_attribList[p++] = wx_attribList[arg++];
                    break;
                case WX_GL_LEVEL:
                    gl_attribList[p++] = GLX_LEVEL;
                    gl_attribList[p++] = wx_attribList[arg++];
                    break;
                case WX_GL_DOUBLEBUFFER:
                    if (wxGLCanvas::GetGLXVersion() <= 12)
                        gl_attribList[p++] = GLX_DOUBLEBUFFER;
                    else
                        // for GLX >= 1.3, GLX_DOUBLEBUFFER format is different (1 <=> True)
                        // it seems this flag is useless for some hardware opengl implementation.
                        // but for Mesa 6.2.1, this flag is used so don't ignore it.
                        gl_attribList[p++] = GLX_DOUBLEBUFFER;
                    gl_attribList[p++] = 1;
                    break;
                case WX_GL_STEREO:
                    gl_attribList[p++] = GLX_STEREO;
                    break;
                case WX_GL_AUX_BUFFERS:
                    gl_attribList[p++] = GLX_AUX_BUFFERS;
                    gl_attribList[p++] = wx_attribList[arg++];
                    break;
                case WX_GL_MIN_RED:
                    gl_attribList[p++] = GLX_RED_SIZE;
                    gl_attribList[p++] = wx_attribList[arg++];
                    break;
                case WX_GL_MIN_GREEN:
                    gl_attribList[p++] = GLX_GREEN_SIZE;
                    gl_attribList[p++] = wx_attribList[arg++];
                    break;
                case WX_GL_MIN_BLUE:
                    gl_attribList[p++] = GLX_BLUE_SIZE;
                    gl_attribList[p++] = wx_attribList[arg++];
                    break;
                case WX_GL_MIN_ALPHA:
                    gl_attribList[p++] = GLX_ALPHA_SIZE;
                    gl_attribList[p++] = wx_attribList[arg++];
                    break;
                case WX_GL_DEPTH_SIZE:
                    gl_attribList[p++] = GLX_DEPTH_SIZE;
                    gl_attribList[p++] = wx_attribList[arg++];
                    break;
                case WX_GL_STENCIL_SIZE:
                    gl_attribList[p++] = GLX_STENCIL_SIZE;
                    gl_attribList[p++] = wx_attribList[arg++];
                    break;
                case WX_GL_MIN_ACCUM_RED:
                    gl_attribList[p++] = GLX_ACCUM_RED_SIZE;
                    gl_attribList[p++] = wx_attribList[arg++];
                    break;
                case WX_GL_MIN_ACCUM_GREEN:
                    gl_attribList[p++] = GLX_ACCUM_GREEN_SIZE;
                    gl_attribList[p++] = wx_attribList[arg++];
                    break;
                case WX_GL_MIN_ACCUM_BLUE:
                    gl_attribList[p++] = GLX_ACCUM_BLUE_SIZE;
                    gl_attribList[p++] = wx_attribList[arg++];
                    break;
                case WX_GL_MIN_ACCUM_ALPHA:
                    gl_attribList[p++] = GLX_ACCUM_ALPHA_SIZE;
                    gl_attribList[p++] = wx_attribList[arg++];
                    break;
                default:
                    break;
            }
        }

        gl_attribList[p] = 0;
    }
}

void wxGLCanvas::QueryGLXVersion()
{
    if (m_glxVersion == 0)
    {
        // check the GLX version
        int glxMajorVer, glxMinorVer;
        bool ok = glXQueryVersion(GDK_DISPLAY(), &glxMajorVer, &glxMinorVer);
        wxASSERT_MSG( ok, _T("GLX version not found") );
        if (!ok)
            m_glxVersion = 10; // 1.0 by default
        else
            m_glxVersion = glxMajorVer*10 + glxMinorVer;
    }
}

int wxGLCanvas::GetGLXVersion()
{
    wxASSERT_MSG( m_glxVersion>0, _T("GLX version has not been initialized with wxGLCanvas::QueryGLXVersion()") );
    return m_glxVersion;
}


void wxGLCanvas::SwapBuffers()
{
    if (m_glContext)
        m_glContext->SwapBuffers();
}

void wxGLCanvas::OnSize(wxSizeEvent& WXUNUSED(event))
{
}

void wxGLCanvas::SetCurrent()
{
    if (m_glContext)
        m_glContext->SetCurrent();
}

void wxGLCanvas::SetColour( const wxChar *colour )
{
    if (m_glContext)
        m_glContext->SetColour( colour );
}

void wxGLCanvas::OnInternalIdle()
{
    if (m_glContext && m_exposed)
    {
        wxPaintEvent event( GetId() );
        event.SetEventObject( this );
        GetEventHandler()->ProcessEvent( event );

        m_exposed = false;
        GetUpdateRegion().Clear();
    }

    wxWindow::OnInternalIdle();
}



//---------------------------------------------------------------------------
// wxGLApp
//---------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGLApp, wxApp)

wxGLApp::~wxGLApp()
{
    if (m_glFBCInfo)
        XFree(m_glFBCInfo);
    if (m_glVisualInfo)
        XFree(m_glVisualInfo);
}

bool wxGLApp::InitGLVisual(int *attribList)
{
    wxGLCanvas::QueryGLXVersion();

    if (wxGLCanvas::GetGLXVersion() >= 13)
    {
        // GLX >= 1.3
        if (m_glFBCInfo)
            XFree(m_glFBCInfo);
        m_glFBCInfo = wxGLCanvas::ChooseGLFBC(attribList);

        if (m_glFBCInfo)
        {
            if (m_glVisualInfo)
                XFree(m_glVisualInfo);
            m_glVisualInfo = glXGetVisualFromFBConfig(GDK_DISPLAY(), ((GLXFBConfig *)m_glFBCInfo)[0]);
        }
        return (m_glFBCInfo != NULL) && (m_glVisualInfo != NULL);
    }
    else
    {
        // GLX <= 1.2
        if (m_glVisualInfo)
            XFree(m_glVisualInfo);
        m_glVisualInfo = wxGLCanvas::ChooseGLVisual(attribList);
        return m_glVisualInfo != NULL;
    }
}

#endif
    // wxUSE_GLCANVAS
