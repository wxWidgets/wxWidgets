/////////////////////////////////////////////////////////////////////////////
// Name:        windows.cpp
// Purpose:     wxWindow
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "window.h"
#endif

#include "wx/setup.h"
#include "wx/menu.h"
#include "wx/dc.h"
#include "wx/dcclient.h"
#include "wx/utils.h"
#include "wx/app.h"
#include "wx/panel.h"
#include "wx/layout.h"
#include "wx/dialog.h"
#include "wx/listbox.h"
#include "wx/button.h"
#include "wx/settings.h"
#include "wx/msgdlg.h"
#include "wx/frame.h"

#include "wx/menuitem.h"
#include "wx/log.h"

#if  USE_DRAG_AND_DROP
#include "wx/dnd.h"
#endif

#include <Xm/Xm.h>

#include <Xm/DrawingA.h>
#include <Xm/ScrolledW.h>
#include <Xm/ScrollBar.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>

#include "wx/motif/private.h"

#include <string.h>

#define SCROLL_MARGIN 4
void wxCanvasRepaintProc (Widget, XtPointer, XmDrawingAreaCallbackStruct * cbs);
void wxCanvasInputEvent (Widget drawingArea, XtPointer data, XmDrawingAreaCallbackStruct * cbs);
void wxCanvasMotionEvent (Widget, XButtonEvent * event);
void wxCanvasEnterLeave (Widget drawingArea, XtPointer clientData, XCrossingEvent * event);

#define event_left_is_down(x) ((x)->xbutton.state & Button1Mask)
#define event_middle_is_down(x) ((x)->xbutton.state & Button2Mask)
#define event_right_is_down(x) ((x)->xbutton.state & Button3Mask)

extern wxList wxPendingDelete;

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxWindow, wxEvtHandler)

BEGIN_EVENT_TABLE(wxWindow, wxEvtHandler)
  EVT_CHAR(wxWindow::OnChar)
  EVT_ERASE_BACKGROUND(wxWindow::OnEraseBackground)
  EVT_SYS_COLOUR_CHANGED(wxWindow::OnSysColourChanged)
  EVT_INIT_DIALOG(wxWindow::OnInitDialog)
  EVT_IDLE(wxWindow::OnIdle)
END_EVENT_TABLE()

#endif


// Constructor
wxWindow::wxWindow()
{
    // Generic
    m_windowId = 0;
    m_windowStyle = 0;
    m_windowParent = NULL;
    m_windowEventHandler = this;
    m_windowName = "";
    m_windowCursor = *wxSTANDARD_CURSOR;
    m_children = new wxList;
    m_constraints = NULL;
    m_constraintsInvolvedIn = NULL;
    m_windowSizer = NULL;
    m_sizerParent = NULL;
    m_autoLayout = FALSE;
    m_windowValidator = NULL;
    m_defaultItem = NULL;
    m_returnCode = 0;
    m_caretWidth = 0; m_caretHeight = 0;
    m_caretEnabled = FALSE;
    m_caretShown = FALSE;
    m_backgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_WINDOW) ; ;
    m_foregroundColour = *wxBLACK;
    m_defaultForegroundColour = *wxBLACK ;
    m_defaultBackgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE) ;

#if  USE_DRAG_AND_DROP
    m_pDropTarget = NULL;
#endif

    /// Motif-specific
    m_mainWidget = (WXWidget) 0;
    m_button1Pressed = FALSE;
    m_button2Pressed = FALSE;
    m_button3Pressed = FALSE;
    m_winCaptured = FALSE;
    m_isShown = TRUE;
    m_hScrollBar = (WXWidget) 0;
    m_vScrollBar = (WXWidget) 0;
    m_borderWidget = (WXWidget) 0;
    m_scrolledWindow = (WXWidget) 0;
    m_drawingArea = (WXWidget) 0;
    m_hScroll = FALSE;
    m_vScroll = FALSE;
    m_hScrollingEnabled = FALSE;
    m_vScrollingEnabled = FALSE;
    m_backingPixmap = (WXPixmap) 0;
    m_pixmapWidth = 0;
    m_pixmapHeight = 0;
    m_pixmapOffsetX = 0;
    m_pixmapOffsetY = 0;
    m_lastTS = 0;
    m_lastButton = 0;
}

// Destructor
wxWindow::~wxWindow()
{
  //// Motif-specific

  // If m_drawingArea, we're a fully-fledged window with drawing area, scrollbars etc. (what wxCanvas used to be)
  if (m_drawingArea)
  {
    // Destroy children before destroying self
    DestroyChildren();

    if (m_backingPixmap)
      XFreePixmap (XtDisplay ((Widget) GetMainWidget()), (Pixmap) m_backingPixmap);

    Widget w = (Widget) m_drawingArea;
    wxDeleteWindowFromTable(w);

    if (w)
      XtDestroyWidget(w);
    m_mainWidget = (WXWidget) 0;

    // Only if we're _really_ a canvas (not a dialog box/panel)
    if (m_scrolledWindow)
    {
      wxDeleteWindowFromTable((Widget) m_scrolledWindow);
    }

    if (m_hScrollBar)
    {
      XtUnmanageChild ((Widget) m_hScrollBar);
      XtDestroyWidget ((Widget) m_hScrollBar);
    }
    if (m_vScrollBar)
    {
      XtUnmanageChild ((Widget) m_vScrollBar);
      XtDestroyWidget ((Widget) m_vScrollBar);
    }
    if (m_scrolledWindow)
    {
      XtUnmanageChild ((Widget) m_scrolledWindow);
      XtDestroyWidget ((Widget) m_scrolledWindow);
    }

    if (m_borderWidget)
    {
      XtDestroyWidget ((Widget) m_borderWidget);
      m_borderWidget = (WXWidget) 0;
    }
  }


  //// Generic stuff

	// Have to delete constraints/sizer FIRST otherwise
	// sizers may try to look at deleted windows as they
	// delete themselves.
#if USE_CONSTRAINTS
    DeleteRelatedConstraints();
    if (m_constraints)
    {
        // This removes any dangling pointers to this window
        // in other windows' constraintsInvolvedIn lists.
        UnsetConstraints(m_constraints);
        delete m_constraints;
        m_constraints = NULL;
    }
    if (m_windowSizer)
    {
        delete m_windowSizer;
        m_windowSizer = NULL;
    }
    // If this is a child of a sizer, remove self from parent
    if (m_sizerParent)
        m_sizerParent->RemoveChild((wxWindow *)this);
#endif

    if (m_windowParent)
        m_windowParent->RemoveChild(this);

    DestroyChildren();

    // Destroy the window
    if (GetMainWidget())
    {
      wxDeleteWindowFromTable((Widget) GetMainWidget());
      XtDestroyWidget((Widget) GetMainWidget());
      SetMainWidget((WXWidget) NULL);
    }

    delete m_children;
    m_children = NULL;

    // Just in case the window has been Closed, but
    // we're then deleting immediately: don't leave
    // dangling pointers.
    wxPendingDelete.DeleteObject(this);

    if ( m_windowValidator )
	    delete m_windowValidator;
}

// Destroy the window (delayed, if a managed window)
bool wxWindow::Destroy()
{
    delete this;
    return TRUE;
}

// Constructor
bool wxWindow::Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    // Generic
    m_windowId = 0;
    m_windowStyle = 0;
    m_windowParent = NULL;
    m_windowEventHandler = this;
    m_windowName = "";
    m_windowCursor = *wxSTANDARD_CURSOR;
    m_constraints = NULL;
    m_constraintsInvolvedIn = NULL;
    m_windowSizer = NULL;
    m_sizerParent = NULL;
    m_autoLayout = FALSE;
    m_windowValidator = NULL;
#if USE_DRAG_AND_DROP
    m_pDropTarget = NULL;
