/////////////////////////////////////////////////////////////////////////////
// Name:        frame.cpp
// Purpose:     wxFrame
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
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
#include "wx/utils.h"

#if defined(__ultrix) || defined(__sgi)
#include <Xm/Frame.h>
#endif

#include <Xm/Xm.h>
#include <X11/Shell.h>
#if XmVersion >= 1002
#include <Xm/XmAll.h>
#else
#include <Xm/Frame.h>
#endif
#include <Xm/MwmUtil.h>
#include <Xm/BulletinB.h>
#include <Xm/Form.h>
#include <Xm/MainW.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/AtomMgr.h>
#include <Xm/LabelG.h>
#include <Xm/Frame.h>
#if   XmVersion > 1000
#include <Xm/Protocols.h>
#endif

#include "wx/motif/private.h"

void wxCloseFrameCallback(Widget, XtPointer, XmAnyCallbackStruct *cbs);
void wxFrameFocusProc(Widget workArea, XtPointer clientData,
                      XmAnyCallbackStruct *cbs);
static void wxFrameMapProc(Widget frameShell, XtPointer clientData,
                           XCrossingEvent * event);

extern wxList wxModelessWindows;
extern wxList wxPendingDelete;

// TODO: this should be tidied so that any frame can be the
// top frame
static bool wxTopLevelUsed = FALSE;

#if !USE_SHARED_LIBRARY
BEGIN_EVENT_TABLE(wxFrame, wxWindow)
EVT_SIZE(wxFrame::OnSize)
EVT_ACTIVATE(wxFrame::OnActivate)
EVT_MENU_HIGHLIGHT_ALL(wxFrame::OnMenuHighlight)
EVT_SYS_COLOUR_CHANGED(wxFrame::OnSysColourChanged)
EVT_IDLE(wxFrame::OnIdle)
EVT_CLOSE(wxFrame::OnCloseWindow)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxFrame, wxWindow)
#endif

#if wxUSE_NATIVE_STATUSBAR
bool wxFrame::m_useNativeStatusBar = TRUE;
#else
bool wxFrame::m_useNativeStatusBar = FALSE;
#endif

wxFrame::wxFrame()
{
#if wxUSE_TOOLBAR
    m_frameToolBar = NULL ;
#endif // wxUSE_TOOLBAR

    m_frameMenuBar = NULL;
    m_frameStatusBar = NULL;

    m_parent = NULL;
    m_iconized = FALSE;

    //// Motif-specific
    m_frameShell = (WXWidget) NULL;
    m_frameWidget = (WXWidget) NULL;;
    m_workArea = (WXWidget) NULL;;
    m_clientArea = (WXWidget) NULL;;
    m_visibleStatus = TRUE;
    m_title = "";
}

