/////////////////////////////////////////////////////////////////////////////
// Name:        penguin.cpp
// Purpose:     wxGLCanvas demo program
// Author:      Robert Roebling
// Modified by: Sandro Sigala
// Created:     04/01/98
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_GLCANVAS
    #error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif

#include "penguin.h"
#ifdef __DARWIN__
    #include <OpenGL/glu.h>
#else
    #include <GL/glu.h>
#endif

#include "../../sample.xpm"

// ---------------------------------------------------------------------------
// MyApp
// ---------------------------------------------------------------------------

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // Create the main frame window
    MyFrame *frame = new MyFrame(nullptr, "wxWidgets Penguin Sample",
        wxDefaultPosition, wxDefaultSize);

#if wxUSE_ZLIB
    if (wxFileExists("penguin.dxf.gz"))
        frame->GetCanvas()->LoadDXF("penguin.dxf.gz");
#else
    if (wxFileExists("penguin.dxf"))
        frame->GetCanvas()->LoadDXF("penguin.dxf");
#endif

    /* Show the frame */
    frame->Show(true);

    return true;
}

wxIMPLEMENT_APP(MyApp);

// ---------------------------------------------------------------------------
// MyFrame
// ---------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_OPEN, MyFrame::OnMenuFileOpen)
    EVT_MENU(wxID_EXIT, MyFrame::OnMenuFileExit)
    EVT_MENU(wxID_HELP, MyFrame::OnMenuHelpAbout)
wxEND_EVENT_TABLE()

// MyFrame constructor
MyFrame::MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
    const wxSize& size, long style)
    : wxFrame(frame, wxID_ANY, title, pos, size, style)
{
    SetIcon(wxICON(sample));

    // Make the "File" menu
    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append(wxID_OPEN, "&Open...");
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, "E&xit\tALT-X");
    // Make the "Help" menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(wxID_HELP, "&About");

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");
    SetMenuBar(menuBar);

    Show(true);

    m_canvas = new TestGLCanvas(this, wxID_ANY, wxDefaultPosition,
        GetClientSize(), wxSUNKEN_BORDER);
}

// File|Open... command
void MyFrame::OnMenuFileOpen( wxCommandEvent& WXUNUSED(event) )
{
    wxString filename = wxFileSelector("Choose DXF Model", "", "", "",
#if wxUSE_ZLIB
        "DXF Drawing (*.dxf;*.dxf.gz)|*.dxf;*.dxf.gz|All files (*.*)|*.*",
#else
        "DXF Drawing (*.dxf)|*.dxf)|All files (*.*)|*.*",
#endif
        wxFD_OPEN);
    if (!filename.IsEmpty())
    {
        m_canvas->LoadDXF(filename);
        m_canvas->Refresh(false);
    }
}

// File|Exit command
void MyFrame::OnMenuFileExit( wxCommandEvent& WXUNUSED(event) )
{
    // true is to force the frame to close
    Close(true);
}

// Help|About command
void MyFrame::OnMenuHelpAbout( wxCommandEvent& WXUNUSED(event) )
{
    wxMessageBox("OpenGL Penguin Sample (c) Robert Roebling, Sandro Sigala et al");
}

// ---------------------------------------------------------------------------
// TestGLCanvas
// ---------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(TestGLCanvas, wxGLCanvas)
    EVT_SIZE(TestGLCanvas::OnSize)
    EVT_PAINT(TestGLCanvas::OnPaint)
    EVT_ERASE_BACKGROUND(TestGLCanvas::OnEraseBackground)
    EVT_MOUSE_EVENTS(TestGLCanvas::OnMouse)
wxEND_EVENT_TABLE()

TestGLCanvas::TestGLCanvas(wxWindow *parent,
                           wxWindowID id,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style,
                           const wxString& name)
    : wxGLCanvas(parent, id, nullptr, pos, size,
                 style | wxFULL_REPAINT_ON_RESIZE, name)
{
    // Explicitly create a new rendering context instance for this canvas.
    m_glRC = new wxGLContext(this);

    m_gldata.initialized = false;

    // initialize view matrix
    m_gldata.beginx = 0.0f;
    m_gldata.beginy = 0.0f;
    m_gldata.zoom   = 45.0f;
    trackball(m_gldata.quat, 0.0f, 0.0f, 0.0f, 0.0f);
}

TestGLCanvas::~TestGLCanvas()
{
    delete m_glRC;
}

void TestGLCanvas::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    // must always be here
    wxPaintDC dc(this);

    SetCurrent(*m_glRC);

    // Initialize OpenGL
    if (!m_gldata.initialized)
    {
        InitGL();
        ResetProjectionMode();
        m_gldata.initialized = true;
    }

    // Clear
    glClearColor( 0.3f, 0.4f, 0.6f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Transformations
    glLoadIdentity();
    glTranslatef( 0.0f, 0.0f, -20.0f );
    GLfloat m[4][4];
    build_rotmatrix( m, m_gldata.quat );
    glMultMatrixf( &m[0][0] );

    m_renderer.Render();

    // Flush
    glFlush();

    // Swap
    SwapBuffers();
}

void TestGLCanvas::OnSize(wxSizeEvent& WXUNUSED(event))
{
    // Reset the OpenGL view aspect.
    // This is OK only because there is only one canvas that uses the context.
    // See the cube sample for that case that multiple canvases are made current with one context.
    ResetProjectionMode();
}

void TestGLCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // Do nothing, to avoid flashing on MSW
}

// Load the DXF file.  If the zlib support is compiled in wxWidgets,
// supports also the ".dxf.gz" gzip compressed files.
void TestGLCanvas::LoadDXF(const wxString& filename)
{
    wxFileInputStream stream(filename);
    if (stream.IsOk())
#if wxUSE_ZLIB
    {
        if (filename.Right(3).Lower() == ".gz")
        {
            wxZlibInputStream zstream(stream);
            m_renderer.Load(zstream);
        } else
        {
            m_renderer.Load(stream);
        }
    }
#else
    {
        m_renderer.Load(stream);
    }
#endif
}

void TestGLCanvas::OnMouse(wxMouseEvent& event)
{
    if (event.Dragging())
    {
        wxSize sz(GetClientSize());

        /* drag in progress, simulate trackball */
        float spin_quat[4];
        trackball(spin_quat,
            (2 * m_gldata.beginx - sz.x) / sz.x,
            (sz.y - 2 * m_gldata.beginy) / sz.y,
            float(2 * event.GetX() - sz.x) / sz.x,
            float(sz.y - 2 * event.GetY()) / sz.y);

        add_quats(spin_quat, m_gldata.quat, m_gldata.quat);

        /* orientation has changed, redraw mesh */
        Refresh(false);
    }

    m_gldata.beginx = event.GetX();
    m_gldata.beginy = event.GetY();
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
    glEnable(GL_CULL_FACE);
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

void TestGLCanvas::ResetProjectionMode()
{
    if ( !IsShownOnScreen() )
        return;

    // This is normally only necessary if there is more than one wxGLCanvas
    // or more than one wxGLContext in the application.
    SetCurrent(*m_glRC);

    const wxSize ClientSize = GetClientSize() * GetContentScaleFactor();

    // It's up to the application code to update the OpenGL viewport settings.
    // In order to avoid extensive context switching, consider doing this in
    // OnPaint() rather than here, though.
    glViewport(0, 0, ClientSize.x, ClientSize.y);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, double(ClientSize.x) / ClientSize.y, 1, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
