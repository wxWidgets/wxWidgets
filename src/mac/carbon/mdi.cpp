/////////////////////////////////////////////////////////////////////////////
// Name:        mdi.cpp
// Purpose:     MDI classes
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "mdi.h"
#endif

#include "wx/mdi.h"
#include "wx/menu.h"
#include "wx/settings.h"

#include "wx/mac/private.h"

extern wxWindowList wxModelessWindows;

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxMDIParentFrame, wxFrame)
IMPLEMENT_DYNAMIC_CLASS(wxMDIChildFrame, wxFrame)
IMPLEMENT_DYNAMIC_CLASS(wxMDIClientWindow, wxWindow)

BEGIN_EVENT_TABLE(wxMDIParentFrame, wxFrame)
  EVT_SIZE(wxMDIParentFrame::OnSize)
  EVT_ACTIVATE(wxMDIParentFrame::OnActivate)
  EVT_SYS_COLOUR_CHANGED(wxMDIParentFrame::OnSysColourChanged)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxMDIClientWindow, wxWindow)
  EVT_SCROLL(wxMDIClientWindow::OnScroll)
END_EVENT_TABLE()

#endif

static const int IDM_WINDOWTILE  = 4001;
static const int IDM_WINDOWTILEHOR  = 4001;
static const int IDM_WINDOWCASCADE = 4002;
static const int IDM_WINDOWICONS = 4003;
static const int IDM_WINDOWNEXT = 4004;
static const int IDM_WINDOWTILEVERT = 4005;
static const int IDM_WINDOWPREV = 4006;

// This range gives a maximum of 500 MDI children. Should be enough :-)
static const int wxFIRST_MDI_CHILD = 4100;
static const int wxLAST_MDI_CHILD = 4600;

// Status border dimensions
static const int wxTHICK_LINE_BORDER = 3;

// Parent frame

wxMDIParentFrame::wxMDIParentFrame()
{
    m_clientWindow = NULL;
    m_currentChild = NULL;
    m_windowMenu = (wxMenu*) NULL;
    m_parentFrameActive = TRUE;
}

bool wxMDIParentFrame::Create(wxWindow *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
  m_clientWindow = NULL;
  m_currentChild = NULL;

  // this style can be used to prevent a window from having the standard MDI
  // "Window" menu
  if ( style & wxFRAME_NO_WINDOW_MENU )
  {
      m_windowMenu = (wxMenu *)NULL;
      style -= wxFRAME_NO_WINDOW_MENU ;
  }
  else // normal case: we have the window menu, so construct it
  {
      m_windowMenu = new wxMenu;

      m_windowMenu->Append(IDM_WINDOWCASCADE, wxT("&Cascade"));
      m_windowMenu->Append(IDM_WINDOWTILEHOR, wxT("Tile &Horizontally"));
      m_windowMenu->Append(IDM_WINDOWTILEVERT, wxT("Tile &Vertically"));
      m_windowMenu->AppendSeparator();
      m_windowMenu->Append(IDM_WINDOWICONS, wxT("&Arrange Icons"));
      m_windowMenu->Append(IDM_WINDOWNEXT, wxT("&Next"));
  }

  wxFrame::Create( parent , id , title , wxPoint( 2000 , 2000 ) , size , style , name ) ;
  m_parentFrameActive = TRUE;
        
  OnCreateClient();

    return TRUE;
}

wxMDIParentFrame::~wxMDIParentFrame()
{
    DestroyChildren();
    // already delete by DestroyChildren()
    m_frameToolBar = NULL;
    m_frameStatusBar = NULL;
    m_clientWindow = NULL ;

    if (m_windowMenu)
    {
        delete m_windowMenu;
        m_windowMenu = (wxMenu*) NULL;
    }

    if ( m_clientWindow )
    {
        delete m_clientWindow;
        m_clientWindow = NULL ;
    }
}


// Get size *available for subwindows* i.e. excluding menu bar.
void wxMDIParentFrame::DoGetClientSize(int *x, int *y) const
{
	wxDisplaySize( x , y ) ;
}

