///////////////////////////////////////////////////////////////////////////////
// Name:        cube.cpp
// Purpose:     wxGLCanvas demo program
// Author:      Julian Smart
// Modified by: Vadim Zeitlin to use new wxGLCanvas API (2007-04-09)
// Created:     04/01/98
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


#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_GLCANVAS
    #error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

#include "cube.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    SpinTimer = wxID_HIGHEST
};

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

static void CheckGLError()
{
    GLenum errLast = GL_NO_ERROR;

    for ( ;; )
    {
        GLenum err = glGetError();
        if ( err == GL_NO_ERROR )
            return;

        // normally the error is reset by the call to glGetError() but if
        // glGetError() itself returns an error, we risk looping forever here
        // so check that we get a different error than the last time
        if ( err == errLast )
        {
            wxLogError("OpenGL error state couldn't be reset.");
            return;
        }

        errLast = err;

        wxLogError("OpenGL error %d", err);
    }
}

// function to draw the texture for cube faces
static wxImage DrawDice(int size, unsigned num)
{
    wxASSERT_MSG( num >= 1 && num <= 6, "invalid dice index" );

    const int dot = size/16;        // radius of a single dot
    const int gap = 5*size/32;      // gap between dots

    wxBitmap bmp(size, size);
    wxMemoryDC dc;
    dc.SelectObject(bmp);
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();
    dc.SetBrush(*wxBLACK_BRUSH);

    // the upper left and lower right points
    if ( num != 1 )
    {
        dc.DrawCircle(gap + dot, gap + dot, dot);
        dc.DrawCircle(size - gap - dot, size - gap - dot, dot);
    }

    // draw the central point for odd dices
    if ( num % 2 )
    {
        dc.DrawCircle(size/2, size/2, dot);
    }

    // the upper right and lower left points
    if ( num > 3 )
    {
        dc.DrawCircle(size - gap - dot, gap + dot, dot);
        dc.DrawCircle(gap + dot, size - gap - dot, dot);
    }

    // finally those 2 are only for the last dice
    if ( num == 6 )
    {
        dc.DrawCircle(gap + dot, size/2, dot);
        dc.DrawCircle(size - gap - dot, size/2, dot);
    }

    dc.SelectObject(wxNullBitmap);

    return bmp.ConvertToImage();
}

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// TestGLContext
// ----------------------------------------------------------------------------

TestGLContext::TestGLContext(wxGLCanvas *canvas)
             : wxGLContext(canvas)
{
    SetCurrent(*canvas);

    // set up the parameters we want to use
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_TEXTURE_2D);

    // add slightly more light, the default lighting is rather dark
    GLfloat ambient[] = { 0.5, 0.5, 0.5, 0.5 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

    // set viewing projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-0.5, 0.5, -0.5, 0.5, 1, 3);

    // create the textures to use for cube sides: they will be reused by all
    // canvases (which is probably not critical in the case of simple textures
    // we use here but could be really important for a real application where
    // each texture could take many megabytes)
    glGenTextures(WXSIZEOF(m_textures), m_textures);

    for ( unsigned i = 0; i < WXSIZEOF(m_textures); i++ )
    {
        glBindTexture(GL_TEXTURE_2D, m_textures[i]);

        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        const wxImage img(DrawDice(256, i + 1));

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.GetWidth(), img.GetHeight(),
                     0, GL_RGB, GL_UNSIGNED_BYTE, img.GetData());
    }

    CheckGLError();
}

