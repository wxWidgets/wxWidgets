/////////////////////////////////////////////////////////////////////////////
// Name:        frame.cpp
// Purpose:     wxFrameMac
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
#include "wx/statusbr.h"
#include "wx/toolbar.h"
#include "wx/menuitem.h"
#include "wx/menu.h"
#include "wx/dcclient.h"
#include "wx/dialog.h"
#include "wx/settings.h"
#include "wx/app.h"

#include <wx/mac/uma.h>

extern wxWindowList wxModelessWindows;
extern wxList wxPendingDelete;

#if !USE_SHARED_LIBRARY
BEGIN_EVENT_TABLE(wxFrameMac, wxFrameBase)
  EVT_ACTIVATE(wxFrameMac::OnActivate)
 // EVT_MENU_HIGHLIGHT_ALL(wxFrameMac::OnMenuHighlight)
  EVT_SYS_COLOUR_CHANGED(wxFrameMac::OnSysColourChanged)
//  EVT_IDLE(wxFrameMac::OnIdle)
//  EVT_CLOSE(wxFrameMac::OnCloseWindow)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxFrameMac, wxWindow)
#endif
#ifndef __WXUNIVERSAL__
IMPLEMENT_DYNAMIC_CLASS(wxFrame, wxFrameMac)
#endif

#if wxUSE_NATIVE_STATUSBAR
bool wxFrameMac::m_useNativeStatusBar = TRUE;
#else
bool wxFrameMac::m_useNativeStatusBar = FALSE;
#endif

#define WX_MAC_STATUSBAR_HEIGHT 15 
// ----------------------------------------------------------------------------
// creation/destruction
// ----------------------------------------------------------------------------

void wxFrameMac::Init()
{
  m_frameMenuBar = NULL;

#if wxUSE_TOOLBAR
  m_frameToolBar = NULL ;
#endif
  m_frameStatusBar = NULL;
  m_winLastFocused = NULL ;

    m_iconized = FALSE;

#if wxUSE_TOOLTIPS
    m_hwndToolTip = 0;
#endif
}

wxPoint wxFrameMac::GetClientAreaOrigin() const
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

bool wxFrameMac::Create(wxWindow *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
  SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_APPWORKSPACE));

    if ( !wxTopLevelWindow::Create(parent, id, title, pos, size, style, name) )
        return FALSE;

  MacCreateRealWindow( title, pos , size , MacRemoveBordersFromStyle(style) , name ) ;
  
	m_macWindowData->m_macWindowBackgroundTheme = kThemeBrushDocumentWindowBackground ;

  wxModelessWindows.Append(this);

  return TRUE;
}

wxFrameMac::~wxFrameMac()
{
  m_isBeingDeleted = TRUE;

  DeleteAllBars();

}


bool wxFrameMac::Enable(bool enable)
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

wxStatusBar *wxFrameMac::OnCreateStatusBar(int number, long style, wxWindowID id,
    const wxString& name)
{
    wxStatusBar *statusBar = NULL;

    statusBar = new wxStatusBar(this, id,
        style, name);
	statusBar->SetSize( 100 , 15 ) ;
    statusBar->SetFieldsCount(number);
    return statusBar;
}

void wxFrameMac::PositionStatusBar()
{
  if (m_frameStatusBar )
  {
    int w, h;
    GetClientSize(&w, &h);
    int sw, sh;
    m_frameStatusBar->GetSize(&sw, &sh);

    // Since we wish the status bar to be directly under the client area,
    // we use the adjusted sizes without using wxSIZE_NO_ADJUSTMENTS.
    m_frameStatusBar->SetSize(0, h, w, sh);
   }
}

// Responds to colour changes, and passes event on to children.
void wxFrameMac::OnSysColourChanged(wxSysColourChangedEvent& event)
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
void wxFrameMac::OnActivate(wxActivateEvent& event)
{
    if ( !event.GetActive() )
    {
       // remember the last focused child if it is our child
        m_winLastFocused = FindFocus();

        // so we NULL it out if it's a child from some other frame
        wxWindow *win = m_winLastFocused;
        while ( win )
        {
            if ( win->IsTopLevel() )
            {
                if ( win != this )
                {
                    m_winLastFocused = NULL;
                }

                break;
            }

            win = win->GetParent();
        }

        event.Skip();
    }
	else
	{
        // restore focus to the child which was last focused
        wxWindow *parent = m_winLastFocused ? m_winLastFocused->GetParent()
                                            : NULL;
        if ( !parent )
        {
            parent = this;
        }

    	wxSetFocusToChild(parent, &m_winLastFocused);

	    if ( m_frameMenuBar != NULL )
	    {
	    	m_frameMenuBar->MacInstallMenuBar() ;
	    }
	}
}

void wxFrameMac::DoGetClientSize(int *x, int *y) const
{
	wxWindow::DoGetClientSize( x , y ) ;

#if wxUSE_STATUSBAR
  if ( GetStatusBar() && y )
  {
    int statusX, statusY;
    GetStatusBar()->GetClientSize(&statusX, &statusY);
    *y -= statusY;
  }
#endif // wxUSE_STATUSBAR

  wxPoint pt(GetClientAreaOrigin());
  if ( y )
    *y -= pt.y;
  if ( x ) 
    *x -= pt.x;
}

void wxFrameMac::DoSetClientSize(int clientwidth, int clientheight)
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
wxToolBar* wxFrameMac::CreateToolBar(long style, wxWindowID id, const wxString& name)
{
    if ( wxFrameBase::CreateToolBar(style, id, name) )
    {
        PositionToolBar();
    }

    return m_frameToolBar;
}

void wxFrameMac::PositionToolBar()
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
