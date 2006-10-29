/////////////////////////////////////////////////////////////////////////////
// Name:        src/aui/tabmdi.cpp
// Purpose:     Generic MDI (Multiple Document Interface) classes
// Author:      Hans Van Leemputten
// Modified by: Benjamin I. Williams / Kirix Corporation
// Created:     29/07/2002
// RCS-ID:      $Id$
// Copyright:   (c) Hans Van Leemputten
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_AUI
#if wxUSE_MDI

#include "wx/aui/tabmdi.h"

#ifndef WX_PRECOMP
    #include "wx/panel.h"
    #include "wx/menu.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/settings.h"
#endif //WX_PRECOMP

#include "wx/stockitem.h"

enum MDI_MENU_ID
{
    wxWINDOWCLOSE = 4001,
    wxWINDOWCLOSEALL,
    wxWINDOWNEXT,
    wxWINDOWPREV
};

//-----------------------------------------------------------------------------
// wxTabMDIParentFrame
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTabMDIParentFrame, wxFrame)

BEGIN_EVENT_TABLE(wxTabMDIParentFrame, wxFrame)
#if wxUSE_MENUS
    EVT_MENU (wxID_ANY, wxTabMDIParentFrame::DoHandleMenu)
#endif
END_EVENT_TABLE()

wxTabMDIParentFrame::wxTabMDIParentFrame()
{
    Init();
}

wxTabMDIParentFrame::wxTabMDIParentFrame(wxWindow *parent,
                                         wxWindowID id,
                                         const wxString& title,
                                         const wxPoint& pos,
                                         const wxSize& size,
                                         long style,
                                         const wxString& name)
{
    Init();
    (void)Create(parent, id, title, pos, size, style, name);
}

wxTabMDIParentFrame::~wxTabMDIParentFrame()
{
    // Make sure the client window is destructed before the menu bars are!
    wxDELETE(m_pClientWindow);

#if wxUSE_MENUS
    RemoveWindowMenu(GetMenuBar());
    delete m_pWindowMenu;
#endif // wxUSE_MENUS
}

bool wxTabMDIParentFrame::Create(wxWindow *parent,
                                 wxWindowID id,
                                 const wxString& title,
                                 const wxPoint& pos,
                                 const wxSize& size,
                                 long style,
                                 const wxString& name)
{
#if wxUSE_MENUS
    // this style can be used to prevent a window from having the standard MDI
    // "Window" menu
    if (!(style & wxFRAME_NO_WINDOW_MENU))
    {
        m_pWindowMenu = new wxMenu;
        m_pWindowMenu->Append(wxWINDOWCLOSE,    _("Cl&ose"));
        m_pWindowMenu->Append(wxWINDOWCLOSEALL, _("Close All"));
        m_pWindowMenu->AppendSeparator();
        m_pWindowMenu->Append(wxWINDOWNEXT,     _("&Next"));
        m_pWindowMenu->Append(wxWINDOWPREV,     _("&Previous"));
    }
#endif // wxUSE_MENUS

    wxFrame::Create(parent, id, title, pos, size, style, name);
    OnCreateClient();
    return true;
}

#if wxUSE_MENUS
void wxTabMDIParentFrame::SetWindowMenu(wxMenu* pMenu)
{
    // Replace the window menu from the currently loaded menu bar.
    wxMenuBar *pMenuBar = GetMenuBar();

    if (m_pWindowMenu)
    {
        RemoveWindowMenu(pMenuBar);
        wxDELETE(m_pWindowMenu);
    }

    if (pMenu)
    {
        m_pWindowMenu = pMenu;
        AddWindowMenu(pMenuBar);
    }
}

void wxTabMDIParentFrame::SetMenuBar(wxMenuBar* pMenuBar)
{
    // Remove the Window menu from the old menu bar
    RemoveWindowMenu(GetMenuBar());

    // Add the Window menu to the new menu bar.
    AddWindowMenu(pMenuBar);

    wxFrame::SetMenuBar(pMenuBar);
    m_pMyMenuBar = GetMenuBar();
}
#endif // wxUSE_MENUS

