/////////////////////////////////////////////////////////////////////////////
// Name:        cube.cpp
// Purpose:     wxGLCanvas demo program
// Author:      Julian Smart
// Modified by:
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

#include "cube.h"
#include "../../sample.xpm"

#ifndef __WXMSW__     // for StopWatch, see remark below
  #if defined(__WXMAC__) && !defined(__DARWIN__)
    #include <utime.h>
    #include <unistd.h>
  #else
    #include <sys/time.h>
    #include <sys/unistd.h>
  #endif
#else
#include <sys/timeb.h>
#endif

#define ID_NEW_WINDOW 10000
#define ID_DEF_ROTATE_LEFT_KEY 10001
#define ID_DEF_ROTATE_RIGHT_KEY 10002

/*----------------------------------------------------------
  Control to get a keycode
  ----------------------------------------------------------*/
class ScanCodeCtrl : public wxTextCtrl
{
public:
    ScanCodeCtrl( wxWindow* parent, wxWindowID id, int code,
        const wxPoint& pos, const wxSize& size );

    void OnChar( wxKeyEvent& WXUNUSED(event) )
    {
        // Do nothing
    }

    void OnKeyDown(wxKeyEvent& event);

private:

    // Any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE( ScanCodeCtrl, wxTextCtrl )
    EVT_CHAR( ScanCodeCtrl::OnChar )
    EVT_KEY_DOWN( ScanCodeCtrl::OnKeyDown )
END_EVENT_TABLE()

ScanCodeCtrl::ScanCodeCtrl( wxWindow* parent, wxWindowID id, int code,
    const wxPoint& pos, const wxSize& size )
    : wxTextCtrl( parent, id, wxEmptyString, pos, size )
{
    SetValue( wxString::Format(wxT("0x%04x"), code) );
}

void ScanCodeCtrl::OnKeyDown( wxKeyEvent& event )
{
    SetValue( wxString::Format(wxT("0x%04x"), event.GetKeyCode()) );
}

/*------------------------------------------------------------------
 Dialog for defining a keypress
-------------------------------------------------------------------*/

class ScanCodeDialog : public wxDialog
{
public:
    ScanCodeDialog( wxWindow* parent, wxWindowID id, const int code,
        const wxString &descr, const wxString& title );
    int GetValue();

private:

