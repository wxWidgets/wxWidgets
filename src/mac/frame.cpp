/////////////////////////////////////////////////////////////////////////////
// Name:        frame.cpp
// Purpose:     wxFrame
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "frame.h"
#endif

#include "wx/frame.h"
#include "wx/mac/statusbr.h"
#include "wx/toolbar.h"
#include "wx/menuitem.h"
#include "wx/menu.h"
#include "wx/dcclient.h"
#include "wx/dialog.h"
#include "wx/settings.h"
#include "wx/app.h"

#include <wx/mac/uma.h>

extern wxList wxModelessWindows;
extern wxList wxPendingDelete;

#if !USE_SHARED_LIBRARY
BEGIN_EVENT_TABLE(wxFrame, wxFrameBase)
//  EVT_SIZE(wxFrame::OnSize)
  EVT_ACTIVATE(wxFrame::OnActivate)
 // EVT_MENU_HIGHLIGHT_ALL(wxFrame::OnMenuHighlight)
  EVT_SYS_COLOUR_CHANGED(wxFrame::OnSysColourChanged)
//  EVT_IDLE(wxFrame::OnIdle)
//  EVT_CLOSE(wxFrame::OnCloseWindow)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxFrame, wxWindow)
#endif

#if wxUSE_NATIVE_STATUSBAR
bool wxFrame::m_useNativeStatusBar = TRUE;
#else
bool wxFrame::m_useNativeStatusBar = FALSE;
#endif

#define WX_MAC_STATUSBAR_HEIGHT 15 
// ----------------------------------------------------------------------------
// creation/destruction
// ----------------------------------------------------------------------------

void wxFrame::Init()
{
    m_iconized = FALSE;

#if wxUSE_TOOLTIPS
    m_hwndToolTip = 0;
#endif
}

wxPoint wxFrame::GetClientAreaOrigin() const
{
	// on mac we are at position -1,-1 with the control
    wxPoint pt(0, 0);

#if wxUSE_TOOLBAR
    if ( GetToolBar() )
    {
        int w, h;
        GetToolBar()->GetSize(& w, & h);

        if ( GetToolBar()->GetWindowStyleFlag() & wxTB_VERTICAL )
        {
            pt.x += w - 1;
        }
        else
        {
            pt.y += h - 1 ;
        }
    }
#endif // wxUSE_TOOLBAR

    return pt;
}

bool wxFrame::Create(wxWindow *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
  m_frameMenuBar = NULL;

#if wxUSE_TOOLBAR
  m_frameToolBar = NULL ;
#endif
  m_frameStatusBar = NULL;

  SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_APPWORKSPACE));

  if ( id > -1 )
    m_windowId = id;
  else
    m_windowId = (int)NewControlId();

  if (parent) parent->AddChild(this);

  if (!parent)
    wxTopLevelWindows.Append(this);

  MacCreateRealWindow( title, pos , size , MacRemoveBordersFromStyle(style) , name ) ;
  
	m_macWindowData->m_macWindowBackgroundTheme = kThemeBrushDocumentWindowBackground ;

  wxModelessWindows.Append(this);

  return TRUE;
}

wxFrame::~wxFrame()
{
  m_isBeingDeleted = TRUE;
  wxTopLevelWindows.DeleteObject(this);

  DeleteAllBars();

/* Check if it's the last top-level window */

  if (wxTheApp && (wxTopLevelWindows.Number() == 0))
  {
    wxTheApp->SetTopWindow(NULL);

    if (wxTheApp->GetExitOnFrameDelete())
    {
       wxTheApp->ExitMainLoop() ;
    }
  }

  wxModelessWindows.DeleteObject(this);
}


bool wxFrame::Enable(bool enable)
{
    if ( !wxWindow::Enable(enable) )
        return FALSE;

	if ( m_frameMenuBar && m_frameMenuBar == wxMenuBar::MacGetInstalledMenuBar() )
	{
		for ( int i = 0 ; i < m_frameMenuBar->GetMenuCount() ; ++ i )
		{
			m_frameMenuBar->EnableTop( i , enable ) ;
		}
	}

    return TRUE;
}
// Equivalent to maximize/restore in Windows
void wxFrame::Maximize(bool maximize)
{
    // TODO
}

bool wxFrame::IsIconized() const
{
    // TODO
    return FALSE;
}

void wxFrame::Iconize(bool iconize)
{
    // TODO
}

// Is the frame maximized?
bool wxFrame::IsMaximized(void) const
{
    // TODO
    return FALSE;
}

void wxFrame::Restore()
{
    // TODO
}

void wxFrame::SetIcon(const wxIcon& icon)
{
   wxFrameBase::SetIcon(icon);
}

wxStatusBar *wxFrame::OnCreateStatusBar(int number, long style, wxWindowID id,
    const wxString& name)
{
    wxStatusBar *statusBar = NULL;

    statusBar = new wxStatusBar(this, id, style, name);

    statusBar->SetFieldsCount(number);
    return statusBar;
}

