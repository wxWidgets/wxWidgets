/////////////////////////////////////////////////////////////////////////////
// Name:        common/framecmn.cpp
// Purpose:     common (for all platforms) wxFrame functions
// Author:      Julian Smart, Vadim Zeitlin
// Created:     01/02/97
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "framebase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/frame.h"
#include "wx/menu.h"
#include "wx/menuitem.h"
#include "wx/dcclient.h"

#if wxUSE_TOOLBAR
    #include "wx/toolbar.h"
#endif
#if wxUSE_STATUSBAR
    #include "wx/statusbr.h"
#endif

// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxFrameBase, wxWindow)
    EVT_IDLE(wxFrameBase::OnIdle)
    EVT_CLOSE(wxFrameBase::OnCloseWindow)
    EVT_MENU_HIGHLIGHT_ALL(wxFrameBase::OnMenuHighlight)
    EVT_SIZE(wxFrameBase::OnSize)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// construction/destruction
// ----------------------------------------------------------------------------

wxFrameBase::wxFrameBase()
{
    m_frameMenuBar = NULL;

#if wxUSE_TOOLBAR
    m_frameToolBar = NULL;
#endif // wxUSE_TOOLBAR

#if wxUSE_STATUSBAR
    m_frameStatusBar = NULL;
#endif // wxUSE_STATUSBAR
}

bool wxFrameBase::Destroy()
{
    // delayed destruction: the frame will be deleted during the next idle
    // loop iteration
    if ( !wxPendingDelete.Member(this) )
        wxPendingDelete.Append(this);

    return TRUE;
}

wxFrame *wxFrameBase::New(wxWindow *parent,
                          wxWindowID id,
                          const wxString& title,
                          const wxPoint& pos,
                          const wxSize& size,
                          long style,
                          const wxString& name)
{
    return new wxFrame(parent, id, title, pos, size, style, name);
}

void wxFrameBase::DeleteAllBars()
{
    if ( m_frameMenuBar )
    {
        delete m_frameMenuBar;
        m_frameMenuBar = (wxMenuBar *) NULL;
    }

#if wxUSE_STATUSBAR
    if ( m_frameStatusBar )
    {
        delete m_frameStatusBar;
        m_frameStatusBar = (wxStatusBar *) NULL;
    }
#endif // wxUSE_STATUSBAR

#if wxUSE_TOOLBAR
    if ( m_frameToolBar )
    {
        delete m_frameToolBar;
        m_frameToolBar = (wxToolBar *) NULL;
    }
#endif // wxUSE_TOOLBAR
}

// ----------------------------------------------------------------------------
// wxFrame size management: we exclude the areas taken by menu/status/toolbars
// from the client area, so the client area is what's really available for the
// frame contents
// ----------------------------------------------------------------------------

// get the origin of the client area in the client coordinates
wxPoint wxFrameBase::GetClientAreaOrigin() const
{
    wxPoint pt(0, 0);

#if wxUSE_TOOLBAR
    if ( GetToolBar() )
    {
        int w, h;
        GetToolBar()->GetSize(& w, & h);

        if ( GetToolBar()->GetWindowStyleFlag() & wxTB_VERTICAL )
        {
            pt.x += w;
        }
        else
        {
            pt.y += h;
        }
    }
#endif // wxUSE_TOOLBAR

    return pt;
}

void wxFrameBase::DoScreenToClient(int *x, int *y) const
{
    wxWindow::DoScreenToClient(x, y);

    // We may be faking the client origin.
    // So a window that's really at (0, 30) may appear
    // (to wxWin apps) to be at (0, 0).
    wxPoint pt(GetClientAreaOrigin());
    *x -= pt.x;
    *y -= pt.y;
}

void wxFrameBase::DoClientToScreen(int *x, int *y) const
{
    // We may be faking the client origin.
    // So a window that's really at (0, 30) may appear
    // (to wxWin apps) to be at (0, 0).
    wxPoint pt1(GetClientAreaOrigin());
    *x += pt1.x;
    *y += pt1.y;

    wxWindow::DoClientToScreen(x, y);
}

