/////////////////////////////////////////////////////////////////////////////
// Name:        mdi.cpp
// Purpose:     MDI classes
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "mdi.h"
#endif

#include "wx/mdi.h"
#include "wx/menu.h"
#include "wx/settings.h"

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

#include "wx/motif/private.h"

extern wxList wxModelessWindows;

// Implemented in frame.cpp
extern void wxFrameFocusProc(Widget workArea, XtPointer clientData, 
                      XmAnyCallbackStruct *cbs);

#define wxID_NOTEBOOK_CLIENT_AREA wxID_HIGHEST + 100

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxMDIParentFrame, wxFrame)
IMPLEMENT_DYNAMIC_CLASS(wxMDIChildFrame, wxFrame)
IMPLEMENT_DYNAMIC_CLASS(wxMDIClientWindow, wxNotebook)

BEGIN_EVENT_TABLE(wxMDIParentFrame, wxFrame)
  EVT_SIZE(wxMDIParentFrame::OnSize)
  EVT_ACTIVATE(wxMDIParentFrame::OnActivate)
  EVT_SYS_COLOUR_CHANGED(wxMDIParentFrame::OnSysColourChanged)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxMDIClientWindow, wxNotebook)
  EVT_SCROLL(wxMDIClientWindow::OnScroll)
  EVT_NOTEBOOK_PAGE_CHANGED(wxID_NOTEBOOK_CLIENT_AREA, wxMDIClientWindow::OnPageChanged)
END_EVENT_TABLE()

#endif

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
        return TRUE;
    }
    else
        return FALSE;
}

wxMDIParentFrame::~wxMDIParentFrame()
{
    // Make sure we delete the client window last of all
    RemoveChild(m_clientWindow);

    DestroyChildren();

    delete m_clientWindow;
    m_clientWindow = NULL;
}

// Get size *available for subwindows* i.e. excluding menu bar.
void wxMDIParentFrame::GetClientSize(int *x, int *y) const
{
    wxFrame::GetClientSize(x, y);
}