bool wxFrame::Create(wxWindow *parent,
                     wxWindowID id,
                     const wxString& title,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style,
                     const wxString& name)
{
    if (!parent)
        wxTopLevelWindows.Append(this);

    SetName(name);

    m_windowStyle = style;
    m_frameMenuBar = NULL;
#if wxUSE_TOOLBAR
    m_frameToolBar = NULL ;
#endif // wxUSE_TOOLBAR
    m_frameStatusBar = NULL;

    //// Motif-specific
    m_frameShell = (WXWidget) NULL;
    m_frameWidget = (WXWidget) NULL;;
    m_workArea = (WXWidget) NULL;;
    m_clientArea = (WXWidget) NULL;;
    m_visibleStatus = TRUE;
    m_title = "";

    m_backgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_APPWORKSPACE);
    m_foregroundColour = *wxBLACK;
    m_font = wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT);

    if ( id > -1 )
        m_windowId = id;
    else
        m_windowId = (int)NewControlId();

    if (parent) parent->AddChild(this);

    wxModelessWindows.Append(this);

    int x = pos.x; int y = pos.y;
    int width = size.x; int height = size.y;

    if (wxTopLevelUsed)
    {
        // Change suggested by Matthew Flatt
        m_frameShell = (WXWidget)XtAppCreateShell
                                 (
                                  name,
                                  wxTheApp->GetClassName(),
                                  topLevelShellWidgetClass,
                                  (Display*) wxGetDisplay(),
                                  NULL,
                                  0
                                 );
    }
    else
    {
        m_frameShell = wxTheApp->GetTopLevelWidget();
        wxTopLevelUsed = TRUE;
    }

    XtVaSetValues((Widget) m_frameShell,
        // Allows menu to resize
        XmNallowShellResize, True,
        XmNdeleteResponse, XmDO_NOTHING,
        XmNmappedWhenManaged, False,
        XmNiconic, (style & wxICONIZE) ? TRUE : FALSE,
        NULL);

    if (!title.IsNull())
        XtVaSetValues((Widget) m_frameShell,
        XmNtitle, (const char*) title,
        NULL);

    m_frameWidget = (WXWidget) XtVaCreateManagedWidget("main_window",
        xmMainWindowWidgetClass, (Widget) m_frameShell,
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

    wxLogDebug("Created frame (0x%08x) with work area 0x%08x and client "
               "area 0x%08x", m_frameWidget, m_workArea, m_clientArea);

    XtAddEventHandler((Widget) m_clientArea, ExposureMask,FALSE,
        wxUniversalRepaintProc, (XtPointer) this);

    XtVaSetValues((Widget) m_frameWidget,
        XmNworkWindow, (Widget) m_workArea,
        NULL);

    XtManageChild((Widget) m_clientArea);
    XtManageChild((Widget) m_workArea);

    wxAddWindowToTable((Widget) m_workArea, this);

    XtTranslations ptr ;

    XtOverrideTranslations((Widget) m_workArea,
        ptr = XtParseTranslationTable("<Configure>: resize()"));

    XtFree((char *)ptr);

    XtAddCallback((Widget) m_workArea, XmNfocusCallback,
        (XtCallbackProc)wxFrameFocusProc, (XtPointer)this);

    /* Part of show-&-hide fix */
    XtAddEventHandler((Widget) m_frameShell, StructureNotifyMask,
        False, (XtEventHandler)wxFrameMapProc,
        (XtPointer)m_workArea);

    if (x > -1)
        XtVaSetValues((Widget) m_frameShell, XmNx, x, NULL);
    if (y > -1)
        XtVaSetValues((Widget) m_frameShell, XmNy, y, NULL);
    if (width > -1)
        XtVaSetValues((Widget) m_frameShell, XmNwidth, width, NULL);
    if (height > -1)
        XtVaSetValues((Widget) m_frameShell, XmNheight, height, NULL);

    m_mainWidget = m_frameWidget;

    ChangeFont(FALSE);

    // This patch comes from Torsten Liermann lier@lier1.muc.de
    if (XmIsMotifWMRunning( (Widget) m_frameShell ))
    {
        int decor = 0 ;
        if (style & wxRESIZE_BORDER)
            decor |= MWM_DECOR_RESIZEH ;
        if (style & wxSYSTEM_MENU)
            decor |= MWM_DECOR_MENU;
        if ((style & wxCAPTION) ||
            (style & wxTINY_CAPTION_HORIZ) ||
            (style & wxTINY_CAPTION_VERT))
            decor |= MWM_DECOR_TITLE;
        if (style & wxTHICK_FRAME)
            decor |= MWM_DECOR_BORDER;
        if (style & wxTHICK_FRAME)
            decor |= MWM_DECOR_BORDER;
        if (style & wxMINIMIZE_BOX)
            decor |= MWM_DECOR_MINIMIZE;
        if (style & wxMAXIMIZE_BOX)
            decor |= MWM_DECOR_MAXIMIZE;
        XtVaSetValues((Widget) m_frameShell,XmNmwmDecorations,decor,NULL) ;
    }
    // This allows non-Motif window managers to support at least the
    // no-decorations case.
    else
    {
        if (style == 0)
            XtVaSetValues((Widget) m_frameShell,XmNoverrideRedirect,TRUE,NULL);
    }
    XtRealizeWidget((Widget) m_frameShell);

    // Intercept CLOSE messages from the window manager
    Atom WM_DELETE_WINDOW = XmInternAtom(XtDisplay((Widget) m_frameShell), "WM_DELETE_WINDOW", False);
#if (XmREVISION > 1 || XmVERSION > 1)
    XmAddWMProtocolCallback((Widget) m_frameShell, WM_DELETE_WINDOW, (XtCallbackProc) wxCloseFrameCallback, (XtPointer)this);
#else
#if XmREVISION == 1
    XmAddWMProtocolCallback((Widget) m_frameShell, WM_DELETE_WINDOW, (XtCallbackProc) wxCloseFrameCallback, (caddr_t)this);
#else
    XmAddWMProtocolCallback((Widget) m_frameShell, WM_DELETE_WINDOW, (void (*)())wxCloseFrameCallback, (caddr_t)this);
#endif
#endif

    ChangeBackgroundColour();

    PreResize();

    wxSizeEvent sizeEvent(wxSize(width, height), GetId());
    sizeEvent.SetEventObject(this);

    GetEventHandler()->ProcessEvent(sizeEvent);

    return TRUE;
}