#endif
    m_caretWidth = 0; m_caretHeight = 0;
    m_caretEnabled = FALSE;
    m_caretShown = FALSE;
    m_minSizeX = -1;
    m_minSizeY = -1;
    m_maxSizeX = -1;
    m_maxSizeY = -1;
    m_defaultItem = NULL;
    m_windowParent = NULL;

    // Motif-specific
    m_mainWidget = (WXWidget) 0;
    m_button1Pressed = FALSE;
    m_button2Pressed = FALSE;
    m_button3Pressed = FALSE;
    m_winCaptured = FALSE;
    m_isShown = TRUE;
    m_hScrollBar = (WXWidget) 0;
    m_vScrollBar = (WXWidget) 0;
    m_borderWidget = (WXWidget) 0;
    m_scrolledWindow = (WXWidget) 0;
    m_drawingArea = (WXWidget) 0;
    m_hScroll = FALSE;
    m_vScroll = FALSE;
    m_hScrollingEnabled = FALSE;
    m_vScrollingEnabled = FALSE;
    m_backingPixmap = (WXPixmap) 0;
    m_pixmapWidth = 0;
    m_pixmapHeight = 0;
    m_pixmapOffsetX = 0;
    m_pixmapOffsetY = 0;

    if (!parent)
        return FALSE;

    if (parent) parent->AddChild(this);

    m_returnCode = 0;

    SetName(name);

    if ( id == -1 )
    	m_windowId = (int)NewControlId();
    else
	m_windowId = id;

    m_backgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_WINDOW) ; ;
    m_foregroundColour = *wxBLACK;
    m_defaultForegroundColour = *wxBLACK ;
    m_defaultBackgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE) ;

    m_windowStyle = style;

    if ( id == -1 )
    	m_windowId = (int)NewControlId();
    else
	m_windowId = id;

    //// TODO: we should probably optimize by only creating a
    //// a drawing area if we have one or more scrollbars (wxVSCROLL/wxHSCROLL).
    //// But for now, let's simplify things by always creating the
    //// drawing area, since otherwise the translations are different.

  // New translations for getting mouse motion feedback
  String translations =
  "<Btn1Motion>: wxCanvasMotionEvent() DrawingAreaInput() ManagerGadgetButtonMotion()\n\
     <Btn2Motion>: wxCanvasMotionEvent() DrawingAreaInput() ManagerGadgetButtonMotion()\n\
     <Btn3Motion>: wxCanvasMotionEvent() DrawingAreaInput() ManagerGadgetButtonMotion()\n\
     <BtnMotion>: wxCanvasMotionEvent() DrawingAreaInput() ManagerGadgetButtonMotion()\n\
     <Btn1Down>: DrawingAreaInput() ManagerGadgetArm()\n\
     <Btn2Down>: DrawingAreaInput() ManagerGadgetArm()\n\
     <Btn3Down>: DrawingAreaInput() ManagerGadgetArm()\n\
     <Btn1Up>: DrawingAreaInput() ManagerGadgetActivate()\n\
     <Btn2Up>: DrawingAreaInput() ManagerGadgetActivate()\n\
     <Btn3Up>: DrawingAreaInput() ManagerGadgetActivate()\n\
     <Motion>: wxCanvasMotionEvent() DrawingAreaInput()\n\
     <EnterWindow>: wxCanvasMotionEvent() DrawingAreaInput()\n\
     <LeaveWindow>: wxCanvasMotionEvent() DrawingAreaInput()\n\
     <Key>: DrawingAreaInput()";

  XtActionsRec actions[1];
  actions[0].string = "wxCanvasMotionEvent";
  actions[0].proc = (XtActionProc) wxCanvasMotionEvent;
  XtAppAddActions ((XtAppContext) wxTheApp->GetAppContext(), actions, 1);

  Widget parentWidget = (Widget) parent->GetClientWidget(); 
  if (style & wxBORDER)
    m_borderWidget = (WXWidget) XtVaCreateManagedWidget ("canvasBorder",
				      xmFrameWidgetClass, parentWidget,
					    XmNshadowType, XmSHADOW_IN,
					    NULL);

  m_scrolledWindow = (WXWidget) XtVaCreateManagedWidget ("scrolledWindow",
					    xmScrolledWindowWidgetClass, m_borderWidget ? (Widget) m_borderWidget : parentWidget,
                                  XmNspacing, 0,
				  XmNscrollingPolicy, XmAPPLICATION_DEFINED,
					    NULL);

  XtTranslations ptr;
  m_drawingArea = (WXWidget) XtVaCreateWidget ((char*) (const char*) name,
				   xmDrawingAreaWidgetClass, (Widget) m_scrolledWindow,
					 XmNunitType, XmPIXELS,
//					 XmNresizePolicy, XmRESIZE_ANY,
					 XmNresizePolicy, XmRESIZE_NONE,
                                         XmNmarginHeight, 0,
                                         XmNmarginWidth, 0,
	      XmNtranslations, ptr = XtParseTranslationTable (translations),
					 NULL);
  /*
  if (GetWindowStyleFlag() & wxOVERRIDE_KEY_TRANSLATIONS)
  {
    XtFree ((char *) ptr);
    ptr = XtParseTranslationTable ("<Key>: DrawingAreaInput()");
    XtOverrideTranslations ((Widget) m_drawingArea, ptr);
    XtFree ((char *) ptr);
  }
  */

  wxAddWindowToTable((Widget) m_drawingArea, this);
  wxAddWindowToTable((Widget) m_scrolledWindow, this);

  /*
   * This order is very important in Motif 1.2.1
   *
   */

  XtRealizeWidget ((Widget) m_scrolledWindow);
  XtRealizeWidget ((Widget) m_drawingArea);
  XtManageChild ((Widget) m_drawingArea);

  XtOverrideTranslations ((Widget) m_drawingArea,
		   ptr = XtParseTranslationTable ("<Configure>: resize()"));
  XtFree ((char *) ptr);

  XtAddCallback ((Widget) m_drawingArea, XmNexposeCallback, (XtCallbackProc) wxCanvasRepaintProc, (XtPointer) this);
  XtAddCallback ((Widget) m_drawingArea, XmNinputCallback, (XtCallbackProc) wxCanvasInputEvent, (XtPointer) this);

  /* TODO?
  display = XtDisplay (scrolledWindow);
  xwindow = XtWindow (drawingArea);
  */

  XtAddEventHandler ((Widget) m_drawingArea, PointerMotionHintMask | EnterWindowMask | LeaveWindowMask | FocusChangeMask,
    False, (XtEventHandler) wxCanvasEnterLeave, (XtPointer) this);

    return TRUE;
}

void wxWindow::SetFocus()
{
  XmProcessTraversal((Widget) GetMainWidget(), XmTRAVERSE_CURRENT);
  XmProcessTraversal((Widget) GetMainWidget(), XmTRAVERSE_CURRENT);
}

void wxWindow::Enable(bool enable)
{
  if (GetMainWidget())
  {
    XtSetSensitive((Widget) GetMainWidget(), enable);
    XmUpdateDisplay((Widget) GetMainWidget());
  }
}

void wxWindow::CaptureMouse()
{
  if (m_winCaptured)
    return;
    
  if (GetMainWidget())
    XtAddGrab((Widget) GetMainWidget(), TRUE, FALSE);

  m_winCaptured = TRUE;
}

void wxWindow::ReleaseMouse()
{
  if (!m_winCaptured)
    return;
    
  if (GetMainWidget())
    XtRemoveGrab((Widget) GetMainWidget());
  m_winCaptured = FALSE;
}

// Push/pop event handler (i.e. allow a chain of event handlers
// be searched)
void wxWindow::PushEventHandler(wxEvtHandler *handler)
{
	handler->SetNextHandler(GetEventHandler());
	SetEventHandler(handler);
}

wxEvtHandler *wxWindow::PopEventHandler(bool deleteHandler)
{
	if ( GetEventHandler() )
	{
		wxEvtHandler *handlerA = GetEventHandler();
		wxEvtHandler *handlerB = handlerA->GetNextHandler();
		handlerA->SetNextHandler(NULL);
		SetEventHandler(handlerB);
		if ( deleteHandler )
		{
			delete handlerA;
			return NULL;
		}
		else
			return handlerA;
	}
	else
		return NULL;
}

#if    USE_DRAG_AND_DROP

void wxWindow::SetDropTarget(wxDropTarget *pDropTarget)
{
  if ( m_pDropTarget != 0 ) {
    delete m_pDropTarget;
  }

  m_pDropTarget = pDropTarget;
  if ( m_pDropTarget != 0 )
  {
    // TODO
  }
}

#endif

// Old style file-manager drag&drop
void wxWindow::DragAcceptFiles(bool accept)
{
    // TODO
}

// Get total size
void wxWindow::GetSize(int *x, int *y) const
{
  if (m_drawingArea)
  {
    CanvasGetSize(x, y);
    return;
  }

  Widget widget = (Widget) GetMainWidget();
  Dimension xx, yy;
  XtVaGetValues(widget, XmNwidth, &xx, XmNheight, &yy, NULL);
  *x = xx; *y = yy;
}

void wxWindow::GetPosition(int *x, int *y) const
{
  if (m_drawingArea)
  {
    CanvasGetPosition(x, y);
    return;
  }
  Widget widget = (Widget) GetMainWidget();
  Position xx, yy;
  XtVaGetValues(widget, XmNx, &xx, XmNy, &yy, NULL);
  *x = xx; *y = yy;
}

void wxWindow::ScreenToClient(int *x, int *y) const
{
    // TODO
}

void wxWindow::ClientToScreen(int *x, int *y) const
{
  Widget widget = (Widget) GetMainWidget();
  Display *display = XtDisplay(widget);
  Window rootWindow = RootWindowOfScreen(XtScreen(widget));
  Window thisWindow;
  if (this->IsKindOf(CLASSINFO(wxFrame)))
  {
    wxFrame *fr = (wxFrame *)this;
    // TODO
    //    thisWindow = XtWindow(fr->m_clientArea);
  }
  else
    thisWindow = XtWindow((Widget)widget);

  Window childWindow;
  int xx = *x;
  int yy = *y;
  XTranslateCoordinates(display, thisWindow, rootWindow, xx, yy, x, y, &childWindow);
}

