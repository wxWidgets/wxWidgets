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

#include "mdi/lib/XsMDICanvas.h"
#include "mdi/lib/XsMotifWindow.h"

#include "wx/motif/private.h"

extern wxList wxModelessWindows;

// Implemented in frame.cpp
extern void wxFrameFocusProc(Widget workArea, XtPointer clientData, 
                      XmAnyCallbackStruct *cbs);

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxMDIParentFrame, wxFrame)
IMPLEMENT_DYNAMIC_CLASS(wxMDIChildFrame, wxFrame)
IMPLEMENT_DYNAMIC_CLASS(wxMDIClientWindow, wxWindow)

BEGIN_EVENT_TABLE(wxMDIParentFrame, wxFrame)
  EVT_SIZE(wxMDIParentFrame::OnSize)
  EVT_ACTIVATE(wxMDIParentFrame::OnActivate)
  EVT_SYS_COLOUR_CHANGED(wxMDIParentFrame::OnSysColourChanged)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxMDIClientWindow, wxWindow)
  EVT_SCROLL(wxMDIClientWindow::OnScroll)
END_EVENT_TABLE()

#endif

  /*
static void _doNothingCallback (Widget, XtPointer, XtPointer)
{
}
*/

// wxXsMDIWindow represents the MDI child frame, as far as the MDI
// package is concerned.
// TODO: override raise, so we can tell which is the 'active'
// (raised) window. We can also use it to send wxActivateEvents,
// and switching menubars when we make the child frame menubar
// appear on the parent frame.

// Note: see XsMotifWindow.C, _XsMotifMenu::_processItem for
// where user menu selections are processed.
// When Close is selected, _win->close() is called.

class wxXsMDIWindow: public XsMotifWindow
{
public:
  wxMDIChildFrame* m_childFrame;