wxFrame::~wxFrame()
{
    m_isBeingDeleted = TRUE;
    
    if (m_clientArea)
      XtRemoveEventHandler((Widget) m_clientArea, ExposureMask, FALSE,
          wxUniversalRepaintProc, (XtPointer) this);

    if (GetMainWidget())
        Show(FALSE);

    if (m_frameMenuBar)
    {
        m_frameMenuBar->DestroyMenuBar();

        // Hack to stop core dump on Ultrix, OSF, for some strange reason.
#if MOTIF_MENUBAR_DELETE_FIX
        GetMenuBar()->SetMainWidget((WXWidget) NULL);
#endif
        delete m_frameMenuBar;
        m_frameMenuBar = NULL;
    }

    wxTopLevelWindows.DeleteObject(this);
    wxModelessWindows.DeleteObject(this);

    if (m_frameStatusBar)
        delete m_frameStatusBar;

    DestroyChildren();

    /*
    int i;
    for (i = 0; i < wxMAX_STATUS; i++)
    if (statusTextWidget[i])
    XtDestroyWidget (statusTextWidget[i]);

      if (statusLineForm)
      XtDestroyWidget (statusLineForm);

        if (statusLineWidget)
        XtDestroyWidget (statusLineWidget);
    */

    if (m_workArea)
    {
        wxDeleteWindowFromTable((Widget) m_workArea);

        XtDestroyWidget ((Widget) m_workArea);
    }

    if (m_frameWidget)
    {
        wxDeleteWindowFromTable((Widget) m_frameWidget);
        XtDestroyWidget ((Widget) m_frameWidget);
    }

    if (m_frameShell)
        XtDestroyWidget ((Widget) m_frameShell);

    SetMainWidget((WXWidget) NULL);

    /* Check if it's the last top-level window */

    if (wxTheApp && (wxTopLevelWindows.Number() == 0))
    {
        wxTheApp->SetTopWindow(NULL);

        if (wxTheApp->GetExitOnFrameDelete())
        {
            // Signal to the app that we're going to close
            wxTheApp->ExitMainLoop();
        }
    }

}

// Get size *available for subwindows* i.e. excluding menu bar, toolbar etc.
void wxFrame::GetClientSize(int *x, int *y) const
{
    Dimension xx, yy;
    XtVaGetValues((Widget) m_workArea, XmNwidth, &xx, XmNheight, &yy, NULL);

    if (m_frameStatusBar)
    {
        int sbw, sbh;
        m_frameStatusBar->GetSize(& sbw, & sbh);
        yy -= sbh;
    }
#if wxUSE_TOOLBAR
    if (m_frameToolBar)
    {
        int tbw, tbh;
        m_frameToolBar->GetSize(& tbw, & tbh);
        if (m_frameToolBar->GetWindowStyleFlag() & wxTB_VERTICAL)
            xx -= tbw;
        else
            yy -= tbh;
    }
#endif // wxUSE_TOOLBAR
    /*
    if (GetMenuBar() != (wxMenuBar*) NULL)
    {
    // it seems that if a frame holds a panel, the menu bar size
    // gets automatically taken care of --- grano@cs.helsinki.fi 4.4.95
    bool hasSubPanel = FALSE;
    for(wxNode* node = GetChildren().First(); node; node = node->Next())
    {
    wxWindow *win = (wxWindow *)node->Data();
    hasSubPanel = (win->IsKindOf(CLASSINFO(wxPanel)) && !win->IsKindOf(CLASSINFO(wxDialog)));

      if (hasSubPanel)
      break;
      }
      if (! hasSubPanel) {
      Dimension ys;
      XtVaGetValues((Widget) GetMenuBarWidget(), XmNheight, &ys, NULL);
      yy -= ys;
      }
      }
    */

    *x = xx; *y = yy;
}

