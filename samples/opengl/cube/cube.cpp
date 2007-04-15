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

TestGLContext& MyApp::GetContext(wxGLCanvas *canvas)
{
    if ( !m_glContext )
        m_glContext = new TestGLContext(canvas);

    m_glContext->SetCurrent(*canvas);

    return *m_glContext;
}

// ----------------------------------------------------------------------------
// TestGLContext
// ----------------------------------------------------------------------------

TestGLContext::TestGLContext(wxGLCanvas *canvas)
             : wxGLContext(canvas)
{
    m_gllist = 0;
}

void TestGLContext::Init()
{
    if ( m_gllist )
        return;

    /* set viewing projection */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-0.5f, 0.5f, -0.5f, 0.5f, 1.0f, 3.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // create the list of commands to draw the cube: then we can just (quickly)
    // execute it in DrawRotatedCube() later
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

void TestGLContext::DrawRotatedCube(float xangle, float yangle)
{
    Init();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -2.0f);
    glRotatef(xangle, 1.0f, 0.0f, 0.0f);
    glRotatef(yangle, 0.0f, 1.0f, 0.0f);

    glCallList(m_gllist);

    glFlush();
}

// ----------------------------------------------------------------------------
// TestGLCanvas
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(TestGLCanvas, wxGLCanvas)
    EVT_SIZE(TestGLCanvas::OnSize)
    EVT_PAINT(TestGLCanvas::OnPaint)

    EVT_KEY_DOWN(TestGLCanvas::OnKeyDown)
END_EVENT_TABLE()

TestGLCanvas::TestGLCanvas(wxWindow *parent)
            : wxGLCanvas(parent, wxID_ANY, NULL /* attribs */)
{
    m_xangle =
    m_yangle = 30;
}

void TestGLCanvas::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);

    wxGetApp().GetContext(this).DrawRotatedCube(m_xangle, m_yangle);

    SwapBuffers();
}

void TestGLCanvas::OnSize(wxSizeEvent& event)
{
    // don't prevent default processing from taking place
    event.Skip();

    if ( !IsShown() )
        return;

    // set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
    int w, h;
    GetClientSize(&w, &h);

    wxGetApp().GetContext(this);
    glViewport(0, 0, w, h);
}

void TestGLCanvas::OnKeyDown( wxKeyEvent& event )
{
    float *p = NULL;

    bool inverse = false;

    switch ( event.GetKeyCode() )
    {
        case WXK_RIGHT:
            inverse = true;
            // fall through

        case WXK_LEFT:
            // rotate around Y axis
            p = &m_yangle;
            break;

        case WXK_DOWN:
            inverse = true;
            // fall through

        case WXK_UP:
            // rotate around X axis
            p = &m_xangle;
            break;

        default:
            event.Skip();
            return;
    }

    float angle = 5;
    if ( inverse )
        angle = -angle;

    *p += angle;

    Refresh(false);
}

// ----------------------------------------------------------------------------
// MyFrame: main application window
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_NEW, MyFrame::OnNewWindow)
    EVT_MENU(wxID_CLOSE, MyFrame::OnClose)
END_EVENT_TABLE()

MyFrame::MyFrame()
       : wxFrame(NULL, wxID_ANY, _T("wxWidgets OpenGL Cube Sample"))
{
    new TestGLCanvas(this);

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

    SetClientSize(400, 400);
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