  wxXsMDIWindow(const char* name, wxMDIChildFrame* frame): XsMotifWindow(name)
  {
    m_childFrame = frame;
  }
  virtual void setSize(Dimension w, Dimension h)
  {
      XsMotifWindow::setSize(w, h);

      // Generate wxSizeEvent here, I think. Maybe also restore, maximize
      // Probably don't need to generate size event here since work area
      // is used
      wxSizeEvent event(wxSize(w, h), m_childFrame->GetId());
      event.SetEventObject(m_childFrame);
      m_childFrame->ProcessEvent(event);
  }
  virtual void close()
  {
      XsMotifWindow::close();
      m_childFrame->Close();
  }
  virtual void _buildClientArea(Widget parent)
  {
      m_childFrame->BuildClientArea((WXWidget) parent);

    // Code from MDI sample
#if 0
   assert (parent != 0);
   
   Widget   pulldown;
   Widget   cascade;
   Widget   button;
   
// Create a main window with some dummy menus

   Widget mainW = XtVaCreateWidget ("mainWin", xmMainWindowWidgetClass, parent,
      XmNtopAttachment, XmATTACH_FORM, XmNbottomAttachment, XmATTACH_FORM,
      XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM,
      NULL);
      
// Create a menubar

   Widget menuBar = XmCreateMenuBar (mainW, "menuBar", NULL, 0);

// Create the "file" menu

   pulldown = XmCreatePulldownMenu (menuBar, "pulldown", NULL, 0);
   cascade = XtVaCreateManagedWidget ("fileMenu", xmCascadeButtonGadgetClass,
      menuBar, XmNsubMenuId, pulldown, NULL);
   
   button = XtVaCreateManagedWidget ("openMenuItem", xmPushButtonGadgetClass,
      pulldown, NULL);
   XtAddCallback (button, XmNactivateCallback, _doNothingCallback, (XtPointer)this);
      
   button = XtVaCreateManagedWidget ("newMenuItem", xmPushButtonGadgetClass,
      pulldown, NULL);
   //   XtAddCallback (button, XmNactivateCallback, _newWindowCallback, (XtPointer)this);

// Create the "edit" menu

   pulldown = XmCreatePulldownMenu (menuBar, "pulldown", NULL, 0);
   cascade = XtVaCreateManagedWidget ("editMenu", xmCascadeButtonGadgetClass,
      menuBar, XmNsubMenuId, pulldown, NULL);
   
   button = XtVaCreateManagedWidget ("cutMenuItem", xmPushButtonGadgetClass,
      pulldown, NULL);
   XtAddCallback (button, XmNactivateCallback, _doNothingCallback, (XtPointer)this);
      
   button = XtVaCreateManagedWidget ("copyMenuItem", xmPushButtonGadgetClass,
      pulldown, NULL);
   XtAddCallback (button, XmNactivateCallback, _doNothingCallback, (XtPointer)this);

   button = XtVaCreateManagedWidget ("pasteMenuItem", xmPushButtonGadgetClass,
      pulldown, NULL);
   XtAddCallback (button, XmNactivateCallback, _doNothingCallback, (XtPointer)this);

// Create the help menu

   pulldown = XmCreatePulldownMenu (menuBar, "pulldown", NULL, 0);
   cascade = XtVaCreateManagedWidget ("helpMenu", xmCascadeButtonGadgetClass,
      menuBar, XmNsubMenuId, pulldown, NULL);
   
   button = XtVaCreateManagedWidget ("aboutMenuItem", xmPushButtonGadgetClass,
      pulldown, NULL);
   XtAddCallback (button, XmNactivateCallback, _doNothingCallback, (XtPointer)this);
      
   XtVaSetValues (menuBar, XmNmenuHelpWidget, cascade, NULL);

// Manage the menubar

   XtManageChild (menuBar);
   
// Create the work area

   const int nargs = 8;
   Arg   args[nargs];
   int n;
	    
   n = 0;
   XtSetArg (args[n], XmNscrollingPolicy, XmAUTOMATIC); n++;
   XtSetArg (args[n], XmNhighlightThickness, (Dimension)0);    n++;
   XtSetArg (args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
   XtSetArg (args[n], XmNeditable, True); n++;
   XtSetArg (args[n], XmNwordWrap, False); n++;
   XtSetArg (args[n], XmNcursorPositionVisible, True);   n++;
   XtSetArg (args[n], XmNverifyBell, True); n++;
			      
   assert (n <= nargs);
				 
   Widget scrolledText = XmCreateScrolledText (mainW, "scrolledText", args, n);
   XtManageChild (scrolledText);
   
// Set the main window area

   XtVaSetValues (mainW, XmNmenuBar, menuBar, XmNworkWindow,
      XtParent (scrolledText), NULL);
   
   XtManageChild (mainW);
#endif
  }
  void Show() { show(); }
};

// Parent frame

wxMDIParentFrame::wxMDIParentFrame()
{
    m_clientWindow = NULL;
}

bool wxMDIParentFrame::Create(wxWindow *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    m_clientWindow = NULL;

    bool success = wxFrame::Create(parent, id, title, pos, size, style, name);
    if (success)
    {
        // TODO: app cannot override OnCreateClient since
        // wxMDIParentFrame::OnCreateClient will still be called
        // (we're in the constructor). How to resolve?

        m_clientWindow = OnCreateClient();
	// Uses own style for client style
	m_clientWindow->CreateClient(this, GetWindowStyleFlag());
        return TRUE;
    }
    else
        return FALSE;
}

wxMDIParentFrame::~wxMDIParentFrame()
{
    DestroyChildren();
    delete m_clientWindow;
}

// Get size *available for subwindows* i.e. excluding menu bar.
void wxMDIParentFrame::GetClientSize(int *x, int *y) const
{
    wxFrame::GetClientSize(x, y);
}

void wxMDIParentFrame::SetMenuBar(wxMenuBar *menu_bar)
{
    wxFrame::SetMenuBar(menu_bar);
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
    // TODO
    return NULL;
}

// Create the client window class (don't Create the window,
// just return a new class)
wxMDIClientWindow *wxMDIParentFrame::OnCreateClient()
{
	return new wxMDIClientWindow ;
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

    if ( id > -1 )
        m_windowId = id;
    else
        m_windowId = (int)NewControlId();

    if (parent) parent->AddChild(this);

    int x = pos.x; int y = pos.y;
    int width = size.x; int height = size.y;

    wxMDIClientWindow* clientWindow = parent->GetClientWindow();
    if (!clientWindow)
        return FALSE;

    m_mdiWindow = new wxXsMDIWindow("mdiChildWindow", this);
    clientWindow->GetMDICanvas()->add(m_mdiWindow);
    m_mdiWindow->Show();
#if 0
    m_mainWidget = (WXWidget) (Widget) (*m_mdiWindow);

    m_frameWidget = (WXWidget) XtVaCreateManagedWidget("main_window",
                    xmMainWindowWidgetClass, (Widget) m_mainWidget,
                    XmNresizePolicy, XmRESIZE_NONE,
                    NULL);

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

    if (x > -1)
      XtVaSetValues((Widget) m_mainWidget, XmNx, x, NULL);
    if (y > -1)
      XtVaSetValues((Widget) m_mainWidget, XmNy, y, NULL);
    if (width > -1)
      XtVaSetValues((Widget) m_mainWidget, XmNwidth, width, NULL);
    if (height > -1)
      XtVaSetValues((Widget) m_mainWidget, XmNheight, height, NULL);
#endif

    SetTitle(title);

    PreResize();

    wxSizeEvent sizeEvent(wxSize(width, height), GetId());
    sizeEvent.SetEventObject(this);

    GetEventHandler()->ProcessEvent(sizeEvent);

    wxModelessWindows.Append(this);
    return TRUE;
}

void wxMDIChildFrame::BuildClientArea(WXWidget parent)
{
    m_mainWidget = parent;

    m_frameWidget = (WXWidget) XtVaCreateManagedWidget("main_window",
                    xmMainWindowWidgetClass, (Widget) m_mainWidget,
                    XmNresizePolicy, XmRESIZE_NONE,
                    XmNtopAttachment, XmATTACH_FORM,
                    XmNbottomAttachment, XmATTACH_FORM,
                    XmNleftAttachment, XmATTACH_FORM,
                    XmNrightAttachment, XmATTACH_FORM,
                    NULL);

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

    /*
    int x = pos.x; int y = pos.y;
    int width = size.x; int height = size.y;

    if (x > -1)
      XtVaSetValues((Widget) m_mainWidget, XmNx, x, NULL);
    if (y > -1)
      XtVaSetValues((Widget) m_mainWidget, XmNy, y, NULL);
    if (width > -1)
      XtVaSetValues((Widget) m_mainWidget, XmNwidth, width, NULL);
    if (height > -1)
      XtVaSetValues((Widget) m_mainWidget, XmNheight, height, NULL);
      */

    XtManageChild((Widget) m_frameWidget);
}


wxMDIChildFrame::~wxMDIChildFrame()
{
    wxMDIClientWindow* clientWindow = ((wxMDIParentFrame*)GetParent())->GetClientWindow();
    clientWindow->GetMDICanvas()->remove(m_mdiWindow);
    m_mainWidget = (WXWidget) 0;
}

// Set the client size (i.e. leave the calculation of borders etc.
// to wxWindows)
void wxMDIChildFrame::SetClientSize(int width, int height)
{
    wxFrame::SetClientSize(width, height);
}

void wxMDIChildFrame::GetClientSize(int* width, int* height) const
{
    wxFrame::GetClientSize(width, height);
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
    return TRUE;
}

void wxMDIChildFrame::SetMenuBar(wxMenuBar *menu_bar)
{
    // TODO
    // Currently, the menu appears on the child frame. 
    // It should eventually be recreated on the main frame
    // whenever the child is activated.
    wxFrame::SetMenuBar(menu_bar);
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
    m_mdiWindow->setTitle(title);
    m_mdiWindow->setIconName(title);
}

// MDI operations
void wxMDIChildFrame::Maximize()
{
   m_mdiWindow->maximize();
}

void wxMDIChildFrame::Iconize(bool iconize)
{
   if (iconize)
      m_mdiWindow->minimize();
   else
      m_mdiWindow->restore();
}

bool wxMDIChildFrame::IsIconized() const
{
    return m_mdiWindow->minimized();
}

void wxMDIChildFrame::Restore()
{
    m_mdiWindow->restore();
}

void wxMDIChildFrame::Activate()
{
    m_mdiWindow->raise();
}

// Client window

wxMDIClientWindow::wxMDIClientWindow()
{
    m_mdiCanvas = NULL;
    m_topWidget = (WXWidget) 0;
}

wxMDIClientWindow::~wxMDIClientWindow()
{
    DestroyChildren();
    delete m_mdiCanvas;

    m_mainWidget = (WXWidget) 0;
    m_topWidget = (WXWidget) 0;
}

bool wxMDIClientWindow::CreateClient(wxMDIParentFrame *parent, long style)
{
    m_windowParent = parent;
    m_backgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_APPWORKSPACE);
    m_mdiCanvas = new XsMDICanvas("mdiClientWindow", (Widget) parent->GetClientWidget());
    m_mainWidget = (WXWidget) m_mdiCanvas->GetDrawingArea();
    //    m_topWidget = (WXWidget) m_mdiCanvas->GetBase();
    m_topWidget = (WXWidget) (Widget) (*m_mdiCanvas);

    m_mdiCanvas->show();

    return TRUE;
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

