/////////////////////////////////////////////////////////////////////////////
// Name:        isosurf.cpp
// Purpose:     wxGLCanvas demo program
// Author:      Brian Paul (original gltk version), Wolfram Gloger
// Modified by: Julian Smart
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
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

#include "wx/timer.h"
#include "wx/glcanvas.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include "isosurf.h"

// The following part is taken largely unchanged from the original C Version

#include <math.h>

GLboolean speed_test = GL_FALSE;
GLboolean use_vertex_arrays = GL_FALSE;

GLboolean doubleBuffer = GL_TRUE;

GLboolean smooth = GL_TRUE;
GLboolean lighting = GL_TRUE;


#define MAXVERTS 10000

static GLfloat verts[MAXVERTS][3];
static GLfloat norms[MAXVERTS][3];
static GLint numverts;

static GLfloat xrot;
static GLfloat yrot;


static void read_surface( char *filename )
{
   FILE *f;

   f = fopen(filename,"r");
   if (!f) {
      wxString msg("Couldn't read ");
      msg += filename;
      wxMessageBox(msg);
      return;
   }

   numverts = 0;
   while (!feof(f) && numverts<MAXVERTS) {
      fscanf( f, "%f %f %f  %f %f %f",
	      &verts[numverts][0], &verts[numverts][1], &verts[numverts][2],
	      &norms[numverts][0], &norms[numverts][1], &norms[numverts][2] );
      numverts++;
   }
   numverts--;

   printf("%d vertices, %d triangles\n", numverts, numverts-2);
   fclose(f);
}


static void draw_surface( void )
{
   GLint i;

#ifdef GL_EXT_vertex_array
   if (use_vertex_arrays) {
      glDrawArraysEXT( GL_TRIANGLE_STRIP, 0, numverts );
   }
   else {
#endif
      glBegin( GL_TRIANGLE_STRIP );
      for (i=0;i<numverts;i++) {
         glNormal3fv( norms[i] );
         glVertex3fv( verts[i] );
      }
      glEnd();
#ifdef GL_EXT_vertex_array
   }
#endif
}


static void draw1(void)
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glPushMatrix();
    glRotatef( yrot, 0.0, 1.0, 0.0 );
    glRotatef( xrot, 1.0, 0.0, 0.0 );

    draw_surface();

    glPopMatrix();

    glFlush();
}


static void InitMaterials(void)
{
    static float ambient[] = {0.1, 0.1, 0.1, 1.0};
    static float diffuse[] = {0.5, 1.0, 1.0, 1.0};
    static float position0[] = {0.0, 0.0, 20.0, 0.0};
    static float position1[] = {0.0, 0.0, -20.0, 0.0};
    static float front_mat_shininess[] = {60.0};
    static float front_mat_specular[] = {0.2, 0.2, 0.2, 1.0};
    static float front_mat_diffuse[] = {0.5, 0.28, 0.38, 1.0};
    /*
    static float back_mat_shininess[] = {60.0};
    static float back_mat_specular[] = {0.5, 0.5, 0.2, 1.0};
    static float back_mat_diffuse[] = {1.0, 1.0, 0.2, 1.0};
    */
    static float lmodel_ambient[] = {1.0, 1.0, 1.0, 1.0};
    static float lmodel_twoside[] = {GL_FALSE};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, position0);
    glEnable(GL_LIGHT0);
    
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, position1);
    glEnable(GL_LIGHT1);
    
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);
    glEnable(GL_LIGHTING);

    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, front_mat_shininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, front_mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, front_mat_diffuse);
}


static void Init(void)
{
   glClearColor(0.0, 0.0, 0.0, 0.0);

   glShadeModel(GL_SMOOTH);
   glEnable(GL_DEPTH_TEST);

   InitMaterials();

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustum( -1.0, 1.0, -1.0, 1.0, 5, 25 );

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef( 0.0, 0.0, -6.0 );

#ifdef GL_EXT_vertex_array
   if (use_vertex_arrays) {
      glVertexPointerEXT( 3, GL_FLOAT, 0, numverts, verts );
      glNormalPointerEXT( GL_FLOAT, 0, numverts, norms );
      glEnable( GL_VERTEX_ARRAY_EXT );
      glEnable( GL_NORMAL_ARRAY_EXT );
   }
#endif
}


static void Reshape(int width, int height)
{
  glViewport(0, 0, (GLint)width, (GLint)height);
}


static GLenum Args(int argc, char **argv)
{
   GLint i;

   for (i = 1; i < argc; i++) {
      if (strcmp(argv[i], "-sb") == 0) {
         doubleBuffer = GL_FALSE;
      }
      else if (strcmp(argv[i], "-db") == 0) {
         doubleBuffer = GL_TRUE;
      }
      else if (strcmp(argv[i], "-speed") == 0) {
         speed_test = GL_TRUE;
         doubleBuffer = GL_TRUE;
      }
      else if (strcmp(argv[i], "-va") == 0) {
         use_vertex_arrays = GL_TRUE;
      }
      else {
         wxString msg("Bad option: ");
         msg += argv[i];
         wxMessageBox(msg);
         return GL_FALSE;
      }
   }

   return GL_TRUE;
}

// The following part was written for wxWindows 1.66
MyFrame *frame = NULL;