// Set the client size (i.e. leave the calculation of borders etc.
// to wxWindows)
void wxFrame::DoSetClientSize(int width, int height)
{
    // Calculate how large the new main window should be
    // by finding the difference between the client area and the
    // main window area, and adding on to the new client area
    if (width > -1)
        XtVaSetValues((Widget) m_workArea, XmNwidth, width, NULL);

    if (height > -1)
    {
        if (m_frameStatusBar)
        {
            int sbw, sbh;
            m_frameStatusBar->GetSize(& sbw, & sbh);
            height += sbh;
        }
#if wxUSE_TOOLBAR
        if (m_frameToolBar)
        {
            int tbw, tbh;
            m_frameToolBar->GetSize(& tbw, & tbh);
            if (m_frameToolBar->GetWindowStyleFlag() & wxTB_VERTICAL)
                width += tbw;
            else
                height += tbh;
        }
#endif // wxUSE_TOOLBAR

        XtVaSetValues((Widget) m_workArea, XmNheight, height, NULL);
    }
    PreResize();

    wxSizeEvent sizeEvent(wxSize(width, height), GetId());
    sizeEvent.SetEventObject(this);

    GetEventHandler()->ProcessEvent(sizeEvent);

}

void wxFrame::GetSize(int *width, int *height) const
{
    Dimension xx, yy;
    XtVaGetValues((Widget) m_frameShell, XmNwidth, &xx, XmNheight, &yy, NULL);
    *width = xx; *height = yy;
}

void wxFrame::GetPosition(int *x, int *y) const
{
    Window parent_window = XtWindow((Widget) m_frameShell),
        next_parent   = XtWindow((Widget) m_frameShell),
        root          = RootWindowOfScreen(XtScreen((Widget) m_frameShell));

    // search for the parent that is child of ROOT, because the WM may
    // reparent twice and notify only the next parent (like FVWM)
    while (next_parent != root) {
        Window *theChildren; unsigned int n;
        parent_window = next_parent;
        XQueryTree(XtDisplay((Widget) m_frameShell), parent_window, &root,
            &next_parent, &theChildren, &n);
        XFree(theChildren); // not needed
    }
    int xx, yy; unsigned int dummy;
    XGetGeometry(XtDisplay((Widget) m_frameShell), parent_window, &root,
        &xx, &yy, &dummy, &dummy, &dummy, &dummy);
    if (x) *x = xx;
    if (y) *y = yy;
}

void wxFrame::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    if (x > -1)
        XtVaSetValues((Widget) m_frameShell, XmNx, x, NULL);
    if (y > -1)
        XtVaSetValues((Widget) m_frameShell, XmNy, y, NULL);
    if (width > -1)
        XtVaSetValues((Widget) m_frameWidget, XmNwidth, width, NULL);
    if (height > -1)
        XtVaSetValues((Widget) m_frameWidget, XmNheight, height, NULL);

    if (!(height == -1 && width == -1))
    {
        PreResize();

        wxSizeEvent sizeEvent(wxSize(width, height), GetId());
        sizeEvent.SetEventObject(this);

        GetEventHandler()->ProcessEvent(sizeEvent);
    }
}

bool wxFrame::Show(bool show)
{
    if (!m_frameShell)
        return wxWindow::Show(show);

    m_visibleStatus = show; /* show-&-hide fix */

    m_isShown = show;
    if (show) {
        XtMapWidget((Widget) m_frameShell);
        XRaiseWindow(XtDisplay((Widget) m_frameShell), XtWindow((Widget) m_frameShell));
    } else {
        XtUnmapWidget((Widget) m_frameShell);
        //    XmUpdateDisplay(wxTheApp->topLevel); // Experimental: may be responsible for crashes
    }
    return TRUE;
}

