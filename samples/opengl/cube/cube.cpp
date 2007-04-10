///////////////////////////////////////////////////////////////////////////////
// Name:        cube.cpp
// Purpose:     wxGLCanvas demo program
// Author:      Julian Smart
// Modified by: Vadim Zeitlin to use new wxGLCanvas API (2007-04-09)
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

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

#include "cube.h"

#if !defined(__WXMSW__) && !defined(__WXPM__)
    #include "../../sample.xpm"
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// MyApp: the application object
// ----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // Create the main window
    new MyFrame();

    return true;
}

int MyApp::OnExit()
{
    delete m_glContext;

    return wxApp::OnExit();
}

void MyApp::SetCurrent(wxGLCanvas *canvas)
{
    wxCHECK_RET( canvas, _T("canvas can't be NULL") );

    if ( !m_glContext )
        m_glContext = new wxGLContext(canvas);

    m_glContext->SetCurrent(*canvas);
}

// ----------------------------------------------------------------------------
// TestGLCanvas
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(TestGLCanvas, wxGLCanvas)
    EVT_SIZE(TestGLCanvas::OnSize)
    EVT_PAINT(TestGLCanvas::OnPaint)

    EVT_KEY_DOWN(TestGLCanvas::OnKeyDown)
END_EVENT_TABLE()

static const int attribs[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, 0 };

TestGLCanvas::TestGLCanvas(wxWindow *parent)
            : wxGLCanvas(parent, wxID_ANY, NULL /* attribs */)
{
    m_gllist = 0;

    // notice that we can't call InitGL() from here: we must wait until the
    // window is shown on screen to be able to perform OpenGL calls
}

// this function is called on each repaint so it should be fast
void TestGLCanvas::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCallList(m_gllist);

    glFlush();
    SwapBuffers();
}

void TestGLCanvas::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxGetApp().SetCurrent(this);

    // initialize if not done yet
    InitGL();

    wxPaintDC dc(this);

    Render();
}

void TestGLCanvas::OnSize(wxSizeEvent& event)
{
    // don't prevent default processing from taking place
    event.Skip();

    if ( !IsInitialized() )
        return;

    // set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
    int w, h;
    GetClientSize(&w, &h);

    wxGetApp().SetCurrent(this);
    glViewport(0, 0, w, h);
}

void TestGLCanvas::InitGL()
{
    if ( IsInitialized() )
        return;

    /* set viewing projection */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-0.5f, 0.5f, -0.5f, 0.5f, 1.0f, 3.0f);

    /* position viewer */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -2.0f);

    /* position object */
    glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(30.0f, 0.0f, 1.0f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // create the list of commands to draw the cube: then we can just (quickly)
    // execute it in Render() later
    m_gllist = glGenLists(1);
    glNewList(m_gllist, GL_COMPILE);

    /* draw six faces of a cube */
    glBegin(GL_QUADS);
    glNormal3f( 0.0f, 0.0f, 1.0f);
    glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f,-0.5f, 0.5f); glVertex3f( 0.5f,-0.5f, 0.5f);

    glNormal3f( 0.0f, 0.0f,-1.0f);
    glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f(-0.5f, 0.5f,-0.5f);
    glVertex3f( 0.5f, 0.5f,-0.5f); glVertex3f( 0.5f,-0.5f,-0.5f);

    glNormal3f( 0.0f, 1.0f, 0.0f);
    glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f( 0.5f, 0.5f,-0.5f);
    glVertex3f(-0.5f, 0.5f,-0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);

    glNormal3f( 0.0f,-1.0f, 0.0f);
    glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f( 0.5f,-0.5f,-0.5f);
    glVertex3f( 0.5f,-0.5f, 0.5f); glVertex3f(-0.5f,-0.5f, 0.5f);

    glNormal3f( 1.0f, 0.0f, 0.0f);
    glVertex3f( 0.5f, 0.5f, 0.5f); glVertex3f( 0.5f,-0.5f, 0.5f);
    glVertex3f( 0.5f,-0.5f,-0.5f); glVertex3f( 0.5f, 0.5f,-0.5f);

    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f,-0.5f,-0.5f); glVertex3f(-0.5f,-0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f,-0.5f);
    glEnd();

    glEndList();
}

void TestGLCanvas::OnKeyDown( wxKeyEvent& event )
{
    GLfloat x = 0,
            y = 0,
            z = 0;

    bool inverse = false;

    switch ( event.GetKeyCode() )
    {
        case WXK_RIGHT:
            inverse = true;
            // fall through

        case WXK_LEFT:
            // rotate around Z axis
            z = 1;
            break;

        case WXK_DOWN:
            inverse = true;
            // fall through

        case WXK_UP:
            // rotate around Y axis
            y = 1;
            break;

        default:
            event.Skip();
            return;
    }

    float angle = 5;
    if ( inverse )
        angle = -angle;

    wxGetApp().SetCurrent(this);

    glMatrixMode(GL_MODELVIEW);
    glRotatef(angle, x, y, z);

    // refresh all cubes
    for ( wxWindowList::const_iterator i = wxTopLevelWindows.begin();
          i != wxTopLevelWindows.end();
          ++i )
    {
        MyFrame *frame = (MyFrame *)*i;
        frame->RefreshCanvas();
    }
}

// ----------------------------------------------------------------------------
// MyFrame: main application window
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_NEW, MyFrame::OnNewWindow)
    EVT_MENU(wxID_CLOSE, MyFrame::OnClose)
END_EVENT_TABLE()

MyFrame::MyFrame()
       : wxFrame(NULL, wxID_ANY, _T("wxWidgets OpenGL Cube Sample"),
                 wxDefaultPosition, wxSize(400, 300))
{
    m_canvas = new TestGLCanvas(this);

    SetIcon(wxICON(sample));

    // Make a menubar
    wxMenu *menu = new wxMenu;
    menu->Append(wxID_NEW);
    menu->AppendSeparator();
    menu->Append(wxID_CLOSE);
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menu, _T("&Cube"));

    SetMenuBar(menuBar);

    CreateStatusBar();

    Show();
}

void MyFrame::OnClose(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnNewWindow( wxCommandEvent& WXUNUSED(event) )
{
    (void) new MyFrame();
}

void MyFrame::RefreshCanvas()
{
    m_canvas->Refresh(false);
}