void wxTabMDIParentFrame::SetChildMenuBar(wxTabMDIChildFrame* pChild)
{
#if wxUSE_MENUS
    if (!pChild)
    {
        // No Child, set Our menu bar back.
        SetMenuBar(m_pMyMenuBar);

        // Make sure we know our menu bar is in use
        //m_pMyMenuBar = NULL;
    }
     else
    {
        if (pChild->GetMenuBar() == NULL)
            return;

        // Do we need to save the current bar?
        if (m_pMyMenuBar == NULL)
            m_pMyMenuBar = GetMenuBar();

        SetMenuBar(pChild->GetMenuBar());
    }
#endif // wxUSE_MENUS
}

bool wxTabMDIParentFrame::ProcessEvent(wxEvent& event)
{
    // Stops the same event being processed repeatedly
    static wxEventType inEvent = wxEVT_NULL;
    if (inEvent == event.GetEventType())
        return false;

    inEvent = event.GetEventType();

    // Let the active child (if any) process the event first.
    bool res = false;
    if (m_pActiveChild &&
        event.IsCommandEvent() &&
        event.GetEventObject() != m_pClientWindow &&
           !(event.GetEventType() == wxEVT_ACTIVATE ||
             event.GetEventType() == wxEVT_SET_FOCUS ||
             event.GetEventType() == wxEVT_KILL_FOCUS ||
             event.GetEventType() == wxEVT_CHILD_FOCUS ||
             event.GetEventType() == wxEVT_COMMAND_SET_FOCUS ||
             event.GetEventType() == wxEVT_COMMAND_KILL_FOCUS )
       )
    {
        res = m_pActiveChild->GetEventHandler()->ProcessEvent(event);
    }

    // If the event was not handled this frame will handle it!
    if (!res)
    {
        //res = GetEventHandler()->ProcessEvent(event);
        res = wxEvtHandler::ProcessEvent(event);
    }

    inEvent = wxEVT_NULL;

    return res;
}

wxTabMDIChildFrame *wxTabMDIParentFrame::GetActiveChild() const
{
    return m_pActiveChild;
}

void wxTabMDIParentFrame::SetActiveChild(wxTabMDIChildFrame* pChildFrame)
{
    m_pActiveChild = pChildFrame;
}

wxTabMDIClientWindow *wxTabMDIParentFrame::GetClientWindow() const
{
    return m_pClientWindow;
}

wxTabMDIClientWindow *wxTabMDIParentFrame::OnCreateClient()
{
    m_pClientWindow = new wxTabMDIClientWindow( this );
    return m_pClientWindow;
}

void wxTabMDIParentFrame::ActivateNext()
{
    if (m_pClientWindow && m_pClientWindow->GetSelection() != wxNOT_FOUND)
    {
        size_t active = m_pClientWindow->GetSelection() + 1;
        if (active >= m_pClientWindow->GetPageCount())
            active = 0;

        m_pClientWindow->SetSelection(active);
    }
}

void wxTabMDIParentFrame::ActivatePrevious()
{
    if (m_pClientWindow && m_pClientWindow->GetSelection() != wxNOT_FOUND)
    {
        int active = m_pClientWindow->GetSelection() - 1;
        if (active < 0)
            active = m_pClientWindow->GetPageCount() - 1;

        m_pClientWindow->SetSelection(active);
    }
}

void wxTabMDIParentFrame::Init()
{
    m_pClientWindow = NULL;
    m_pActiveChild = NULL;
#if wxUSE_MENUS
    m_pWindowMenu = NULL;
    m_pMyMenuBar = NULL;
#endif // wxUSE_MENUS
}

#if wxUSE_MENUS
void wxTabMDIParentFrame::RemoveWindowMenu(wxMenuBar* pMenuBar)
{
    if (pMenuBar && m_pWindowMenu)
    {
        // Remove old window menu
        int pos = pMenuBar->FindMenu(_("&Window"));
        if (pos != wxNOT_FOUND)
        {
            // DBG:: We're going to delete the wrong menu!!!
            wxASSERT(m_pWindowMenu == pMenuBar->GetMenu(pos));
            pMenuBar->Remove(pos);
        }
    }
}