void wxFrame::Iconize(bool iconize)
{
    if (!iconize)
        Show(TRUE);

    if (m_frameShell)
        XtVaSetValues((Widget) m_frameShell, XmNiconic, (Boolean)iconize, NULL);
}

// Equivalent to maximize/restore in Windows
void wxFrame::Maximize(bool maximize)
{
    Show(TRUE);

    if (maximize && m_frameShell)
        XtVaSetValues((Widget) m_frameShell, XmNiconic, FALSE, NULL);
}

bool wxFrame::IsIconized() const
{
    if (!m_frameShell)
        return FALSE;

    Boolean iconic;
    XtVaGetValues((Widget) m_frameShell, XmNiconic, &iconic, NULL);
    return iconic;
}

// Is it maximized?
bool wxFrame::IsMaximized(void) const
{
    // No maximizing in Motif (?)
    return FALSE;
}

void wxFrame::SetTitle(const wxString& title)
{
    if (title == m_title)
        return;

    m_title = title;

    if (!title.IsNull())
        XtVaSetValues((Widget) m_frameShell,
        XmNtitle, (const char*) title,
        XmNiconName, (const char*) title,
        NULL);
}

void wxFrame::SetIcon(const wxIcon& icon)
{
    m_icon = icon;

    if (!m_frameShell)
        return;

    if (!icon.Ok() || !icon.GetPixmap())
        return;

    XtVaSetValues((Widget) m_frameShell, XtNiconPixmap, icon.GetPixmap(), NULL);
}

wxStatusBar *wxFrame::OnCreateStatusBar(int number, long style, wxWindowID id,
                                        const wxString& name)
{
    wxStatusBar *statusBar = NULL;

    statusBar = new wxStatusBar(this, id, wxPoint(0, 0), wxSize(100, 20),
        style, name);

    // Set the height according to the font and the border size
    wxClientDC dc(statusBar);
    dc.SetFont(statusBar->GetFont());

    long x, y;
    dc.GetTextExtent("X", &x, &y);

    int height = (int)( (y  * 1.1) + 2* statusBar->GetBorderY());

    statusBar->SetSize(-1, -1, 100, height);

    statusBar->SetFieldsCount(number);
    return statusBar;
}

wxStatusBar* wxFrame::CreateStatusBar(int number, long style, wxWindowID id,
                                      const wxString& name)
{
    // Calling CreateStatusBar twice is an error.
    wxCHECK_MSG( m_frameStatusBar == NULL, FALSE,
        "recreating status bar in wxFrame" );

    m_frameStatusBar = OnCreateStatusBar(number, style, id,
        name);
    if ( m_frameStatusBar )
    {
        PositionStatusBar();
        return m_frameStatusBar;
    }
    else
        return NULL;
}

void wxFrame::SetStatusText(const wxString& text, int number)
{
    wxCHECK_RET( m_frameStatusBar != NULL, "no statusbar to set text for" );

    m_frameStatusBar->SetStatusText(text, number);
}

void wxFrame::SetStatusWidths(int n, const int widths_field[])
{
    wxCHECK_RET( m_frameStatusBar != NULL, "no statusbar to set widths for" );

    m_frameStatusBar->SetStatusWidths(n, widths_field);
    PositionStatusBar();
}

void wxFrame::PositionStatusBar()
{
    if (!m_frameStatusBar)
        return;

    int w, h;
    GetClientSize(&w, &h);
    int sw, sh;
    m_frameStatusBar->GetSize(&sw, &sh);

    // Since we wish the status bar to be directly under the client area,
    // we use the adjusted sizes without using wxSIZE_NO_ADJUSTMENTS.
    m_frameStatusBar->SetSize(0, h, w, sh);
}

WXWidget wxFrame::GetMenuBarWidget() const
{
    if (GetMenuBar())
        return GetMenuBar()->GetMainWidget();
    else
        return (WXWidget) NULL;
}

