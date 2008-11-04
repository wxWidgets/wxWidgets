/////////////////////////////////////////////////////////////////////////////
// Name:        src/motif/mdi.cpp
// Purpose:     MDI classes
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/mdi.h"

#ifndef WX_PRECOMP
    #include "wx/menu.h"
    #include "wx/icon.h"
    #include "wx/settings.h"
#endif

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/Xm.h>
#include <Xm/BulletinB.h>
#include <Xm/Form.h>
#include <Xm/MainW.h>
#include <Xm/RowColumn.h>
#include <Xm/CascadeBG.h>
#include <Xm/Text.h>
#include <Xm/PushBG.h>
#include <Xm/AtomMgr.h>
#include <Xm/Protocols.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif

#include "wx/motif/private.h"

extern wxList wxModelessWindows;

// Implemented in frame.cpp
extern void wxFrameFocusProc(Widget workArea, XtPointer clientData,
                             XmAnyCallbackStruct *cbs);

#define wxID_NOTEBOOK_CLIENT_AREA wxID_HIGHEST + 100

IMPLEMENT_DYNAMIC_CLASS(wxMDIParentFrame, wxFrame)
IMPLEMENT_DYNAMIC_CLASS(wxMDIChildFrame, wxFrame)
IMPLEMENT_DYNAMIC_CLASS(wxMDIClientWindow, wxNotebook)

BEGIN_EVENT_TABLE(wxMDIParentFrame, wxFrame)
    EVT_SIZE(wxMDIParentFrame::OnSize)
    EVT_ACTIVATE(wxMDIParentFrame::OnActivate)
    EVT_SYS_COLOUR_CHANGED(wxMDIParentFrame::OnSysColourChanged)
    EVT_MENU_HIGHLIGHT_ALL(wxMDIParentFrame::OnMenuHighlight)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxMDIClientWindow, wxNotebook)
    EVT_NOTEBOOK_PAGE_CHANGED(wxID_NOTEBOOK_CLIENT_AREA, wxMDIClientWindow::OnPageChanged)
END_EVENT_TABLE()


// Parent frame

void wxMDIParentFrame::Init()
{
    m_activeMenuBar = NULL;
}

bool wxMDIParentFrame::Create(wxWindow *parent,
                              wxWindowID id,
                              const wxString& title,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              const wxString& name)
{
    if ( !wxFrame::Create(parent, id, title, pos, size, style, name) )
        return false;

    m_clientWindow = OnCreateClient();
    if ( !m_clientWindow->CreateClient(this, GetWindowStyleFlag()) )
        return false;

    int w, h;
    GetClientSize(& w, & h);
    m_clientWindow->SetSize(0, 0, w, h);
    return true;
}

wxMDIParentFrame::~wxMDIParentFrame()
{
    // Make sure we delete the client window last of all
    RemoveChild(m_clientWindow);

    DestroyChildren();

    delete m_clientWindow;
    m_clientWindow = NULL;
}

void wxMDIParentFrame::SetMenuBar(wxMenuBar *menu_bar)
{
    m_frameMenuBar = menu_bar;

    SetChildMenuBar(NULL);
}

void wxMDIParentFrame::OnSize(wxSizeEvent& WXUNUSED(event))
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

void wxMDIParentFrame::OnActivate(wxActivateEvent& WXUNUSED(event))
{
    // Do nothing
}

