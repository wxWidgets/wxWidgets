/////////////////////////////////////////////////////////////////////////////
// Name:        common/framecmn.cpp
// Purpose:     common (for all platforms) wxFrame functions
// Author:      Julian Smart, Vadim Zeitlin
// Created:     01/02/97
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling and Julian Smart
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

#ifndef WX_PRECOMP
    #include "wx/frame.h"
    #include "wx/menu.h"
    #include "wx/menuitem.h"
    #include "wx/dcclient.h"
#endif // WX_PRECOMP

#if wxUSE_TOOLBAR
    #include "wx/toolbar.h"
#endif
#if wxUSE_STATUSBAR
    #include "wx/statusbr.h"
#endif

// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxFrameBase, wxTopLevelWindow)
#if wxUSE_MENUS && wxUSE_IDLEMENUUPDATES
    EVT_IDLE(wxFrameBase::OnIdle)
#endif
#if wxUSE_MENUS && !wxUSE_IDLEMENUUPDATES
    EVT_MENU_OPEN(wxFrameBase::OnMenuOpen)
#endif
    EVT_MENU_HIGHLIGHT_ALL(wxFrameBase::OnMenuHighlight)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// construction/destruction
// ----------------------------------------------------------------------------

wxFrameBase::wxFrameBase()
{
#if wxUSE_MENUS
    m_frameMenuBar = NULL;
#endif // wxUSE_MENUS

#if wxUSE_TOOLBAR
    m_frameToolBar = NULL;
#endif // wxUSE_TOOLBAR

#if wxUSE_STATUSBAR
    m_frameStatusBar = NULL;
#endif // wxUSE_STATUSBAR

    m_statusBarPane = 0;
}

