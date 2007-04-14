/////////////////////////////////////////////////////////////////////////////
// Name:        isosurf.cpp
// Purpose:     wxGLCanvas demo program
// Author:      Brian Paul (original gltk version), Wolfram Gloger
// Modified by: Julian Smart
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

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

#include "wx/timer.h"
#include "wx/glcanvas.h"
#include "wx/math.h"

#if defined(__WXMAC__) || defined(__WXCOCOA__)
#   ifdef __DARWIN__
#       include <OpenGL/gl.h>
#       include <OpenGL/glu.h>
#   else
#       include <gl.h>
#       include <glu.h>
#   endif
#else
#   include <GL/gl.h>
#   include <GL/glu.h>
#endif

// disabled because this has apparently changed in OpenGL 1.2, so doesn't link
// correctly if this is on...
#ifdef GL_EXT_vertex_array
#undef GL_EXT_vertex_array
#endif

#include "isosurf.h"

#include "../../sample.xpm"

// The following part is taken largely unchanged from the original C Version

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


static void read_surface( const wxChar *filename )
{
    FILE *f = wxFopen(filename,_T("r"));
    if (!f)
    {
        wxString msg = _T("Couldn't read ");
        msg += filename;
        wxMessageBox(msg);
        return;
    }

    numverts = 0;
    while (!feof(f) && numverts<MAXVERTS)
    {
        fscanf( f, "%f %f %f  %f %f %f",
            &verts[numverts][0], &verts[numverts][1], &verts[numverts][2],
            &norms[numverts][0], &norms[numverts][1], &norms[numverts][2] );
        numverts++;
    }

    numverts--;

    wxPrintf(_T("%d vertices, %d triangles\n"), numverts, numverts-2);

    fclose(f);
}


static void draw_surface()
{
    GLint i;

#ifdef GL_EXT_vertex_array
    if (use_vertex_arrays)
    {
        glDrawArraysEXT( GL_TRIANGLE_STRIP, 0, numverts );
    }
    else
#endif
    {
        glBegin( GL_TRIANGLE_STRIP );
        for (i=0;i<numverts;i++)
        {
            glNormal3fv( norms[i] );
            glVertex3fv( verts[i] );
        }
        glEnd();
    }
}


static void draw1()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glPushMatrix();
    glRotatef( yrot, 0.0f, 1.0f, 0.0f );
    glRotatef( xrot, 1.0f, 0.0f, 0.0f );

    draw_surface();

    glPopMatrix();

    glFlush();
}


static void InitMaterials()
{
    static const GLfloat ambient[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    static const GLfloat diffuse[4] = {0.5f, 1.0f, 1.0f, 1.0f};
    static const GLfloat position0[4] = {0.0f, 0.0f, 20.0f, 0.0f};
    static const GLfloat position1[4] = {0.0f, 0.0f, -20.0f, 0.0f};
    static const GLfloat front_mat_shininess[1] = {60.0f};
    static const GLfloat front_mat_specular[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    static const GLfloat front_mat_diffuse[4] = {0.5f, 0.28f, 0.38f, 1.0f};
    /*
    static const GLfloat back_mat_shininess[1] = {60.0f};
    static const GLfloat back_mat_specular[4] = {0.5f, 0.5f, 0.2f, 1.0f};
    static const GLfloat back_mat_diffuse[4] = {1.0f, 1.0f, 0.2f, 1.0f};
    */
    static const GLfloat lmodel_ambient[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    static const GLfloat lmodel_twoside[1] = {GL_FALSE};

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
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);

    InitMaterials();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum( -1.0, 1.0, -1.0, 1.0, 5.0, 25.0 );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef( 0.0, 0.0, -6.0 );

#ifdef GL_EXT_vertex_array
    if (use_vertex_arrays)
    {
        glVertexPointerEXT( 3, GL_FLOAT, 0, numverts, verts );
        glNormalPointerEXT( GL_FLOAT, 0, numverts, norms );
        glEnable( GL_VERTEX_ARRAY_EXT );
        glEnable( GL_NORMAL_ARRAY_EXT );
    }
#endif
}

static GLenum Args(int argc, wxChar **argv)
{
    GLint i;

    for (i = 1; i < argc; i++)
    {
        if (wxStrcmp(argv[i], _T("-sb")) == 0)
        {
            doubleBuffer = GL_FALSE;
        }
        else if (wxStrcmp(argv[i], _T("-db")) == 0)
        {
            doubleBuffer = GL_TRUE;
        }
        else if (wxStrcmp(argv[i], _T("-speed")) == 0)
        {
            speed_test = GL_TRUE;
            doubleBuffer = GL_TRUE;
        }
        else if (wxStrcmp(argv[i], _T("-va")) == 0)
        {
            use_vertex_arrays = GL_TRUE;
        }
        else
        {
            wxString msg = _T("Bad option: ");
            msg += argv[i];
            wxMessageBox(msg);
            return GL_FALSE;
        }
    }

    return GL_TRUE;
}

// The following part was written for wxWidgets 1.66
MyFrame *frame = NULL;

IMPLEMENT_APP(MyApp)

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    Args(argc, argv);

    // Create the main frame window
    frame = new MyFrame(NULL, wxT("wxWidgets OpenGL Isosurf Sample"),
        wxDefaultPosition, wxDefaultSize);

    // Make a menubar
    wxMenu *fileMenu = new wxMenu;

    fileMenu->Append(wxID_EXIT, _T("E&xit"));
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(fileMenu, _T("&File"));
    frame->SetMenuBar(menuBar);

  // Make a TestGLCanvas

  // JACS
#ifdef __WXMSW__
    int *gl_attrib = NULL;
#else
    int gl_attrib[20] = { WX_GL_RGBA, WX_GL_MIN_RED, 1, WX_GL_MIN_GREEN, 1,
        WX_GL_MIN_BLUE, 1, WX_GL_DEPTH_SIZE, 1,
        WX_GL_DOUBLEBUFFER,
#  if defined(__WXMAC__) || defined(__WXCOCOA__)
        GL_NONE };
#  else
        None };
#  endif
#endif

    if(!doubleBuffer)
    {
        printf("don't have double buffer, disabling\n");
#ifdef __WXGTK__
        gl_attrib[9] = None;
#endif
        doubleBuffer = GL_FALSE;
    }

    frame->m_canvas = new TestGLCanvas(frame, wxID_ANY, wxDefaultPosition,
        wxDefaultSize, 0, _T("TestGLCanvas"), gl_attrib );

  // Show the frame
    frame->Show(true);

    frame->m_canvas->SetCurrent();
    read_surface( _T("isosurf.dat") );

    Init();

    return true;
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MyFrame::OnExit)
END_EVENT_TABLE()