void wxTabMDIParentFrame::AddWindowMenu(wxMenuBar *pMenuBar)
{
    if (pMenuBar && m_pWindowMenu)
    {
        int pos = pMenuBar->FindMenu(wxGetStockLabel(wxID_HELP,wxSTOCK_NOFLAGS));
        if (pos == wxNOT_FOUND)
            pMenuBar->Append(m_pWindowMenu, _("&Window"));
             else
            pMenuBar->Insert(pos, m_pWindowMenu, _("&Window"));
    }
}

void wxTabMDIParentFrame::DoHandleMenu(wxCommandEvent& event)
{
    switch (event.GetId())
    {
        case wxWINDOWCLOSE:
            if (m_pActiveChild)
                m_pActiveChild->Close();
            break;
        case wxWINDOWCLOSEALL:
            while (m_pActiveChild)
            {
                if (!m_pActiveChild->Close())
                {
                    return; // failure
                }
                 else
                {
                    delete m_pActiveChild;
                    m_pActiveChild = NULL;
                }
            }
            break;
        case wxWINDOWNEXT:
            ActivateNext();
            break;
        case wxWINDOWPREV:
            ActivatePrevious();
            break;
        default:
            event.Skip();
    }
}
#endif // wxUSE_MENUS

void wxTabMDIParentFrame::DoGetClientSize(int* width, int* height) const
{
    wxFrame::DoGetClientSize(width, height);
}

//-----------------------------------------------------------------------------
// wxTabMDIChildFrame
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTabMDIChildFrame, wxPanel)

BEGIN_EVENT_TABLE(wxTabMDIChildFrame, wxPanel)
    EVT_MENU_HIGHLIGHT_ALL(wxTabMDIChildFrame::OnMenuHighlight)
    EVT_ACTIVATE(wxTabMDIChildFrame::OnActivate)
    EVT_CLOSE(wxTabMDIChildFrame::OnCloseWindow)
END_EVENT_TABLE()

wxTabMDIChildFrame::wxTabMDIChildFrame()
{
    Init();
}

wxTabMDIChildFrame::wxTabMDIChildFrame(wxTabMDIParentFrame *parent,
                                       wxWindowID id,
                                       const wxString& title,
                                       const wxPoint& WXUNUSED(pos),
                                       const wxSize& size,
                                       long style,
                                       const wxString& name)
{
    Init();
    Create(parent, id, title, wxDefaultPosition, size, style, name);
}

wxTabMDIChildFrame::~wxTabMDIChildFrame()
{
#if wxUSE_MENUS
    wxDELETE(m_pMenuBar);
#endif // wxUSE_MENUS
}

bool wxTabMDIChildFrame::Create(wxTabMDIParentFrame* parent,
                                wxWindowID id,
                                const wxString& title,
                                const wxPoint& WXUNUSED(pos),
                                const wxSize& size,
                                long style,
                                const wxString& name)
{
    wxTabMDIClientWindow* pClientWindow = parent->GetClientWindow();
    wxASSERT_MSG((pClientWindow != (wxWindow*) NULL), wxT("Missing MDI client window."));

    wxPanel::Create(pClientWindow, id, wxDefaultPosition, size, style|wxNO_BORDER, name);

    SetMDIParentFrame(parent);

    // this is the currently active child
    parent->SetActiveChild(this);

    m_title = title;

    pClientWindow->AddPage(this, title, true);
    pClientWindow->Refresh();

    return true;
}

bool wxTabMDIChildFrame::Destroy()
{
    wxTabMDIParentFrame* pParentFrame = GetMDIParentFrame();
    wxASSERT_MSG(pParentFrame, wxT("Missing MDI Parent Frame"));

    wxTabMDIClientWindow* pClientWindow = pParentFrame->GetClientWindow();
    wxASSERT_MSG(pClientWindow, wxT("Missing MDI Client Window"));

    if (pParentFrame->GetActiveChild() == this)
    {
        pParentFrame->SetActiveChild(NULL);
        pParentFrame->SetChildMenuBar(NULL);
    }

    const size_t page_count = pClientWindow->GetPageCount();
    for (size_t pos = 0; pos < page_count; pos++)
    {
        if (pClientWindow->GetPage(pos) == this)
            return pClientWindow->DeletePage(pos);
    }

    return false;
}