void wxFrame::SetMenuBar(wxMenuBar *menuBar)
{
    if (!menuBar)
    {
        m_frameMenuBar = NULL;
        return;
    }

    // Currently can't set it twice
    //    wxASSERT_MSG( (m_frameMenuBar == (wxMenuBar*) NULL), "Cannot set the menubar more than once");

    if (m_frameMenuBar)
    {
        m_frameMenuBar->DestroyMenuBar();
        delete m_frameMenuBar;
    }

    m_frameMenuBar = menuBar;
    m_frameMenuBar->CreateMenuBar(this);
}

void wxFrame::Fit()
{
    // Work out max. size
    wxNode *node = GetChildren().First();
    int max_width = 0;
    int max_height = 0;
    while (node)
    {
        // Find a child that's a subwindow, but not a dialog box.
        wxWindow *win = (wxWindow *)node->Data();

        if (!win->IsKindOf(CLASSINFO(wxFrame)) &&
            !win->IsKindOf(CLASSINFO(wxDialog)))
        {
            int width, height;
            int x, y;
            win->GetSize(&width, &height);
            win->GetPosition(&x, &y);

            if ((x + width) > max_width)
                max_width = x + width;
            if ((y + height) > max_height)
                max_height = y + height;
        }
        node = node->Next();
    }
    SetClientSize(max_width, max_height);
}

// Responds to colour changes, and passes event on to children.
void wxFrame::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_APPWORKSPACE));
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

// Default resizing behaviour - if only ONE subwindow,
// resize to client rectangle size
void wxFrame::OnSize(wxSizeEvent& event)
{
    // if we're using constraints - do use them
#if wxUSE_CONSTRAINTS
    if ( GetAutoLayout() ) {
        Layout();
        return;
    }
#endif

    // do we have _exactly_ one child?
    wxWindow *child = NULL;
    for ( wxNode *node = GetChildren().First(); node; node = node->Next() )
    {
        wxWindow *win = (wxWindow *)node->Data();
        if ( !win->IsKindOf(CLASSINFO(wxFrame))  &&
            !win->IsKindOf(CLASSINFO(wxDialog)) &&
            (win != GetStatusBar())
#if wxUSE_TOOLBAR
             && (win != GetToolBar())
#endif // wxUSE_TOOLBAR
           )
        {
            if ( child )
                return;     // it's our second subwindow - nothing to do
            child = win;
        }
    }

    if ( child ) {
        // we have exactly one child - set it's size to fill the whole frame
        int clientW, clientH;
        GetClientSize(&clientW, &clientH);

        int x = 0;
        int y = 0;

        child->SetSize(x, y, clientW, clientH);
    }
}

// Default activation behaviour - set the focus for the first child
// subwindow found.
void wxFrame::OnActivate(wxActivateEvent& event)
{
    for(wxNode *node = GetChildren().First(); node; node = node->Next())
    {
        // Find a child that's a subwindow, but not a dialog box.
        wxWindow *child = (wxWindow *)node->Data();
        if (!child->IsKindOf(CLASSINFO(wxFrame)) &&
            !child->IsKindOf(CLASSINFO(wxDialog)))
        {
            child->SetFocus();
            return;
        }
    }
}

// The default implementation for the close window event.
// OnClose for backward compatibility.

void wxFrame::OnCloseWindow(wxCloseEvent& event)
{
    this->Destroy();
}

// Destroy the window (delayed, if a managed window)
bool wxFrame::Destroy()
{
    if (!wxPendingDelete.Member(this))
        wxPendingDelete.Append(this);
    return TRUE;
}

// Default menu selection behaviour - display a help string
void wxFrame::OnMenuHighlight(wxMenuEvent& event)
{
    if (GetStatusBar())
    {
        if (event.GetMenuId() == -1)
            SetStatusText("");
        else
        {
            wxMenuBar *menuBar = GetMenuBar();
            if (menuBar)
            {
                wxString helpString(menuBar->GetHelpString(event.GetMenuId()));
                if (helpString != "")
                    SetStatusText(helpString);
            }
        }
    }
}

wxMenuBar *wxFrame::GetMenuBar() const
{
    return m_frameMenuBar;
}