void TestGLContext::DrawRotatedCube(float xangle, float yangle)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -2.0f);
    glRotatef(xangle, 1.0f, 0.0f, 0.0f);
    glRotatef(yangle, 0.0f, 1.0f, 0.0f);

    // draw six faces of a cube of size 1 centered at (0, 0, 0)
    glBindTexture(GL_TEXTURE_2D, m_textures[0]);
    glBegin(GL_QUADS);
        glNormal3f( 0.0f, 0.0f, 1.0f);
        glTexCoord2f(0, 0); glVertex3f( 0.5f, 0.5f, 0.5f);
        glTexCoord2f(1, 0); glVertex3f(-0.5f, 0.5f, 0.5f);
        glTexCoord2f(1, 1); glVertex3f(-0.5f,-0.5f, 0.5f);
        glTexCoord2f(0, 1); glVertex3f( 0.5f,-0.5f, 0.5f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, m_textures[1]);
    glBegin(GL_QUADS);
        glNormal3f( 0.0f, 0.0f,-1.0f);
        glTexCoord2f(0, 0); glVertex3f(-0.5f,-0.5f,-0.5f);
        glTexCoord2f(1, 0); glVertex3f(-0.5f, 0.5f,-0.5f);
        glTexCoord2f(1, 1); glVertex3f( 0.5f, 0.5f,-0.5f);
        glTexCoord2f(0, 1); glVertex3f( 0.5f,-0.5f,-0.5f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, m_textures[2]);
    glBegin(GL_QUADS);
        glNormal3f( 0.0f, 1.0f, 0.0f);
        glTexCoord2f(0, 0); glVertex3f( 0.5f, 0.5f, 0.5f);
        glTexCoord2f(1, 0); glVertex3f( 0.5f, 0.5f,-0.5f);
        glTexCoord2f(1, 1); glVertex3f(-0.5f, 0.5f,-0.5f);
        glTexCoord2f(0, 1); glVertex3f(-0.5f, 0.5f, 0.5f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, m_textures[3]);
    glBegin(GL_QUADS);
        glNormal3f( 0.0f,-1.0f, 0.0f);
        glTexCoord2f(0, 0); glVertex3f(-0.5f,-0.5f,-0.5f);
        glTexCoord2f(1, 0); glVertex3f( 0.5f,-0.5f,-0.5f);
        glTexCoord2f(1, 1); glVertex3f( 0.5f,-0.5f, 0.5f);
        glTexCoord2f(0, 1); glVertex3f(-0.5f,-0.5f, 0.5f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, m_textures[4]);
    glBegin(GL_QUADS);
        glNormal3f( 1.0f, 0.0f, 0.0f);
        glTexCoord2f(0, 0); glVertex3f( 0.5f, 0.5f, 0.5f);
        glTexCoord2f(1, 0); glVertex3f( 0.5f,-0.5f, 0.5f);
        glTexCoord2f(1, 1); glVertex3f( 0.5f,-0.5f,-0.5f);
        glTexCoord2f(0, 1); glVertex3f( 0.5f, 0.5f,-0.5f);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, m_textures[5]);
    glBegin(GL_QUADS);
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(0, 0); glVertex3f(-0.5f,-0.5f,-0.5f);
        glTexCoord2f(1, 0); glVertex3f(-0.5f,-0.5f, 0.5f);
        glTexCoord2f(1, 1); glVertex3f(-0.5f, 0.5f, 0.5f);
        glTexCoord2f(0, 1); glVertex3f(-0.5f, 0.5f,-0.5f);
    glEnd();

    glFlush();

    CheckGLError();
}


// ----------------------------------------------------------------------------
// MyApp: the application object
// ----------------------------------------------------------------------------

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    new MyFrame();

    return true;
}

int MyApp::OnExit()
{
    delete m_glContext;
    delete m_glStereoContext;

    return wxApp::OnExit();
}

TestGLContext& MyApp::GetContext(wxGLCanvas *canvas, bool useStereo)
{
    TestGLContext *glContext;
    if ( useStereo )
    {
        if ( !m_glStereoContext )
        {
            // Create the OpenGL context for the first stereo window which needs it:
            // subsequently created windows will all share the same context.
            m_glStereoContext = new TestGLContext(canvas);
        }
        glContext = m_glStereoContext;
    }
    else
    {
        if ( !m_glContext )
        {
            // Create the OpenGL context for the first mono window which needs it:
            // subsequently created windows will all share the same context.
            m_glContext = new TestGLContext(canvas);
        }
        glContext = m_glContext;
    }

    glContext->SetCurrent(*canvas);

    return *glContext;
}

// ----------------------------------------------------------------------------
// TestGLCanvas
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(TestGLCanvas, wxGLCanvas)
    EVT_PAINT(TestGLCanvas::OnPaint)
    EVT_KEY_DOWN(TestGLCanvas::OnKeyDown)
    EVT_TIMER(SpinTimer, TestGLCanvas::OnSpinTimer)
wxEND_EVENT_TABLE()

TestGLCanvas::TestGLCanvas(wxWindow *parent, bool useStereo)
    // With perspective OpenGL graphics, the wxFULL_REPAINT_ON_RESIZE style
    // flag should always be set, because even making the canvas smaller should
    // be followed by a paint event that updates the entire canvas with new
    // viewport settings.
    : m_xangle(30.0),
      m_yangle(30.0),
      m_spinTimer(this,SpinTimer),
      m_useStereo(useStereo),
      m_stereoWarningAlreadyDisplayed(false)
{
    wxGLAttributes attribs = wxGLAttributes().Defaults();
    if ( useStereo )
        attribs.Stereo();
    attribs.EndList();

    if ( !wxGLCanvas::Create(parent, attribs, wxID_ANY,
                             wxDefaultPosition, wxDefaultSize,
                             wxFULL_REPAINT_ON_RESIZE) )
    {
        wxLogError("Creating OpenGL window failed.");
    }
}

