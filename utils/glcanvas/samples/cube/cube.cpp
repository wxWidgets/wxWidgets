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

#define ID_NEW_WINDOW 10000
#define ID_DEF_ROTATE_LEFT_KEY 10001
#define ID_DEF_ROTATE_RIGHT_KEY 10002

//////////////////////////////////////////////////////////////////////////////////
// Control to get a keycode

class CScanTextCtrl : public wxTextCtrl
{
public:
  CScanTextCtrl( wxWindow* parent, wxWindowID id, int code,
                 const wxPoint& pos, const wxSize& size );

  void OnChar( wxKeyEvent& event ) { } /* do nothing */
  void OnKeyDown(wxKeyEvent& event);

private:
// any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE( CScanTextCtrl, wxTextCtrl )
  EVT_CHAR( CScanTextCtrl::OnChar )
  EVT_KEY_DOWN( CScanTextCtrl::OnKeyDown )
END_EVENT_TABLE()

CScanTextCtrl::CScanTextCtrl( wxWindow* parent, wxWindowID id, int code,
                              const wxPoint& pos, const wxSize& size )
                  : wxTextCtrl( parent, id, "", pos, size )
{
  wxString buf;
  buf.Printf( "0x%04x", code );
  SetValue( buf );
}
void CScanTextCtrl::OnKeyDown( wxKeyEvent& event )
{
  #ifdef __WXDEBUG__
    wxLogTrace(wxTraceMessages, "[EVT_KEYDOWN]: Key = %04x, time = %d\n", event.KeyCode(),
            event.m_timeStamp );
  #endif // __WXDEBUG__

  wxString buf;
  buf.Printf( "0x%04x", event.KeyCode() );
  SetValue( buf );
}

///////////////////////////////////////////////////////////
// Dialog for defining a keypress

class CMenuKeyDialog : public wxDialog
{
public:
  CMenuKeyDialog( wxWindow* parent, wxWindowID id, const int code, const wxString &descr,
                  const wxString& title );
  int GetValue(); 

private:
  CScanTextCtrl      *m_ScanCode;
  wxTextCtrl         *m_Description;

// any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE( CMenuKeyDialog, wxDialog )
//
END_EVENT_TABLE()

CMenuKeyDialog::CMenuKeyDialog( wxWindow* parent, wxWindowID id, const int code,
                                 const wxString &descr, const wxString& title )
          : wxDialog( parent, id, title, wxPoint(-1, -1), wxSize(96*2,76*2) )
{
  new wxStaticText( this, -1, "Scancode", wxPoint(4*2,3*2), wxSize(31*2,12*2) );
  m_ScanCode = new CScanTextCtrl( this, -1, code, wxPoint(37*2,6*2), wxSize(53*2,14*2) );

  new wxStaticText( this, -1, "Description", wxPoint(4*2,24*2), wxSize(32*2,12*2) );
  m_Description = new wxTextCtrl( this, -1, descr, wxPoint(37*2,27*2), wxSize(53*2,14*2) );

  new wxButton( this, wxID_OK, "Ok", wxPoint(20*2,50*2), wxSize(20*2,13*2) );
  new wxButton( this, wxID_CANCEL, "Cancel", wxPoint(44*2,50*2), wxSize(25*2,13*2) );
}
int CMenuKeyDialog::GetValue()
{
  int code;
  wxString buf = m_ScanCode->GetValue();
  #ifdef __WXDEBUG__
    wxLogTrace(wxTraceMessages, buf.c_str() );
  #endif // __WXDEBUG__
  sscanf( buf.c_str(), "%i", &code );
  return( code );
}

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit(void)
{
  wxLog::SetTraceMask(wxTraceMessages);

  // Create the main frame window
  MyFrame *frame = new MyFrame(NULL, "Cube OpenGL Demo", wxPoint(50, 50), wxSize(400, 300));

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

void MyFrame::OnNewWindow()
{
  MyFrame *frame = new MyFrame(NULL, "Cube OpenGL Demo Clone", wxPoint(50, 50), wxSize(400, 300));

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
  CMenuKeyDialog dial( this, -1, m_canvas->m_rleft, wxString("Rotate left key"), "Define key" );
  int result = dial.ShowModal();
  if( result == wxID_OK )
    m_canvas->m_rleft = dial.GetValue();
}
void MyFrame::OnDefRotateRightKey()
{
  CMenuKeyDialog dial( this, -1, m_canvas->m_rright, wxString("Rotate right key"), "Define key" );
  int result = dial.ShowModal();
  if( result == wxID_OK )
    m_canvas->m_rright = dial.GetValue();
}

IMPLEMENT_APP(MyApp)

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MyFrame::OnExit)
    EVT_MENU( ID_NEW_WINDOW, MyFrame::OnNewWindow)
    EVT_MENU( ID_DEF_ROTATE_LEFT_KEY, MyFrame::OnDefRotateLeftKey)
    EVT_MENU( ID_DEF_ROTATE_RIGHT_KEY, MyFrame::OnDefRotateRightKey)
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

BEGIN_EVENT_TABLE(TestGLCanvas, wxGLCanvas)
    EVT_SIZE(TestGLCanvas::OnSize)
    EVT_PAINT(TestGLCanvas::OnPaint)
    EVT_ERASE_BACKGROUND(TestGLCanvas::OnEraseBackground)
    EVT_KEY_DOWN( TestGLCanvas::OnKeyDown )
    EVT_KEY_UP( TestGLCanvas::OnKeyUp )
END_EVENT_TABLE()

TestGLCanvas::TestGLCanvas(wxWindow *parent, wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name):
  wxGLCanvas(parent, NULL, id, pos, size, style, name )
{
  m_init = FALSE;
  m_gllist = 0;
  m_rleft = 0x13b;
  m_rright = 0x13d;
}

TestGLCanvas::TestGLCanvas(wxWindow *parent, const TestGLCanvas &other, 
    wxWindowID id, const wxPoint& pos, const wxSize& size, long style,
    const wxString& name ) :
      wxGLCanvas(parent, other.GetContext(), id, pos, size, style, name  )
{
  m_init = FALSE;
  m_gllist = other.m_gllist;    /* share display list */
  m_rleft = 0x13b;
  m_rright = 0x13d;

//  SetBackgroundColour( *wxGREEN );
// virtual void SetBackgroundColour(const wxColour& colour)

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
  printf( "in erase background\n" );
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

void TestGLCanvas::OnKeyDown( wxKeyEvent& event )
{
  if( event.KeyCode() == m_rleft ) Rotate( -5.0 );
  else if( event.KeyCode() == m_rright ) Rotate( 5.0 );
  #ifdef __WXDEBUG__
    wxLogTrace(wxTraceMessages, "[EVT_KEYDOWN]: Key = %04x, time = %d\n", event.KeyCode(),
            event.m_timeStamp );
  #endif // __WXDEBUG__
}
void TestGLCanvas::OnKeyUp( wxKeyEvent& event )
{
  #ifdef __WXDEBUG__
    wxLogTrace(wxTraceMessages, "[EVT_KEYUP]: Key = %04x, time = %d\n", event.KeyCode(),
            event.m_timeStamp );
  #endif // __WXDEBUG__
}

void TestGLCanvas::Rotate( double deg )
{
    SetCurrent();

    glMatrixMode(GL_MODELVIEW);
    glRotatef((GLfloat)deg, 0.0F, 0.0F, 1.0F);
    Refresh(FALSE);
}

