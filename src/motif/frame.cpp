/////////////////////////////////////////////////////////////////////////////
// Name:        motif/frame.cpp
// Purpose:     wxFrame
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

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

#ifdef __VMS__
    #pragma message disable nosimpint
#endif

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

#ifdef __VMS__
    #pragma message enable nosimpint
#endif

#include "wx/motif/private.h"

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static void wxCloseFrameCallback(Widget, XtPointer, XmAnyCallbackStruct *cbs);
static void wxFrameFocusProc(Widget workArea, XtPointer clientData,
                            XmAnyCallbackStruct *cbs);
static void wxFrameMapProc(Widget frameShell, XtPointer clientData,
                           XCrossingEvent * event);

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

extern wxList wxModelessWindows;
extern wxList wxPendingDelete;

// TODO: this should be tidied so that any frame can be the
// top frame
static bool wxTopLevelUsed = FALSE;

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
BEGIN_EVENT_TABLE(wxFrame, wxFrameBase)
    EVT_ACTIVATE(wxFrame::OnActivate)
    EVT_SYS_COLOUR_CHANGED(wxFrame::OnSysColourChanged)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxFrame, wxWindow)
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// frame construction
// ----------------------------------------------------------------------------

void wxFrame::Init()
{
    m_iconized = FALSE;

    //// Motif-specific
    m_frameShell = (WXWidget) NULL;
    m_frameWidget = (WXWidget) NULL;;
    m_workArea = (WXWidget) NULL;;
    m_clientArea = (WXWidget) NULL;;
    m_visibleStatus = TRUE;
}

bool wxFrame::Create(wxWindow *parent,
                     wxWindowID id,
                     const wxString& title,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style,
                     const wxString& name)
{
    if ( parent )
        AddChild(this);
    else
        wxTopLevelWindows.Append(this);

    wxModelessWindows.Append(this);

    SetName(name);

    m_windowStyle = style;

    m_backgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_APPWORKSPACE);
    m_foregroundColour = *wxBLACK;
    m_font = wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT);

    if ( id > -1 )
        m_windowId = id;
    else
        m_windowId = (int)NewControlId();

    int x = pos.x, y = pos.y;
    int width = size.x, height = size.y;

    // Set reasonable values for position and size if defaults have been
    // requested
    //
    // MB TODO: something better than these arbitrary values ?
    // VZ       should use X resources for this...
    if ( width == -1 )
        width = 400;
    if ( height == -1 )
        height = 400;

    int displayW, displayH;
    wxDisplaySize( &displayW, &displayH );

    if ( x == -1 )
    {
        x = (displayW - width) / 2;
        if (x < 10) x = 10;
    }
    if ( y == -1 )
    {
        y = (displayH - height) / 2;
        if (y < 10) y = 10;
    }

    // VZ: what does this do??
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

    if (!title.IsEmpty())
        XtVaSetValues((Widget) m_frameShell,
        XmNtitle, title.c_str(),
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

    wxLogTrace(wxTRACE_Messages,
               "Created frame (0x%08x) with work area 0x%08x and client "
               "area 0x%08x", m_frameWidget, m_workArea, m_clientArea);

    XtAddEventHandler((Widget) m_clientArea, ExposureMask,FALSE,
        wxUniversalRepaintProc, (XtPointer) this);

    XtVaSetValues((Widget) m_frameWidget,
        XmNworkWindow, (Widget) m_workArea,
        NULL);

    XtManageChild((Widget) m_clientArea);
    XtManageChild((Widget) m_workArea);

    wxAddWindowToTable((Widget) m_workArea, this);

    XtTranslations ptr;

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
        int decor = 0;
        if (style & wxRESIZE_BORDER)
            decor |= MWM_DECOR_RESIZEH;
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
        XtVaSetValues((Widget) m_frameShell,XmNmwmDecorations,decor,NULL);
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
    {
        delete m_frameStatusBar;
        m_frameStatusBar = NULL;
    }

    DestroyChildren();

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
void wxFrame::DoGetClientSize(int *x, int *y) const
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

void wxFrame::DoGetSize(int *width, int *height) const
{
    Dimension xx, yy;
    XtVaGetValues((Widget) m_frameShell, XmNwidth, &xx, XmNheight, &yy, NULL);
    *width = xx; *height = yy;
}

void wxFrame::DoGetPosition(int *x, int *y) const
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

void wxFrame::DoSetSize(int x, int y, int width, int height, int WXUNUSED(sizeFlags))
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

void wxFrame::Restore()
{
    if ( m_frameShell )
        XtVaSetValues((Widget) m_frameShell, XmNiconic, FALSE, NULL);
}

void wxFrame::Maximize(bool maximize)
{
    Show(TRUE);

    if ( maximize )
        Restore();
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
bool wxFrame::IsMaximized() const
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
        XmNtitle, title.c_str(),
        XmNiconName, title.c_str(),
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

// Default activation behaviour - set the focus for the first child
// subwindow found.
void wxFrame::OnActivate(wxActivateEvent& event)
{
    if (!event.GetActive())
        return;

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

#if wxUSE_TOOLBAR

wxToolBar* wxFrame::CreateToolBar(long style,
                                  wxWindowID id,
                                  const wxString& name)
{
    if ( wxFrameBase::CreateToolBar(style, id, name) )
    {
        PositionToolBar();
    }

    return m_frameToolBar;
}

void wxFrame::PositionToolBar()
{
    if (GetToolBar())
    {
        int cw, ch;
        GetClientSize(& cw, &ch);

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

void wxFrame::Raise()
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

void wxFrame::Lower()
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

void wxFrameFocusProc(Widget WXUNUSED(workArea), XtPointer WXUNUSED(clientData),
                      XmAnyCallbackStruct *WXUNUSED(cbs))
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

#if wxUSE_STATUSBAR
    PositionStatusBar();
#endif // wxUSE_STATUSBAR

    return TRUE;
}

WXWidget wxFrame::GetClientWidget() const
{
    return m_clientArea;
}

void wxFrame::ChangeFont(bool WXUNUSED(keepOriginalSize))
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

void wxCloseFrameCallback(Widget WXUNUSED(widget), XtPointer client_data, XmAnyCallbackStruct *WXUNUSED(cbs))
{
    wxFrame *frame = (wxFrame *)client_data;

    wxCloseEvent closeEvent(wxEVT_CLOSE_WINDOW, frame->GetId());
    closeEvent.SetEventObject(frame);

    // May delete the frame (with delayed deletion)
    frame->GetEventHandler()->ProcessEvent(closeEvent);
}