    ScanCodeCtrl       *m_ScanCode;
    wxTextCtrl         *m_Description;
};

ScanCodeDialog::ScanCodeDialog( wxWindow* parent, wxWindowID id,
    const int code, const wxString &descr, const wxString& title )
    : wxDialog( parent, id, title, wxDefaultPosition, wxSize(96*2,76*2) )
{
    new wxStaticText( this, wxID_ANY, _T("Scancode"), wxPoint(4*2,3*2),
        wxSize(31*2,12*2) );
    m_ScanCode = new ScanCodeCtrl( this, wxID_ANY, code, wxPoint(37*2,6*2),
        wxSize(53*2,14*2) );

    new wxStaticText( this, wxID_ANY, _T("Description"), wxPoint(4*2,24*2),
        wxSize(32*2,12*2) );
    m_Description = new wxTextCtrl( this, wxID_ANY, descr, wxPoint(37*2,27*2),
        wxSize(53*2,14*2) );

    new wxButton( this, wxID_OK, _T("Ok"), wxPoint(20*2,50*2), wxSize(20*2,13*2) );
    new wxButton( this, wxID_CANCEL, _T("Cancel"), wxPoint(44*2,50*2),
        wxSize(25*2,13*2) );
}

int ScanCodeDialog::GetValue()
{
    int code;
    wxString buf = m_ScanCode->GetValue();
    wxSscanf( buf.c_str(), _T("%i"), &code );
    return code;
}

/*----------------------------------------------------------------------
  Utility function to get the elapsed time (in msec) since a given point
  in time (in sec)  (because current version of wxGetElapsedTime doesn´t
  works right with glibc-2.1 and linux, at least for me)
-----------------------------------------------------------------------*/
unsigned long StopWatch( unsigned long *sec_base )
{
  unsigned long secs,msec;

#if defined(__WXMSW__)
  struct timeb tb;
  ftime( &tb );
  secs = tb.time;
  msec = tb.millitm;
#elif defined(__WXMAC__) && !defined(__DARWIN__)
  wxLongLong tl = wxGetLocalTimeMillis();
  secs = (unsigned long) (tl.GetValue() / 1000);
  msec = (unsigned long) (tl.GetValue() - secs*1000);
#else
  // think every unice has gettimeofday
  struct timeval tv;
  gettimeofday( &tv, (struct timezone *)NULL );
  secs = tv.tv_sec;
  msec = tv.tv_usec/1000;
#endif

  if( *sec_base == 0 )
    *sec_base = secs;

  return( (secs-*sec_base)*1000 + msec );
}

/*----------------------------------------------------------------
  Implementation of Test-GLCanvas
-----------------------------------------------------------------*/

BEGIN_EVENT_TABLE(TestGLCanvas, wxGLCanvas)
    EVT_SIZE(TestGLCanvas::OnSize)
    EVT_PAINT(TestGLCanvas::OnPaint)
    EVT_ERASE_BACKGROUND(TestGLCanvas::OnEraseBackground)
    EVT_KEY_DOWN( TestGLCanvas::OnKeyDown )
    EVT_KEY_UP( TestGLCanvas::OnKeyUp )
    EVT_ENTER_WINDOW( TestGLCanvas::OnEnterWindow )
END_EVENT_TABLE()

unsigned long  TestGLCanvas::m_secbase = 0;
int            TestGLCanvas::m_TimeInitialized = 0;
unsigned long  TestGLCanvas::m_xsynct;
unsigned long  TestGLCanvas::m_gsynct;

TestGLCanvas::TestGLCanvas(wxWindow *parent, wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name)
    : wxGLCanvas(parent, (wxGLCanvas*) NULL, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE , name )
{
    m_init = false;
    m_gllist = 0;
    m_rleft = WXK_LEFT;
    m_rright = WXK_RIGHT;
}

TestGLCanvas::TestGLCanvas(wxWindow *parent, const TestGLCanvas *other,
    wxWindowID id, const wxPoint& pos, const wxSize& size, long style,
    const wxString& name )
    : wxGLCanvas(parent, other->GetContext(), id, pos, size, style|wxFULL_REPAINT_ON_RESIZE , name)
{
    m_init = false;
    m_gllist = other->m_gllist; // share display list
    m_rleft = WXK_LEFT;
    m_rright = WXK_RIGHT;
}

TestGLCanvas::~TestGLCanvas()
{
}

void TestGLCanvas::Render()
{
    wxPaintDC dc(this);

#ifndef __WXMOTIF__
    if (!GetContext()) return;
#endif

    SetCurrent();
    // Init OpenGL once, but after SetCurrent
    if (!m_init)
    {
        InitGL();
        m_init = true;
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-0.5f, 0.5f, -0.5f, 0.5f, 1.0f, 3.0f);
    glMatrixMode(GL_MODELVIEW);

    /* clear color and depth buffers */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if( m_gllist == 0 )
    {
        m_gllist = glGenLists( 1 );
        glNewList( m_gllist, GL_COMPILE_AND_EXECUTE );
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
    else
    {
        glCallList(m_gllist);
    }

    glFlush();
    SwapBuffers();
}

void TestGLCanvas::OnEnterWindow( wxMouseEvent& WXUNUSED(event) )
{
    SetFocus();
}

void TestGLCanvas::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    Render();
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

void TestGLCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
  // Do nothing, to avoid flashing.
}

void TestGLCanvas::InitGL()
{
    SetCurrent();

    /* set viewing projection */
    glMatrixMode(GL_PROJECTION);
    glFrustum(-0.5f, 0.5f, -0.5f, 0.5f, 1.0f, 3.0f);

    /* position viewer */
    glMatrixMode(GL_MODELVIEW);
    glTranslatef(0.0f, 0.0f, -2.0f);

    /* position object */
    glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(30.0f, 0.0f, 1.0f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

GLfloat TestGLCanvas::CalcRotateSpeed( unsigned long acceltime )
{
  GLfloat t,v;

  t = ((GLfloat)acceltime) / 1000.0f;

  if( t < 0.5f )
    v = t;
  else if( t < 1.0f )
    v = t * (2.0f - t);
  else
    v = 0.75f;

  return(v);
}

GLfloat TestGLCanvas::CalcRotateAngle( unsigned long lasttime,
                                  unsigned long acceltime )
{
    GLfloat t,s1,s2;

    t = ((GLfloat)(acceltime - lasttime)) / 1000.0f;
    s1 = CalcRotateSpeed( lasttime );
    s2 = CalcRotateSpeed( acceltime );

    return( t * (s1 + s2) * 135.0f );
}

void TestGLCanvas::Action( long code, unsigned long lasttime,
                           unsigned long acceltime )
{
    GLfloat angle = CalcRotateAngle( lasttime, acceltime );

    if (code == m_rleft)
        Rotate( angle );
    else if (code == m_rright)
            Rotate( -angle );
}

void TestGLCanvas::OnKeyDown( wxKeyEvent& event )
{
    long evkey = event.GetKeyCode();
    if (evkey == 0) return;

    if (!m_TimeInitialized)
    {
        m_TimeInitialized = 1;
        m_xsynct = event.GetTimestamp();
        m_gsynct = StopWatch(&m_secbase);

        m_Key = evkey;
        m_StartTime = 0;
        m_LastTime = 0;
        m_LastRedraw = 0;
    }

    unsigned long currTime = event.GetTimestamp() - m_xsynct;

    if (evkey != m_Key)
    {
        m_Key = evkey;
        m_LastRedraw = m_StartTime = m_LastTime = currTime;
    }

    if (currTime >= m_LastRedraw)      // Redraw:
    {
        Action( m_Key, m_LastTime-m_StartTime, currTime-m_StartTime );

#if defined(__WXMAC__) && !defined(__DARWIN__)
        m_LastRedraw = currTime;    // StopWatch() doesn't work on Mac...
#else
        m_LastRedraw = StopWatch(&m_secbase) - m_gsynct;
#endif
        m_LastTime = currTime;
    }

    event.Skip();
}

void TestGLCanvas::OnKeyUp( wxKeyEvent& event )
{
    m_Key = 0;
    m_StartTime = 0;
    m_LastTime = 0;
    m_LastRedraw = 0;

    event.Skip();
}

void TestGLCanvas::Rotate( GLfloat deg )
{
    SetCurrent();

    glMatrixMode(GL_MODELVIEW);
    glRotatef((GLfloat)deg, 0.0f, 0.0f, 1.0f);
    Refresh(false);
}


/* -----------------------------------------------------------------------
  Main Window
-------------------------------------------------------------------------*/

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MyFrame::OnExit)
    EVT_MENU( ID_NEW_WINDOW, MyFrame::OnNewWindow)
    EVT_MENU( ID_DEF_ROTATE_LEFT_KEY, MyFrame::OnDefRotateLeftKey)
    EVT_MENU( ID_DEF_ROTATE_RIGHT_KEY, MyFrame::OnDefRotateRightKey)
END_EVENT_TABLE()

// My frame constructor
MyFrame::MyFrame(wxWindow *parent, const wxString& title, const wxPoint& pos,
    const wxSize& size, long style)
    : wxFrame(parent, wxID_ANY, title, pos, size, style)
{
    m_canvas = NULL;
    SetIcon(wxIcon(sample_xpm));
}

// Intercept menu commands
void MyFrame::OnExit( wxCommandEvent& WXUNUSED(event) )
{
    // true is to force the frame to close
    Close(true);
}

/*static*/ MyFrame *MyFrame::Create(MyFrame *parentFrame, bool isCloneWindow)
{
    wxString str = wxT("wxWidgets OpenGL Cube Sample");
    if (isCloneWindow) str += wxT(" - Clone");

    MyFrame *frame = new MyFrame(NULL, str, wxDefaultPosition,
        wxSize(400, 300));

    // Make a menubar
    wxMenu *winMenu = new wxMenu;

    winMenu->Append(wxID_EXIT, _T("&Close"));
    winMenu->Append(ID_NEW_WINDOW, _T("&New") );
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(winMenu, _T("&Window"));

    winMenu = new wxMenu;
    winMenu->Append(ID_DEF_ROTATE_LEFT_KEY, _T("Rotate &left"));
    winMenu->Append(ID_DEF_ROTATE_RIGHT_KEY, _T("Rotate &right"));
    menuBar->Append(winMenu, _T("&Key"));

    frame->SetMenuBar(menuBar);

    if (parentFrame)
    {
        frame->m_canvas = new TestGLCanvas( frame, parentFrame->m_canvas,
            wxID_ANY, wxDefaultPosition, wxDefaultSize );
    }
    else
    {
        frame->m_canvas = new TestGLCanvas(frame, wxID_ANY,
            wxDefaultPosition, wxDefaultSize);
    }

    // Show the frame
    frame->Show(true);

    return frame;
}

void MyFrame::OnNewWindow( wxCommandEvent& WXUNUSED(event) )
{
    (void) Create(this, true);
}

void MyFrame::OnDefRotateLeftKey( wxCommandEvent& WXUNUSED(event) )
{
    ScanCodeDialog dial( this, wxID_ANY, m_canvas->m_rleft,
        wxString(_T("Left")), _T("Define key") );

    int result = dial.ShowModal();

    if( result == wxID_OK )
        m_canvas->m_rleft = dial.GetValue();
}

void MyFrame::OnDefRotateRightKey( wxCommandEvent& WXUNUSED(event) )
{
    ScanCodeDialog dial( this, wxID_ANY, m_canvas->m_rright,
        wxString(_T("Right")), _T("Define key") );

    int result = dial.ShowModal();

    if( result == wxID_OK )
        m_canvas->m_rright = dial.GetValue();
}

/*------------------------------------------------------------------
  Application object ( equivalent to main() )
------------------------------------------------------------------ */

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // Create the main frame window
    (void) MyFrame::Create(NULL);

    return true;
}
