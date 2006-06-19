/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/frame.cpp
// Purpose:     wxFrame
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/frame.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/dcclient.h"
    #include "wx/menu.h"
    #include "wx/dialog.h"
    #include "wx/settings.h"
    #include "wx/toolbar.h"
    #include "wx/statusbr.h"
    #include "wx/menuitem.h"
#endif // WX_PRECOMP

#include "wx/mac/uma.h"

extern wxWindowList wxModelessWindows;

BEGIN_EVENT_TABLE(wxFrame, wxFrameBase)
  EVT_ACTIVATE(wxFrame::OnActivate)
 // EVT_MENU_HIGHLIGHT_ALL(wxFrame::OnMenuHighlight)
  EVT_SYS_COLOUR_CHANGED(wxFrame::OnSysColourChanged)
//  EVT_IDLE(wxFrame::OnIdle)
//  EVT_CLOSE(wxFrame::OnCloseWindow)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxFrame, wxTopLevelWindow)

#define WX_MAC_STATUSBAR_HEIGHT 15
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

    m_iconized = false;

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
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE));

    if ( !wxTopLevelWindow::Create(parent, id, title, pos, size, style, name) )
        return false;

    MacCreateRealWindow( title, pos , size , MacRemoveBordersFromStyle(style) , name ) ;

    m_macWindowBackgroundTheme = kThemeBrushDocumentWindowBackground ;
    SetThemeWindowBackground( (WindowRef) m_macWindow , m_macWindowBackgroundTheme , false ) ;

    wxModelessWindows.Append(this);

    return true;
}

wxFrame::~wxFrame()
{
    m_isBeingDeleted = true;
    DeleteAllBars();
}


bool wxFrame::Enable(bool enable)
{
    if ( !wxWindow::Enable(enable) )
        return false;

    if ( m_frameMenuBar && m_frameMenuBar == wxMenuBar::MacGetInstalledMenuBar() )
    {
        int iMaxMenu = m_frameMenuBar->GetMenuCount();
        for ( int i = 0 ; i < iMaxMenu ; ++ i )
        {
            m_frameMenuBar->EnableTop( i , enable ) ;
        }
    }

    return true;
}

wxStatusBar *wxFrame::OnCreateStatusBar(int number, long style, wxWindowID id,
    const wxString& name)
{
    wxStatusBar *statusBar = NULL;

    statusBar = new wxStatusBar(this, id,
        style, name);
    statusBar->SetSize( 100 , 15 ) ;
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
        m_frameStatusBar->SetSize(0, h, w, sh);
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