IMPLEMENT_APP(MyApp)

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit(void)
{
  Args(argc, argv);

  // Create the main frame window
  frame = new MyFrame(NULL, "Isosurf GL Sample", wxPoint(50, 50), wxSize(200, 200));

  // Give it an icon
  frame->SetIcon(wxIcon("mondrian"));

  // Make a menubar
  wxMenu *fileMenu = new wxMenu;

  fileMenu->Append(wxID_EXIT, "E&xit");
  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(fileMenu, "&File");
  frame->SetMenuBar(menuBar);

  // Make a TestGLCanvas

  // JACS
#ifdef __WXMSW__
  int *gl_attrib = NULL;
#else
  int gl_attrib[20] = { GLX_RGBA, GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1,
			GLX_BLUE_SIZE, 1, GLX_DEPTH_SIZE, 1,
			GLX_DOUBLEBUFFER, None };
#endif

  if(!doubleBuffer)
   {
      printf("don't have double buffer, disabling\n");
#ifdef __WXGTK__
      gl_attrib[9] = None;
#endif
      doubleBuffer = GL_FALSE;
  }
  frame->m_canvas = new TestGLCanvas(frame, -1, wxPoint(0, 0), wxSize(200, 200), 0, "TestGLCanvas",
				   gl_attrib);

  // Show the frame
  frame->Show(TRUE);

  frame->m_canvas->SetCurrent();
  read_surface( "isosurf.dat" );

  Init();

  return TRUE;
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MyFrame::OnExit)
END_EVENT_TABLE()

// My frame constructor
MyFrame::MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
    const wxSize& size, long style):
  wxFrame(frame, -1, title, pos, size, style)
{
    m_canvas = NULL;
}

// Intercept menu commands
void MyFrame::OnExit(wxCommandEvent& event)
{
    Destroy();
}

/*
 * TestGLCanvas implementation
 */

BEGIN_EVENT_TABLE(TestGLCanvas, wxGLCanvas)
    EVT_SIZE(TestGLCanvas::OnSize)
    EVT_PAINT(TestGLCanvas::OnPaint)
    EVT_CHAR(TestGLCanvas::OnChar)
    EVT_MOUSE_EVENTS(TestGLCanvas::OnMouseEvent)
    EVT_ERASE_BACKGROUND(TestGLCanvas::OnEraseBackground)
END_EVENT_TABLE()

TestGLCanvas::TestGLCanvas(wxWindow *parent, wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name, int* gl_attrib):
  wxGLCanvas(parent, id, pos, size, style, name, gl_attrib)
{
   parent->Show(TRUE);
   SetCurrent();
   /* Make sure server supports the vertex array extension */
   char* extensions = (char *) glGetString( GL_EXTENSIONS );
   if (!extensions || !strstr( extensions, "GL_EXT_vertex_array" )) {
      use_vertex_arrays = GL_FALSE;
   }
}


TestGLCanvas::~TestGLCanvas(void)
{
}

void TestGLCanvas::OnPaint( wxPaintEvent& event )
{
    // This is a dummy, to avoid an endless succession of paint messages.
    // OnPaint handlers must always create a wxPaintDC.
    wxPaintDC dc(this);

#ifndef __WXMOTIF__
    if (!GetContext()) return;
#endif

    draw1();
    SwapBuffers();
}

void TestGLCanvas::OnSize(wxSizeEvent& event)
{
#ifndef __WXMOTIF__
    if (!GetContext()) return;
#endif

    SetCurrent();
    int width, height;
    GetClientSize(& width, & height);
    Reshape(width, height);
}

void TestGLCanvas::OnChar(wxKeyEvent& event)
{
    switch(event.KeyCode()) {
    case WXK_ESCAPE:
	exit(0);
    case WXK_LEFT:
	yrot -= 15.0;
	break;
    case WXK_RIGHT:
	yrot += 15.0;
	break;
    case WXK_UP:
	xrot += 15.0;
	break;
    case WXK_DOWN:
	xrot -= 15.0;
	break;
    case 's': case 'S':
	smooth = !smooth;
	if (smooth) {
	    glShadeModel(GL_SMOOTH);
	} else {
	    glShadeModel(GL_FLAT);
	}
	break;
    case 'l': case 'L':
	lighting = !lighting;
	if (lighting) {
	    glEnable(GL_LIGHTING);
	} else {
	    glDisable(GL_LIGHTING);
	}
	break;
     default:
      {
        event.Skip();
	return;
      }
    }

    Refresh(FALSE);
}

void TestGLCanvas::OnMouseEvent(wxMouseEvent& event)
{
    static int dragging = 0;
    static float last_x, last_y;

    //printf("%f %f %d\n", event.GetX(), event.GetY(), (int)event.LeftIsDown());
    if(event.LeftIsDown()) {
	if(!dragging) {
	    dragging = 1;
	} else {
	    yrot += (event.GetX() - last_x)*1.0;
	    xrot += (event.GetY() - last_y)*1.0;
	    Refresh(FALSE);
	}
	last_x = event.GetX();
	last_y = event.GetY();
    } else
	dragging = 0;
}

void TestGLCanvas::OnEraseBackground(wxEraseEvent& event)
{
    // Do nothing, to avoid flashing.
}