void wxWindow::SetCursor(const wxCursor& cursor)
{
  m_windowCursor = cursor;
  if (m_windowCursor.Ok())
  {
    /* TODO when wxCursor implemented
    WXDisplay *dpy = GetXDisplay();
    Cursor x_cursor = cursor.GetXCursor(dpy);

    Widget w = (Widget) GetMainWidget();
    Window win = XtWindow(w);
    XDefineCursor((Display*) dpy, win, x_cursor);
    */
  }
}


// Get size *available for subwindows* i.e. excluding menu bar etc.
void wxWindow::GetClientSize(int *x, int *y) const
{
  Widget widget = (Widget) GetMainWidget();
  Dimension xx, yy;
  XtVaGetValues(widget, XmNwidth, &xx, XmNheight, &yy, NULL);
  *x = xx; *y = yy;
}

void wxWindow::SetSize(int x, int y, int width, int height, int sizeFlags)
{
  if (m_drawingArea)
  {
    CanvasSetSize(x, y, width, height, sizeFlags);
    return;
  }
  Widget widget = (Widget) GetMainWidget();

  if (x > -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    XtVaSetValues(widget, XmNx, x, NULL);
  if (y > -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    XtVaSetValues(widget, XmNy, y, NULL);
  if (width > -1)
    XtVaSetValues(widget, XmNwidth, width, NULL);
  if (height > -1)
    XtVaSetValues(widget, XmNheight, height, NULL);

  wxSizeEvent sizeEvent(wxSize(width, height), GetId());
  sizeEvent.SetEventObject(this);
  
  GetEventHandler()->ProcessEvent(sizeEvent);
}

void wxWindow::SetClientSize(int width, int height)
{
  if (m_drawingArea)
  {
    CanvasSetClientSize(width, height);
    return;
  }

  Widget widget = (Widget) GetMainWidget();

  if (width > -1)
    XtVaSetValues(widget, XmNwidth, width, NULL);
  if (height > -1)
    XtVaSetValues(widget, XmNheight, height, NULL);

  wxSizeEvent sizeEvent(wxSize(width, height), GetId());
  sizeEvent.SetEventObject(this);
  
  GetEventHandler()->ProcessEvent(sizeEvent);
}

// For implementation purposes - sometimes decorations make the client area
// smaller
wxPoint wxWindow::GetClientAreaOrigin() const
{
    return wxPoint(0, 0);
}

// Makes an adjustment to the window position (for example, a frame that has
// a toolbar that it manages itself).
void wxWindow::AdjustForParentClientOrigin(int& x, int& y, int sizeFlags)
{
    if (((sizeFlags & wxSIZE_NO_ADJUSTMENTS) == 0) && GetParent())
    {
        wxPoint pt(GetParent()->GetClientAreaOrigin());
        x += pt.x; y += pt.y;
    }
}

bool wxWindow::Show(bool show)
{
    if (show)
    {
       if (m_borderWidget || m_scrolledWindow)
       {
           XtMapWidget(m_borderWidget ? (Widget) m_borderWidget : (Widget) m_scrolledWindow);
       }
       else
       {
           XtMapWidget((Widget) GetMainWidget());
       }
    }
    else
    {
       if (m_borderWidget || m_scrolledWindow)
       {
           XtUnmapWidget(m_borderWidget ? (Widget) m_borderWidget : (Widget) m_scrolledWindow);
       }
       else
       {
           XtUnmapWidget((Widget) GetMainWidget());
       }
    }

  /*
    Window xwin = (Window) GetXWindow();
    Display *xdisp = (Display*) GetXDisplay();
    if (show)
        XMapWindow(xdisp, xwin);
    else
        XUnmapWindow(xdisp, xwin);
   */

    m_isShown = show;

    return TRUE;
}

bool wxWindow::IsShown() const
{
    return m_isShown;
}

int wxWindow::GetCharHeight() const
{
    // TODO
    return 0;
}

int wxWindow::GetCharWidth() const
{
    // TODO
    return 0;
}

void wxWindow::GetTextExtent(const wxString& string, int *x, int *y,
                           int *descent, int *externalLeading, const wxFont *theFont, bool) const
{
  wxFont *fontToUse = (wxFont *)theFont;
  if (!fontToUse)
    fontToUse = (wxFont *) & m_windowFont;

    // TODO
}

void wxWindow::Refresh(bool eraseBack, const wxRectangle *rect)
{
    Display *display = XtDisplay((Widget) GetMainWidget());
    Window thisWindow = XtWindow((Widget) GetMainWidget());

    XExposeEvent dummyEvent;
    int width, height;
    GetSize(&width, &height);

    dummyEvent.type = Expose;
    dummyEvent.display = display;
    dummyEvent.send_event = True;
    dummyEvent.window = thisWindow;
    if (rect)
    {
      dummyEvent.x = rect->x;
      dummyEvent.y = rect->y;
      dummyEvent.width = rect->width;
      dummyEvent.height = rect->height;
    }
    else
    {
      dummyEvent.x = 0;
      dummyEvent.y = 0;
      dummyEvent.width = width;
      dummyEvent.height = height;
    }
    dummyEvent.count = 0;

    if (eraseBack)
    {
        wxClientDC dc(this);
        dc.Clear();
    }

    XSendEvent(display, thisWindow, False, ExposureMask, (XEvent *)&dummyEvent);
}

// Responds to colour changes: passes event on to children.
void wxWindow::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    wxNode *node = GetChildren()->First();
    while ( node )
    {
        // Only propagate to non-top-level windows
        wxWindow *win = (wxWindow *)node->Data();
        if ( win->GetParent() )
        {
            wxSysColourChangedEvent event2;
            event.m_eventObject = win;
            win->GetEventHandler()->ProcessEvent(event2);
        }

        node = node->Next();
    }
}

// This can be called by the app (or wxWindows) to do default processing for the current
// event. Save message/event info in wxWindow so they can be used in this function.
long wxWindow::Default()
{
    // TODO
    return 0;
}

void wxWindow::InitDialog()
{
  wxInitDialogEvent event(GetId());
  event.SetEventObject( this );
  GetEventHandler()->ProcessEvent(event);
}

// Default init dialog behaviour is to transfer data to window
void wxWindow::OnInitDialog(wxInitDialogEvent& event)
{
  TransferDataToWindow();
}

// Caret manipulation
void wxWindow::CreateCaret(int w, int h)
{
  m_caretWidth = w;
  m_caretHeight = h;
  m_caretEnabled = TRUE;
}

void wxWindow::CreateCaret(const wxBitmap *WXUNUSED(bitmap))
{
    // TODO
}

void wxWindow::ShowCaret(bool show)
{
    // TODO
}

void wxWindow::DestroyCaret()
{
    // TODO
    m_caretEnabled = FALSE;
}

void wxWindow::SetCaretPos(int x, int y)
{
    // TODO
}

void wxWindow::GetCaretPos(int *x, int *y) const
{
    // TODO
}

wxWindow *wxGetActiveWindow()
{
    // TODO
    return NULL;
}

void wxWindow::SetSizeHints(int minW, int minH, int maxW, int maxH, int incW, int incH)
{
    m_minSizeX = minW;
    m_minSizeY = minH;
    m_maxSizeX = maxW;
    m_maxSizeY = maxH;

    if (!this->IsKindOf(CLASSINFO(wxFrame)))
      return;

    wxFrame *frame = (wxFrame *)this;

    /* Uncomment when wxFrame implemented
    if (minW > -1)
      XtVaSetValues((Widget) frame->m_frameShell, XmNminWidth, minW, NULL);
    if (minH > -1)
      XtVaSetValues((Widget) frame->m_frameShell, XmNminHeight, minH, NULL);
    if (maxW > -1)
      XtVaSetValues((Widget) frame->m_frameShell, XmNmaxWidth, maxW, NULL);
    if (maxH > -1)
      XtVaSetValues((Widget) frame->m_frameShell, XmNmaxHeight, maxH, NULL);
    if (incW > -1)
      XtVaSetValues((Widget) frame->m_frameShell, XmNwidthInc, incW, NULL);
    if (incH > -1)
      XtVaSetValues((Widget) frame->m_frameShell, XmNheightInc, incH, NULL);
      */
}

void wxWindow::Centre(int direction)
{
  int x, y, width, height, panel_width, panel_height, new_x, new_y;

  wxWindow *father = (wxWindow *)GetParent();
  if (!father)
    return;

  father->GetClientSize(&panel_width, &panel_height);
  GetSize(&width, &height);
  GetPosition(&x, &y);

  new_x = -1;
  new_y = -1;

  if (direction & wxHORIZONTAL)
    new_x = (int)((panel_width - width)/2);

  if (direction & wxVERTICAL)
    new_y = (int)((panel_height - height)/2);

  SetSize(new_x, new_y, -1, -1);

}

// Coordinates relative to the window
void wxWindow::WarpPointer (int x, int y)
{
  XWarpPointer (XtDisplay((Widget) GetClientWidget()), None, XtWindow((Widget) GetClientWidget()), 0, 0, 0, 0, x, y);
}

void wxWindow::OnEraseBackground(wxEraseEvent& event)
{
    // TODO
    Default();
}

int wxWindow::GetScrollPos(int orient) const
{
    // TODO
    return 0;
}

// This now returns the whole range, not just the number
// of positions that we can scroll.
int wxWindow::GetScrollRange(int orient) const
{
    // TODO
    return 0;
}

int wxWindow::GetScrollThumb(int orient) const
{
    // TODO
    return 0;
}

void wxWindow::SetScrollPos(int orient, int pos, bool refresh)
{
    // TODO
    return;
}

// New function that will replace some of the above.
void wxWindow::SetScrollbar(int orient, int pos, int thumbVisible,
    int range, bool refresh)
{
    // TODO
}

// Does a physical scroll
void wxWindow::ScrollWindow(int dx, int dy, const wxRectangle *rect)
{
    // TODO
    return;
}

void wxWindow::SetFont(const wxFont& font)
{
    m_windowFont = font;

    if (!m_windowFont.Ok())
	    return;
    // TODO
}

void wxWindow::OnChar(wxKeyEvent& event)
{
    if ( event.KeyCode() == WXK_TAB ) {
        // propagate the TABs to the parent - it's up to it to decide what
        // to do with it
        if ( GetParent() ) {
            if ( GetParent()->ProcessEvent(event) )
                return;
        }
    }
}

void wxWindow::OnPaint(wxPaintEvent& event)
{
	Default();
}

bool wxWindow::IsEnabled() const
{
    // TODO
    return FALSE;
}

// Dialog support: override these and call
// base class members to add functionality
// that can't be done using validators.
// NOTE: these functions assume that controls
// are direct children of this window, not grandchildren
// or other levels of descendant.

// Transfer values to controls. If returns FALSE,
// it's an application error (pops up a dialog)
bool wxWindow::TransferDataToWindow()
{
	wxNode *node = GetChildren()->First();
	while ( node )
	{
		wxWindow *child = (wxWindow *)node->Data();
		if ( child->GetValidator() &&
		     !child->GetValidator()->TransferToWindow() )
		{
			wxMessageBox("Application Error", "Could not transfer data to window", wxOK|wxICON_EXCLAMATION);
			return FALSE;
		}

		node = node->Next();
	}
	return TRUE;
}

// Transfer values from controls. If returns FALSE,
// validation failed: don't quit
bool wxWindow::TransferDataFromWindow()
{
	wxNode *node = GetChildren()->First();
	while ( node )
	{
		wxWindow *child = (wxWindow *)node->Data();
		if ( child->GetValidator() && !child->GetValidator()->TransferFromWindow() )
		{
			return FALSE;
		}

		node = node->Next();
	}
	return TRUE;
}

bool wxWindow::Validate()
{
	wxNode *node = GetChildren()->First();
	while ( node )
	{
		wxWindow *child = (wxWindow *)node->Data();
		if ( child->GetValidator() && /* child->GetValidator()->Ok() && */ !child->GetValidator()->Validate(this) )
		{
			return FALSE;
		}

		node = node->Next();
	}
	return TRUE;
}

// Get the window with the focus
wxWindow *wxWindow::FindFocus()
{
    // TODO
    return NULL;
}

void wxWindow::AddChild(wxWindow *child)
{
    GetChildren()->Append(child);
    child->m_windowParent = this;
}

void wxWindow::RemoveChild(wxWindow *child)
{
    if (GetChildren())
        GetChildren()->DeleteObject(child);
    child->m_windowParent = NULL;
}

void wxWindow::DestroyChildren()
{
  if (GetChildren()) {
    wxNode *node;
    while ((node = GetChildren()->First()) != (wxNode *)NULL) {
      wxWindow *child;
      if ((child = (wxWindow *)node->Data()) != (wxWindow *)NULL) {
        delete child;
		if ( GetChildren()->Member(child) )
			delete node;
      }
    } /* while */
  }
}

void wxWindow::MakeModal(bool modal)
{
  // Disable all other windows
  if (this->IsKindOf(CLASSINFO(wxDialog)) || this->IsKindOf(CLASSINFO(wxFrame)))
  {
    wxNode *node = wxTopLevelWindows.First();
    while (node)
    {
      wxWindow *win = (wxWindow *)node->Data();
      if (win != this)
        win->Enable(!modal);

      node = node->Next();
    }
  }
}

void wxWindow::SetConstraints(wxLayoutConstraints *c)
{
  if (m_constraints)
  {
    UnsetConstraints(m_constraints);
    delete m_constraints;
  }
  m_constraints = c;
  if (m_constraints)
  {
    // Make sure other windows know they're part of a 'meaningful relationship'
    if (m_constraints->left.GetOtherWindow() && (m_constraints->left.GetOtherWindow() != this))
      m_constraints->left.GetOtherWindow()->AddConstraintReference((wxWindow *)this);
    if (m_constraints->top.GetOtherWindow() && (m_constraints->top.GetOtherWindow() != this))
      m_constraints->top.GetOtherWindow()->AddConstraintReference((wxWindow *)this);
    if (m_constraints->right.GetOtherWindow() && (m_constraints->right.GetOtherWindow() != this))
      m_constraints->right.GetOtherWindow()->AddConstraintReference((wxWindow *)this);
    if (m_constraints->bottom.GetOtherWindow() && (m_constraints->bottom.GetOtherWindow() != this))
      m_constraints->bottom.GetOtherWindow()->AddConstraintReference((wxWindow *)this);
    if (m_constraints->width.GetOtherWindow() && (m_constraints->width.GetOtherWindow() != this))
      m_constraints->width.GetOtherWindow()->AddConstraintReference((wxWindow *)this);
    if (m_constraints->height.GetOtherWindow() && (m_constraints->height.GetOtherWindow() != this))
      m_constraints->height.GetOtherWindow()->AddConstraintReference((wxWindow *)this);
    if (m_constraints->centreX.GetOtherWindow() && (m_constraints->centreX.GetOtherWindow() != this))
      m_constraints->centreX.GetOtherWindow()->AddConstraintReference((wxWindow *)this);
    if (m_constraints->centreY.GetOtherWindow() && (m_constraints->centreY.GetOtherWindow() != this))
      m_constraints->centreY.GetOtherWindow()->AddConstraintReference((wxWindow *)this);
  }
}

// This removes any dangling pointers to this window
// in other windows' constraintsInvolvedIn lists.
void wxWindow::UnsetConstraints(wxLayoutConstraints *c)
{
  if (c)
  {
    if (c->left.GetOtherWindow() && (c->top.GetOtherWindow() != this))
      c->left.GetOtherWindow()->RemoveConstraintReference((wxWindow *)this);
    if (c->top.GetOtherWindow() && (c->top.GetOtherWindow() != this))
      c->top.GetOtherWindow()->RemoveConstraintReference((wxWindow *)this);
    if (c->right.GetOtherWindow() && (c->right.GetOtherWindow() != this))
      c->right.GetOtherWindow()->RemoveConstraintReference((wxWindow *)this);
    if (c->bottom.GetOtherWindow() && (c->bottom.GetOtherWindow() != this))
      c->bottom.GetOtherWindow()->RemoveConstraintReference((wxWindow *)this);
    if (c->width.GetOtherWindow() && (c->width.GetOtherWindow() != this))
      c->width.GetOtherWindow()->RemoveConstraintReference((wxWindow *)this);
    if (c->height.GetOtherWindow() && (c->height.GetOtherWindow() != this))
      c->height.GetOtherWindow()->RemoveConstraintReference((wxWindow *)this);
    if (c->centreX.GetOtherWindow() && (c->centreX.GetOtherWindow() != this))
      c->centreX.GetOtherWindow()->RemoveConstraintReference((wxWindow *)this);
    if (c->centreY.GetOtherWindow() && (c->centreY.GetOtherWindow() != this))
      c->centreY.GetOtherWindow()->RemoveConstraintReference((wxWindow *)this);
  }
}

// Back-pointer to other windows we're involved with, so if we delete
// this window, we must delete any constraints we're involved with.
void wxWindow::AddConstraintReference(wxWindow *otherWin)
{
  if (!m_constraintsInvolvedIn)
    m_constraintsInvolvedIn = new wxList;
  if (!m_constraintsInvolvedIn->Member(otherWin))
    m_constraintsInvolvedIn->Append(otherWin);
}

// REMOVE back-pointer to other windows we're involved with.
void wxWindow::RemoveConstraintReference(wxWindow *otherWin)
{
  if (m_constraintsInvolvedIn)
    m_constraintsInvolvedIn->DeleteObject(otherWin);
}

// Reset any constraints that mention this window
void wxWindow::DeleteRelatedConstraints()
{
  if (m_constraintsInvolvedIn)
  {
    wxNode *node = m_constraintsInvolvedIn->First();
    while (node)
    {
      wxWindow *win = (wxWindow *)node->Data();
      wxNode *next = node->Next();
      wxLayoutConstraints *constr = win->GetConstraints();

      // Reset any constraints involving this window
      if (constr)
      {
        constr->left.ResetIfWin((wxWindow *)this);
        constr->top.ResetIfWin((wxWindow *)this);
        constr->right.ResetIfWin((wxWindow *)this);
        constr->bottom.ResetIfWin((wxWindow *)this);
        constr->width.ResetIfWin((wxWindow *)this);
        constr->height.ResetIfWin((wxWindow *)this);
        constr->centreX.ResetIfWin((wxWindow *)this);
        constr->centreY.ResetIfWin((wxWindow *)this);
      }
      delete node;
      node = next;
    }
    delete m_constraintsInvolvedIn;
    m_constraintsInvolvedIn = NULL;
  }
}

void wxWindow::SetSizer(wxSizer *sizer)
{
  m_windowSizer = sizer;
  if (sizer)
    sizer->SetSizerParent((wxWindow *)this);
}

/*
 * New version
 */

bool wxWindow::Layout()
{
  if (GetConstraints())
  {
    int w, h;
    GetClientSize(&w, &h);
    GetConstraints()->width.SetValue(w);
    GetConstraints()->height.SetValue(h);
  }
  
  // If top level (one sizer), evaluate the sizer's constraints.
  if (GetSizer())
  {
    int noChanges;
    GetSizer()->ResetConstraints();   // Mark all constraints as unevaluated
    GetSizer()->LayoutPhase1(&noChanges);
    GetSizer()->LayoutPhase2(&noChanges);
    GetSizer()->SetConstraintSizes(); // Recursively set the real window sizes
    return TRUE;
  }
  else
  {
    // Otherwise, evaluate child constraints
    ResetConstraints();   // Mark all constraints as unevaluated
    DoPhase(1);           // Just one phase need if no sizers involved
    DoPhase(2);
    SetConstraintSizes(); // Recursively set the real window sizes
  }
  return TRUE;
}


// Do a phase of evaluating constraints:
// the default behaviour. wxSizers may do a similar
// thing, but also impose their own 'constraints'
// and order the evaluation differently.
bool wxWindow::LayoutPhase1(int *noChanges)
{
  wxLayoutConstraints *constr = GetConstraints();
  if (constr)
  {
    return constr->SatisfyConstraints((wxWindow *)this, noChanges);
  }
  else
    return TRUE;
}

bool wxWindow::LayoutPhase2(int *noChanges)
{
  *noChanges = 0;
  
  // Layout children
  DoPhase(1);
  DoPhase(2);
  return TRUE;
}

// Do a phase of evaluating child constraints
bool wxWindow::DoPhase(int phase)
{
  int noIterations = 0;
  int maxIterations = 500;
  int noChanges = 1;
  int noFailures = 0;
  wxList succeeded;
  while ((noChanges > 0) && (noIterations < maxIterations))
  {
    noChanges = 0;
    noFailures = 0;
    wxNode *node = GetChildren()->First();
    while (node)
    {
      wxWindow *child = (wxWindow *)node->Data();
      if (!child->IsKindOf(CLASSINFO(wxFrame)) && !child->IsKindOf(CLASSINFO(wxDialog)))
      {
        wxLayoutConstraints *constr = child->GetConstraints();
        if (constr)
        {
          if (succeeded.Member(child))
          {
          }
          else
          {
            int tempNoChanges = 0;
            bool success = ( (phase == 1) ? child->LayoutPhase1(&tempNoChanges) : child->LayoutPhase2(&tempNoChanges) ) ;
            noChanges += tempNoChanges;
            if (success)
            {
              succeeded.Append(child);
            }
          }
        }
      }
      node = node->Next();
    }
    noIterations ++;
  }
  return TRUE;
}

void wxWindow::ResetConstraints()
{
  wxLayoutConstraints *constr = GetConstraints();
  if (constr)
  {
    constr->left.SetDone(FALSE);
    constr->top.SetDone(FALSE);
    constr->right.SetDone(FALSE);
    constr->bottom.SetDone(FALSE);
    constr->width.SetDone(FALSE);
    constr->height.SetDone(FALSE);
    constr->centreX.SetDone(FALSE);
    constr->centreY.SetDone(FALSE);
  }
  wxNode *node = GetChildren()->First();
  while (node)
  {
    wxWindow *win = (wxWindow *)node->Data();
    if (!win->IsKindOf(CLASSINFO(wxFrame)) && !win->IsKindOf(CLASSINFO(wxDialog)))
      win->ResetConstraints();
    node = node->Next();
  }
}

// Need to distinguish between setting the 'fake' size for
// windows and sizers, and setting the real values.
void wxWindow::SetConstraintSizes(bool recurse)
{
  wxLayoutConstraints *constr = GetConstraints();
  if (constr && constr->left.GetDone() && constr->right.GetDone() &&
                constr->width.GetDone() && constr->height.GetDone())
  {
    int x = constr->left.GetValue();
    int y = constr->top.GetValue();
    int w = constr->width.GetValue();
    int h = constr->height.GetValue();

    // If we don't want to resize this window, just move it...
    if ((constr->width.GetRelationship() != wxAsIs) ||
        (constr->height.GetRelationship() != wxAsIs))
    {
      // Calls Layout() recursively. AAAGH. How can we stop that.
      // Simply take Layout() out of non-top level OnSizes.
      SizerSetSize(x, y, w, h);
    }
    else
    {
      SizerMove(x, y);
    }
  }
  else if (constr)
  {
    char *windowClass = this->GetClassInfo()->GetClassName();

    wxString winName;
	if (GetName() == "")
		winName = "unnamed";
	else
		winName = GetName();
    wxDebugMsg("Constraint(s) not satisfied for window of type %s, name %s:\n", (const char *)windowClass, (const char *)winName);
    if (!constr->left.GetDone())
      wxDebugMsg("  unsatisfied 'left' constraint.\n");
    if (!constr->right.GetDone())
      wxDebugMsg("  unsatisfied 'right' constraint.\n");
    if (!constr->width.GetDone())
      wxDebugMsg("  unsatisfied 'width' constraint.\n");
    if (!constr->height.GetDone())
      wxDebugMsg("  unsatisfied 'height' constraint.\n");
    wxDebugMsg("Please check constraints: try adding AsIs() constraints.\n");
  }

  if (recurse)
  {
    wxNode *node = GetChildren()->First();
    while (node)
    {
      wxWindow *win = (wxWindow *)node->Data();
      if (!win->IsKindOf(CLASSINFO(wxFrame)) && !win->IsKindOf(CLASSINFO(wxDialog)))
        win->SetConstraintSizes();
      node = node->Next();
    }
  }
}

// This assumes that all sizers are 'on' the same
// window, i.e. the parent of this window.
void wxWindow::TransformSizerToActual(int *x, int *y) const
{
  if (!m_sizerParent || m_sizerParent->IsKindOf(CLASSINFO(wxDialog)) ||
  			 m_sizerParent->IsKindOf(CLASSINFO(wxFrame)) )
    return;
    
  int xp, yp;
  m_sizerParent->GetPosition(&xp, &yp);
  m_sizerParent->TransformSizerToActual(&xp, &yp);
  *x += xp;
  *y += yp;
}

void wxWindow::SizerSetSize(int x, int y, int w, int h)
{
	int xx = x;
	int yy = y;
  TransformSizerToActual(&xx, &yy);
  SetSize(xx, yy, w, h);
}

void wxWindow::SizerMove(int x, int y)
{
	int xx = x;
	int yy = y;
  TransformSizerToActual(&xx, &yy);
  Move(xx, yy);
}

// Only set the size/position of the constraint (if any)
void wxWindow::SetSizeConstraint(int x, int y, int w, int h)
{
  wxLayoutConstraints *constr = GetConstraints();
  if (constr)
  {
    if (x != -1)
    {
      constr->left.SetValue(x);
      constr->left.SetDone(TRUE);
    }
    if (y != -1)
    {
      constr->top.SetValue(y);
      constr->top.SetDone(TRUE);
    }
    if (w != -1)
    {
      constr->width.SetValue(w);
      constr->width.SetDone(TRUE);
    }
    if (h != -1)
    {
      constr->height.SetValue(h);
      constr->height.SetDone(TRUE);
    }
  }
}

void wxWindow::MoveConstraint(int x, int y)
{
  wxLayoutConstraints *constr = GetConstraints();
  if (constr)
  {
    if (x != -1)
    {
      constr->left.SetValue(x);
      constr->left.SetDone(TRUE);
    }
    if (y != -1)
    {
      constr->top.SetValue(y);
      constr->top.SetDone(TRUE);
    }
  }
}

void wxWindow::GetSizeConstraint(int *w, int *h) const
{
  wxLayoutConstraints *constr = GetConstraints();
  if (constr)
  {
    *w = constr->width.GetValue();
    *h = constr->height.GetValue();
  }
  else
    GetSize(w, h);
}

void wxWindow::GetClientSizeConstraint(int *w, int *h) const
{
  wxLayoutConstraints *constr = GetConstraints();
  if (constr)
  {
    *w = constr->width.GetValue();
    *h = constr->height.GetValue();
  }
  else
    GetClientSize(w, h);
}

void wxWindow::GetPositionConstraint(int *x, int *y) const
{
  wxLayoutConstraints *constr = GetConstraints();
  if (constr)
  {
    *x = constr->left.GetValue();
    *y = constr->top.GetValue();
  }
  else
    GetPosition(x, y);
}

bool wxWindow::Close(bool force)
{
  wxCloseEvent event(wxEVT_CLOSE_WINDOW, m_windowId);
  event.SetEventObject(this);
  event.SetForce(force);

  return GetEventHandler()->ProcessEvent(event);
}

wxObject* wxWindow::GetChild(int number) const
{
  // Return a pointer to the Nth object in the window
  if (!GetChildren())
    return(NULL) ;
  wxNode *node = GetChildren()->First();
  int n = number;
  while (node && n--)
    node = node->Next() ;
  if (node)
  {
    wxObject *obj = (wxObject *)node->Data();
    return(obj) ;
  }
  else
    return NULL ;
}

void wxWindow::OnDefaultAction(wxControl *initiatingItem)
{
    // Obsolete function
}

void wxWindow::Clear()
{
	wxClientDC dc(this);
    wxBrush brush(GetBackgroundColour(), wxSOLID);
    dc.SetBackground(brush);
    dc.Clear();
}

// Fits the panel around the items
void wxWindow::Fit()
{
	int maxX = 0;
	int maxY = 0;
	wxNode *node = GetChildren()->First();
	while ( node )
	{
		wxWindow *win = (wxWindow *)node->Data();
		int wx, wy, ww, wh;
		win->GetPosition(&wx, &wy);
		win->GetSize(&ww, &wh);
		if ( wx + ww > maxX )
			maxX = wx + ww;
		if ( wy + wh > maxY )
			maxY = wy + wh;

		node = node->Next();
	}
	SetClientSize(maxX + 5, maxY + 5);
}

void wxWindow::SetValidator(const wxValidator& validator)
{
	if ( m_windowValidator )
		delete m_windowValidator;
	m_windowValidator = validator.Clone();

	if ( m_windowValidator )
		m_windowValidator->SetWindow(this) ;
}

// Find a window by id or name
wxWindow *wxWindow::FindWindow(long id)
{
	if ( GetId() == id)
		return this;

	wxNode *node = GetChildren()->First();
	while ( node )
	{
		wxWindow *child = (wxWindow *)node->Data();
		wxWindow *found = child->FindWindow(id);
		if ( found )
			return found;
		node = node->Next();
	}
	return NULL;
}

wxWindow *wxWindow::FindWindow(const wxString& name)
{
	if ( GetName() == name)
		return this;

	wxNode *node = GetChildren()->First();
	while ( node )
	{
		wxWindow *child = (wxWindow *)node->Data();
		wxWindow *found = child->FindWindow(name);
		if ( found )
			return found;
		node = node->Next();
	}
	return NULL;
}

void wxWindow::OnIdle(wxIdleEvent& event)
{
/* TODO: you may need to do something like this
 * if your GUI doesn't generate enter/leave events

    // Check if we need to send a LEAVE event
    if (m_mouseInWindow)
    {
        POINT pt;
        ::GetCursorPos(&pt);
        if (::WindowFromPoint(pt) != (HWND) GetHWND())
        {
            // Generate a LEAVE event
            m_mouseInWindow = FALSE;
            MSWOnMouseLeave(pt.x, pt.y, 0);
        }
    }
*/

    // This calls the UI-update mechanism (querying windows for
    // menu/toolbar/control state information)
	UpdateWindowUI();
}

// Raise the window to the top of the Z order
void wxWindow::Raise()
{
    Window window = XtWindow((Widget) GetMainWidget());
    XRaiseWindow(XtDisplay((Widget) GetMainWidget()), window);
}

// Lower the window to the bottom of the Z order
void wxWindow::Lower()
{
    Window window = XtWindow((Widget) GetMainWidget());
    XLowerWindow(XtDisplay((Widget) GetMainWidget()), window);
}

bool wxWindow::AcceptsFocus() const
{
  return IsShown() && IsEnabled();
}

// Update region access
wxRegion wxWindow::GetUpdateRegion() const
{
    return m_updateRegion;
}

bool wxWindow::IsExposed(int x, int y, int w, int h) const
{
    return (m_updateRegion.Contains(x, y, w, h) != wxOutRegion);
}

bool wxWindow::IsExposed(const wxPoint& pt) const
{
    return (m_updateRegion.Contains(pt) != wxOutRegion);
}

bool wxWindow::IsExposed(const wxRect& rect) const
{
    return (m_updateRegion.Contains(rect) != wxOutRegion);
}

/*
 * Allocates control IDs
 */

int wxWindow::NewControlId()
{
    static int s_controlId = 0;
    s_controlId ++;
    return s_controlId;
}

void wxWindow::SetAcceleratorTable(const wxAcceleratorTable& accel)
{
    m_acceleratorTable = accel;
}

// All widgets should have this as their resize proc.
// OnSize sent to wxWindow via client data.
void wxWidgetResizeProc(Widget w, XConfigureEvent *event, String args[], int *num_args)
{
    wxWindow *win = (wxWindow *)wxWidgetHashTable->Get((long)w);
    if (!win)
        return;

    if (win->PreResize())
    {
        int width, height;
        win->GetSize(&width, &height);
        wxSizeEvent sizeEvent(wxSize(width, height), win->GetId());
        sizeEvent.SetEventObject(win);
        win->GetEventHandler()->ProcessEvent(sizeEvent);
    }
}

bool wxAddWindowToTable(Widget w, wxWindow *win)
{
  wxWindow *oldItem = NULL;
#if DEBUG
//  printf("Adding widget %ld, name = %s\n", w, win->GetClassInfo()->GetClassName());
#endif
  if ((oldItem = (wxWindow *)wxWidgetHashTable->Get ((long) w)))
  {
    char buf[300];
    sprintf(buf, "Widget table clash: new widget is %ld, %s", (long)w, win->GetClassInfo()->GetClassName());
    wxError (buf);
    fflush(stderr);
    sprintf(buf, "Old widget was %s", oldItem->GetClassInfo()->GetClassName());
    wxError (buf);
    return FALSE;
  }

  wxWidgetHashTable->Put ((long) w, win);
  return TRUE;
}

wxWindow *wxGetWindowFromTable(Widget w)
{
  return (wxWindow *)wxWidgetHashTable->Get ((long) w);
}

void wxDeleteWindowFromTable(Widget w)
{
#if DEBUG
//  printf("Deleting widget %ld\n", w);
#endif
//  wxWindow *win = (wxWindow *)wxWidgetHashTable->Get ((long) w);

  wxWidgetHashTable->Delete((long)w);
}
  
// Get the underlying X window and display
WXWindow wxWindow::GetXWindow() const
{
    return (WXWindow) XtWindow((Widget) GetMainWidget());
}

WXDisplay *wxWindow::GetXDisplay() const
{
    return (WXDisplay*) XtDisplay((Widget) GetMainWidget());
}

WXWidget wxWindow::GetMainWidget() const
{
    if (m_drawingArea)
      return m_drawingArea;
    else
      return m_mainWidget;
}

WXWidget wxWindow::GetClientWidget() const
{
    if (m_drawingArea != (WXWidget) 0)
        return m_drawingArea;
    else
        return m_mainWidget;
}

void wxCanvasRepaintProc (Widget drawingArea, XtPointer clientData,
 XmDrawingAreaCallbackStruct * cbs)
{
    if (!wxWidgetHashTable->Get ((long) (Widget) drawingArea))
	return;

    XEvent * event = cbs->event;
    wxWindow * canvas = (wxWindow *) clientData;
    Display * display = (Display *) canvas->GetXDisplay();
    //    GC gc = (GC) canvas->GetDC()->gc;

    switch (event->type)
    {
      case Expose:
      {
	/* TODO
        wxCanvasDC* canvasDC = canvas->GetDC();
        if (canvasDC)
	{
          if (canvasDC->onpaint_reg)
            XDestroyRegion(canvasDC->onpaint_reg);
          canvasDC->onpaint_reg = XCreateRegion();
          
	}
	*/

	int n = canvas->m_updateRects.Number();
        XRectangle* xrects = new XRectangle[n];
        int i;
	for (i = 0; i < canvas->m_updateRects.Number(); i++)
	{
	  wxRect* rect = (wxRect*) canvas->m_updateRects.Nth(i)->Data();
	  xrects[i].x = rect->x;
          xrects[i].y = rect->y;
          xrects[i].width = rect->width;
          xrects[i].height = rect->height;
	  /* TODO (?) Actually ignore it I think.
          if (canvasDC)
            XUnionRectWithRegion(&(xrects[i]), canvasDC->onpaint_reg,
              canvasDC->onpaint_reg);
      */
	}
	/* TODO must clip the area being repainted. So we need a gc.
         * Alternatively, wxPaintDC must do the clipping
         * when it's created.
        XSetClipRectangles(display, gc, 0, 0, xrects, n, Unsorted);
	*/

        canvas->DoPaint() ; // xrects, n);
        delete[] xrects;

        canvas->m_updateRects.Clear();

	/*
        if (canvasDC)
	{
          XDestroyRegion(canvasDC->onpaint_reg);
          canvasDC->onpaint_reg = NULL;
	}

	XGCValues gc_val;
	gc_val.clip_mask = None;
	XChangeGC(display, gc, GCClipMask, &gc_val);
	*/

	break;
    }
    default:
    {
	cout << "\n\nNew Event ! is = " << event -> type << "\n";
	break;
    }
  }
}

// Unable to deal with Enter/Leave without a separate EventHandler (Motif 1.1.4)
void 
wxCanvasEnterLeave (Widget drawingArea, XtPointer clientData, XCrossingEvent * event)
{
  XmDrawingAreaCallbackStruct cbs;
  XEvent ev;

  //if (event->mode!=NotifyNormal)
  //  return ;

//  ev = *((XEvent *) event); // Causes Purify error (copying too many bytes)
  ((XCrossingEvent &) ev) = *event;

  cbs.reason = XmCR_INPUT;
  cbs.event = &ev;

  wxCanvasInputEvent (drawingArea, (XtPointer) NULL, &cbs);
}

// Fix to make it work under Motif 1.0 (!)
void wxCanvasMotionEvent (Widget drawingArea, XButtonEvent * event)
{
#if   XmVersion<=1000

  XmDrawingAreaCallbackStruct cbs;
  XEvent ev;

  //ev.xbutton = *event;
  ev = *((XEvent *) event);
  cbs.reason = XmCR_INPUT;
  cbs.event = &ev;

  wxCanvasInputEvent (drawingArea, (XtPointer) NULL, &cbs);
#endif
}

void wxCanvasInputEvent (Widget drawingArea, XtPointer data, XmDrawingAreaCallbackStruct * cbs)
{
  wxWindow *canvas = (wxWindow *) wxWidgetHashTable->Get ((long) (Widget) drawingArea);
  XEvent local_event;

  if (canvas==NULL)
    return ;

  if (cbs->reason != XmCR_INPUT)
    return;

  local_event = *(cbs->event);	// We must keep a copy!

  switch (local_event.xany.type)
    {
    case EnterNotify:
    case LeaveNotify:
    case ButtonPress:
    case ButtonRelease:
    case MotionNotify:
      {
	wxEventType eventType = wxEVT_NULL;

	if (local_event.xany.type == EnterNotify)
	  {
	    //if (local_event.xcrossing.mode!=NotifyNormal)
	    //  return ; // Ignore grab events
	    eventType = wxEVT_ENTER_WINDOW;
//            canvas->GetEventHandler()->OnSetFocus();
	  }
	else if (local_event.xany.type == LeaveNotify)
	  {
	    //if (local_event.xcrossing.mode!=NotifyNormal)
	    //  return ; // Ignore grab events
	    eventType = wxEVT_LEAVE_WINDOW;
//            canvas->GetEventHandler()->OnKillFocus();
	  }
	else if (local_event.xany.type == MotionNotify)
	  {
	    eventType = wxEVT_MOTION;
	    if (local_event.xmotion.is_hint == NotifyHint)
	      {
		Window root, child;
		Display *dpy = XtDisplay (drawingArea);

		XQueryPointer (dpy, XtWindow (drawingArea),
			       &root, &child,
			       &local_event.xmotion.x_root,
			       &local_event.xmotion.y_root,
			       &local_event.xmotion.x,
			       &local_event.xmotion.y,
			       &local_event.xmotion.state);
	      }
	    else
	      {
	      }
	  }

	else if (local_event.xany.type == ButtonPress)
	  {
	    if (local_event.xbutton.button == Button1)
	      {
		eventType = wxEVT_LEFT_DOWN;
		canvas->m_button1Pressed = TRUE;
	      }
	    else if (local_event.xbutton.button == Button2)
	      {
		eventType = wxEVT_MIDDLE_DOWN;
		canvas->m_button2Pressed = TRUE;
	      }
	    else if (local_event.xbutton.button == Button3)
	      {
		eventType = wxEVT_RIGHT_DOWN;
		canvas->m_button3Pressed = TRUE;
	      }
	  }
	else if (local_event.xany.type == ButtonRelease)
	  {
	    if (local_event.xbutton.button == Button1)
	      {
		eventType = wxEVT_LEFT_UP;
		canvas->m_button1Pressed = FALSE;
	      }
	    else if (local_event.xbutton.button == Button2)
	      {
		eventType = wxEVT_MIDDLE_UP;
		canvas->m_button2Pressed = FALSE;
	      }
	    else if (local_event.xbutton.button == Button3)
	      {
		eventType = wxEVT_RIGHT_UP;
		canvas->m_button3Pressed = FALSE;
	      }
	  }

	wxMouseEvent wxevent (eventType);
	wxevent.m_eventHandle = (char *) &local_event;

	wxevent.m_leftDown = ((eventType == wxEVT_LEFT_DOWN)
			    || (event_left_is_down (&local_event) 
				&& (eventType != wxEVT_LEFT_UP)));
	wxevent.m_middleDown = ((eventType == wxEVT_MIDDLE_DOWN)
			      || (event_middle_is_down (&local_event) 
				  && (eventType != wxEVT_MIDDLE_UP)));
	wxevent.m_rightDown = ((eventType == wxEVT_RIGHT_DOWN)
			     || (event_right_is_down (&local_event) 
				 && (eventType != wxEVT_RIGHT_UP)));

	wxevent.m_shiftDown = local_event.xbutton.state & ShiftMask;
	wxevent.m_controlDown = local_event.xbutton.state & ControlMask;
        wxevent.m_altDown = local_event.xbutton.state & Mod3Mask;
        wxevent.m_metaDown = local_event.xbutton.state & Mod1Mask;
        wxevent.SetTimestamp(local_event.xbutton.time);

    // Now check if we need to translate this event into a double click
    if (TRUE) // canvas->doubleClickAllowed)
    {
       if (wxevent.ButtonDown())
       {
             long dclickTime = XtGetMultiClickTime((Display*) wxGetDisplay()) ;

	     // get button and time-stamp
	     int button = 0;
	     if      (wxevent.LeftDown())   button = 1;
	     else if (wxevent.MiddleDown()) button = 2;
	     else if (wxevent.RightDown())  button = 3;
	     long ts = wxevent.GetTimestamp();
	     // check, if single or double click
	     if (canvas->m_lastButton && canvas->m_lastButton==button && (ts - canvas->m_lastTS) < dclickTime)
	       {
	        // I have a dclick
		 canvas->m_lastButton = 0;
		 switch ( eventType )
		   {
		   case wxEVT_LEFT_DOWN:
		     wxevent.SetEventType(wxEVT_LEFT_DCLICK);
		     break;
		   case wxEVT_MIDDLE_DOWN:
		     wxevent.SetEventType(wxEVT_MIDDLE_DCLICK);
		     break;
		   case wxEVT_RIGHT_DOWN:
		     wxevent.SetEventType(wxEVT_RIGHT_DCLICK);
		     break;

		   default :
		     break;
		   }

	       }
	     else
	       {
		 // not fast enough or different button
		 canvas->m_lastTS     = ts;
		 canvas->m_lastButton = button;
	       }
       }
    }

        wxevent.SetId(canvas->GetId());
        wxevent.SetEventObject(canvas);
	canvas->GetEventHandler()->ProcessEvent (wxevent);
	/*
	if (eventType == wxEVT_ENTER_WINDOW ||
	    eventType == wxEVT_LEAVE_WINDOW ||
	    eventType == wxEVT_MOTION
	  )
	  return;
	  */
	break;
      }
    case KeyPress:
      {
	KeySym keySym;
//	XComposeStatus compose;
//	(void) XLookupString ((XKeyEvent *) & local_event, wxBuffer, 20, &keySym, &compose);
	(void) XLookupString ((XKeyEvent *) & local_event, wxBuffer, 20, &keySym, NULL);
	int id = wxCharCodeXToWX (keySym);

	wxKeyEvent event (wxEVT_CHAR);

	if (local_event.xkey.state & ShiftMask)
	  event.m_shiftDown = TRUE;
	if (local_event.xkey.state & ControlMask)
	  event.m_controlDown = TRUE;
	if (local_event.xkey.state & Mod3Mask)
	  event.m_altDown = TRUE;
	if (local_event.xkey.state & Mod1Mask)
	  event.m_metaDown = TRUE;
	event.SetEventObject(canvas);
	event.m_keyCode = id;
        event.SetTimestamp(local_event.xkey.time);

	if (id > -1)
        {
          // Implement wxFrame::OnCharHook by checking ancestor.
          wxWindow *parent = canvas->GetParent();
          while (parent && !parent->IsKindOf(CLASSINFO(wxFrame)))
            parent = parent->GetParent();
            
          if (parent)
          {
            event.SetEventType(wxEVT_CHAR_HOOK);
            if (parent->GetEventHandler()->ProcessEvent(event))
              return;
            event.SetEventType(wxEVT_CHAR);
          }

	  canvas->GetEventHandler()->ProcessEvent (event);
        }
	break;
      }
    case FocusIn:
      {
        if (local_event.xfocus.detail != NotifyPointer)
	{
          wxFocusEvent event(wxEVT_SET_FOCUS, canvas->GetId());
          event.SetEventObject(canvas);
          canvas->GetEventHandler()->ProcessEvent(event);
        }
	break;
      }
    case FocusOut:
      {
        if (local_event.xfocus.detail != NotifyPointer)
	{
          wxFocusEvent event(wxEVT_KILL_FOCUS, canvas->GetId());
          event.SetEventObject(canvas);
          canvas->GetEventHandler()->ProcessEvent(event);
	}
        break;
      }
    default:
      break;
    }
}

void wxWindow::DoPaint()
{
  //TODO : make a temporary gc so we can do the XCopyArea below
  if (0) // m_backingPixmap)
    {
      /*
      Widget drawingArea = (Widget) m_drawingArea;
      //      int orig = GetDC()->GetLogicalFunction();
      //      GetDC()->SetLogicalFunction (wxCOPY);

      // TODO: it may not be necessary to store m_pixmapOffsetX/Y; we
      // should be able to calculate them.
      XCopyArea (XtDisplay (drawingArea), m_backingPixmap, XtWindow (drawingArea), GetDC ()->gc,
		 m_pixmapOffsetX, m_pixmapOffsetY,
		 m_pixmapWidth, m_pixmapHeight,
		 0, 0);

      //      GetDC()->SetLogicalFunction (orig);
      */
    }
  else
    {
        wxPaintEvent event(GetId());
        event.SetEventObject(this);
        GetEventHandler()->ProcessEvent(event);
    }
}

// SetSize, but as per old wxCanvas (with drawing widget etc.)
void wxWindow::CanvasSetSize (int x, int y, int w, int h, int sizeFlags)
{
  Widget drawingArea = (Widget) m_drawingArea;
  bool managed = XtIsManaged(m_borderWidget ? (Widget) m_borderWidget : (Widget) m_scrolledWindow);

  if (managed)
    XtUnmanageChild (m_borderWidget ? (Widget) m_borderWidget : (Widget) m_scrolledWindow);
  XtVaSetValues((Widget) m_drawingArea, XmNresizePolicy, XmRESIZE_ANY, NULL);

  if (x > -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    {
      XtVaSetValues (m_borderWidget ? (Widget) m_borderWidget : (Widget) m_scrolledWindow,
		     XmNx, x, NULL);
    }

  if (y > -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    {
      XtVaSetValues (m_borderWidget ? (Widget) m_borderWidget : (Widget) m_scrolledWindow,
		     XmNy, y, NULL);
    }

  if (w > -1)
    {
      if (m_borderWidget)
	{
	  XtVaSetValues ((Widget) m_borderWidget, XmNwidth, w, NULL);
	  short thick, margin;
	  XtVaGetValues ((Widget) m_borderWidget,
			 XmNshadowThickness, &thick,
			 XmNmarginWidth, &margin,
			 NULL);
	  w -= 2 * (thick + margin);
	}

      XtVaSetValues ((Widget) m_scrolledWindow, XmNwidth, w, NULL);

      Dimension spacing;
      Widget sbar;
      XtVaGetValues ((Widget) m_scrolledWindow,
		     XmNspacing, &spacing,
		     XmNverticalScrollBar, &sbar,
		     NULL);
      Dimension wsbar;
      if (sbar)
	XtVaGetValues (sbar, XmNwidth, &wsbar, NULL);
      else
	wsbar = 0;

      w -= (spacing + wsbar);

      XtVaSetValues ((Widget) m_drawingArea, XmNwidth, w, NULL);
    }
  if (h > -1)
    {
      if (m_borderWidget)
	{
	  XtVaSetValues ((Widget) m_borderWidget, XmNheight, h, NULL);
	  short thick, margin;
	  XtVaGetValues ((Widget) m_borderWidget,
			 XmNshadowThickness, &thick,
			 XmNmarginHeight, &margin,
			 NULL);
	  h -= 2 * (thick + margin);
	}

      XtVaSetValues ((Widget) m_scrolledWindow, XmNheight, h, NULL);

      Dimension spacing;
      Widget sbar;
      XtVaGetValues ((Widget) m_scrolledWindow,
		     XmNspacing, &spacing,
		     XmNhorizontalScrollBar, &sbar,
		     NULL);
      Dimension wsbar;
      if (sbar)
	XtVaGetValues (sbar, XmNheight, &wsbar, NULL);
      else
	wsbar = 0;

      h -= (spacing + wsbar);

      XtVaSetValues ((Widget) m_drawingArea, XmNheight, h, NULL);
    }
  if (managed)
    XtManageChild (m_borderWidget ? (Widget) m_borderWidget : (Widget) m_scrolledWindow);
  XtVaSetValues((Widget) m_drawingArea, XmNresizePolicy, XmRESIZE_NONE, NULL);

  int ww, hh;
  GetClientSize (&ww, &hh);
  wxSizeEvent sizeEvent(wxSize(ww, hh), GetId());
  sizeEvent.SetEventObject(this);
  
  GetEventHandler()->ProcessEvent(sizeEvent);
}

void wxWindow::CanvasSetClientSize (int w, int h)
{
  Widget drawingArea = (Widget) m_drawingArea;

  XtVaSetValues((Widget) m_drawingArea, XmNresizePolicy, XmRESIZE_ANY, NULL);

  if (w > -1)
    XtVaSetValues ((Widget) m_drawingArea, XmNwidth, w, NULL);
  if (h > -1)
    XtVaSetValues ((Widget) m_drawingArea, XmNheight, h, NULL);
  /* TODO: is this necessary?
  allowRepainting = FALSE;

  XSync (XtDisplay (drawingArea), FALSE);
  XEvent event;
  while (XtAppPending (wxTheApp->appContext))
    {
      XFlush (XtDisplay (drawingArea));
      XtAppNextEvent (wxTheApp->appContext, &event);
      XtDispatchEvent (&event);
    }
    */

  XtVaSetValues((Widget) m_drawingArea, XmNresizePolicy, XmRESIZE_NONE, NULL);

  /* TODO
  allowRepainting = TRUE;
  DoRefresh ();
  */

  wxSizeEvent sizeEvent(wxSize(w, h), GetId());
  sizeEvent.SetEventObject(this);
  
  GetEventHandler()->ProcessEvent(sizeEvent);
}

void wxWindow::CanvasGetClientSize (int *w, int *h) const
{
  // Must return the same thing that was set via SetClientSize
  Dimension xx, yy;
  XtVaGetValues ((Widget) m_drawingArea, XmNwidth, &xx, XmNheight, &yy, NULL);
  *w = xx;
  *h = yy;
}

void wxWindow::CanvasGetSize (int *w, int *h) const
{
  Dimension xx, yy;
  if ((Widget) m_borderWidget)
    XtVaGetValues ((Widget) m_borderWidget, XmNwidth, &xx, XmNheight, &yy, NULL);
  else if ((Widget) m_scrolledWindow)
    XtVaGetValues ((Widget) m_scrolledWindow, XmNwidth, &xx, XmNheight, &yy, NULL);
  else
    XtVaGetValues ((Widget) m_drawingArea, XmNwidth, &xx, XmNheight, &yy, NULL);

  *w = xx;
  *h = yy;
}

void wxWindow::CanvasGetPosition (int *x, int *y) const
{
  Position xx, yy;
  XtVaGetValues (m_borderWidget ? (Widget) m_borderWidget : (Widget) m_scrolledWindow, XmNx, &xx, XmNy, &yy, NULL);
  *x = xx;
  *y = yy;
}
