/////////////////////////////////////////////////////////////////////////////
// Name:        frame.cpp
// Purpose:     wxFrame
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
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

#include "wx/mac/uma.h"

extern wxWindowList wxModelessWindows;
extern wxList wxPendingDelete;

#if !USE_SHARED_LIBRARY
BEGIN_EVENT_TABLE(wxFrame, wxFrameBase)
  EVT_ACTIVATE(wxFrame::OnActivate)
 // EVT_MENU_HIGHLIGHT_ALL(wxFrame::OnMenuHighlight)
  EVT_SYS_COLOUR_CHANGED(wxFrame::OnSysColourChanged)
//  EVT_IDLE(wxFrame::OnIdle)
//  EVT_CLOSE(wxFrame::OnCloseWindow)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxFrame, wxTopLevelWindow)
#endif

#define WX_MAC_STATUSBAR_HEIGHT 18 
// ----------------------------------------------------------------------------
// creation/destruction
// ----------------------------------------------------------------------------

void wxFrame::Init()
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

bool wxFrame::Create(wxWindow *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    
    if ( !wxTopLevelWindow::Create(parent, id, title, pos, size, style, name) )
        return FALSE;
    
    wxModelessWindows.Append(this);
    
    return TRUE;
}

wxFrame::~wxFrame()
{
    m_isBeingDeleted = TRUE;
    DeleteAllBars();
}


bool wxFrame::Enable(bool enable)
{
    if ( !wxWindow::Enable(enable) )
        return FALSE;

    if ( m_frameMenuBar && m_frameMenuBar == wxMenuBar::MacGetInstalledMenuBar() )
    {
         int iMaxMenu = m_frameMenuBar->GetMenuCount(); 
        for ( int i = 0 ; i < iMaxMenu ; ++ i )
        {
            m_frameMenuBar->EnableTop( i , enable ) ;
        }
    }

    return TRUE;
}

wxStatusBar *wxFrame::OnCreateStatusBar(int number, long style, wxWindowID id,
    const wxString& name)
{
    wxStatusBar *statusBar = NULL;

    statusBar = new wxStatusBar(this, id,
        style, name);
    statusBar->SetSize( 100 , WX_MAC_STATUSBAR_HEIGHT ) ;
    statusBar->SetFieldsCount(number);
    return statusBar;
}

void wxFrame::PositionStatusBar()
{
    if (m_frameStatusBar && m_frameStatusBar->IsShown() )
    {
        int w, h;
        GetClientSize(&w, &h);
        
        // Since we wish the status bar to be directly under the client area,
        // we use the adjusted sizes without using wxSIZE_NO_ADJUSTMENTS.
        m_frameStatusBar->SetSize(0, h, w, WX_MAC_STATUSBAR_HEIGHT);
    }
}

// Responds to colour changes, and passes event on to children.
void wxFrame::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE));
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
        else if (wxTheApp->GetTopWindow() && wxTheApp->GetTopWindow()->IsKindOf(CLASSINFO(wxFrame)))
        {
            // Trying toplevel frame menbar
            if( ((wxFrame*)wxTheApp->GetTopWindow())->GetMenuBar() )
                ((wxFrame*)wxTheApp->GetTopWindow())->GetMenuBar()->MacInstallMenuBar();
         }
    }
}

void wxFrame::DetachMenuBar()
{
    if ( m_frameMenuBar )
    {
        m_frameMenuBar->UnsetInvokingWindow();
    }

    wxFrameBase::DetachMenuBar();
}

void wxFrame::AttachMenuBar( wxMenuBar *menuBar )
{
    wxFrameBase::AttachMenuBar(menuBar);

    if (m_frameMenuBar)
    {
        m_frameMenuBar->SetInvokingWindow( this );
    }
}

void wxFrame::DoGetClientSize(int *x, int *y) const
{
    wxTopLevelWindow::DoGetClientSize( x , y ) ;
    
#if wxUSE_STATUSBAR
    if ( GetStatusBar() && GetStatusBar()->IsShown() && y )
    {
        if ( y) *y -= WX_MAC_STATUSBAR_HEIGHT;
    }
#endif // wxUSE_STATUSBAR
    
#if wxUSE_TOOLBAR
    wxToolBar *toolbar = GetToolBar();
    if ( toolbar && toolbar->IsShown() )
    {
        int w, h;
        toolbar->GetSize(&w, &h);

        if ( toolbar->GetWindowStyleFlag() & wxTB_VERTICAL )
        {
            if ( x )  *x -= w;
        }
        else
        {
            if ( y )  *y -= h;
        }
    }
#endif // wxUSE_TOOLBAR
}

void wxFrame::DoSetClientSize(int clientwidth, int clientheight)
{
    int currentclientwidth , currentclientheight ;
    int currentwidth , currentheight ;
    
    GetClientSize( &currentclientwidth , &currentclientheight ) ;
    if ( clientwidth == -1 )
        clientwidth = currentclientwidth ;
    if ( clientheight == -1 )
        clientheight = currentclientheight ;
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

    GetSize( &cw , &ch ) ;

    if ( GetStatusBar() && GetStatusBar()->IsShown())
    {
      int statusX, statusY;
      GetStatusBar()->GetClientSize(&statusX, &statusY);
      ch -= statusY;
    }

    if (GetToolBar())
    {
        int tx, ty, tw, th;
        tx = ty = 0 ;
        
        GetToolBar()->GetSize(& tw, & th);
        if (GetToolBar()->GetWindowStyleFlag() & wxTB_VERTICAL)
        {
            // Use the 'real' position. wxSIZE_NO_ADJUSTMENTS
            // means, pretend we don't have toolbar/status bar, so we
            // have the original client size.
            GetToolBar()->SetSize(tx , ty , tw, ch , wxSIZE_NO_ADJUSTMENTS );
        }
        else
        {
            // Use the 'real' position
            GetToolBar()->SetSize(tx , ty , cw , th, wxSIZE_NO_ADJUSTMENTS );
        }
    }
}
#endif