// My frame constructor
MyFrame::MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
    const wxSize& size, long style)
    : wxFrame(frame, wxID_ANY, title, pos, size, style)
{
    m_canvas = NULL;
    SetIcon(wxICON(sample));
}

MyFrame::~MyFrame()
{
    delete m_canvas;
}

// Intercept menu commands
void MyFrame::OnExit( wxCommandEvent& WXUNUSED(event) )
{
    // true is to force the frame to close
    Close(true);
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
    const wxPoint& pos, const wxSize& size, long style,
    const wxString& name, int* gl_attrib)
    : wxGLCanvas(parent, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE, name, gl_attrib)
{
    parent->Show(true);
    SetCurrent();

    /* Make sure server supports the vertex array extension */
    char* extensions = (char *) glGetString( GL_EXTENSIONS );
    if (!extensions || !strstr( extensions, "GL_EXT_vertex_array" ))
    {
        use_vertex_arrays = GL_FALSE;
    }
}


void TestGLCanvas::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    // This is a dummy, to avoid an endless succession of paint messages.
    // OnPaint handlers must always create a wxPaintDC.
    wxPaintDC dc(this);

#ifndef __WXMOTIF__
    if (!GetContext()) return;
#endif

    SetCurrent();

    draw1();
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
    if (GetContext())
#endif
    {
        SetCurrent();
        glViewport(0, 0, (GLint) w, (GLint) h);
    }
}

void TestGLCanvas::OnChar(wxKeyEvent& event)
{
    switch( event.GetKeyCode() )
    {
    case WXK_ESCAPE:
        wxTheApp->ExitMainLoop();
        return;

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
        if (smooth)
        {
            glShadeModel(GL_SMOOTH);
        }
        else
        {
            glShadeModel(GL_FLAT);
        }
        break;

    case 'l': case 'L':
        lighting = !lighting;
        if (lighting)
        {
            glEnable(GL_LIGHTING);
        }
        else
        {
            glDisable(GL_LIGHTING);
        }
        break;

    default:
        event.Skip();
        return;
    }

    Refresh(false);
}

void TestGLCanvas::OnMouseEvent(wxMouseEvent& event)
{
    static int dragging = 0;
    static float last_x, last_y;

    //printf("%f %f %d\n", event.GetX(), event.GetY(), (int)event.LeftIsDown());
    if(event.LeftIsDown())
    {
        if(!dragging)
        {
            dragging = 1;
        }
        else
        {
            yrot += (event.GetX() - last_x)*1.0;
            xrot += (event.GetY() - last_y)*1.0;
            Refresh(false);
        }
        last_x = event.GetX();
        last_y = event.GetY();
    }
    else
        dragging = 0;

}

void TestGLCanvas::OnEraseBackground( wxEraseEvent& WXUNUSED(event) )
{
    // Do nothing, to avoid flashing.
}