// ----------------------------------------------------------------------------
// misc
// ----------------------------------------------------------------------------

// make the window modal (all other windows unresponsive)
void wxFrameBase::MakeModal(bool modal)
{
    if ( modal )
    {
        wxEnableTopLevelWindows(FALSE);
        Enable(TRUE);           // keep this window enabled
    }
    else
    {
        wxEnableTopLevelWindows(TRUE);
    }
}

bool wxFrameBase::ProcessCommand(int id)
{
    wxMenuBar *bar = GetMenuBar();
    if ( !bar )
        return FALSE;

    wxMenuItem *item = bar->FindItem(id);
    if ( item && item->IsCheckable() )
    {
        item->Toggle();
    }

    wxCommandEvent commandEvent(wxEVT_COMMAND_MENU_SELECTED, id);
    commandEvent.SetInt(id);
    commandEvent.SetEventObject(this);

    return GetEventHandler()->ProcessEvent(commandEvent);
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

// default resizing behaviour - if only ONE subwindow, resize to fill the
// whole client area
void wxFrameBase::OnSize(wxSizeEvent& event)
{
    // if we're using constraints - do use them
#if wxUSE_CONSTRAINTS
    if ( GetAutoLayout() )
    {
        Layout();
    }
    else
#endif
    {
        // do we have _exactly_ one child?
        wxWindow *child = (wxWindow *)NULL;
        for ( wxWindowList::Node *node = GetChildren().GetFirst();
              node;
              node = node->GetNext() )
        {
            wxWindow *win = node->GetData();

            // exclude top level and managed windows (status bar isn't
            // currently in the children list except under wxMac anyhow, but
            // it makes no harm to test for it)
            if ( !win->IsTopLevel()
#if wxUSE_STATUSBAR
                    && (win != GetStatusBar())
#endif // wxUSE_STATUSBAR
#if wxUSE_TOOLBAR
                    && (win != GetToolBar())
#endif // wxUSE_TOOLBAR
               )
            {
                if ( child )
                {
                    return;     // it's our second subwindow - nothing to do
                }

                child = win;
            }
        }

        // do we have any children at all?
        if ( child )
        {
            // exactly one child - set it's size to fill the whole frame
            int clientW, clientH;
            DoGetClientSize(&clientW, &clientH);

            // for whatever reasons, wxGTK wants to have a small offset - it
            // probably looks better with it?
#ifdef __WXGTK__
            static const int ofs = 0;
#else
            static const int ofs = 1;
#endif

            child->SetSize(ofs, ofs, clientW - 2*ofs, clientH - 2*ofs);
        }
    }
}

// The default implementation for the close window event.
void wxFrameBase::OnCloseWindow(wxCloseEvent& event)
{
    Destroy();
}

void wxFrameBase::OnMenuHighlight(wxMenuEvent& event)
{
#if wxUSE_STATUSBAR
    if ( GetStatusBar() )
    {
        // if no help string found, we will clear the status bar text
        wxString helpString;

        int menuId = event.GetMenuId();
        if ( menuId != wxID_SEPARATOR && menuId != -2 /* wxID_TITLE */ )
        {
            wxMenuBar *menuBar = GetMenuBar();
            if ( menuBar )
            {
                // it's ok if we don't find the item because it might belong
                // to the popup menu
                wxMenuItem *item = menuBar->FindItem(menuId);
                if ( item )
                    helpString = item->GetHelp();
            }
        }

        // set status text even if the string is empty - this will at least
        // remove the string from the item which was previously selected
        SetStatusText(helpString);
    }
#endif // wxUSE_STATUSBAR
}

// ----------------------------------------------------------------------------
// status bar stuff
// ----------------------------------------------------------------------------

#if wxUSE_STATUSBAR

wxStatusBar* wxFrameBase::CreateStatusBar(int number,
                                          long style,
                                          wxWindowID id,
                                          const wxString& name)
{
    // the main status bar can only be created once (or else it should be
    // deleted before calling CreateStatusBar() again)
    wxCHECK_MSG( !m_frameStatusBar, (wxStatusBar *)NULL,
                 wxT("recreating status bar in wxFrame") );

    m_frameStatusBar = OnCreateStatusBar( number, style, id, name );
    if ( m_frameStatusBar )
        PositionStatusBar();

    return m_frameStatusBar;
}

wxStatusBar *wxFrameBase::OnCreateStatusBar(int number,
                                            long style,
                                            wxWindowID id,
                                            const wxString& name)
{
    wxStatusBar *statusBar = new wxStatusBar(this, id,
                                             wxPoint(0, 0), wxSize(100, 20),
                                             style, name);

    // Set the height according to the font and the border size
    wxClientDC dc(statusBar);
    dc.SetFont(statusBar->GetFont());

    long y;
    dc.GetTextExtent( "X", NULL, &y );

    int height = (int)( (11*y)/10 + 2*statusBar->GetBorderY());

    statusBar->SetSize( -1, -1, 100, height );

    statusBar->SetFieldsCount(number);

    return statusBar;
}

void wxFrameBase::SetStatusText(const wxString& text, int number)
{
    wxCHECK_RET( m_frameStatusBar != NULL, wxT("no statusbar to set text for") );

    m_frameStatusBar->SetStatusText(text, number);
}

void wxFrameBase::SetStatusWidths(int n, const int widths_field[] )
{
    wxCHECK_RET( m_frameStatusBar != NULL, wxT("no statusbar to set widths for") );

    m_frameStatusBar->SetStatusWidths(n, widths_field);

    PositionStatusBar();
}

#endif // wxUSE_STATUSBAR

// ----------------------------------------------------------------------------
// toolbar stuff
// ----------------------------------------------------------------------------

#if wxUSE_TOOLBAR

wxToolBar* wxFrameBase::CreateToolBar(long style,
                                      wxWindowID id,
                                      const wxString& name)
{
    // the main toolbar can't be recreated (unless it was explicitly deeleted
    // before)
    wxCHECK_MSG( !m_frameToolBar, (wxToolBar *)NULL,
                 wxT("recreating toolbar in wxFrame") );

    m_frameToolBar = OnCreateToolBar(style, id, name);

    return m_frameToolBar;
}

wxToolBar* wxFrameBase::OnCreateToolBar(long style,
                                        wxWindowID id,
                                        const wxString& name)
{
    return new wxToolBar(this, id,
                         wxDefaultPosition, wxDefaultSize,
                         style, name);
}

#endif // wxUSE_TOOLBAR

// ----------------------------------------------------------------------------
// Menu UI updating
// ----------------------------------------------------------------------------

void wxFrameBase::OnIdle(wxIdleEvent& WXUNUSED(event) )
{
    DoMenuUpdates();
}

// update all menus
void wxFrameBase::DoMenuUpdates()
{
    wxMenuBar* bar = GetMenuBar();

    if ( bar != NULL )
    {
        int nCount = bar->GetMenuCount();
        for (int n = 0; n < nCount; n++)
            DoMenuUpdates(bar->GetMenu(n), (wxWindow*) NULL);
    }
}

// update a menu and all submenus recursively
void wxFrameBase::DoMenuUpdates(wxMenu* menu, wxWindow* WXUNUSED(focusWin))
{
    wxEvtHandler* evtHandler = GetEventHandler();
    wxMenuItemList::Node* node = menu->GetMenuItems().GetFirst();
    while (node)
    {
        wxMenuItem* item = node->GetData();
        if ( !item->IsSeparator() )
        {
            wxWindowID id = item->GetId();
            wxUpdateUIEvent event(id);
            event.SetEventObject( this );

            if (evtHandler->ProcessEvent(event))
            {
                if (event.GetSetText())
                    menu->SetLabel(id, event.GetText());
                if (event.GetSetChecked())
                    menu->Check(id, event.GetChecked());
                if (event.GetSetEnabled())
                    menu->Enable(id, event.GetEnabled());
            }

            if (item->GetSubMenu())
                DoMenuUpdates(item->GetSubMenu(), (wxWindow*) NULL);
        }
        node = node->GetNext();
    }
}