void wxFrame::Centre(int direction)
{
    int display_width, display_height, width, height, x, y;
    wxDisplaySize(&display_width, &display_height);

    GetSize(&width, &height);
    GetPosition(&x, &y);

    if (direction & wxHORIZONTAL)
        x = (int)((display_width - width)/2);
    if (direction & wxVERTICAL)
        y = (int)((display_height - height)/2);

    SetSize(x, y, width, height);
}

// Call this to simulate a menu command
void wxFrame::Command(int id)
{
    ProcessCommand(id);
}

void wxFrame::ProcessCommand(int id)
{
  wxCommandEvent commandEvent(wxEVT_COMMAND_MENU_SELECTED, id);
  commandEvent.SetInt( id );
  commandEvent.SetEventObject( this );

  wxMenuBar *bar = GetMenuBar() ;
  if (!bar)
    return;

/* TODO: check the menu item if required
  wxMenuItem *item = bar->FindItemForId(id) ;
  if (item && item->IsCheckable())
  {
    bar->Check(id,!bar->Checked(id)) ;
  }
*/

  wxEvtHandler* evtHandler = GetEventHandler();

  evtHandler->ProcessEvent(commandEvent);
}

// Checks if there is a toolbar, and returns the first free client position
wxPoint wxFrame::GetClientAreaOrigin() const
{
    wxPoint pt(0, 0);
#if wxUSE_TOOLBAR
    if (GetToolBar())
    {
        int w, h;
        GetToolBar()->GetSize(& w, & h);

        if (GetToolBar()->GetWindowStyleFlag() & wxTB_VERTICAL)
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

void wxFrame::ScreenToClient(int *x, int *y) const
{
    wxWindow::ScreenToClient(x, y);

    // We may be faking the client origin.
    // So a window that's really at (0, 30) may appear
    // (to wxWin apps) to be at (0, 0).
    wxPoint pt(GetClientAreaOrigin());
    *x -= pt.x;
    *y -= pt.y;
}

void wxFrame::ClientToScreen(int *x, int *y) const
{
    // We may be faking the client origin.
    // So a window that's really at (0, 30) may appear
    // (to wxWin apps) to be at (0, 0).
    wxPoint pt1(GetClientAreaOrigin());
    *x += pt1.x;
    *y += pt1.y;

    wxWindow::ClientToScreen(x, y);
}

#if wxUSE_TOOLBAR
wxToolBar* wxFrame::CreateToolBar(long style, wxWindowID id, const wxString& name)
{
    wxCHECK_MSG( m_frameToolBar == NULL, FALSE,
        "recreating toolbar in wxFrame" );

    wxToolBar* toolBar = OnCreateToolBar(style, id, name);
    if (toolBar)
    {
        SetToolBar(toolBar);
        PositionToolBar();
        return toolBar;
    }
    else
    {
        return NULL;
    }
}

wxToolBar* wxFrame::OnCreateToolBar(long style, wxWindowID id, const wxString& name)
{
    return new wxToolBar(this, id, wxPoint(0, 0), wxSize(100, 24), style, name);
}

void wxFrame::SetToolBar(wxToolBar *toolbar)
{
    m_frameToolBar = toolbar;
}

wxToolBar *wxFrame::GetToolBar() const
{
    return m_frameToolBar;
}

void wxFrame::PositionToolBar()
{
    int cw, ch;

    GetClientSize(& cw, &ch);

    if (GetToolBar())
    {
        int tw, th;
        GetToolBar()->GetSize(& tw, & th);

        if (GetToolBar()->GetWindowStyleFlag() & wxTB_VERTICAL)
        {
            // Use the 'real' position. wxSIZE_NO_ADJUSTMENTS
            // means, pretend we don't have toolbar/status bar, so we
            // have the original client size.
            GetToolBar()->SetSize(0, 0, tw, ch + th, wxSIZE_NO_ADJUSTMENTS);
        }
        else
        {
            // Use the 'real' position
            GetToolBar()->SetSize(0, 0, cw, th, wxSIZE_NO_ADJUSTMENTS);
        }
    }
}
#endif // wxUSE_TOOLBAR

void wxFrame::CaptureMouse()
{
    if (m_winCaptured)
        return;

    if (GetMainWidget())
        XtAddGrab((Widget) m_frameShell, TRUE, FALSE);
    m_winCaptured = TRUE;
}

void wxFrame::ReleaseMouse()
{
    if (!m_winCaptured)
        return;

    if (GetMainWidget())
        XtRemoveGrab((Widget) m_frameShell);
    m_winCaptured = FALSE;
}

void wxFrame::Raise(void)
{
    Window parent_window = XtWindow((Widget) m_frameShell),
        next_parent   = XtWindow((Widget) m_frameShell),
        root          = RootWindowOfScreen(XtScreen((Widget) m_frameShell));
    // search for the parent that is child of ROOT, because the WM may
    // reparent twice and notify only the next parent (like FVWM)
    while (next_parent != root) {
        Window *theChildren; unsigned int n;
        parent_window = next_parent;
        XQueryTree(XtDisplay((Widget) m_frameShell), parent_window, &root,
            &next_parent, &theChildren, &n);
        XFree(theChildren); // not needed
    }
    XRaiseWindow(XtDisplay((Widget) m_frameShell), parent_window);
}

void wxFrame::Lower(void)
{
    Window parent_window = XtWindow((Widget) m_frameShell),
        next_parent   = XtWindow((Widget) m_frameShell),
        root          = RootWindowOfScreen(XtScreen((Widget) m_frameShell));
    // search for the parent that is child of ROOT, because the WM may
    // reparent twice and notify only the next parent (like FVWM)
    while (next_parent != root) {
        Window *theChildren; unsigned int n;
        parent_window = next_parent;
        XQueryTree(XtDisplay((Widget) m_frameShell), parent_window, &root,
            &next_parent, &theChildren, &n);
        XFree(theChildren); // not needed
    }
    XLowerWindow(XtDisplay((Widget) m_frameShell), parent_window);
}

void wxFrameFocusProc(Widget workArea, XtPointer clientData,
                      XmAnyCallbackStruct *cbs)
{
    // wxDebugMsg("focus proc from frame %ld\n",(long)frame);
    // TODO
    // wxFrame *frame = (wxFrame *)clientData;
    // frame->GetEventHandler()->OnSetFocus();
}

/* MATTEW: Used to insure that hide-&-show within an event cycle works */
static void wxFrameMapProc(Widget frameShell, XtPointer clientData,
                           XCrossingEvent * event)
{
    wxFrame *frame = (wxFrame *)wxGetWindowFromTable((Widget)clientData);

    if (frame) {
        XEvent *e = (XEvent *)event;

        if (e->xany.type == MapNotify)
        {
            // Iconize fix
            XtVaSetValues(frameShell, XmNiconic, (Boolean)False, NULL);
            if (!frame->GetVisibleStatus())
            {
                /* We really wanted this to be hidden! */
                XtUnmapWidget((Widget) frame->GetShellWidget());
            }
        }
        else if (e->xany.type == UnmapNotify)
            // Iconize fix
            XtVaSetValues(frameShell, XmNiconic, (Boolean)True, NULL);
    }
}

//// Motif-specific
bool wxFrame::PreResize()
{
#if wxUSE_TOOLBAR
    PositionToolBar();
#endif // wxUSE_TOOLBAR
    PositionStatusBar();
    return TRUE;
}

WXWidget wxFrame::GetClientWidget() const
{
    return m_clientArea;
}

void wxFrame::ChangeFont(bool keepOriginalSize)
{
    // TODO
}

void wxFrame::ChangeBackgroundColour()
{
    if (GetClientWidget())
        DoChangeBackgroundColour(GetClientWidget(), m_backgroundColour);
}

void wxFrame::ChangeForegroundColour()
{
    if (GetClientWidget())
        DoChangeForegroundColour(GetClientWidget(), m_foregroundColour);
}

void wxCloseFrameCallback(Widget widget, XtPointer client_data, XmAnyCallbackStruct *cbs)
{
    wxFrame *frame = (wxFrame *)client_data;

    wxCloseEvent closeEvent(wxEVT_CLOSE_WINDOW, frame->GetId());
    closeEvent.SetEventObject(frame);

    // May delete the frame (with delayed deletion)
    frame->GetEventHandler()->ProcessEvent(closeEvent);
}