void wxFrame::PositionStatusBar()
{
  if (m_frameStatusBar )
  {
    int w, h;
    GetClientSize(&w, &h);
    int sw, sh;
    m_frameStatusBar->GetSize(&sw, &sh);

    // Since we wish the status bar to be directly under the client area,
    // we use the adjusted sizes without using wxSIZE_NO_ADJUSTMENTS.
    m_frameStatusBar->SetSize(0, h, w, WX_MAC_STATUSBAR_HEIGHT );
   }
}

void wxFrame::SetMenuBar(wxMenuBar *menuBar)
{
    if (!menuBar)
    {
        return;
    }
  
    m_frameMenuBar = NULL;
    m_frameMenuBar = menuBar;
    m_frameMenuBar->MacInstallMenuBar() ;
    m_frameMenuBar->Attach(this);
}


// Responds to colour changes, and passes event on to children.
void wxFrame::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_APPWORKSPACE));
    Refresh();

    if ( m_frameStatusBar )
    {
        wxSysColourChangedEvent event2;
        event2.SetEventObject( m_frameStatusBar );
        m_frameStatusBar->ProcessEvent(event2);
    }

    // Propagate the event to the non-top-level children
    wxWindow::OnSysColourChanged(event);
}


// Default activation behaviour - set the focus for the first child
// subwindow found.
void wxFrame::OnActivate(wxActivateEvent& event)
{
    if ( !event.GetActive() )
    {
        event.Skip();
        return;
    }

    for ( wxWindowList::Node *node = GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        // FIXME all this is totally bogus - we need to do the same as wxPanel,
        //       but how to do it without duplicating the code?

        // restore focus
        wxWindow *child = node->GetData();

        if ( !child->IsTopLevel()
#if wxUSE_TOOLBAR
             && !wxDynamicCast(child, wxToolBar)
#endif // wxUSE_TOOLBAR
#if wxUSE_STATUSBAR
             && !wxDynamicCast(child, wxStatusBar)
#endif // wxUSE_STATUSBAR
           )
        {
            child->SetFocus();
            return;
        }
    }
}

void wxFrame::DoGetClientSize(int *x, int *y) const
{
	wxWindow::DoGetClientSize( x , y ) ;

#if wxUSE_STATUSBAR
  if ( GetStatusBar() )
  {
    int statusX, statusY;
    GetStatusBar()->GetClientSize(&statusX, &statusY);
    // right now this is a constant, this might change someday
    *y -= WX_MAC_STATUSBAR_HEIGHT ;
  }
#endif // wxUSE_STATUSBAR

  wxPoint pt(GetClientAreaOrigin());
  *y -= pt.y;
  *x -= pt.x;
}

void wxFrame::DoSetClientSize(int clientwidth, int clientheight)
{
	int currentclientwidth , currentclientheight ;
	int currentwidth , currentheight ;
	
	GetClientSize( &currentclientwidth , &currentclientheight ) ;
	GetSize( &currentwidth , &currentheight ) ;
	
	// find the current client size

  // Find the difference between the entire window (title bar and all)
  // and the client area; add this to the new client size to move the
  // window

	DoSetSize( -1 , -1 , currentwidth + clientwidth - currentclientwidth ,
		currentheight + clientheight - currentclientheight , wxSIZE_USE_EXISTING ) ;
}


#if wxUSE_TOOLBAR
wxToolBar* wxFrame::CreateToolBar(long style, wxWindowID id, const wxString& name)
{
    if ( wxFrameBase::CreateToolBar(style, id, name) )
    {
        PositionToolBar();
    }

    return m_frameToolBar;
}

void wxFrame::PositionToolBar()
{
    int cw, ch;

	cw = m_width ;
	ch = m_height ;

    if ( GetStatusBar() )
    {
      int statusX, statusY;
      GetStatusBar()->GetClientSize(&statusX, &statusY);
      ch -= statusY;
    }

    if (GetToolBar())
    {
        int tw, th;
        GetToolBar()->GetSize(& tw, & th);

        if (GetToolBar()->GetWindowStyleFlag() & wxTB_VERTICAL)
        {
            // Use the 'real' position. wxSIZE_NO_ADJUSTMENTS
            // means, pretend we don't have toolbar/status bar, so we
            // have the original client size.
            GetToolBar()->SetSize(-1, -1, tw, ch + 2 , wxSIZE_NO_ADJUSTMENTS | wxSIZE_ALLOW_MINUS_ONE );
        }
        else
        {
            // Use the 'real' position
            GetToolBar()->SetSize(-1, -1, cw + 2, th, wxSIZE_NO_ADJUSTMENTS | wxSIZE_ALLOW_MINUS_ONE );
        }
    }
}
#endif