void wxMDIParentFrame::SetMenuBar(wxMenuBar *menu_bar)
{
    m_frameMenuBar = menu_bar;

    SetChildMenuBar((wxMDIChildFrame*) NULL);
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
        return FALSE;

    inEvent = event.GetEventType();
    
    bool res = FALSE;
    if (m_activeChild && event.IsKindOf(CLASSINFO(wxCommandEvent)))
    {
      res = m_activeChild->GetEventHandler()->ProcessEvent(event);
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

    m_backgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_APPWORKSPACE);
    m_foregroundColour = *wxBLACK;
    m_windowFont = wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT);

    if ( id > -1 )
        m_windowId = id;
    else
        m_windowId = (int)NewControlId();

    wxMDIClientWindow* clientWindow = parent->GetClientWindow();

    wxASSERT_MSG( (clientWindow != (wxWindow*) NULL), "Missing MDI client window.");

    if (clientWindow) clientWindow->AddChild(this);

    SetMDIParentFrame(parent);

    int x = pos.x; int y = pos.y;
    int width = size.x; int height = size.y;
    if (width == -1)
        width = 200; // TODO: give reasonable default
    if (height == -1)
        height = 200; // TODO: give reasonable default

    // We're deactivating the old child
    wxMDIChildFrame* oldActiveChild = parent->GetActiveChild();
    if (oldActiveChild)
    {
        wxActivateEvent event(wxEVT_ACTIVATE, FALSE, oldActiveChild->GetId());
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
    
    SetCanAddEventHandler(TRUE);
    AttachWidget (parent, m_mainWidget, (WXWidget) NULL, pos.x, pos.y, size.x, size.y);

    ChangeBackgroundColour();

    // Old stuff
#if 0

    m_frameWidget = (WXWidget) XtVaCreateManagedWidget("main_window",
                    xmMainWindowWidgetClass, (Widget) clientWindow->GetTopWidget(),
                    XmNresizePolicy, XmRESIZE_NONE,
                    NULL);

    // TODO: make sure this doesn't cause problems.
    // I think ~wxFrame will do the right thing since it deletes m_frameWidget,
    // then sets the main widget to NULL.
    m_mainWidget = m_frameWidget;

    m_workArea = (WXWidget) XtVaCreateWidget("form",
                    xmFormWidgetClass, (Widget) m_frameWidget,
                    XmNresizePolicy, XmRESIZE_NONE,
                    NULL);

    m_clientArea = (WXWidget) XtVaCreateWidget("client",
                    xmBulletinBoardWidgetClass, (Widget) m_workArea,
                    XmNmarginWidth, 0,
                    XmNmarginHeight, 0,
                    XmNrightAttachment, XmATTACH_FORM,
                    XmNleftAttachment, XmATTACH_FORM,
                    XmNtopAttachment, XmATTACH_FORM,
                    XmNbottomAttachment, XmATTACH_FORM,
//                    XmNresizePolicy, XmRESIZE_ANY,
                    NULL);

    XtVaSetValues((Widget) m_frameWidget,
      XmNworkWindow, (Widget) m_workArea,
      NULL);

    XtManageChild((Widget) m_clientArea);
    XtManageChild((Widget) m_workArea);

    wxASSERT_MSG ((wxWidgetHashTable->Get((long)m_workArea) == (wxObject*) NULL), "Widget table clash in frame.cpp") ;

    wxAddWindowToTable((Widget) m_workArea, this);

    XtTranslations ptr ;

    XtOverrideTranslations((Widget) m_workArea,
                ptr = XtParseTranslationTable("<Configure>: resize()"));

    XtFree((char *)ptr);

    XtAddCallback((Widget) m_workArea, XmNfocusCallback, 
                (XtCallbackProc)wxFrameFocusProc, (XtPointer)this);

    XtManageChild((Widget) m_mainWidget);

    if (x > -1)
      XtVaSetValues((Widget) m_mainWidget, XmNx, x, NULL);
    if (y > -1)
      XtVaSetValues((Widget) m_mainWidget, XmNy, y, NULL);
    if (width > -1)
      XtVaSetValues((Widget) m_mainWidget, XmNwidth, width, NULL);
    if (height > -1)
      XtVaSetValues((Widget) m_mainWidget, XmNheight, height, NULL);

#endif

    XtManageChild((Widget) m_mainWidget);

    SetTitle(title);

    clientWindow->AddPage(this, title, TRUE);
    clientWindow->Refresh();

    // Positions the toolbar and status bar -- but we don't have any.
    //    PreResize();

    wxModelessWindows.Append(this);
    return TRUE;
}


wxMDIChildFrame::~wxMDIChildFrame()
{
    if (GetMDIParentFrame())
    {
        wxMDIParentFrame* parentFrame = GetMDIParentFrame();

        if (parentFrame->GetActiveChild() == this)
            parentFrame->SetActiveChild((wxMDIChildFrame*) NULL);
        wxMDIClientWindow* clientWindow = parentFrame->GetClientWindow();

        // Remove page if still there
        if (clientWindow->RemovePage(this))
          clientWindow->Refresh();

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
        wxActivateEvent event(wxEVT_ACTIVATE, FALSE, oldActiveChild->GetId());
        event.SetEventObject( oldActiveChild );
        oldActiveChild->GetEventHandler()->ProcessEvent(event);
    }

    wxActivateEvent event(wxEVT_ACTIVATE, TRUE, this->GetId());
    event.SetEventObject( this );
    this->GetEventHandler()->ProcessEvent(event);
}

void wxMDIChildFrame::OnLower()
{
    wxMDIParentFrame* parentFrame = (wxMDIParentFrame*) GetParent() ;
    wxMDIChildFrame* oldActiveChild = parentFrame->GetActiveChild();

    if (oldActiveChild == this)
    {
        wxActivateEvent event(wxEVT_ACTIVATE, FALSE, oldActiveChild->GetId());
        event.SetEventObject( oldActiveChild );
        oldActiveChild->GetEventHandler()->ProcessEvent(event);
    }
    // TODO: unfortunately we don't now know which is the top-most child,
    // so make the active child NULL.
    parentFrame->SetActiveChild((wxMDIChildFrame*) NULL);
}
#endif

