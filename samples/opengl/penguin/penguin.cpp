/////////////////////////////////////////////////////////////////////////////
// Name:        penguin.cpp
// Purpose:     wxGLCanvas demo program
// Author:      Robert Roebling
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:   	wxWindows licence
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
#error Please set wxUSE_GLCANVAS to 1 in setup.h.
#endif

#include "penguin.h"
#include <GL/glu.h>

#define VIEW_ASPECT 1.3

/* `Main program' equivalent, creating windows and returning main app frame */
bool MyApp::OnInit(void)
{

  /* Create the main frame window */
  MyFrame *frame = new MyFrame(NULL, "wxWindows OpenGL Demo", wxPoint(50, 50), wxSize(400, 300));

  /* Make a menubar */
  wxMenu *fileMenu = new wxMenu;

  fileMenu->Append(wxID_EXIT, "E&xit");
  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(fileMenu, "&File");
  frame->SetMenuBar(menuBar);

  frame->m_canvas = new TestGLCanvas(frame, -1, wxPoint(0, 0), wxSize(200, 200));

  /* Load file wiht mesh data */
  frame->m_canvas->LoadLWO( "penguin.lwo" );

  /* Show the frame */
  frame->Show(TRUE);
  
  return TRUE;
}

IMPLEMENT_APP(MyApp)

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MyFrame::OnExit)
END_EVENT_TABLE()

/* My frame constructor */
MyFrame::MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
    const wxSize& size, long style):
  wxFrame(frame, -1, title, pos, size, style)
{
    m_canvas = NULL;
}

/* Intercept menu commands */
void MyFrame::OnExit(wxCommandEvent& event)
{
    Destroy();
}

BEGIN_EVENT_TABLE(TestGLCanvas, wxGLCanvas)
    EVT_SIZE(TestGLCanvas::OnSize)
    EVT_PAINT(TestGLCanvas::OnPaint)
    EVT_ERASE_BACKGROUND(TestGLCanvas::OnEraseBackground)
    EVT_MOUSE_EVENTS(TestGLCanvas::OnMouse)
END_EVENT_TABLE()

TestGLCanvas::TestGLCanvas(wxWindow *parent, wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name):
  wxGLCanvas(parent, id, pos, size, style, name)
{
   block = FALSE;
}

TestGLCanvas::~TestGLCanvas(void)
{
    /* destroy mesh */
    lw_object_free(info.lwobject);
}

void TestGLCanvas::OnPaint( wxPaintEvent& event )
{
    /* must always be here */
    wxPaintDC dc(this);

#ifndef __WXMOTIF__
    if (!GetContext()) return;
#endif

    SetCurrent();
    
    /* initialize OpenGL */
    if (info.do_init == TRUE) 
    {
        InitGL();
        info.do_init = FALSE;
    }
  
    /* view */
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( info.zoom, VIEW_ASPECT, 1, 100 );
    glMatrixMode( GL_MODELVIEW );

    /* clear */
    glClearColor( .3, .4, .6, 1 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    /* transformations */
    GLfloat m[4][4];
    glLoadIdentity();
    glTranslatef( 0, 0, -30 );
    build_rotmatrix( m,info.quat );
    glMultMatrixf( &m[0][0] );

    /* draw object */
    lw_object_show( info.lwobject );
    
    /* flush */
    glFlush();

    /* swap */
    SwapBuffers();
}

void TestGLCanvas::OnSize(wxSizeEvent& event)
{
    int width, height;
    GetClientSize(& width, & height);
    
#ifndef __WXMOTIF__
    if (GetContext())
#endif
    {
        SetCurrent();
        glViewport(0, 0, width, height);
    }
}

void TestGLCanvas::OnEraseBackground(wxEraseEvent& event)
{
    /* Do nothing, to avoid flashing on MSW */
}

void TestGLCanvas::LoadLWO(const wxString &filename)
{
    /* test if lightwave object */
    if (!lw_is_lwobject(filename)) return;
  
    /* read lightwave object */
    lwObject *lwobject = lw_object_read(filename);
    
    /* scale */
    lw_object_scale(lwobject, 10.0 / lw_object_radius(lwobject));
    
    /* set up mesh info */
    info.do_init = TRUE;
    info.lwobject = lwobject;
    info.beginx = 0;
    info.beginy = 0;
    info.zoom   = 45;
    trackball( info.quat, 0.0, 0.0, 0.0, 0.0 );
}

void TestGLCanvas::OnMouse( wxMouseEvent& event )
{
    wxSize sz(GetClientSize());
    if (event.Dragging())
    {
        /* drag in progress, simulate trackball */
        float spin_quat[4];
        trackball(spin_quat,
	      (2.0*info.beginx -       sz.x) / sz.x,
	      (     sz.y - 2.0*info.beginy) / sz.y,
	      (     2.0*event.GetX() - sz.x) / sz.x,
	      (    sz.y - 2.0*event.GetY()) / sz.y);
	      
        add_quats( spin_quat, info.quat, info.quat );
	
        /* orientation has changed, redraw mesh */
  	Refresh(FALSE);
    }
    
    info.beginx = event.GetX();
    info.beginy = event.GetY();
}

void TestGLCanvas::InitGL(void)
{
    GLfloat light0_pos[4]   = { -50.0, 50.0, 0.0, 0.0 };
    GLfloat light0_color[4] = { .6, .6, .6, 1.0 }; /* white light */
    GLfloat light1_pos[4]   = {  50.0, 50.0, 0.0, 0.0 };
    GLfloat light1_color[4] = { .4, .4, 1, 1.0 };  /* cold blue light */

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
    
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);  
}