wxFrameBase::~wxFrameBase()
{
    // this destructor is required for Darwin
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
#if wxUSE_MENUS
    if ( m_frameMenuBar )
    {
        delete m_frameMenuBar;
        m_frameMenuBar = (wxMenuBar *) NULL;
    }
#endif // wxUSE_MENUS

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

bool wxFrameBase::IsOneOfBars(const wxWindow *win) const
{
#if wxUSE_MENUS
    if ( win == GetMenuBar() )
        return TRUE;
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    if ( win == GetStatusBar() )
        return TRUE;
#endif // wxUSE_STATUSBAR

#if wxUSE_TOOLBAR
    if ( win == GetToolBar() )
        return TRUE;
#endif // wxUSE_TOOLBAR

    return FALSE;
}

// ----------------------------------------------------------------------------
// wxFrame size management: we exclude the areas taken by menu/status/toolbars
// from the client area, so the client area is what's really available for the
// frame contents
// ----------------------------------------------------------------------------

// get the origin of the client area in the client coordinates
wxPoint wxFrameBase::GetClientAreaOrigin() const
{
    wxPoint pt = wxTopLevelWindow::GetClientAreaOrigin();

#if wxUSE_TOOLBAR && !defined(__WXUNIVERSAL__)
    wxToolBar *toolbar = GetToolBar();
    if ( toolbar && toolbar->IsShown() )
    {
        int w, h;
        toolbar->GetSize(&w, &h);

        if ( toolbar->GetWindowStyleFlag() & wxTB_VERTICAL )
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

// ----------------------------------------------------------------------------
// misc
// ----------------------------------------------------------------------------

bool wxFrameBase::ProcessCommand(int id)
{
#if wxUSE_MENUS
    wxMenuBar *bar = GetMenuBar();
    if ( !bar )
        return FALSE;

    wxCommandEvent commandEvent(wxEVT_COMMAND_MENU_SELECTED, id);
    commandEvent.SetEventObject(this);

    wxMenuItem *item = bar->FindItem(id);
    if (item)
    {
        if (!item->IsEnabled())
            return TRUE;

        if (item->IsCheckable())
        {
            item->Toggle();

            // use the new value
            commandEvent.SetInt(item->IsChecked());
        }
    }

    GetEventHandler()->ProcessEvent(commandEvent);
    return TRUE;
#else // !wxUSE_MENUS
    return FALSE;
#endif // wxUSE_MENUS/!wxUSE_MENUS
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void wxFrameBase::OnMenuHighlight(wxMenuEvent& event)
{
#if wxUSE_STATUSBAR
    (void)ShowMenuHelp(GetStatusBar(), event.GetMenuId());
#endif // wxUSE_STATUSBAR
}

void wxFrameBase::OnIdle(wxIdleEvent& WXUNUSED(event) )
{
#if wxUSE_MENUS && wxUSE_IDLEMENUUPDATES
    if (wxUpdateUIEvent::CanUpdate())
        DoMenuUpdates();
#endif
}

void wxFrameBase::OnMenuOpen(wxMenuEvent& WXUNUSED(event))
{
#if wxUSE_MENUS && !wxUSE_IDLEMENUUPDATES
    DoMenuUpdates();
#endif
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
    wxStatusBar *statusBar = new wxStatusBar(this, id, style, name);

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

void wxFrameBase::PushStatusText(const wxString& text, int number)
{
    wxCHECK_RET( m_frameStatusBar != NULL, wxT("no statusbar to set text for") );

    m_frameStatusBar->PushStatusText(text, number);
}

void wxFrameBase::PopStatusText(int number)
{
    wxCHECK_RET( m_frameStatusBar != NULL, wxT("no statusbar to set text for") );

    m_frameStatusBar->PopStatusText(number);
}

bool wxFrameBase::ShowMenuHelp(wxStatusBar *WXUNUSED(statbar), int menuId)
{
#if wxUSE_MENUS
    // if no help string found, we will clear the status bar text
    wxString helpString;
    bool show = menuId != wxID_SEPARATOR && menuId != -2 /* wxID_TITLE */;

    if ( show )
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

    DoGiveHelp(helpString, show);

    return !helpString.IsEmpty();
#else // !wxUSE_MENUS
    return FALSE;
#endif // wxUSE_MENUS/!wxUSE_MENUS
}

#endif // wxUSE_STATUSBAR

void wxFrameBase::DoGiveHelp(const wxString& text, bool show)
{
#if wxUSE_STATUSBAR
    if ( m_statusBarPane < 0 ) return;
    wxStatusBar* statbar = GetStatusBar();
    if ( !statbar ) return;

    wxString help = show ? text : wxString();
    statbar->SetStatusText( help, m_statusBarPane );
#endif // wxUSE_STATUSBAR
}


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
// menus
// ----------------------------------------------------------------------------

#if wxUSE_MENUS

// update all menus
void wxFrameBase::DoMenuUpdates()
{
    wxMenuBar* bar = GetMenuBar();

#ifdef __WXMSW__
    wxWindow* focusWin = wxFindFocusDescendant((wxWindow*) this);
#else
    wxWindow* focusWin = (wxWindow*) NULL;
#endif
    if ( bar != NULL )
    {
        int nCount = bar->GetMenuCount();
        for (int n = 0; n < nCount; n++)
            DoMenuUpdates(bar->GetMenu(n), focusWin);
    }
}

// update a menu and all submenus recursively
void wxFrameBase::DoMenuUpdates(wxMenu* menu, wxWindow* focusWin)
{
    wxEvtHandler* evtHandler = focusWin ? focusWin->GetEventHandler() : GetEventHandler();
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
                DoMenuUpdates(item->GetSubMenu(), focusWin);
        }
        node = node->GetNext();
    }
}

void wxFrameBase::DetachMenuBar()
{
    if ( m_frameMenuBar )
    {
        m_frameMenuBar->Detach();
        m_frameMenuBar = NULL;
    }
}

void wxFrameBase::AttachMenuBar(wxMenuBar *menubar)
{
    if ( menubar )
    {
        menubar->Attach((wxFrame *)this);
        m_frameMenuBar = menubar;
    }
}

void wxFrameBase::SetMenuBar(wxMenuBar *menubar)
{
    if ( menubar == GetMenuBar() )
    {
        // nothing to do
        return;
    }

    DetachMenuBar();

    AttachMenuBar(menubar);
}

#endif // wxUSE_MENUS