void TestGLCanvas::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    // This is required even though dc is not used otherwise.
    wxPaintDC dc(this);

    // Set the OpenGL viewport according to the client size of this canvas.
    // This is done here rather than in a wxSizeEvent handler because our
    // OpenGL rendering context (and thus viewport setting) is used with
    // multiple canvases: If we updated the viewport in the wxSizeEvent
    // handler, changing the size of one canvas causes a viewport setting that
    // is wrong when next another canvas is repainted.
    const wxSize ClientSize = GetClientSize() * GetContentScaleFactor();

    TestGLContext& canvas = wxGetApp().GetContext(this, m_useStereo);
    glViewport(0, 0, ClientSize.x, ClientSize.y);

    // Render the graphics and swap the buffers.
    GLboolean quadStereoSupported;
    glGetBooleanv( GL_STEREO, &quadStereoSupported);
    if ( quadStereoSupported )
    {
        glDrawBuffer( GL_BACK_LEFT );
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glFrustum(-0.47, 0.53, -0.5, 0.5, 1, 3);
        canvas.DrawRotatedCube(m_xangle, m_yangle);
        CheckGLError();
        glDrawBuffer( GL_BACK_RIGHT );
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glFrustum(-0.53, 0.47, -0.5, 0.5, 1, 3);
        canvas.DrawRotatedCube(m_xangle, m_yangle);
        CheckGLError();
    }
    else
    {
        canvas.DrawRotatedCube(m_xangle, m_yangle);
        if ( m_useStereo && !m_stereoWarningAlreadyDisplayed )
        {
            m_stereoWarningAlreadyDisplayed = true;
            wxLogError("Stereo not supported by the graphics card.");
        }
    }
    SwapBuffers();
}

void TestGLCanvas::Spin(float xSpin, float ySpin)
{
    m_xangle += xSpin;
    m_yangle += ySpin;

    Refresh(false);
}

void TestGLCanvas::OnKeyDown(wxKeyEvent& event)
{
    float angle = 5.0;

    switch ( event.GetKeyCode() )
    {
        case WXK_RIGHT:
            Spin( 0.0, -angle );
            break;

        case WXK_LEFT:
            Spin( 0.0, angle );
            break;

        case WXK_DOWN:
            Spin( -angle, 0.0 );
            break;

        case WXK_UP:
            Spin( angle, 0.0 );
            break;

        case WXK_SPACE:
            if ( m_spinTimer.IsRunning() )
                m_spinTimer.Stop();
            else
                m_spinTimer.Start( 25 );
            break;

        default:
            event.Skip();
            return;
    }
}

void TestGLCanvas::OnSpinTimer(wxTimerEvent& WXUNUSED(event))
{
    Spin(0.0, 4.0);
}

wxString glGetwxString(GLenum name)
{
    const GLubyte *v = glGetString(name);
    if ( v == 0 )
    {
        // The error is not important. It is GL_INVALID_ENUM.
        // We just want to clear the error stack.
        glGetError();

        return wxString();
    }

    return wxString((const char*)v);
}


// ----------------------------------------------------------------------------
// MyFrame: main application window
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_NEW, MyFrame::OnNewWindow)
    EVT_MENU(NEW_STEREO_WINDOW, MyFrame::OnNewStereoWindow)
    EVT_MENU(wxID_CLOSE, MyFrame::OnClose)
wxEND_EVENT_TABLE()

MyFrame::MyFrame( bool stereoWindow )
       : wxFrame(nullptr, wxID_ANY, "wxWidgets OpenGL Cube Sample")
{
    new TestGLCanvas(this, stereoWindow);

    SetIcon(wxICON(sample));

    // Make a menubar
    wxMenu *menu = new wxMenu;
    menu->Append(wxID_NEW);
    menu->Append(NEW_STEREO_WINDOW, "New Stereo Window");
    menu->AppendSeparator();
    menu->Append(wxID_CLOSE);
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menu, "&Cube");

    SetMenuBar(menuBar);

    CreateStatusBar();

    SetClientSize(400, 400);
    Show();

    // test IsDisplaySupported() function:
    wxGLAttributes attribs;
    attribs.RGBA().DoubleBuffer().EndList();
    wxLogStatus("Double-buffered display %s supported",
                wxGLCanvas::IsDisplaySupported(attribs) ? "is" : "not");

    if ( stereoWindow )
    {
        const wxString vendor = glGetwxString(GL_VENDOR).Lower();
        const wxString renderer = glGetwxString(GL_RENDERER).Lower();
        if ( vendor.find("nvidia") != wxString::npos &&
                renderer.find("quadro") == wxString::npos )
            ShowFullScreen(true);
    }
}

void MyFrame::OnClose(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnNewWindow( wxCommandEvent& WXUNUSED(event) )
{
    new MyFrame();
}

void MyFrame::OnNewStereoWindow( wxCommandEvent& WXUNUSED(event) )
{
    wxGLAttributes attribs;
    attribs.RGBA().DoubleBuffer().Stereo().EndList();
    if ( wxGLCanvas::IsDisplaySupported(attribs) )
    {
        new MyFrame(true);
    }
    else
    {
        wxLogError("Stereo not supported by OpenGL on this system, sorry.");
    }
}