// Set the child's menu into the parent frame
void wxMDIParentFrame::SetChildMenuBar(wxMDIChildFrame* child)
{
    wxMenuBar* oldMenuBar = m_activeMenuBar;

    if (child == NULL)  // No child: use parent frame
    {
        if (GetMenuBar() && (GetMenuBar() != m_activeMenuBar))
        {
            //            if (m_activeMenuBar)
            //                m_activeMenuBar->DestroyMenuBar();

            m_activeMenuBar = GetMenuBar();
            m_activeMenuBar->CreateMenuBar(this);
            /*
            if (oldMenuBar && XtIsManaged((Widget) oldMenuBar->GetMainWidget()))
            XtUnmanageChild((Widget) oldMenuBar->GetMainWidget());
            */
            if (oldMenuBar && oldMenuBar->GetMainWidget())
                XtUnmapWidget((Widget) oldMenuBar->GetMainWidget());

        }
    }
    else if (child->GetMenuBar() == NULL) // No child menu bar: use parent frame
    {
        if (GetMenuBar() && (GetMenuBar() != m_activeMenuBar))
        {
            //            if (m_activeMenuBar)
            //                m_activeMenuBar->DestroyMenuBar();
            m_activeMenuBar = GetMenuBar();
            m_activeMenuBar->CreateMenuBar(this);
            /*
            if (oldMenuBar && XtIsManaged((Widget) oldMenuBar->GetMainWidget()))
            XtUnmanageChild((Widget) oldMenuBar->GetMainWidget());
            */
            if (oldMenuBar && oldMenuBar->GetMainWidget())
                XtUnmapWidget((Widget) oldMenuBar->GetMainWidget());
        }
    }
    else // The child has a menubar
    {
        if (child->GetMenuBar() != m_activeMenuBar)
        {
            //            if (m_activeMenuBar)
            //                m_activeMenuBar->DestroyMenuBar();

            m_activeMenuBar = child->GetMenuBar();
            m_activeMenuBar->CreateMenuBar(this);
            /*
            if (oldMenuBar && XtIsManaged((Widget) oldMenuBar->GetMainWidget()))
            XtUnmanageChild((Widget) oldMenuBar->GetMainWidget());
            */
            if (oldMenuBar && oldMenuBar->GetMainWidget())
                XtUnmapWidget((Widget) oldMenuBar->GetMainWidget());
        }
    }
}

// Redirect events to active child first
bool wxMDIParentFrame::ProcessEvent(wxEvent& event)
{
    // Stops the same event being processed repeatedly
    static wxEventType inEvent = wxEVT_NULL;
    if (inEvent == event.GetEventType())
        return false;

    inEvent = event.GetEventType();

    bool res = false;
    if (m_currentChild && event.IsKindOf(CLASSINFO(wxCommandEvent)))
    {
        res = m_currentChild->HandleWindowEvent(event);
    }

    if (!res)
        res = GetEventHandler()->wxEvtHandler::ProcessEvent(event);

    inEvent = wxEVT_NULL;

    return res;
}

// Responds to colour changes, and passes event on to children.
void wxMDIParentFrame::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    // TODO

    // Propagate the event to the non-top-level children
    wxFrame::OnSysColourChanged(event);
}

// Default menu selection behaviour - display a help string
void wxMDIParentFrame::OnMenuHighlight(wxMenuEvent& event)
{
    if (GetStatusBar())
    {
        if (event.GetMenuId() == -1)
            SetStatusText(wxEmptyString);
        else
        {
            wxMenuBar *menuBar = NULL;
            if (GetActiveChild())
              menuBar = GetActiveChild()->GetMenuBar();
            else
              menuBar = GetMenuBar();
            if (menuBar)
            {
                wxString helpString(menuBar->GetHelpString(event.GetMenuId()));
                if (!helpString.empty())
                    SetStatusText(helpString);
            }
        }
    }
}

// Child frame

bool wxMDIChildFrame::Create(wxMDIParentFrame *parent,
                             wxWindowID id,
                             const wxString& title,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style,
                             const wxString& name)
{
    SetName(name);
    SetWindowStyleFlag(style);

    if ( id > -1 )
        m_windowId = id;
    else
        m_windowId = (int)NewControlId();

    wxMDIClientWindow* clientWindow = parent->GetClientWindow();

    wxCHECK_MSG( clientWindow, false, "Missing MDI client window." );

    clientWindow->AddChild(this);

    m_mdiParent = parent;
    PreCreation();

    int width = size.x;
    int height = size.y;
    if (width == -1)
        width = 200; // TODO: give reasonable default
    if (height == -1)
        height = 200; // TODO: give reasonable default

    // We're deactivating the old child
    wxMDIChildFrame* oldActiveChild = parent->GetActiveChild();
    if (oldActiveChild)
    {
        wxActivateEvent event(wxEVT_ACTIVATE, false, oldActiveChild->GetId());
        event.SetEventObject( oldActiveChild );
        oldActiveChild->HandleWindowEvent(event);
    }

    // This is the currently active child
    parent->SetActiveChild((wxMDIChildFrame*) this);

    // This time we'll try a bog-standard bulletin board for
    // the 'frame'. A main window doesn't seem to work.

    m_mainWidget = (WXWidget) XtVaCreateWidget("client",
        xmBulletinBoardWidgetClass, (Widget) clientWindow->GetTopWidget(),
        XmNmarginWidth, 0,
        XmNmarginHeight, 0,
        /*
        XmNrightAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        */
        XmNresizePolicy, XmRESIZE_NONE,
        NULL);

    XtAddEventHandler((Widget) m_mainWidget, ExposureMask,False,
        wxUniversalRepaintProc, (XtPointer) this);

    PostCreation();
    AttachWidget (parent, m_mainWidget, (WXWidget) NULL, pos.x, pos.y, size.x, size.y);

    XtManageChild((Widget) m_mainWidget);

    SetTitle(title);

    clientWindow->AddPage(this, title, true);
    clientWindow->Refresh();

    // Positions the toolbar and status bar -- but we don't have any.
    //    PreResize();

    wxModelessWindows.Append(this);
    return true;
}