void wxMDIParentFrame::SetMenuBar(wxMenuBar *menu_bar)
{
	wxFrame::SetMenuBar( menu_bar ) ;
}

void wxMDIParentFrame::OnSize(wxSizeEvent& event)
{
#if wxUSE_CONSTRAINTS
    if (GetAutoLayout())
      Layout();
#endif
    int x = 0;
    int y = 0;
    int width, height;
    GetClientSize(&width, &height);

    if ( GetClientWindow() )
        GetClientWindow()->SetSize(x, y, width, height);
}

void wxMDIParentFrame::OnActivate(wxActivateEvent& event)
{
  if ( m_currentChild && event.GetActive() )
  {
    wxActivateEvent event(wxEVT_ACTIVATE, TRUE, m_currentChild->GetId());
    event.SetEventObject( m_currentChild );
    m_currentChild->GetEventHandler()->ProcessEvent(event) ;
  }
  else if ( event.GetActive() )
  {
  	  if ( m_frameMenuBar != NULL )
	    {
	    	  m_frameMenuBar->MacInstallMenuBar() ;
	    }

  }
}

// Returns the active MDI child window
wxMDIChildFrame *wxMDIParentFrame::GetActiveChild() const
{
  return m_currentChild ;
}

// Create the client window class (don't Create the window,
// just return a new class)
wxMDIClientWindow *wxMDIParentFrame::OnCreateClient()
{
    m_clientWindow = new wxMDIClientWindow( this );
    return m_clientWindow;
}

// Responds to colour changes, and passes event on to children.
void wxMDIParentFrame::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    // TODO

    // Propagate the event to the non-top-level children
    wxFrame::OnSysColourChanged(event);
}

// MDI operations
void wxMDIParentFrame::Cascade()
{
    // TODO
}

void wxMDIParentFrame::Tile()
{
    // TODO
}

void wxMDIParentFrame::ArrangeIcons()
{
    // TODO
}

void wxMDIParentFrame::ActivateNext()
{
    // TODO
}

void wxMDIParentFrame::ActivatePrevious()
{
    // TODO
}

// Child frame

wxMDIChildFrame::wxMDIChildFrame()
{
    Init() ;
}
void wxMDIChildFrame::Init()
{
}

bool wxMDIChildFrame::Create(wxMDIParentFrame *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    SetName(name);

    if ( id > -1 )
        m_windowId = id;
    else
        m_windowId = (int)NewControlId();

    if (parent) parent->AddChild(this);

  	MacCreateRealWindow( title, pos , size , MacRemoveBordersFromStyle(style) , name ) ;
  
	m_macWindowBackgroundTheme = kThemeBrushDocumentWindowBackground ;
	SetThemeWindowBackground( (WindowRef) m_macWindow , m_macWindowBackgroundTheme , false ) ;

    wxModelessWindows.Append(this);
    return FALSE;
}

wxMDIChildFrame::~wxMDIChildFrame()
{
    DestroyChildren();
    // already delete by DestroyChildren()
    m_frameToolBar = NULL;
    m_frameStatusBar = NULL;
}

void wxMDIChildFrame::SetMenuBar(wxMenuBar *menu_bar)
{
	return wxFrame::SetMenuBar( menu_bar ) ;
}

// MDI operations
void wxMDIChildFrame::Maximize()
{
    wxFrame::Maximize() ;
}

void wxMDIChildFrame::Restore()
{
    wxFrame::Restore() ;
}

void wxMDIChildFrame::Activate()
{
}

//-----------------------------------------------------------------------------
// wxMDIClientWindow
//-----------------------------------------------------------------------------

wxMDIClientWindow::wxMDIClientWindow()
{
}

wxMDIClientWindow::~wxMDIClientWindow()
{
    DestroyChildren();
}

bool wxMDIClientWindow::CreateClient(wxMDIParentFrame *parent, long style)
{

    m_windowId = (int)NewControlId();

    if ( parent )
    {
       parent->AddChild(this);
    }
    m_backgroundColour = wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE);

    wxModelessWindows.Append(this);
    return TRUE;
}

// Explicitly call default scroll behaviour
void wxMDIClientWindow::OnScroll(wxScrollEvent& event)
{
}