#if wxUSE_MENUS
void wxTabMDIChildFrame::SetMenuBar(wxMenuBar *menu_bar)
{
    wxMenuBar *pOldMenuBar = m_pMenuBar;
    m_pMenuBar = menu_bar;

    if (m_pMenuBar)
    {
        wxTabMDIParentFrame* pParentFrame = GetMDIParentFrame();
        wxASSERT_MSG(pParentFrame, wxT("Missing MDI Parent Frame"));

        m_pMenuBar->SetParent(pParentFrame);
        if (pParentFrame->GetActiveChild() == this)
        {
            // replace current menu bars
            if (pOldMenuBar)
                pParentFrame->SetChildMenuBar(NULL);
            pParentFrame->SetChildMenuBar(this);
        }
    }
}

wxMenuBar *wxTabMDIChildFrame::GetMenuBar() const
{
    return m_pMenuBar;
}
#endif // wxUSE_MENUS

void wxTabMDIChildFrame::SetTitle(const wxString& title)
{
    m_title = title;

    wxTabMDIParentFrame* pParentFrame = GetMDIParentFrame();
    wxASSERT_MSG(pParentFrame, wxT("Missing MDI Parent Frame"));

    wxTabMDIClientWindow* pClientWindow = pParentFrame->GetClientWindow();
    if (pClientWindow != NULL)
    {
        size_t pos;
        for (pos = 0; pos < pClientWindow->GetPageCount(); pos++)
        {
            if (pClientWindow->GetPage(pos) == this)
            {
                pClientWindow->SetPageText(pos, m_title);
                break;
            }
        }
    }
}

wxString wxTabMDIChildFrame::GetTitle() const
{
    return m_title;
}

void wxTabMDIChildFrame::Activate()
{
    wxTabMDIParentFrame* pParentFrame = GetMDIParentFrame();
    wxASSERT_MSG(pParentFrame, wxT("Missing MDI Parent Frame"));

    wxTabMDIClientWindow* pClientWindow = pParentFrame->GetClientWindow();

    if (pClientWindow != NULL)
    {
        size_t pos;
        for (pos = 0; pos < pClientWindow->GetPageCount(); pos++)
        {
            if (pClientWindow->GetPage(pos) == this)
            {
                pClientWindow->SetSelection(pos);
                break;
            }
        }
    }
}

void wxTabMDIChildFrame::OnMenuHighlight(wxMenuEvent& event)
{
#if wxUSE_STATUSBAR
    if (m_pMDIParentFrame)
    {
        // we don't have any help text for this item,
        // but may be the MDI frame does?
        m_pMDIParentFrame->OnMenuHighlight(event);
    }
#else
    wxUnusedVar(event);
#endif // wxUSE_STATUSBAR
}

void wxTabMDIChildFrame::OnActivate(wxActivateEvent& WXUNUSED(event))
{
    // do nothing
}

void wxTabMDIChildFrame::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
    Destroy();
}

void wxTabMDIChildFrame::SetMDIParentFrame(wxTabMDIParentFrame* parentFrame)
{
    m_pMDIParentFrame = parentFrame;
}

wxTabMDIParentFrame* wxTabMDIChildFrame::GetMDIParentFrame() const
{
    return m_pMDIParentFrame;
}

void wxTabMDIChildFrame::Init()
{
    m_pMDIParentFrame = NULL;
#if wxUSE_MENUS
    m_pMenuBar = NULL;
#endif // wxUSE_MENUS
}

bool wxTabMDIChildFrame::Show(bool WXUNUSED(show))
{
    // do nothing
    return true;
}

void wxTabMDIChildFrame::DoShow(bool show)
{
    wxWindow::Show(show);
}

void wxTabMDIChildFrame::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    m_mdi_newrect = wxRect(x, y, width, height);
#ifdef __WXGTK__
    wxPanel::DoSetSize(x,y,width, height, sizeFlags);
#else
    wxUnusedVar(sizeFlags);
#endif
}

void wxTabMDIChildFrame::DoMoveWindow(int x, int y, int width, int height)
{
    m_mdi_newrect = wxRect(x, y, width, height);
}