wxMDIChildFrame::~wxMDIChildFrame()
{
    if (m_mainWidget)
      XtRemoveEventHandler((Widget) m_mainWidget, ExposureMask,False,
        wxUniversalRepaintProc, (XtPointer) this);

    if (GetMDIParentFrame())
    {
        wxMDIParentFrame* parentFrame = GetMDIParentFrame();

        if (parentFrame->GetActiveChild() == this)
            parentFrame->SetActiveChild(NULL);
        wxMDIClientWindow* clientWindow = parentFrame->GetClientWindow();

        // Remove page if still there
        {
            int i = clientWindow->FindPage(this);

            if (i != -1)
            {
                clientWindow->RemovePage(i);
                clientWindow->Refresh();
            }
        }

        // Set the selection to the first remaining page
        if (clientWindow->GetPageCount() > 0)
        {
            wxMDIChildFrame* child = (wxMDIChildFrame*)  clientWindow->GetPage(0);
            parentFrame->SetActiveChild(child);
            parentFrame->SetChildMenuBar(child);
        }
        else
        {
            parentFrame->SetActiveChild(NULL);
            parentFrame->SetChildMenuBar(NULL);
        }
    }
}

bool wxMDIChildFrame::Show(bool show)
{
    SetVisibleStatus( show );
    return wxWindow::Show(show);
}

void wxMDIChildFrame::SetMenuBar(wxMenuBar *menuBar)
{
    // Don't create the underlying menubar yet; need to recreate
    // it every time the child is activated.
    m_frameMenuBar = menuBar;

    // We make the assumption that if you're setting the menubar,
    // this is the currently active child.
    GetMDIParentFrame()->SetChildMenuBar(this);
}

void wxMDIChildFrame::SetTitle(const wxString& title)
{
    m_title = title;
    wxMDIClientWindow* clientWindow = GetMDIParentFrame()->GetClientWindow();

    int i = clientWindow->FindPage(this);
    if (i != wxNOT_FOUND)
        clientWindow->SetPageText(i, title);
}

void wxMDIChildFrame::Activate()
{
    // TODO
}

// Client window

wxMDIClientWindow::~wxMDIClientWindow()
{
    // By the time this destructor is called, the child frames will have been
    // deleted and removed from the notebook/client window.
    DestroyChildren();

    m_mainWidget = (WXWidget) 0;
}

bool wxMDIClientWindow::CreateClient(wxMDIParentFrame *parent, long style)
{
    SetWindowStyleFlag(style);

    if ( !wxNotebook::Create(parent, wxID_NOTEBOOK_CLIENT_AREA,
                             wxPoint(0, 0), wxSize(100, 100), 0) )
        return false;

    return true;
}

int wxMDIClientWindow::FindPage(const wxNotebookPage* page)
{
    for (int i = GetPageCount() - 1; i >= 0; --i)
    {
        if (GetPage(i) == page)
            return i;
    }

    return -1;
}

void wxMDIClientWindow::OnPageChanged(wxBookCtrlEvent& event)
{
    // Notify child that it has been activated
    if (event.GetOldSelection() != -1)
    {
        wxMDIChildFrame* oldChild = (wxMDIChildFrame*) GetPage(event.GetOldSelection());
        if (oldChild)
        {
            wxActivateEvent event(wxEVT_ACTIVATE, false, oldChild->GetId());
            event.SetEventObject( oldChild );
            oldChild->HandleWindowEvent(event);
        }
    }
    if (event.GetSelection() != -1)
    {
        wxMDIChildFrame* activeChild = (wxMDIChildFrame*) GetPage(event.GetSelection());
        if (activeChild)
        {
            wxActivateEvent event(wxEVT_ACTIVATE, true, activeChild->GetId());
            event.SetEventObject( activeChild );
            activeChild->HandleWindowEvent(event);

            if (activeChild->GetMDIParentFrame())
            {
                activeChild->GetMDIParentFrame()->SetActiveChild(activeChild);
                activeChild->GetMDIParentFrame()->SetChildMenuBar(activeChild);
            }
        }
    }
    event.Skip();
}
