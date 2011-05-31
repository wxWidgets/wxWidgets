/////////////////////////////////////////////////////////////////////////////
// Name:        isosurf.cpp
// Purpose:     wxGLCanvas demo program
// Author:      Brian Paul (original gltk version), Wolfram Gloger
// Modified by: Julian Smart, Francesco Montorsi
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
#include "wx/log.h"
#include "wx/cmdline.h"
#include "wx/wfstream.h"
#include "wx/zstream.h"
#include "wx/txtstrm.h"

#include "isosurf.h"
#include "../../sample.xpm"


// global options which can be set through command-line options
GLboolean g_use_vertex_arrays = GL_FALSE;
GLboolean g_doubleBuffer = GL_TRUE;
GLboolean g_smooth = GL_TRUE;
GLboolean g_lighting = GL_TRUE;



//---------------------------------------------------------------------------
// MyApp
//---------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // Create the main frame window
    MyFrame *frame = new MyFrame(NULL, wxT("wxWidgets OpenGL Isosurf Sample"));

    return true;
}

void MyApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    parser.AddSwitch("", "sb", "Do not use double buffering");
    parser.AddSwitch("", "db", "Use double buffering");
    parser.AddSwitch("", "va", "Use vertex arrays");

    wxApp::OnInitCmdLine(parser);
}

bool MyApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    if (parser.Found("sb"))
        g_doubleBuffer = GL_FALSE;
    else if (parser.Found("db"))
        g_doubleBuffer = GL_TRUE;

    if (parser.Found("va"))
        g_use_vertex_arrays = GL_TRUE;

    return wxApp::OnCmdLineParsed(parser);
}

//---------------------------------------------------------------------------
// MyFrame
//---------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MyFrame::OnExit)
END_EVENT_TABLE()

MyFrame::MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
                 const wxSize& size, long style)
    : wxFrame(frame, wxID_ANY, title, pos, size, style),
      m_canvas(NULL)
{
    SetIcon(wxICON(sample));


    // Make a menubar
    wxMenu *fileMenu = new wxMenu;

    fileMenu->Append(wxID_EXIT, wxT("E&xit"));
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(fileMenu, wxT("&File"));
    SetMenuBar(menuBar);


  // Make a TestGLCanvas

  // JACS
#ifdef __WXMSW__
    int *gl_attrib = NULL;
#else
    int gl_attrib[20] =
        { WX_GL_RGBA, WX_GL_MIN_RED, 1, WX_GL_MIN_GREEN, 1,
        WX_GL_MIN_BLUE, 1, WX_GL_DEPTH_SIZE, 1,
        WX_GL_DOUBLEBUFFER,
#  if defined(__WXMAC__) || defined(__WXCOCOA__)
        GL_NONE };
#  else
        None };
#  endif
#endif

    if (!g_doubleBuffer)
    {
        wxLogWarning("Disabling double buffering");

#ifdef __WXGTK__
        gl_attrib[9] = None;
#endif
        g_doubleBuffer = GL_FALSE;
    }

    // Show the frame
    Show(true);

    m_canvas = new TestGLCanvas(this, wxID_ANY, gl_attrib);
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


//---------------------------------------------------------------------------
// TestGLCanvas
//---------------------------------------------------------------------------

BEGIN_EVENT_TABLE(TestGLCanvas, wxGLCanvas)
    EVT_SIZE(TestGLCanvas::OnSize)
    EVT_PAINT(TestGLCanvas::OnPaint)
    EVT_CHAR(TestGLCanvas::OnChar)
    EVT_MOUSE_EVENTS(TestGLCanvas::OnMouseEvent)
END_EVENT_TABLE()

TestGLCanvas::TestGLCanvas(wxWindow *parent,
                           wxWindowID id,
                           int* gl_attrib)
    : wxGLCanvas(parent, id, gl_attrib)
{
    m_xrot = 0;
    m_yrot = 0;
    m_numverts = 0;

    // Explicitly create a new rendering context instance for this canvas.
    m_glRC = new wxGLContext(this);

    // Make the new context current (activate it for use) with this canvas.
    SetCurrent(*m_glRC);

    InitGL();
    InitMaterials();
    LoadSurface("isosurf.dat.gz");
}

TestGLCanvas::~TestGLCanvas()
{
    delete m_glRC;
}

