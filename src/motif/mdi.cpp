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

#ifdef __VMS
#define XtDisplay XTDISPLAY
#define XtWindow XTWINDOW
#endif

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
    EVT_SCROLL(wxMDIClientWindow::OnScroll)
    EVT_NOTEBOOK_PAGE_CHANGED(wxID_NOTEBOOK_CLIENT_AREA, wxMDIClientWindow::OnPageChanged)
END_EVENT_TABLE()


// Parent frame

wxMDIParentFrame::wxMDIParentFrame()
{
    m_clientWindow = (wxMDIClientWindow*) NULL;
    m_activeChild = (wxMDIChildFrame*) NULL;
    m_activeMenuBar = (wxMenuBar*) NULL;
}

bool wxMDIParentFrame::Create(wxWindow *parent,
                              wxWindowID id,
                              const wxString& title,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              const wxString& name)
{
    m_clientWindow = (wxMDIClientWindow*) NULL;
    m_activeChild = (wxMDIChildFrame*) NULL;
    m_activeMenuBar = (wxMenuBar*) NULL;

    bool success = wxFrame::Create(parent, id, title, pos, size, style, name);
    if (success)
    {
        // TODO: app cannot override OnCreateClient since
        // wxMDIParentFrame::OnCreateClient will still be called
        // (we're in the constructor). How to resolve?

        m_clientWindow = OnCreateClient();

        // Uses own style for client style
        m_clientWindow->CreateClient(this, GetWindowStyleFlag());

        int w, h;
        GetClientSize(& w, & h);
        m_clientWindow->SetSize(0, 0, w, h);
        return true;
    }
    else
        return false;
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

    SetChildMenuBar((wxMDIChildFrame*) NULL);
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

void wxMDIParentFrame::DoGetClientSize(int *width, int *height) const
{
    wxFrame::DoGetClientSize(width, height);
}

void wxMDIParentFrame::OnActivate(wxActivateEvent& WXUNUSED(event))
{
    // Do nothing
}

// Returns the active MDI child window
wxMDIChildFrame *wxMDIParentFrame::GetActiveChild() const
{
    return m_activeChild;
}

// Create the client window class (don't Create the window,
// just return a new class)
wxMDIClientWindow *wxMDIParentFrame::OnCreateClient()
{
    return new wxMDIClientWindow ;
}

// Set the child's menu into the parent frame
void wxMDIParentFrame::SetChildMenuBar(wxMDIChildFrame* child)
{
    wxMenuBar* oldMenuBar = m_activeMenuBar;

    if (child == (wxMDIChildFrame*) NULL)  // No child: use parent frame
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
    else if (child->GetMenuBar() == (wxMenuBar*) NULL) // No child menu bar: use parent frame
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
    if (m_activeChild && event.IsKindOf(CLASSINFO(wxCommandEvent)))
    {
        res = m_activeChild->GetEventHandler()->ProcessEvent(event);
    }

    if (!res)
        res = GetEventHandler()->wxEvtHandler::ProcessEvent(event);

    inEvent = wxEVT_NULL;

    return res;
}

void wxMDIParentFrame::DoSetSize(int x, int y,
                                 int width, int height,
                                 int sizeFlags)
{
    wxWindow::DoSetSize(x, y, width, height, sizeFlags);
}

void wxMDIParentFrame::DoSetClientSize(int width, int height)
{
    wxWindow::DoSetClientSize(width, height);
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

void wxMDIParentFrame::Tile(wxOrientation WXUNUSED(orient))
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

// Default menu selection behaviour - display a help string
void wxMDIParentFrame::OnMenuHighlight(wxMenuEvent& event)
{
    if (GetStatusBar())
    {
        if (event.GetMenuId() == -1)
            SetStatusText(wxEmptyString);
        else
        {
            wxMenuBar *menuBar = (wxMenuBar*) NULL;
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

wxMDIChildFrame::wxMDIChildFrame()
{
    m_mdiParentFrame = (wxMDIParentFrame*) NULL;
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
    SetWindowStyleFlag(style);

    m_backgroundColour = wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE);
    m_foregroundColour = *wxBLACK;
    m_font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);

    if ( id > -1 )
        m_windowId = id;
    else
        m_windowId = (int)NewControlId();

    wxMDIClientWindow* clientWindow = parent->GetClientWindow();

    wxCHECK_MSG( clientWindow, false, "Missing MDI client window." );

    clientWindow->AddChild(this);

    SetMDIParentFrame(parent);

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
        oldActiveChild->GetEventHandler()->ProcessEvent(event);
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

    AttachWidget (parent, m_mainWidget, (WXWidget) NULL, pos.x, pos.y, size.x, size.y);

    ChangeBackgroundColour();

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
            parentFrame->SetActiveChild((wxMDIChildFrame*) NULL);
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
            parentFrame->SetActiveChild((wxMDIChildFrame*) NULL);
            parentFrame->SetChildMenuBar((wxMDIChildFrame*) NULL);
        }
    }
}

#if 0
// Implementation: intercept and act upon raise and lower commands.
void wxMDIChildFrame::OnRaise()
{
    wxMDIParentFrame* parentFrame = (wxMDIParentFrame*) GetParent() ;
    wxMDIChildFrame* oldActiveChild = parentFrame->GetActiveChild();
    parentFrame->SetActiveChild(this);

    if (oldActiveChild)
    {
        wxActivateEvent event(wxEVT_ACTIVATE, false, oldActiveChild->GetId());
        event.SetEventObject( oldActiveChild );
        oldActiveChild->GetEventHandler()->ProcessEvent(event);
    }

    wxActivateEvent event(wxEVT_ACTIVATE, true, this->GetId());
    event.SetEventObject( this );
    this->GetEventHandler()->ProcessEvent(event);
}

void wxMDIChildFrame::OnLower()
{
    wxMDIParentFrame* parentFrame = (wxMDIParentFrame*) GetParent() ;
    wxMDIChildFrame* oldActiveChild = parentFrame->GetActiveChild();

    if (oldActiveChild == this)
    {
        wxActivateEvent event(wxEVT_ACTIVATE, false, oldActiveChild->GetId());
        event.SetEventObject( oldActiveChild );
        oldActiveChild->GetEventHandler()->ProcessEvent(event);
    }
    // TODO: unfortunately we don't now know which is the top-most child,
    // so make the active child NULL.
    parentFrame->SetActiveChild((wxMDIChildFrame*) NULL);
}
#endif

// Set the client size (i.e. leave the calculation of borders etc.
// to wxWidgets)
void wxMDIChildFrame::DoSetClientSize(int width, int height)
{
    wxWindow::DoSetClientSize(width, height);
}

void wxMDIChildFrame::DoGetClientSize(int* width, int* height) const
{
    wxWindow::DoGetSize(width, height);
}

void wxMDIChildFrame::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    wxWindow::DoSetSize(x, y, width, height, sizeFlags);
}

