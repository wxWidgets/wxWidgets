/////////////////////////////////////////////////////////////////////////////
// Name:        cube.cpp
// Purpose:     wxGLCanvas demo program
// Author:      Julian Smart
// Modified by:
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

#include "wx/log.h"

#include "cube.h"

#ifndef __WXMSW__     // for wxStopWatch, see remark below 
#include <sys/time.h>
#include <sys/unistd.h>
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
  void OnChar( wxKeyEvent& event ) { } /* do nothing */
  void OnKeyDown(wxKeyEvent& event);
private:
// any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE( ScanCodeCtrl, wxTextCtrl )
  EVT_CHAR( ScanCodeCtrl::OnChar )
  EVT_KEY_DOWN( ScanCodeCtrl::OnKeyDown )
END_EVENT_TABLE()

ScanCodeCtrl::ScanCodeCtrl( wxWindow* parent, wxWindowID id, int code,
                             const wxPoint& pos, const wxSize& size )
                  : wxTextCtrl( parent, id, "", pos, size )
{ wxString buf;
  buf.Printf( "0x%04x", code );
  SetValue( buf );
}

void ScanCodeCtrl::OnKeyDown( wxKeyEvent& event )
{ wxString buf;
  buf.Printf( "0x%04x", event.KeyCode() );
  SetValue( buf );
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
// any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE( ScanCodeDialog, wxDialog )
//
END_EVENT_TABLE()

/* ---------------------------------------------------------------- */

ScanCodeDialog::ScanCodeDialog( wxWindow* parent, wxWindowID id, 
               const int code, const wxString &descr, const wxString& title )
          : wxDialog( parent, id, title, wxPoint(-1, -1), wxSize(96*2,76*2) )
{
  new wxStaticText( this, -1, "Scancode", wxPoint(4*2,3*2), 
                    wxSize(31*2,12*2) );
  m_ScanCode = new ScanCodeCtrl( this, -1, code, wxPoint(37*2,6*2), 
                                 wxSize(53*2,14*2) );

  new wxStaticText( this, -1, "Description", wxPoint(4*2,24*2),
                    wxSize(32*2,12*2) );
  m_Description = new wxTextCtrl( this, -1, descr, wxPoint(37*2,27*2),
                                  wxSize(53*2,14*2) );

  new wxButton( this, wxID_OK, "Ok", wxPoint(20*2,50*2), wxSize(20*2,13*2) );
  new wxButton( this, wxID_CANCEL, "Cancel", wxPoint(44*2,50*2),
                wxSize(25*2,13*2) );
}

int ScanCodeDialog::GetValue()
{
  int code;
  wxString buf = m_ScanCode->GetValue();
  sscanf( buf.c_str(), "%i", &code );
  return( code );
}

/*----------------------------------------------------------------------
  Utility function to get the elapsed time (in msec) since a given point
  in time (in sec)  (because current version of wxGetElapsedTime doesn´t 
  works right with glibc-2.1 and linux, at least for me) 
-----------------------------------------------------------------------*/
unsigned long wxStopWatch( unsigned long *sec_base )
{
  unsigned long secs,msec;

#ifndef __WXMSW__        // think every unice has gettimeofday
  struct timeval tv;
  gettimeofday( &tv, (struct timezone *)NULL );
  secs = tv.tv_sec;
  msec = tv.tv_usec/1000;
#else
  struct timeb tb;
  ftime( &tb );
  secs = tb.time;
  msec = tb.millitm;
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
    const wxPoint& pos, const wxSize& size, long style, const wxString& name):
  wxGLCanvas(parent, NULL, id, pos, size, style, name )
{
  m_init = FALSE;
  m_gllist = 0;
  m_rleft = WXK_LEFT;
  m_rright = WXK_RIGHT;
}
TestGLCanvas::TestGLCanvas(wxWindow *parent, const TestGLCanvas &other, 
    wxWindowID id, const wxPoint& pos, const wxSize& size, long style,
    const wxString& name ) :
      wxGLCanvas(parent, other.GetContext(), id, pos, size, style, name  )
{
  m_init = FALSE;
  m_gllist = other.m_gllist;    /* share display list */
  m_rleft = WXK_LEFT;
  m_rright = WXK_RIGHT;
}
TestGLCanvas::~TestGLCanvas(void)
{
}

void TestGLCanvas::Render( void  )
{
  wxPaintDC dc(this);

#ifndef __WXMOTIF__
  if (!GetContext()) return;
#endif
  SetCurrent();
  /* init OpenGL once, but after SetCurrent */
  if (!m_init)
  {
    InitGL();
    m_init = TRUE;
  }
  /* clear color and depth buffers */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if( m_gllist == 0 )
  {
    m_gllist = glGenLists( 1 );
    printf( "List=%d\n", m_gllist );
    glNewList( m_gllist, GL_COMPILE_AND_EXECUTE );        
    /* draw six faces of a cube */
    glBegin(GL_QUADS);
    glNormal3f( 0.0F, 0.0F, 1.0F);
    glVertex3f( 0.5F, 0.5F, 0.5F); glVertex3f(-0.5F, 0.5F, 0.5F);
    glVertex3f(-0.5F,-0.5F, 0.5F); glVertex3f( 0.5F,-0.5F, 0.5F);

    glNormal3f( 0.0F, 0.0F,-1.0F);
    glVertex3f(-0.5F,-0.5F,-0.5F); glVertex3f(-0.5F, 0.5F,-0.5F);
    glVertex3f( 0.5F, 0.5F,-0.5F); glVertex3f( 0.5F,-0.5F,-0.5F);

    glNormal3f( 0.0F, 1.0F, 0.0F);
    glVertex3f( 0.5F, 0.5F, 0.5F); glVertex3f( 0.5F, 0.5F,-0.5F);
    glVertex3f(-0.5F, 0.5F,-0.5F); glVertex3f(-0.5F, 0.5F, 0.5F);

    glNormal3f( 0.0F,-1.0F, 0.0F);
    glVertex3f(-0.5F,-0.5F,-0.5F); glVertex3f( 0.5F,-0.5F,-0.5F);
    glVertex3f( 0.5F,-0.5F, 0.5F); glVertex3f(-0.5F,-0.5F, 0.5F);

    glNormal3f( 1.0F, 0.0F, 0.0F);
    glVertex3f( 0.5F, 0.5F, 0.5F); glVertex3f( 0.5F,-0.5F, 0.5F);
    glVertex3f( 0.5F,-0.5F,-0.5F); glVertex3f( 0.5F, 0.5F,-0.5F);

    glNormal3f(-1.0F, 0.0F, 0.0F);
    glVertex3f(-0.5F,-0.5F,-0.5F); glVertex3f(-0.5F,-0.5F, 0.5F);
    glVertex3f(-0.5F, 0.5F, 0.5F); glVertex3f(-0.5F, 0.5F,-0.5F);
    glEnd();

    glEndList();
  }
  else
    glCallList( m_gllist );    

  glFlush();
  SwapBuffers();
}

void TestGLCanvas::OnEnterWindow( wxMouseEvent& event )
{
  SetFocus();
}

void TestGLCanvas::OnPaint( wxPaintEvent& event )
{
  Render();
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
  // Do nothing, to avoid flashing.
}

void TestGLCanvas::InitGL(void)
{
  SetCurrent();
    
  /* set viewing projection */
  glMatrixMode(GL_PROJECTION);
  glFrustum(-0.5F, 0.5F, -0.5F, 0.5F, 1.0F, 3.0F);

  /* position viewer */
  glMatrixMode(GL_MODELVIEW);
  glTranslatef(0.0F, 0.0F, -2.0F);

  /* position object */
  glRotatef(30.0F, 1.0F, 0.0F, 0.0F);
  glRotatef(30.0F, 0.0F, 1.0F, 0.0F);

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

  if( code == m_rleft ) Rotate( angle );
  else if( code == m_rright ) Rotate( -angle );
}

void TestGLCanvas::OnKeyDown( wxKeyEvent& event )
{
  long evkey = event.KeyCode();
  if( evkey == 0 ) return;

  if( !m_TimeInitialized )
  {
    m_TimeInitialized = 1;
    m_xsynct = event.m_timeStamp;
    m_gsynct = wxStopWatch(&m_secbase);

    m_Key = evkey;
    m_StartTime = 0;
    m_LastTime = 0;
    m_LastRedraw = 0;
  }

  unsigned long currTime = event.m_timeStamp - m_xsynct;
  
  if( evkey != m_Key )
  {
    m_Key = evkey;
    m_LastRedraw = m_StartTime = m_LastTime = currTime;
  }

  if( currTime >= m_LastRedraw )      // Redraw:
  {
    Action( m_Key, m_LastTime-m_StartTime, currTime-m_StartTime );
                  
    m_LastRedraw = wxStopWatch(&m_secbase) - m_gsynct;
    m_LastTime = currTime;
  }
}

void TestGLCanvas::OnKeyUp( wxKeyEvent& event )
{
  m_Key = 0;
  m_StartTime = 0;
  m_LastTime = 0;
  m_LastRedraw = 0;
}

void TestGLCanvas::Rotate( GLfloat deg )
{
    SetCurrent();

    glMatrixMode(GL_MODELVIEW);
    glRotatef((GLfloat)deg, 0.0F, 0.0F, 1.0F);
    Refresh(FALSE);
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
MyFrame::MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
                 const wxSize& size, long style) 
         : wxFrame(frame, -1, title, pos, size, style)
{
    m_canvas = NULL;
}

// Intercept menu commands
void MyFrame::OnExit(wxCommandEvent& event)
{
    Destroy();
}

void MyFrame::OnNewWindow()
{
  MyFrame *frame = new MyFrame(NULL, "Cube OpenGL Demo Clone",
                               wxPoint(50, 50), wxSize(400, 300));
  // Give it an icon
#ifdef wx_msw
  frame->SetIcon(wxIcon("mondrian"));
#endif

  // Make a menubar
  wxMenu *winMenu = new wxMenu;

  winMenu->Append(wxID_EXIT, "&Close");
  winMenu->Append(ID_NEW_WINDOW, "&New" );
  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(winMenu, "&Window");

  winMenu = new wxMenu;
  winMenu->Append(ID_DEF_ROTATE_LEFT_KEY, "Rotate &left");
  winMenu->Append(ID_DEF_ROTATE_RIGHT_KEY, "Rotate &right");
  menuBar->Append(winMenu, "&Key");

  frame->SetMenuBar(menuBar);

  frame->m_canvas = new TestGLCanvas( frame, *m_canvas, -1, 
               wxPoint(0, 0), wxSize(200, 200) );
  
  // Show the frame
  frame->Show(TRUE);
}

void MyFrame::OnDefRotateLeftKey()
{
  ScanCodeDialog dial( this, -1, m_canvas->m_rleft, 
                       wxString("Left"), "Define key" );
  int result = dial.ShowModal();
  if( result == wxID_OK )
    m_canvas->m_rleft = dial.GetValue();
}
void MyFrame::OnDefRotateRightKey()
{
  ScanCodeDialog dial( this, -1, m_canvas->m_rright, 
                       wxString("Right"), "Define key" );
  int result = dial.ShowModal();
  if( result == wxID_OK )
    m_canvas->m_rright = dial.GetValue();
}

/*------------------------------------------------------------------
  Application object ( equivalent to main() )
------------------------------------------------------------------ */

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit(void)
{
  wxLog::SetTraceMask(wxTraceMessages);

  // Create the main frame window
  MyFrame *frame = new MyFrame(NULL, "Cube OpenGL Demo", wxPoint(50, 50),
                               wxSize(400, 300));
  // Give it an icon
#ifdef wx_msw
  frame->SetIcon(wxIcon("mondrian"));
#endif

  // Make a menubar
  wxMenu *winMenu = new wxMenu;

  winMenu->Append(wxID_EXIT, "&Close");
  winMenu->Append(ID_NEW_WINDOW, "&New" );
  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(winMenu, "&Window");

  winMenu = new wxMenu;
  winMenu->Append(ID_DEF_ROTATE_LEFT_KEY, "Rotate &left");
  winMenu->Append(ID_DEF_ROTATE_RIGHT_KEY, "Rotate &right");
  menuBar->Append(winMenu, "&Key");

  frame->SetMenuBar(menuBar);

  frame->m_canvas = new TestGLCanvas(frame, -1, wxPoint(0, 0), wxSize(200, 200));

  // Show the frame
  frame->Show(TRUE);
  
  return TRUE;
}