void wxTabMDIChildFrame::ApplyMDIChildFrameRect()
{
    if (m_mdi_currect != m_mdi_newrect)
    {
        wxPanel::DoMoveWindow(m_mdi_newrect.x, m_mdi_newrect.y,
                              m_mdi_newrect.width, m_mdi_newrect.height);
        m_mdi_currect = m_mdi_newrect;
    }
}


//-----------------------------------------------------------------------------
// wxTabMDIClientWindow
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTabMDIClientWindow, wxAuiMultiNotebook)

BEGIN_EVENT_TABLE(wxTabMDIClientWindow, wxAuiMultiNotebook)
    EVT_AUINOTEBOOK_PAGE_CHANGED(wxID_ANY, wxTabMDIClientWindow::OnPageChanged)
    EVT_SIZE(wxTabMDIClientWindow::OnSize)
END_EVENT_TABLE()

wxTabMDIClientWindow::wxTabMDIClientWindow()
{
}

wxTabMDIClientWindow::wxTabMDIClientWindow(wxTabMDIParentFrame* parent, long style)
{
    CreateClient(parent, style);
}

wxTabMDIClientWindow::~wxTabMDIClientWindow()
{
    DestroyChildren();
}

bool wxTabMDIClientWindow::CreateClient(wxTabMDIParentFrame* parent, long style)
{
    SetWindowStyleFlag(style);

    if (!wxAuiMultiNotebook::Create(parent,
                                    wxID_ANY,
                                    wxPoint(0,0),
                                    wxSize(100, 100),
                                    wxNO_BORDER))
    {
        return false;
    }

    wxColour bkcolour = wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE);
    SetBackgroundColour(bkcolour);

    m_mgr.GetArtProvider()->SetColour(wxAUI_ART_BACKGROUND_COLOUR, bkcolour);

    return true;
}

int wxTabMDIClientWindow::SetSelection(size_t nPage)
{
    return wxAuiMultiNotebook::SetSelection(nPage);
}

void wxTabMDIClientWindow::PageChanged(int old_selection, int new_selection)
{
    // don't do anything if the page doesn't actually change
    if (old_selection == new_selection)
        return;

    // don't do anything if the new page is already active
    if (new_selection != -1)
    {
        wxTabMDIChildFrame* child = (wxTabMDIChildFrame*)GetPage(new_selection);
        if (child->GetMDIParentFrame()->GetActiveChild() == child)
            return;
    }

    // notify old active child that it has been deactivated
    if (old_selection != -1)
    {
        wxTabMDIChildFrame* old_child = (wxTabMDIChildFrame*)GetPage(old_selection);
        wxASSERT_MSG(old_child, wxT("wxTabMDIClientWindow::PageChanged - null page pointer"));

        wxActivateEvent event(wxEVT_ACTIVATE, false, old_child->GetId());
        event.SetEventObject(old_child);
        old_child->GetEventHandler()->ProcessEvent(event);
    }

    // notify new active child that it has been activated
    if (new_selection != -1)
    {
        wxTabMDIChildFrame* active_child = (wxTabMDIChildFrame*)GetPage(new_selection);
        wxASSERT_MSG(active_child, wxT("wxTabMDIClientWindow::PageChanged - null page pointer"));

        wxActivateEvent event(wxEVT_ACTIVATE, true, active_child->GetId());
        event.SetEventObject(active_child);
        active_child->GetEventHandler()->ProcessEvent(event);

        if (active_child->GetMDIParentFrame())
        {
            active_child->GetMDIParentFrame()->SetActiveChild(active_child);
            active_child->GetMDIParentFrame()->SetChildMenuBar(active_child);
        }
    }
}

void wxTabMDIClientWindow::OnPageChanged(wxAuiNotebookEvent& evt)
{
    PageChanged(evt.GetOldSelection(), evt.GetSelection());
    evt.Skip();
}

void wxTabMDIClientWindow::OnSize(wxSizeEvent& evt)
{
    wxAuiMultiNotebook::OnSize(evt);

    for (size_t pos = 0; pos < GetPageCount(); pos++)
        ((wxTabMDIChildFrame *)GetPage(pos))->ApplyMDIChildFrameRect();
}

#endif //wxUSE_AUI
#endif // wxUSE_MDI