void wxMDIChildFrame::DoGetSize(int* width, int* height) const
{
    wxWindow::DoGetSize(width, height);
}

void wxMDIChildFrame::DoGetPosition(int *x, int *y) const
{
    wxWindow::DoGetPosition(x, y);
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

// Set icon
void wxMDIChildFrame::SetIcon(const wxIcon& icon)
{
    m_icons = wxIconBundle( icon );

    if (icon.Ok())
    {
        // Not appropriate since there are no icons in
        // a tabbed window
    }
}

void wxMDIChildFrame::SetIcons(const wxIconBundle& icons)
{
    m_icons = icons;
}

void wxMDIChildFrame::SetTitle(const wxString& title)
{
    wxTopLevelWindow::SetTitle( title );
    wxMDIClientWindow* clientWindow = GetMDIParentFrame()->GetClientWindow();

    // Remove page if still there
    {
        int i = clientWindow->FindPage(this);

        if (i != -1)
            clientWindow->SetPageText(i, title);
    }
}

// MDI operations
void wxMDIChildFrame::Maximize()
{
    // TODO
}

void wxMDIChildFrame::Iconize(bool WXUNUSED(iconize))
{
    // TODO
}

bool wxMDIChildFrame::IsIconized() const
{
    return false;
}

// Is it maximized? Always maximized under Motif, using the
// tabbed MDI implementation.
bool wxMDIChildFrame::IsMaximized(void) const
{
    return true;
}

void wxMDIChildFrame::Restore()
{
    // TODO
}

void wxMDIChildFrame::Activate()
{
    // TODO
}

void wxMDIChildFrame::CaptureMouse()
{
    wxWindow::CaptureMouse();
}

void wxMDIChildFrame::ReleaseMouse()
{
    wxWindow::ReleaseMouse();
}

void wxMDIChildFrame::Raise()
{
    wxWindow::Raise();
}

void wxMDIChildFrame::Lower(void)
{
    wxWindow::Raise();
}

void wxMDIChildFrame::DoSetSizeHints(int WXUNUSED(minW), int WXUNUSED(minH), int WXUNUSED(maxW), int WXUNUSED(maxH), int WXUNUSED(incW), int WXUNUSED(incH))
{
}

// Client window

wxMDIClientWindow::wxMDIClientWindow()
{
}

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

    //    m_windowParent = parent;
    //    m_backgroundColour = wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE);

    bool success = wxNotebook::Create(parent, wxID_NOTEBOOK_CLIENT_AREA, wxPoint(0, 0), wxSize(100, 100), 0);
    if (success)
    {
        wxFont font(10, wxSWISS, wxNORMAL, wxNORMAL);
        SetFont(font);
        return true;
    }
    else
        return false;
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

void wxMDIClientWindow::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    wxWindow::DoSetSize(x, y, width, height, sizeFlags);
}