void TestGLCanvas::LoadSurface(const wxString& filename)
{
    // FIXME
    // we need to set english locale to force wxTextInputStream's calls to
    // wxStrtod to use the point and not the comma as decimal separator...
    // (the isosurf.dat contains points and not commas)...
    wxLocale l(wxLANGUAGE_ENGLISH);

    wxZlibInputStream* stream =
        new wxZlibInputStream(new wxFFileInputStream(filename));
    if (!stream || !stream->IsOk())
    {
        wxLogError("Cannot load '%s' type of files!", filename.c_str());
        delete stream;
        return;
    }

    {
        // we suppose to have in input a text file containing floating numbers
        // space/newline-separated... first 3 numbers are the coordinates of a
        // vertex and the following 3 are the relative vertex normal and so on...

        wxTextInputStream inFile(*stream);
        m_numverts = 0;

        while (!stream->Eof() && m_numverts < MAXVERTS)// && m_numverts<MAXVERTS)
        {
            inFile >> m_verts[m_numverts][0] >> m_verts[m_numverts][1] >> m_verts[m_numverts][2];
            inFile >> m_norms[m_numverts][0] >> m_norms[m_numverts][1] >> m_norms[m_numverts][2];

            m_numverts++;
        }

        // discard last vertex; it is a zero caused by the EOF
        m_numverts--;
    }

    delete stream;

    wxLogMessage(wxT("Loaded %d vertices, %d triangles from '%s'"),
                 m_numverts, m_numverts-2, filename.c_str());

    // NOTE: for some reason under wxGTK the following is required to avoid that
    //       the surface gets rendered in a small rectangle in the top-left corner of the frame
    PostSizeEventToParent();
}

void TestGLCanvas::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    // This is a dummy, to avoid an endless succession of paint messages.
    // OnPaint handlers must always create a wxPaintDC.
    wxPaintDC dc(this);

    // This is normally only necessary if there is more than one wxGLCanvas
    // or more than one wxGLContext in the application.
    SetCurrent(*m_glRC);

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glPushMatrix();
    glRotatef( m_yrot, 0.0f, 1.0f, 0.0f );
    glRotatef( m_xrot, 1.0f, 0.0f, 0.0f );

    // draw the surface
    if (g_use_vertex_arrays)
    {
        glDrawArrays( GL_TRIANGLE_STRIP, 0, m_numverts );
    }
    else
    {
        glBegin( GL_TRIANGLE_STRIP );

        for (int i=0;i<m_numverts;i++)
        {
            glNormal3fv( m_norms[i] );
            glVertex3fv( m_verts[i] );
        }

        glEnd();
    }

    glPopMatrix();
    glFlush(); // Not really necessary: buffer swapping below implies glFlush()

    SwapBuffers();
}

void TestGLCanvas::OnSize(wxSizeEvent& event)
{
    // This is normally only necessary if there is more than one wxGLCanvas
    // or more than one wxGLContext in the application.
    SetCurrent(*m_glRC);

    // It's up to the application code to update the OpenGL viewport settings.
    // This is OK here only because there is only one canvas that uses the
    // context. See the cube sample for that case that multiple canvases are
    // made current with one context.
    glViewport(0, 0, event.GetSize().x, event.GetSize().y);
}

void TestGLCanvas::OnChar(wxKeyEvent& event)
{
    switch( event.GetKeyCode() )
    {
    case WXK_ESCAPE:
        wxTheApp->ExitMainLoop();
        return;

    case WXK_LEFT:
        m_yrot -= 15.0;
        break;

    case WXK_RIGHT:
        m_yrot += 15.0;
        break;

    case WXK_UP:
        m_xrot += 15.0;
        break;

    case WXK_DOWN:
        m_xrot -= 15.0;
        break;

    case 's': case 'S':
        g_smooth = !g_smooth;
        if (g_smooth)
            glShadeModel(GL_SMOOTH);
        else
            glShadeModel(GL_FLAT);
        break;

    case 'l': case 'L':
        g_lighting = !g_lighting;
        if (g_lighting)
            glEnable(GL_LIGHTING);
        else
            glDisable(GL_LIGHTING);
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

    // Allow default processing to happen, or else the canvas cannot gain focus
    // (for key events).
    event.Skip();

    if (event.LeftIsDown())
    {
        if (!dragging)
        {
            dragging = 1;
        }
        else
        {
            m_yrot += (event.GetX() - last_x)*1.0;
            m_xrot += (event.GetY() - last_y)*1.0;
            Refresh(false);
        }
        last_x = event.GetX();
        last_y = event.GetY();
    }
    else
    {
        dragging = 0;
    }
}

void TestGLCanvas::InitMaterials()
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

void TestGLCanvas::InitGL()
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

    if (g_use_vertex_arrays)
    {
        glVertexPointer( 3, GL_FLOAT, 0, m_verts );
        glNormalPointer( GL_FLOAT, 0, m_norms );
        glEnable( GL_VERTEX_ARRAY );
        glEnable( GL_NORMAL_ARRAY );
    }
}