// Set the client size (i.e. leave the calculation of borders etc.
// to wxWindows)
void wxMDIChildFrame::SetClientSize(int width, int height)
{
  wxWindow::SetClientSize(width, height);
}

void wxMDIChildFrame::GetClientSize(int* width, int* height) const
{
    wxWindow::GetSize(width, height);
}

void wxMDIChildFrame::SetSize(int x, int y, int width, int height, int sizeFlags)
{
  wxWindow::SetSize(x, y, width, height, sizeFlags);
}

void wxMDIChildFrame::GetSize(int* width, int* height) const
{
    wxWindow::GetSize(width, height);
}

void wxMDIChildFrame::GetPosition(int *x, int *y) const
{
    wxWindow::GetPosition(x, y);
}

bool wxMDIChildFrame::Show(bool show)
{
    m_visibleStatus = show; /* show-&-hide fix */
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
    m_icon = icon;
    if (m_icon.Ok())
    {
      /* TODO: doesn't work yet (crashes in XCopyArea)
        Pixmap pixmap = (Pixmap) m_icon.GetPixmap();
        m_mdiWindow->setPixmap(pixmap);
	*/
    }
}

void wxMDIChildFrame::SetTitle(const wxString& title)
{
    m_title = title;
    // TODO: set parent frame title
}

// MDI operations
void wxMDIChildFrame::Maximize()
{
    // TODO
}

void wxMDIChildFrame::Iconize(bool iconize)
{
  // TODO
}

bool wxMDIChildFrame::IsIconized() const
{
    return FALSE;
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

void wxMDIChildFrame::SetSizeHints(int WXUNUSED(minW), int WXUNUSED(minH), int WXUNUSED(maxW), int WXUNUSED(maxH), int WXUNUSED(incW), int WXUNUSED(incH))
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
  //    m_windowParent = parent;
    //    m_backgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_APPWORKSPACE);

    return wxNotebook::Create(parent, wxID_NOTEBOOK_CLIENT_AREA, wxPoint(0, 0), wxSize(100, 100), 0);
}

void wxMDIClientWindow::SetSize(int x, int y, int width, int height, int sizeFlags)
{
    wxWindow::SetSize(x, y, width, height, sizeFlags);
}

void wxMDIClientWindow::SetClientSize(int width, int height)
{
    wxWindow::SetClientSize(width, height);
}

void wxMDIClientWindow::GetClientSize(int *width, int *height) const
{
    wxWindow::GetClientSize(width, height);
}

void wxMDIClientWindow::GetSize(int *width, int *height) const
{
    wxWindow::GetSize(width, height);
}

void wxMDIClientWindow::GetPosition(int *x, int *y) const
{
    wxWindow::GetPosition(x, y);
}

// Explicitly call default scroll behaviour
void wxMDIClientWindow::OnScroll(wxScrollEvent& event)
{
    Default(); // Default processing
}

void wxMDIClientWindow::OnPageChanged(wxNotebookEvent& event)
{
    // Notify child that it has been activated
    if (event.GetOldSelection() != -1)
    {
        wxMDIChildFrame* oldChild = (wxMDIChildFrame*) GetPage(event.GetOldSelection());
        if (oldChild)
        {
            wxActivateEvent event(wxEVT_ACTIVATE, FALSE, oldChild->GetId());
            event.SetEventObject( oldChild );
            oldChild->GetEventHandler()->ProcessEvent(event);
        }
    }
    wxMDIChildFrame* activeChild = (wxMDIChildFrame*) GetPage(event.GetSelection());
    if (activeChild)
    {
        wxActivateEvent event(wxEVT_ACTIVATE, TRUE, activeChild->GetId());
        event.SetEventObject( activeChild );
        activeChild->GetEventHandler()->ProcessEvent(event);

        if (activeChild->GetMDIParentFrame())
        {
            activeChild->GetMDIParentFrame()->SetActiveChild(activeChild);
            activeChild->GetMDIParentFrame()->SetChildMenuBar(activeChild);
        }
    }
    event.Skip();
}