void wxMDIClientWindow::DoSetClientSize(int width, int height)
{
    wxWindow::DoSetClientSize(width, height);
}

void wxMDIClientWindow::DoGetClientSize(int *width, int *height) const
{
    wxWindow::DoGetClientSize(width, height);
}

void wxMDIClientWindow::DoGetSize(int *width, int *height) const
{
    wxWindow::DoGetSize(width, height);
}

void wxMDIClientWindow::DoGetPosition(int *x, int *y) const
{
    wxWindow::DoGetPosition(x, y);
}

void wxMDIClientWindow::OnScroll(wxScrollEvent& event)
{
    //    Default(); // Default processing: OBSOLETE FUNCTION
    event.Skip();
}

void wxMDIClientWindow::OnPageChanged(wxNotebookEvent& event)
{
    // Notify child that it has been activated
    if (event.GetOldSelection() != -1)
    {
        wxMDIChildFrame* oldChild = (wxMDIChildFrame*) GetPage(event.GetOldSelection());
        if (oldChild)
        {
            wxActivateEvent event(wxEVT_ACTIVATE, false, oldChild->GetId());
            event.SetEventObject( oldChild );
            oldChild->GetEventHandler()->ProcessEvent(event);
        }
    }
    if (event.GetSelection() != -1)
    {
        wxMDIChildFrame* activeChild = (wxMDIChildFrame*) GetPage(event.GetSelection());
        if (activeChild)
        {
            wxActivateEvent event(wxEVT_ACTIVATE, true, activeChild->GetId());
            event.SetEventObject( activeChild );
            activeChild->GetEventHandler()->ProcessEvent(event);

            if (activeChild->GetMDIParentFrame())
            {
                activeChild->GetMDIParentFrame()->SetActiveChild(activeChild);
                activeChild->GetMDIParentFrame()->SetChildMenuBar(activeChild);
            }
        }
    }
    event.Skip();
}
