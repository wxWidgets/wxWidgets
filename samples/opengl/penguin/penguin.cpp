/////////////////////////////////////////////////////////////////////////////
// Name:        penguin.cpp
// Purpose:     wxGLCanvas demo program
// Author:      Robert Roebling
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_GLCANVAS
    #error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

#include "penguin.h"
#ifdef __WXMAC__
#  ifdef __DARWIN__
#    include <OpenGL/glu.h>
#  else
#    include <glu.h>
#  endif
#else
#  include <GL/glu.h>
#endif

#define VIEW_ASPECT 1.3

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit()
{

    // Create the main frame window
    MyFrame *frame = new MyFrame(NULL, wxT("wxWidgets OpenGL Penguin Sample"),
        wxDefaultPosition, wxDefaultSize);

    /* Make a menubar */
    wxMenu *fileMenu = new wxMenu;

    fileMenu->Append(wxID_EXIT, wxT("E&xit"));
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(fileMenu, wxT("&File"));
    frame->SetMenuBar(menuBar);

    frame->SetCanvas( new TestGLCanvas(frame, wxID_ANY, wxDefaultPosition,
        wxSize(200, 200), wxSUNKEN_BORDER) );

    /* Load file wiht mesh data */
    frame->GetCanvas()->LoadLWO( wxT("penguin.lwo") );

    /* Show the frame */
    frame->Show(true);

    return true;
}

IMPLEMENT_APP(MyApp)

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MyFrame::OnExit)
END_EVENT_TABLE()

/* My frame constructor */
MyFrame::MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
    const wxSize& size, long style)
    : wxFrame(frame, wxID_ANY, title, pos, size, style)
{
    m_canvas = NULL;
}

/* Intercept menu commands */
void MyFrame::OnExit( wxCommandEvent& WXUNUSED(event) )
{
    // true is to force the frame to close
    Close(true);
}

BEGIN_EVENT_TABLE(TestGLCanvas, wxGLCanvas)
    EVT_SIZE(TestGLCanvas::OnSize)
    EVT_PAINT(TestGLCanvas::OnPaint)
    EVT_ERASE_BACKGROUND(TestGLCanvas::OnEraseBackground)
    EVT_MOUSE_EVENTS(TestGLCanvas::OnMouse)
END_EVENT_TABLE()

TestGLCanvas::TestGLCanvas(wxWindow *parent, wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name)
    : wxGLCanvas(parent, id, pos, size, style, name)
{
    block = false;
}

TestGLCanvas::~TestGLCanvas()
{
    /* destroy mesh */
    lw_object_free(info.lwobject);
}

void TestGLCanvas::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    /* must always be here */
    wxPaintDC dc(this);

#ifndef __WXMOTIF__
    if (!GetContext()) return;
#endif

    SetCurrent();

    // Initialize OpenGL
    if (info.do_init)
    {
        InitGL();
        info.do_init = false;
    }

    // View
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( info.zoom, VIEW_ASPECT, 1.0, 100.0 );
    glMatrixMode( GL_MODELVIEW );

    // Clear
    glClearColor( 0.3f, 0.4f, 0.6f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Transformations
    GLfloat m[4][4];
    glLoadIdentity();
    glTranslatef( 0.0f, 0.0f, -30.0f );
    build_rotmatrix( m,info.quat );
    glMultMatrixf( &m[0][0] );

    // Draw object
    lw_object_show( info.lwobject );

    // Flush
    glFlush();

    // Swap
    SwapBuffers();
}

void TestGLCanvas::OnSize(wxSizeEvent& event)
{
    // this is also necessary to update the context on some platforms
    wxGLCanvas::OnSize(event);

    // set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
    int w, h;
    GetClientSize(&w, &h);
#ifndef __WXMOTIF__
    if ( GetContext() )
#endif
    {
        SetCurrent();
        glViewport(0, 0, (GLint) w, (GLint) h);
    }
}

void TestGLCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    /* Do nothing, to avoid flashing on MSW */
}

void TestGLCanvas::LoadLWO(const wxString &filename)
{
    /* test if lightwave object */
    if (!lw_is_lwobject(filename.mb_str())) return;

    /* read lightwave object */
    lwObject *lwobject = lw_object_read(filename.mb_str());

    /* scale */
    lw_object_scale(lwobject, 10.0 / lw_object_radius(lwobject));

    /* set up mesh info */
    info.do_init = true;
    info.lwobject = lwobject;
    info.beginx = 0.0f;
    info.beginy = 0.0f;
    info.zoom   = 45.0f;
    trackball( info.quat, 0.0f, 0.0f, 0.0f, 0.0f );
}

void TestGLCanvas::OnMouse( wxMouseEvent& event )
{

    if ( event.Dragging() )
    {
        wxSize sz( GetClientSize() );

        /* drag in progress, simulate trackball */
        float spin_quat[4];
        trackball(spin_quat,
            (2.0*info.beginx -       sz.x) / sz.x,
            (     sz.y - 2.0*info.beginy) / sz.y,
            (     2.0*event.GetX() - sz.x) / sz.x,
            (    sz.y - 2.0*event.GetY()) / sz.y);

        add_quats( spin_quat, info.quat, info.quat );

        /* orientation has changed, redraw mesh */
        Refresh(false);
    }

    info.beginx = event.GetX();
    info.beginy = event.GetY();
}

void TestGLCanvas::InitGL()
{
    static const GLfloat light0_pos[4]   = { -50.0f, 50.0f, 0.0f, 0.0f };

    // white light
    static const GLfloat light0_color[4] = { 0.6f, 0.6f, 0.6f, 1.0f };

    static const GLfloat light1_pos[4]   = {  50.0f, 50.0f, 0.0f, 0.0f };

    // cold blue light
    static const GLfloat light1_color[4] = { 0.4f, 0.4f, 1.0f, 1.0f };

    /* remove back faces */
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    /* speedups */
    glEnable(GL_DITHER);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);

    /* light */
    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light0_color);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  light1_color);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHTING);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
}

