/////////////////////////////////////////////////////////////////////////////
// Name:        windows.cpp
// Purpose:     wxWindow
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "window.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
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
#endif

#if     wxUSE_OWNER_DRAWN
#include "wx/ownerdrw.h"
#endif

#if     wxUSE_DRAG_AND_DROP
#include "wx/msw/ole/droptgt.h"
#endif

#include "wx/menuitem.h"
#include "wx/log.h"
#include "wx/msw/private.h"

#include <string.h>

#ifndef __GNUWIN32__
#include <shellapi.h>
#include <mmsystem.h>
#endif

#ifdef __WIN32__
#include <windowsx.h>
#endif

#ifdef __GNUWIN32__
#include <wx/msw/gnuwin32/extra.h>
#endif

#ifdef GetCharWidth
#undef GetCharWidth
#endif

#ifdef FindWindow
#undef FindWindow
#endif

#ifdef GetClassName
#undef GetClassName
#endif

#ifdef GetClassInfo
#undef GetClassInfo
#endif

#ifdef  __WXDEBUG__
const char *wxGetMessageName(int message);
#endif  //WXDEBUG

#define WINDOW_MARGIN 3 // This defines sensitivity of Leave events

wxMenu *wxCurrentPopupMenu = NULL;
extern wxList wxPendingDelete;

void wxRemoveHandleAssociation(wxWindow *win);
void wxAssociateWinWithHandle(HWND hWnd, wxWindow *win);
wxWindow *wxFindWinFromHandle(WXHWND hWnd);

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

// Find an item given the MS Windows id
wxWindow *wxWindow::FindItem(int id) const
{
    if (!GetChildren())
        return NULL;
    wxNode *current = GetChildren()->First();
    while (current)
    {
        wxWindow *childWin = (wxWindow *)current->Data();

        wxWindow *wnd = childWin->FindItem(id) ;
        if (wnd)
            return wnd ;

        if (childWin->IsKindOf(CLASSINFO(wxControl)))
        {
            wxControl *item = (wxControl *)childWin;
            if (item->m_windowId == id)
                return item;
            else
            {
                // In case it's a 'virtual' control (e.g. radiobox)
                if (item->GetSubcontrols().Member((wxObject *)id))
                    return item;
            }
        }
        current = current->Next();
    }
    return NULL;
}

// Find an item given the MS Windows handle
wxWindow *wxWindow::FindItemByHWND(WXHWND hWnd, bool controlOnly) const
{
    if (!GetChildren())
        return NULL;
    wxNode *current = GetChildren()->First();
    while (current)
    {
        wxObject *obj = (wxObject *)current->Data() ;
        // Do a recursive search.
        wxWindow *parent = (wxWindow *)obj ;
        wxWindow *wnd = parent->FindItemByHWND(hWnd) ;
        if (wnd)
            return wnd ;

        if ((!controlOnly) || obj->IsKindOf(CLASSINFO(wxControl)))
        {
            wxWindow *item = (wxWindow *)current->Data();
            if ((HWND)(item->GetHWND()) == (HWND) hWnd)
                return item;
            else
            {
                if ( item->ContainsHWND(hWnd) )
                    return item;
            }
        }
        current = current->Next();
    }
    return NULL;
}

// Default command handler
bool wxWindow::MSWCommand(WXUINT WXUNUSED(param), WXWORD WXUNUSED(id))
{
    return FALSE;
}

bool wxWindow::MSWNotify(WXWPARAM WXUNUSED(wParam), WXLPARAM WXUNUSED(lParam))
{
    return FALSE;
}

void wxWindow::PreDelete(WXHDC WXUNUSED(dc))
{
}

WXHWND wxWindow::GetHWND(void) const
{
    return (WXHWND) m_hWnd;
}

void wxWindow::SetHWND(WXHWND hWnd)
{
    m_hWnd = hWnd;
}

// Constructor
wxWindow::wxWindow(void)
{
    // Generic
    m_windowId = 0;
    m_isShown = TRUE;
    m_windowStyle = 0;
    m_windowParent = NULL;
    m_windowEventHandler = this;
    m_windowName = "";
    m_windowCursor = *wxSTANDARD_CURSOR;
    m_children = new wxList;
    m_doubleClickAllowed = 0 ;
    m_winCaptured = FALSE;
    m_constraints = NULL;
    m_constraintsInvolvedIn = NULL;
    m_windowSizer = NULL;
    m_sizerParent = NULL;
    m_autoLayout = FALSE;
    m_windowValidator = NULL;

    // MSW-specific
    m_hWnd = 0;
    m_winEnabled = TRUE;
    m_caretWidth = 0; m_caretHeight = 0;
    m_caretEnabled = FALSE;
    m_caretShown = FALSE;
    m_inOnSize = FALSE;
    m_minSizeX = -1;
    m_minSizeY = -1;
    m_maxSizeX = -1;
    m_maxSizeY = -1;
    //  m_paintHDC = 0;
    //  m_tempHDC = 0;
    m_isBeingDeleted = FALSE;
    m_oldWndProc = 0;
#ifndef __WIN32__
    m_globalHandle = 0;
#endif
    m_useCtl3D = FALSE;

    m_defaultItem = NULL;

    wxSystemSettings settings;

    m_backgroundColour = settings.GetSystemColour(wxSYS_COLOUR_3DFACE) ;
    // m_backgroundColour = settings.GetSystemColour(wxSYS_COLOUR_WINDOW) ; ;
    m_foregroundColour = *wxBLACK;

    /*
    wxColour(GetRValue(GetSysColor(COLOR_WINDOW)),
    GetGValue(GetSysColor(COLOR_BTNFACE)), GetBValue(GetSysColor(COLOR_BTNFACE)));
    */

    // wxWnd
    m_lastMsg = 0;
    m_lastWParam = 0;
    m_lastLParam = 0;
    //  m_acceleratorTable = 0;
    m_hMenu = 0;

    m_xThumbSize = 0;
    m_yThumbSize = 0;
    m_backgroundTransparent = FALSE;

    m_lastXPos = (float)-1.0;
    m_lastYPos = (float)-1.0;
    m_lastEvent = -1;
    m_returnCode = 0;

#if  wxUSE_DRAG_AND_DROP
    m_pDropTarget = NULL;
#endif
}

// Destructor
wxWindow::~wxWindow(void)
{
    m_isBeingDeleted = TRUE;

    // JACS - if behaviour is odd, restore this
    // to the start of ~wxWindow. Vadim has changed
    // it to nearer the end. Unsure of side-effects
    // e.g. when deleting associated global data.
    // Restore old Window proc, if required
    //  UnsubclassWin();

    // Have to delete constraints/sizer FIRST otherwise
    // sizers may try to look at deleted windows as they
    // delete themselves.
#if wxUSE_CONSTRAINTS
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

    // wxWnd
    MSWDetachWindowMenu();

    if (m_windowParent)
        m_windowParent->RemoveChild(this);

    DestroyChildren();

    if (m_hWnd)
        ::DestroyWindow((HWND)m_hWnd);

    wxRemoveHandleAssociation(this);
    m_hWnd = 0;
#ifndef __WIN32__
    if (m_globalHandle)
    {
        GlobalFree((HGLOBAL) m_globalHandle);
        m_globalHandle = 0;
    }
#endif

    delete m_children;
    m_children = NULL;

    // Just in case the window has been Closed, but
    // we're then deleting immediately: don't leave
    // dangling pointers.
    wxPendingDelete.DeleteObject(this);

    // Just in case we've loaded a top-level window via
    // wxWindow::LoadNativeDialog but we weren't a dialog
    // class
    wxTopLevelWindows.DeleteObject(this);

    if ( m_windowValidator )
        delete m_windowValidator;

    // Restore old Window proc, if required
    // and remove hWnd <-> wxWindow association
    UnsubclassWin();
}

// Destroy the window (delayed, if a managed window)
bool wxWindow::Destroy(void)
{
    delete this;
    return TRUE;
}

extern char wxCanvasClassName[];

// Constructor
bool wxWindow::Create(wxWindow *parent, wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxString& name)
{
    // Generic
    m_isBeingDeleted = FALSE;
    m_windowId = 0;
    m_isShown = TRUE;
    m_windowStyle = 0;
    m_windowParent = NULL;
    m_windowEventHandler = this;
    m_windowName = "";
    m_windowCursor = *wxSTANDARD_CURSOR;
    m_doubleClickAllowed = 0 ;
    m_winCaptured = FALSE;
    m_constraints = NULL;
    m_constraintsInvolvedIn = NULL;
    m_windowSizer = NULL;
    m_sizerParent = NULL;
    m_autoLayout = FALSE;
    m_windowValidator = NULL;
#if  wxUSE_DRAG_AND_DROP
    m_pDropTarget = NULL;
#endif

    // MSW-specific
    m_hWnd = 0;
    m_winEnabled = TRUE;
    m_caretWidth = 0; m_caretHeight = 0;
    m_caretEnabled = FALSE;
    m_caretShown = FALSE;
    m_inOnSize = FALSE;
    m_minSizeX = -1;
    m_minSizeY = -1;
    m_maxSizeX = -1;
    m_maxSizeY = -1;
    m_oldWndProc = 0;
#ifndef __WIN32__
    m_globalHandle = 0;
#endif
    m_useCtl3D = FALSE;
    m_defaultItem = NULL;
    m_windowParent = NULL;
    m_mouseInWindow = FALSE;
    if (!parent)
        return FALSE;

    if (parent) parent->AddChild(this);

    // wxWnd
    m_lastMsg = 0;
    m_lastWParam = 0;
    m_lastLParam = 0;
    m_hMenu = 0;

    m_xThumbSize = 0;
    m_yThumbSize = 0;
    m_backgroundTransparent = FALSE;

    m_lastXPos = (float)-1.0;
    m_lastYPos = (float)-1.0;
    m_lastEvent = -1;
    m_returnCode = 0;

    SetName(name);

    if ( id == -1 )
        m_windowId = (int)NewControlId();
    else
        m_windowId = id;

    int x = pos.x;
    int y = pos.y;
    int width = size.x;
    int height = size.y;

    // To be consistent with wxGTK
    if (width == -1)
        width = 20;
    if (height == -1)
        height = 20;

    wxSystemSettings settings;

    m_backgroundColour = settings.GetSystemColour(wxSYS_COLOUR_WINDOW) ; ;
    // m_backgroundColour = settings.GetSystemColour(wxSYS_COLOUR_3DFACE) ;
    m_foregroundColour = *wxBLACK;

    m_windowStyle = style;

    DWORD msflags = 0;
    if (style & wxBORDER)
        msflags |= WS_BORDER;
    if (style & wxTHICK_FRAME)
        msflags |= WS_THICKFRAME;

    msflags |= WS_CHILD | WS_VISIBLE;
    if (style & wxCLIP_CHILDREN)
        msflags |= WS_CLIPCHILDREN;

    bool want3D;
    WXDWORD exStyle = Determine3DEffects(WS_EX_CLIENTEDGE, &want3D) ;

    // Even with extended styles, need to combine with WS_BORDER
    // for them to look right.
    if (want3D || (m_windowStyle & wxSIMPLE_BORDER) || (m_windowStyle & wxRAISED_BORDER) ||
        (m_windowStyle & wxSUNKEN_BORDER) || (m_windowStyle & wxDOUBLE_BORDER))
        msflags |= WS_BORDER;

    m_mouseInWindow = FALSE ;

    MSWCreate(m_windowId, parent, wxCanvasClassName, this, NULL,
        x, y, width, height, msflags, NULL, exStyle);

    return TRUE;
}

void wxWindow::SetFocus(void)
{
    HWND hWnd = (HWND) GetHWND();
    if (hWnd)
        ::SetFocus(hWnd);
}

void wxWindow::Enable(bool enable)
{
    m_winEnabled = enable;
    HWND hWnd = (HWND) GetHWND();
    if (hWnd)
        ::EnableWindow(hWnd, (BOOL)enable);
}

void wxWindow::CaptureMouse(void)
{
    HWND hWnd = (HWND) GetHWND();
    if (hWnd && !m_winCaptured)
    {
        SetCapture(hWnd);
        m_winCaptured = TRUE;
    }
}

void wxWindow::ReleaseMouse(void)
{
    if (m_winCaptured)
    {
        ReleaseCapture();
        m_winCaptured = FALSE;
    }
}

void wxWindow::SetAcceleratorTable(const wxAcceleratorTable& accel)
{
    m_acceleratorTable = accel;
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

#if    wxUSE_DRAG_AND_DROP

void wxWindow::SetDropTarget(wxDropTarget *pDropTarget)
{
    if ( m_pDropTarget != 0 ) {
        m_pDropTarget->Revoke(m_hWnd);
        delete m_pDropTarget;
    }

    m_pDropTarget = pDropTarget;
    if ( m_pDropTarget != 0 )
        m_pDropTarget->Register(m_hWnd);
}

#endif

//old style file-manager drag&drop support
// I think we should retain the old-style
// DragAcceptFiles in parallel with SetDropTarget.
// JACS
void wxWindow::DragAcceptFiles(bool accept)
{
    HWND hWnd = (HWND) GetHWND();
    if (hWnd)
        ::DragAcceptFiles(hWnd, (BOOL)accept);
}

// Get total size
void wxWindow::GetSize(int *x, int *y) const
{
    HWND hWnd = (HWND) GetHWND();
    RECT rect;
    GetWindowRect(hWnd, &rect);
    *x = rect.right - rect.left;
    *y = rect.bottom - rect.top;
}

void wxWindow::GetPosition(int *x, int *y) const
{
    HWND hWnd = (HWND) GetHWND();
    HWND hParentWnd = 0;
    if (GetParent())
        hParentWnd = (HWND) GetParent()->GetHWND();

    RECT rect;
    GetWindowRect(hWnd, &rect);

    // Since we now have the absolute screen coords,
    // if there's a parent we must subtract its top left corner
    POINT point;
    point.x = rect.left;
    point.y = rect.top;
    if (hParentWnd)
    {
        ::ScreenToClient(hParentWnd, &point);
    }

    // We may be faking the client origin.
    // So a window that's really at (0, 30) may appear
    // (to wxWin apps) to be at (0, 0).
    if (GetParent())
    {
        wxPoint pt(GetParent()->GetClientAreaOrigin());
        point.x -= pt.x;
        point.y -= pt.y;
    }
    *x = point.x;
    *y = point.y;
}

void wxWindow::ScreenToClient(int *x, int *y) const
{
    HWND hWnd = (HWND) GetHWND();
    POINT pt;
    pt.x = *x;
    pt.y = *y;
    ::ScreenToClient(hWnd, &pt);

    /*
    // We may be faking the client origin.
    // So a window that's really at (0, 30) may appear
    // (to wxWin apps) to be at (0, 0).
    if (GetParent())
    {
    wxPoint pt1(GetParent()->GetClientAreaOrigin());
    pt.x -= pt1.x;
    pt.y -= pt1.y;
    }
    */

    *x = pt.x;
    *y = pt.y;
}

void wxWindow::ClientToScreen(int *x, int *y) const
{
    HWND hWnd = (HWND) GetHWND();
    POINT pt;
    pt.x = *x;
    pt.y = *y;

    /*
    // We may be faking the client origin.
    // So a window that's really at (0, 30) may appear
    // (to wxWin apps) to be at (0, 0).
    if (GetParent())
    {
    wxPoint pt1(GetParent()->GetClientAreaOrigin());
    pt.x += pt1.x;
    pt.y += pt1.y;
    }
    */

    ::ClientToScreen(hWnd, &pt);

    *x = pt.x;
    *y = pt.y;
}

void wxWindow::SetCursor(const wxCursor& cursor)
{
    m_windowCursor = cursor;
    if (m_windowCursor.Ok())
    {
        HWND hWnd = (HWND) GetHWND();

        // Change the cursor NOW if we're within the correct window
        POINT point;
        ::GetCursorPos(&point);

        RECT rect;
        ::GetWindowRect(hWnd, &rect);

        if (::PtInRect(&rect, point) && !wxIsBusy())
            ::SetCursor((HCURSOR) m_windowCursor.GetHCURSOR());
    }

    // This will cause big reentrancy problems if wxFlushEvents is implemented.
    //  wxFlushEvents();
    //  return old_cursor;
}


// Get size *available for subwindows* i.e. excluding menu bar etc.
void wxWindow::GetClientSize(int *x, int *y) const
{
    HWND hWnd = (HWND) GetHWND();
    RECT rect;
    GetClientRect(hWnd, &rect);
    *x = rect.right;
    *y = rect.bottom;
}

void wxWindow::SetSize(int x, int y, int width, int height, int sizeFlags)
{
    int currentX, currentY;
    GetPosition(&currentX, &currentY);
    int actualWidth = width;
    int actualHeight = height;
    int actualX = x;
    int actualY = y;
    if (x == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
        actualX = currentX;
    if (y == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
        actualY = currentY;

    AdjustForParentClientOrigin(actualX, actualY, sizeFlags);

    int currentW,currentH;
    GetSize(&currentW, &currentH);
    if (width == -1)
        actualWidth = currentW ;
    if (height == -1)
        actualHeight = currentH ;

    HWND hWnd = (HWND) GetHWND();
    if (hWnd)
        MoveWindow(hWnd, actualX, actualY, actualWidth, actualHeight, (BOOL)TRUE);
}

void wxWindow::SetClientSize(int width, int height)
{
    wxWindow *parent = GetParent();
    HWND hWnd = (HWND) GetHWND();
    HWND hParentWnd = (HWND) (HWND) parent->GetHWND();

    RECT rect;
    GetClientRect(hWnd, &rect);

    RECT rect2;
    GetWindowRect(hWnd, &rect2);

    // Find the difference between the entire window (title bar and all)
    // and the client area; add this to the new client size to move the
    // window
    int actual_width = rect2.right - rect2.left - rect.right + width;
    int actual_height = rect2.bottom - rect2.top - rect.bottom + height;

    // If there's a parent, must subtract the parent's top left corner
    // since MoveWindow moves relative to the parent

    POINT point;
    point.x = rect2.left;
    point.y = rect2.top;
    if (parent)
    {
        ::ScreenToClient(hParentWnd, &point);
    }

    MoveWindow(hWnd, point.x, point.y, actual_width, actual_height, (BOOL)TRUE);

    wxSizeEvent event(wxSize(width, height), m_windowId);
    event.SetEventObject(this);
    GetEventHandler()->ProcessEvent(event);
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
    HWND hWnd = (HWND) GetHWND();
    int cshow;
    if (show)
        cshow = SW_SHOW;
    else
        cshow = SW_HIDE;
    ShowWindow(hWnd, (BOOL)cshow);
    if (show)
    {
        BringWindowToTop(hWnd);
        // Next line causes a crash on NT, apparently.
        //    UpdateWindow(hWnd); // Should this be here or will it cause inefficiency?
    }
    return TRUE;
}

bool wxWindow::IsShown(void) const
{
    return (::IsWindowVisible((HWND) GetHWND()) != 0);
}

int wxWindow::GetCharHeight(void) const
{
    TEXTMETRIC lpTextMetric;
    HWND hWnd = (HWND) GetHWND();
    HDC dc = ::GetDC(hWnd);

    GetTextMetrics(dc, &lpTextMetric);
    ::ReleaseDC(hWnd, dc);

    return lpTextMetric.tmHeight;
}

int wxWindow::GetCharWidth(void) const
{
    TEXTMETRIC lpTextMetric;
    HWND hWnd = (HWND) GetHWND();
    HDC dc = ::GetDC(hWnd);

    GetTextMetrics(dc, &lpTextMetric);
    ::ReleaseDC(hWnd, dc);

    return lpTextMetric.tmAveCharWidth;
}

void wxWindow::GetTextExtent(const wxString& string, int *x, int *y,
                             int *descent, int *externalLeading, const wxFont *theFont, bool) const
{
    wxFont *fontToUse = (wxFont *)theFont;
    if (!fontToUse)
        fontToUse = (wxFont *) & m_windowFont;

    HWND hWnd = (HWND) GetHWND();
    HDC dc = ::GetDC(hWnd);

    HFONT fnt = 0;
    HFONT was = 0;
    if (fontToUse && fontToUse->Ok())
    {
        if ((fnt=(HFONT) fontToUse->GetResourceHandle()))
            was = (HFONT) SelectObject(dc,fnt) ;
    }

    SIZE sizeRect;
    TEXTMETRIC tm;
    GetTextExtentPoint(dc, (const char *)string, (int)string.Length(), &sizeRect);
    GetTextMetrics(dc, &tm);

    if (fontToUse && fnt && was)
        SelectObject(dc,was) ;

    ReleaseDC(hWnd, dc);

    *x = sizeRect.cx;
    *y = sizeRect.cy;
    if (descent) *descent = tm.tmDescent;
    if (externalLeading) *externalLeading = tm.tmExternalLeading;

    //  if (fontToUse)
    //    fontToUse->ReleaseResource();
}

void wxWindow::Refresh(bool eraseBack, const wxRectangle *rect)
{
    HWND hWnd = (HWND) GetHWND();
    if (hWnd)
    {
        if (rect)
        {
            RECT mswRect;
            mswRect.left = rect->x;
            mswRect.top = rect->y;
            mswRect.right = rect->x + rect->width;
            mswRect.bottom = rect->y + rect->height;

            ::InvalidateRect(hWnd, &mswRect, eraseBack);
        }
        else
            ::InvalidateRect(hWnd, NULL, eraseBack);
    }
}

bool wxWindow::ProcessEvent(wxEvent& event)
{
    // we save here the information about the last message because it might be
    // overwritten if the event handler sends any messages to our window (case
    // in point: wxNotebook::OnSize) - and then if we call Default() later
    // (which is done quite often if the message is not processed) it will use
    // incorrect values for m_lastXXX variables
    WXUINT lastMsg = m_lastMsg;
    WXWPARAM lastWParam = m_lastWParam;
    WXLPARAM lastLParam = m_lastLParam;

    // call the base version
    bool bProcessed = wxEvtHandler::ProcessEvent(event);

    // restore
    m_lastMsg = lastMsg;
    m_lastWParam = lastWParam;
    m_lastLParam = lastLParam;

    return bProcessed;
}

// Hook for new window just as it's being created,
// when the window isn't yet associated with the handle
wxWindow *wxWndHook = NULL;

// Main window proc
LRESULT APIENTRY _EXPORT wxWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    wxWindow *wnd = wxFindWinFromHandle((WXHWND) hWnd);

    if (!wnd && wxWndHook)
    {
        wxAssociateWinWithHandle(hWnd, wxWndHook);
        wnd = wxWndHook;
        wxWndHook = NULL;
        wnd->m_hWnd = (WXHWND) hWnd;
    }
#if (WXDEBUG > 1)
    wxDebugMsg("hWnd = %d, m_hWnd = %d, msg = %d\n", hWnd, m_hWnd, message);
#endif
    // Stop right here if we don't have a valid handle
    // in our wxWnd object.
    if (wnd && !wnd->m_hWnd) {
        //    wxDebugMsg("Warning: could not find a valid handle, wx_win.cc/wxWndProc.\n");
        wnd->m_hWnd = (WXHWND) hWnd;
        long res = wnd->MSWDefWindowProc(message, wParam, lParam );
        wnd->m_hWnd = 0;
        return res;
    }

    if (wnd) {
        wnd->m_lastMsg = message;
        wnd->m_lastWParam = wParam;
        wnd->m_lastLParam = lParam;
    }
    if (wnd)
        return wnd->MSWWindowProc(message, wParam, lParam);
    else
        return DefWindowProc( hWnd, message, wParam, lParam );
}

// Should probably have a test for 'genuine' NT
#if defined(__WIN32__)
#define DIMENSION_TYPE short
#else
#define DIMENSION_TYPE int
#endif

// Main Windows 3 window proc
long wxWindow::MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
    wxASSERT( m_lastMsg == message &&
        m_lastWParam == wParam &&
        m_lastLParam == lParam );

#ifdef __WXDEBUG__
    wxLogTrace(wxTraceMessages, "Processing %s(%lx, %lx)",
        wxGetMessageName(message), wParam, lParam);
#endif // WXDEBUG

    HWND hWnd = (HWND)m_hWnd;

    switch (message)
    {
    case WM_ACTIVATE:
        {
#ifdef __WIN32__
            WORD state = LOWORD(wParam);
            WORD minimized = HIWORD(wParam);
            HWND hwnd = (HWND)lParam;
#else
            WORD state = (WORD)wParam;
            WORD minimized = LOWORD(lParam);
            HWND hwnd = (HWND)HIWORD(lParam);
#endif
            MSWOnActivate(state, (minimized != 0), (WXHWND) hwnd);
            return 0;
            break;
        }
    case WM_SETFOCUS:
        {
            HWND hwnd = (HWND)wParam;
            //            return OnSetFocus(hwnd);

            if (MSWOnSetFocus((WXHWND) hwnd))
                return 0;
            else return MSWDefWindowProc(message, wParam, lParam );
            break;
        }
    case WM_KILLFOCUS:
        {
            HWND hwnd = (HWND)lParam;
            //            return OnKillFocus(hwnd);
            if (MSWOnKillFocus((WXHWND) hwnd))
                return 0;
            else
                return MSWDefWindowProc(message, wParam, lParam );
            break;
        }
    case WM_CREATE:
        {
            MSWOnCreate((WXLPCREATESTRUCT) (LPCREATESTRUCT)lParam);
            return 0;
            break;
        }
    case WM_SHOWWINDOW:
        {
            MSWOnShow((wParam != 0), (int) lParam);
            break;
        }
    case WM_PAINT:
        {
            if (MSWOnPaint())
                return 0;
            else return MSWDefWindowProc(message, wParam, lParam );
            break;
        }
    case WM_QUERYDRAGICON:
        {
            HICON hIcon = 0;
            if ((hIcon = (HICON) MSWOnQueryDragIcon()))
                return (long)hIcon;
            else return MSWDefWindowProc(message, wParam, lParam );
            break;
        }

    case WM_SIZE:
        {
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);
            MSWOnSize(width, height, wParam);
            break;
        }

    case WM_MOVE:
        {
            wxMoveEvent event(wxPoint(LOWORD(lParam), HIWORD(lParam)),
                m_windowId);
            event.SetEventObject(this);
            if ( !GetEventHandler()->ProcessEvent(event) )
                Default();
        }
        break;

    case WM_WINDOWPOSCHANGING:
        {
            MSWOnWindowPosChanging((void *)lParam);
            break;
        }

    case WM_RBUTTONDOWN:
        {
            int x = (DIMENSION_TYPE) LOWORD(lParam);
            int y = (DIMENSION_TYPE) HIWORD(lParam);
            MSWOnRButtonDown(x, y, wParam);
            break;
        }
    case WM_RBUTTONUP:
        {
            int x = (DIMENSION_TYPE) LOWORD(lParam);
            int y = (DIMENSION_TYPE) HIWORD(lParam);
            MSWOnRButtonUp(x, y, wParam);
            break;
        }
    case WM_RBUTTONDBLCLK:
        {
            int x = (DIMENSION_TYPE) LOWORD(lParam);
            int y = (DIMENSION_TYPE) HIWORD(lParam);
            MSWOnRButtonDClick(x, y, wParam);
            break;
        }
    case WM_MBUTTONDOWN:
        {
            int x = (DIMENSION_TYPE) LOWORD(lParam);
            int y = (DIMENSION_TYPE) HIWORD(lParam);
            MSWOnMButtonDown(x, y, wParam);
            break;
        }
    case WM_MBUTTONUP:
        {
            int x = (DIMENSION_TYPE) LOWORD(lParam);
            int y = (DIMENSION_TYPE) HIWORD(lParam);
            MSWOnMButtonUp(x, y, wParam);
            break;
        }
    case WM_MBUTTONDBLCLK:
        {
            int x = (DIMENSION_TYPE) LOWORD(lParam);
            int y = (DIMENSION_TYPE) HIWORD(lParam);
            MSWOnMButtonDClick(x, y, wParam);
            break;
        }
    case WM_LBUTTONDOWN:
        {
            int x = (DIMENSION_TYPE) LOWORD(lParam);
            int y = (DIMENSION_TYPE) HIWORD(lParam);
            MSWOnLButtonDown(x, y, wParam);
            break;
        }
    case WM_LBUTTONUP:
        {
            int x = (DIMENSION_TYPE) LOWORD(lParam);
            int y = (DIMENSION_TYPE) HIWORD(lParam);
            MSWOnLButtonUp(x, y, wParam);
            break;
        }
    case WM_LBUTTONDBLCLK:
        {
            int x = (DIMENSION_TYPE) LOWORD(lParam);
            int y = (DIMENSION_TYPE) HIWORD(lParam);
            MSWOnLButtonDClick(x, y, wParam);
            break;
        }
    case WM_MOUSEMOVE:
        {
            int x = (DIMENSION_TYPE) LOWORD(lParam);
            int y = (DIMENSION_TYPE) HIWORD(lParam);
            MSWOnMouseMove(x, y, wParam);
            break;
        }
    case MM_JOY1BUTTONDOWN:
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            MSWOnJoyDown(wxJOYSTICK1, x, y, wParam);
            break;
        }
    case MM_JOY2BUTTONDOWN:
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            MSWOnJoyDown(wxJOYSTICK2, x, y, wParam);
            break;
        }
    case MM_JOY1BUTTONUP:
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            MSWOnJoyUp(wxJOYSTICK1, x, y, wParam);
            break;
        }
    case MM_JOY2BUTTONUP:
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            MSWOnJoyUp(wxJOYSTICK2, x, y, wParam);
            break;
        }
    case MM_JOY1MOVE:
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            MSWOnJoyMove(wxJOYSTICK1, x, y, wParam);
            break;
        }
    case MM_JOY2MOVE:
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            MSWOnJoyMove(wxJOYSTICK2, x, y, wParam);
            break;
        }
    case MM_JOY1ZMOVE:
        {
            int z = LOWORD(lParam);
            MSWOnJoyZMove(wxJOYSTICK1, z, wParam);
            break;
        }
    case MM_JOY2ZMOVE:
        {
            int z = LOWORD(lParam);
            MSWOnJoyZMove(wxJOYSTICK2, z, wParam);
            break;
        }
    case WM_DESTROY:
        {
            if (MSWOnDestroy())
                return 0;
            else return MSWDefWindowProc(message, wParam, lParam );
            break;
        }
    case WM_SYSCOMMAND:
        {
            return MSWOnSysCommand(wParam, lParam);
            break;
        }
    case WM_COMMAND:
        {
#ifdef __WIN32__
            WORD id = LOWORD(wParam);
            HWND hwnd = (HWND)lParam;
            WORD cmd = HIWORD(wParam);
#else
            WORD id = (WORD)wParam;
            HWND hwnd = (HWND)LOWORD(lParam) ;
            WORD cmd = HIWORD(lParam);
#endif
            if (!MSWOnCommand(id, cmd, (WXHWND) hwnd))
                return MSWDefWindowProc(message, wParam, lParam );
            break;
        }
#if defined(__WIN95__)
    case WM_NOTIFY:
        {
            if (!MSWOnNotify(wParam, lParam))
                return MSWDefWindowProc(message, wParam, lParam );
            break;
        }
#endif
    case WM_MENUSELECT:
        {
#ifdef __WIN32__
            WORD flags = HIWORD(wParam);
            HMENU sysmenu = (HMENU)lParam;
#else
            WORD flags = LOWORD(lParam);
            HMENU sysmenu = (HMENU)HIWORD(lParam);
#endif
            MSWOnMenuHighlight((WORD)wParam, flags, (WXHMENU) sysmenu);
            break;
        }
    case WM_INITMENUPOPUP:
        {
            MSWOnInitMenuPopup((WXHMENU) (HMENU)wParam, (int)LOWORD(lParam), (HIWORD(lParam) != 0));
            break;
        }
    case WM_DRAWITEM:
        {
            return MSWOnDrawItem((int)wParam, (WXDRAWITEMSTRUCT *)lParam);
            break;
        }
    case WM_MEASUREITEM:
        {
            return MSWOnMeasureItem((int)wParam, (WXMEASUREITEMSTRUCT *)lParam);
            break;
        }

    case WM_KEYDOWN:
        // we consider these message "not interesting"
        if ( wParam == VK_SHIFT || wParam == VK_CONTROL )
            return Default();

        // Avoid duplicate messages to OnChar
        if ( (wParam != VK_ESCAPE) && (wParam != VK_SPACE) &&
            (wParam != VK_RETURN) && (wParam != VK_BACK) &&
            (wParam != VK_TAB) )
        {
            MSWOnChar((WORD)wParam, lParam);
            if ( ::GetKeyState(VK_CONTROL) & 0x100 )
                return Default();
        }
        else if ( ::GetKeyState(VK_CONTROL) & 0x100 )
            MSWOnChar((WORD)wParam, lParam);
        else
            return Default();
        break;

    case WM_CHAR: // Always an ASCII character
        {
            MSWOnChar((WORD)wParam, lParam, TRUE);
            break;
        }

    case WM_HSCROLL:
        {
#ifdef __WIN32__
            WORD code = LOWORD(wParam);
            WORD pos = HIWORD(wParam);
            HWND control = (HWND)lParam;
#else
            WORD code = (WORD)wParam;
            WORD pos = LOWORD(lParam);
            HWND control = (HWND)HIWORD(lParam);
#endif
            MSWOnHScroll(code, pos, (WXHWND) control);
            break;
        }
    case WM_VSCROLL:
        {
#ifdef __WIN32__
            WORD code = LOWORD(wParam);
            WORD pos = HIWORD(wParam);
            HWND control = (HWND)lParam;
#else
            WORD code = (WORD)wParam;
            WORD pos = LOWORD(lParam);
            HWND control = (HWND)HIWORD(lParam);
#endif
            MSWOnVScroll(code, pos, (WXHWND) control);
            break;
        }
#ifdef __WIN32__
    case WM_CTLCOLORBTN:
        {
            int nCtlColor = CTLCOLOR_BTN;
            HWND control = (HWND)lParam;
            HDC pDC = (HDC)wParam;
            return (DWORD)MSWOnCtlColor((WXHDC) pDC, (WXHWND) control, nCtlColor,
                message, wParam, lParam);
            break;
        }
    case WM_CTLCOLORDLG:
        {
            int nCtlColor = CTLCOLOR_DLG;
            HWND control = (HWND)lParam;
            HDC pDC = (HDC)wParam;
            return (DWORD)MSWOnCtlColor((WXHDC) pDC, (WXHWND) control, nCtlColor,
                message, wParam, lParam);\
                break;
        }
    case WM_CTLCOLORLISTBOX:
        {
            int nCtlColor = CTLCOLOR_LISTBOX;
            HWND control = (HWND)lParam;
            HDC pDC = (HDC)wParam;
            return (DWORD)MSWOnCtlColor((WXHDC) pDC, (WXHWND) control, nCtlColor,
                message, wParam, lParam);
            break;
        }
    case WM_CTLCOLORMSGBOX:
        {
            int nCtlColor = CTLCOLOR_MSGBOX;
            HWND control = (HWND)lParam;
            HDC pDC = (HDC)wParam;
            return (DWORD)MSWOnCtlColor((WXHDC) pDC, (WXHWND) control, nCtlColor,
                message, wParam, lParam);
            break;
        }
    case WM_CTLCOLORSCROLLBAR:
        {
            int nCtlColor = CTLCOLOR_SCROLLBAR;
            HWND control = (HWND)lParam;
            HDC pDC = (HDC)wParam;
            return (DWORD)MSWOnCtlColor((WXHDC) pDC, (WXHWND) control, nCtlColor,
                message, wParam, lParam);
            break;
        }
    case WM_CTLCOLORSTATIC:
        {
            int nCtlColor = CTLCOLOR_STATIC;
            HWND control = (HWND)lParam;
            HDC pDC = (HDC)wParam;
            return (DWORD)MSWOnCtlColor((WXHDC) pDC, (WXHWND) control, nCtlColor,
                message, wParam, lParam);
            break;
        }
    case WM_CTLCOLOREDIT:
        {
            int nCtlColor = CTLCOLOR_EDIT;
            HWND control = (HWND)lParam;
            HDC pDC = (HDC)wParam;
            return (DWORD)MSWOnCtlColor((WXHDC) pDC, (WXHWND) control, nCtlColor,
                message, wParam, lParam);
            break;
        }
#else
    case WM_CTLCOLOR:
        {
            HWND control = (HWND)LOWORD(lParam);
            int nCtlColor = (int)HIWORD(lParam);
            HDC pDC = (HDC)wParam;
            return (DWORD)MSWOnCtlColor((WXHDC) pDC, (WXHWND) control, nCtlColor,
                message, wParam, lParam);
            break;
        }
#endif
    case WM_SYSCOLORCHANGE:
        {
            // Return value of 0 means, we processed it.
            if (MSWOnColorChange((WXHWND) hWnd, message, wParam, lParam) == 0)
                return 0;
            else
                return MSWDefWindowProc(message, wParam, lParam );
            break;
        }
    case WM_PALETTECHANGED:
        {
            return MSWOnPaletteChanged((WXHWND) (HWND) wParam);
            break;
        }
    case WM_QUERYNEWPALETTE:
        {
            return MSWOnQueryNewPalette();
            break;
        }
    case WM_ERASEBKGND:
        {
            // Prevents flicker when dragging
            if (IsIconic(hWnd)) return 1;

            if (!MSWOnEraseBkgnd((WXHDC) (HDC)wParam))
                return 0; // Default(); MSWDefWindowProc(message, wParam, lParam );
            else return 1;
            break;
        }
    case WM_MDIACTIVATE:
        {
#ifdef __WIN32__
            HWND hWndActivate = GET_WM_MDIACTIVATE_HWNDACTIVATE(wParam,lParam);
            HWND hWndDeactivate = GET_WM_MDIACTIVATE_HWNDDEACT(wParam,lParam);
            BOOL activate = GET_WM_MDIACTIVATE_FACTIVATE(hWnd,wParam,lParam);
            return MSWOnMDIActivate((long) activate, (WXHWND) hWndActivate, (WXHWND) hWndDeactivate);
#else
            return MSWOnMDIActivate((BOOL)wParam, (HWND)LOWORD(lParam),
                (HWND)HIWORD(lParam));
#endif
        }
    case WM_DROPFILES:
        {
            MSWOnDropFiles(wParam);
            break;
        }
    case WM_INITDIALOG:
        {
            return 0; // MSWOnInitDialog((WXHWND)(HWND)wParam);
            break;
        }
    case WM_QUERYENDSESSION:
        {
            // Same as WM_CLOSE, but inverted results. Thx Microsoft :-)
            //            return MSWOnClose();

            return MSWOnQueryEndSession(lParam);
            break;
        }
    case WM_ENDSESSION:
        {
            // Same as WM_CLOSE, but inverted results. Thx Microsoft :-)
            MSWOnEndSession((wParam != 0), lParam);
            return 0L;
            break;
        }
    case WM_CLOSE:
        {
            if (MSWOnClose())
                return 0L;
            else
                return 1L;
            break;
        }

    case WM_GETMINMAXINFO:
        {
            MINMAXINFO *info = (MINMAXINFO *)lParam;
            if (m_minSizeX != -1)
                info->ptMinTrackSize.x = (int)m_minSizeX;
            if (m_minSizeY != -1)
                info->ptMinTrackSize.y = (int)m_minSizeY;
            if (m_maxSizeX != -1)
                info->ptMaxTrackSize.x = (int)m_maxSizeX;
            if (m_maxSizeY != -1)
                info->ptMaxTrackSize.y = (int)m_maxSizeY;
            return MSWDefWindowProc(message, wParam, lParam );
            break;
        }

    case WM_GETDLGCODE:
        return MSWGetDlgCode();

    default:
        return MSWDefWindowProc(message, wParam, lParam );
    }
    return 0; // Success: we processed this command.
}

// Dialog window proc
LONG APIENTRY _EXPORT
wxDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return 0;
}

wxList *wxWinHandleList = NULL;
wxWindow *wxFindWinFromHandle(WXHWND hWnd)
{
    wxNode *node = wxWinHandleList->Find((long)hWnd);
    if (!node)
        return NULL;
    return (wxWindow *)node->Data();
}

void wxAssociateWinWithHandle(HWND hWnd, wxWindow *win)
{
    // adding NULL hWnd is (first) surely a result of an error and
    // (secondly) breaks menu command processing
    wxCHECK_RET( hWnd != NULL, "attempt to add a NULL hWnd to window list" );

    if ( !wxWinHandleList->Find((long)hWnd) )
        wxWinHandleList->Append((long)hWnd, win);
}

void wxRemoveHandleAssociation(wxWindow *win)
{
    wxWinHandleList->DeleteObject(win);
}

// Default destroyer - override if you destroy it in some other way
// (e.g. with MDI child windows)
void wxWindow::MSWDestroyWindow(void)
{
}

void wxWindow::MSWCreate(int id, wxWindow *parent, const char *wclass, wxWindow *wx_win, const char *title,
                         int x, int y, int width, int height,
                         WXDWORD style, const char *dialog_template, WXDWORD extendedStyle)
{
    bool is_dialog = (dialog_template != NULL);
    int x1 = CW_USEDEFAULT;
    int y1 = 0;
    int width1 = CW_USEDEFAULT;
    int height1 = 100;

    // Find parent's size, if it exists, to set up a possible default
    // panel size the size of the parent window
    RECT parent_rect;
    if (parent)
    {
        // Was GetWindowRect: JACS 5/5/95
        ::GetClientRect((HWND) parent->GetHWND(), &parent_rect);

        width1 = parent_rect.right - parent_rect.left;
        height1 = parent_rect.bottom - parent_rect.top;
    }

    if (x > -1) x1 = x;
    if (y > -1) y1 = y;
    if (width > -1) width1 = width;
    if (height > -1) height1 = height;

    HWND hParent = NULL;
    if (parent)
        hParent = (HWND) parent->GetHWND();

    wxWndHook = this;

    if (is_dialog)
    {
        // MakeProcInstance doesn't seem to be needed in C7. Is it needed for
        // other compilers???
        // VZ: it's always needed for Win16 and never for Win32
#ifdef __WIN32__
        m_hWnd = (WXHWND) ::CreateDialog(wxGetInstance(), dialog_template, hParent,
            (DLGPROC)wxDlgProc);
#else
        DLGPROC dlgproc = (DLGPROC)MakeProcInstance((DLGPROC)wxWndProc, wxGetInstance());

        m_hWnd = (WXHWND) ::CreateDialog(wxGetInstance(), dialog_template, hParent,
            (DLGPROC)dlgproc);
#endif

        if (m_hWnd == 0)
            MessageBox(NULL, "Can't find dummy dialog template!\nCheck resource include path for finding wx.rc.",
            "wxWindows Error", MB_ICONEXCLAMATION | MB_OK);
        else MoveWindow((HWND) m_hWnd, x1, y1, width1, height1, FALSE);
    }
    else
    {
        int controlId = 0;
        if (style & WS_CHILD)
            controlId = id;
        if (!title)
            title = "";

        m_hWnd = (WXHWND)CreateWindowEx(extendedStyle, wclass,
            title,
            style,
            x1, y1,
            width1, height1,
            hParent, (HMENU)controlId, wxGetInstance(),
            NULL);

        if ( !m_hWnd ) {
            wxLogError("Can't create window of class %s!\n"
                "Possible Windows 3.x compatibility problem?", wclass);
        }
    }

    wxWndHook = NULL;
    wxWinHandleList->Append((long)m_hWnd, this);

#if WXDEBUG > 1
    wxDebugMsg("wxWindow::MSWCreate %d\n", m_hWnd);
#endif
}

void wxWindow::MSWOnCreate(WXLPCREATESTRUCT WXUNUSED(cs))
{
}

bool wxWindow::MSWOnClose(void)
{
#if WXDEBUG > 1
    wxDebugMsg("wxWindow::MSWOnClose %d\n", handle);
#endif
    return FALSE;
}

// Some compilers don't define this
#ifndef ENDSESSION_LOGOFF
#define ENDSESSION_LOGOFF    0x80000000
#endif

// Return TRUE to end session, FALSE to veto end session.
bool wxWindow::MSWOnQueryEndSession(long logOff)
{
    wxCloseEvent event(wxEVT_QUERY_END_SESSION, -1);
    event.SetEventObject(wxTheApp);
    event.SetCanVeto(TRUE);
    event.SetLoggingOff( (logOff == ENDSESSION_LOGOFF) );
    if ((this == wxTheApp->GetTopWindow()) && // Only send once
        wxTheApp->ProcessEvent(event) && event.GetVeto())
    {
        return FALSE; // Veto!
    }
    else
    {
        return TRUE; // Don't veto
    }
}

bool wxWindow::MSWOnEndSession(bool endSession, long logOff)
{
    wxCloseEvent event(wxEVT_END_SESSION, -1);
    event.SetEventObject(wxTheApp);
    event.SetCanVeto(FALSE);
    event.SetLoggingOff( (logOff == ENDSESSION_LOGOFF) );
    if (endSession &&                         // No need to send if the session isn't ending
        (this == wxTheApp->GetTopWindow()) && // Only send once
        wxTheApp->ProcessEvent(event))
    {
    }
    return TRUE;
}

bool wxWindow::MSWOnDestroy(void)
{
#if WXDEBUG > 1
    wxDebugMsg("wxWindow::MSWOnDestroy %d\n", handle);
#endif
    // delete our drop target if we've got one
#if wxUSE_DRAG_AND_DROP
    if ( m_pDropTarget != NULL ) {
        m_pDropTarget->Revoke(m_hWnd);

        delete m_pDropTarget;
        m_pDropTarget = NULL;
    }
#endif

    return TRUE;
}

// Deal with child commands from buttons etc.

bool wxWindow::MSWOnNotify(WXWPARAM wParam, WXLPARAM lParam)
{
#if defined(__WIN95__)
    // Find a child window to send the notification to, e.g. a toolbar.
    // There's a problem here. NMHDR::hwndFrom doesn't give us the
    // handle of the toolbar; it's probably the handle of the tooltip
    // window (anyway, it's parent is also the toolbar's parent).
    // So, since we don't know which hWnd or wxWindow originated the
    // WM_NOTIFY, we'll need to go through all the children of this window
    // trying out MSWNotify.
    // This won't work now, though, because any number of controls
    // could respond to the same generic messages :-(

    /* This doesn't work for toolbars, but try for other controls first.
    */
    NMHDR *hdr = (NMHDR *)lParam;
    HWND hWnd = (HWND)hdr->hwndFrom;
    wxWindow *win = wxFindWinFromHandle((WXHWND) hWnd);

    if ( win )
        return win->MSWNotify(wParam, lParam);
    else
    {
        // Rely on MSWNotify to check whether the message
        // belongs to the window or not
        wxNode *node = GetChildren()->First();
        while (node)
        {
            wxWindow *child = (wxWindow *)node->Data();
            if ( child->MSWNotify(wParam, lParam) )
                return TRUE;
            node = node->Next();
        }
    }

    return FALSE;

#endif
    return FALSE;
}

void wxWindow::MSWOnMenuHighlight(WXWORD WXUNUSED(item), WXWORD WXUNUSED(flags), WXHMENU WXUNUSED(sysmenu))
{
#if WXDEBUG > 1
    wxDebugMsg("wxWindow::MSWOnMenuHighlight %d\n", handle);
#endif
}

void wxWindow::MSWOnInitMenuPopup(WXHMENU menu, int pos, bool isSystem)
{
}

bool wxWindow::MSWOnActivate(int state, bool WXUNUSED(minimized), WXHWND WXUNUSED(activate))
{
#if WXDEBUG > 1
    wxDebugMsg("wxWindow::MSWOnActivate %d\n", handle);
#endif

    wxActivateEvent event(wxEVT_ACTIVATE, ((state == WA_ACTIVE) || (state == WA_CLICKACTIVE)),
        m_windowId);
    event.SetEventObject(this);
    GetEventHandler()->ProcessEvent(event);
    return 0;
}

bool wxWindow::MSWOnSetFocus(WXHWND WXUNUSED(hwnd))
{
#if WXDEBUG > 1
    wxDebugMsg("wxWindow::MSWOnSetFocus %d\n", m_hWnd);
#endif
    // Deal with caret
    if (m_caretEnabled && (m_caretWidth > 0) && (m_caretHeight > 0))
    {
        ::CreateCaret((HWND) GetHWND(), NULL, m_caretWidth, m_caretHeight);
        if (m_caretShown)
            ::ShowCaret((HWND) GetHWND());
    }

    wxFocusEvent event(wxEVT_SET_FOCUS, m_windowId);
    event.SetEventObject(this);
    if (!GetEventHandler()->ProcessEvent(event))
        Default();
    return TRUE;
}

bool wxWindow::MSWOnKillFocus(WXHWND WXUNUSED(hwnd))
{
#if WXDEBUG > 1
    wxDebugMsg("wxWindow::MSWOnKillFocus %d\n", m_hWnd);
#endif
    // Deal with caret
    if (m_caretEnabled)
    {
        ::DestroyCaret();
    }

    wxFocusEvent event(wxEVT_KILL_FOCUS, m_windowId);
    event.SetEventObject(this);
    if (!GetEventHandler()->ProcessEvent(event))
        Default();
    return TRUE;
}

void wxWindow::MSWOnDropFiles(WXWPARAM wParam)
{
#if WXDEBUG > 1
    wxDebugMsg("wxWindow::MSWOnDropFiles %d\n", m_hWnd);
#endif

    HDROP hFilesInfo = (HDROP) wParam;
    POINT dropPoint;
    DragQueryPoint(hFilesInfo, (LPPOINT) &dropPoint);

    // Get the total number of files dropped
    WORD gwFilesDropped = (WORD)DragQueryFile ((HDROP)hFilesInfo,
        (UINT)-1,
        (LPSTR)0,
        (UINT)0);

    wxString *files = new wxString[gwFilesDropped];
    int wIndex;
    for (wIndex=0; wIndex < (int)gwFilesDropped; wIndex++)
    {
        DragQueryFile (hFilesInfo, wIndex, (LPSTR) wxBuffer, 1000);
        files[wIndex] = wxBuffer;
    }
    DragFinish (hFilesInfo);

    wxDropFilesEvent event(wxEVT_DROP_FILES, gwFilesDropped, files);
    event.m_eventObject = this;
    event.m_pos.x = dropPoint.x; event.m_pos.x = dropPoint.y;

    if (!GetEventHandler()->ProcessEvent(event))
        Default();

    delete[] files;
}

bool wxWindow::MSWOnDrawItem(int id, WXDRAWITEMSTRUCT *itemStruct)
{
#if wxUSE_OWNER_DRAWN
    if ( id == 0 ) {    // is it a menu item?
        DRAWITEMSTRUCT *pDrawStruct = (DRAWITEMSTRUCT *)itemStruct;
        wxMenuItem *pMenuItem = (wxMenuItem *)(pDrawStruct->itemData);
        wxCHECK( pMenuItem->IsKindOf(CLASSINFO(wxMenuItem)), FALSE );

        // prepare to call OnDrawItem()
        wxDC dc;
        dc.SetHDC((WXHDC)pDrawStruct->hDC, FALSE);
        wxRect rect(pDrawStruct->rcItem.left, pDrawStruct->rcItem.top,
            pDrawStruct->rcItem.right - pDrawStruct->rcItem.left,
            pDrawStruct->rcItem.bottom - pDrawStruct->rcItem.top);
        return pMenuItem->OnDrawItem(
            dc, rect,
            (wxOwnerDrawn::wxODAction)pDrawStruct->itemAction,
            (wxOwnerDrawn::wxODStatus)pDrawStruct->itemState
            );
    }
#endif  // owner-drawn menus

    wxWindow *item = FindItem(id);
#if wxUSE_DYNAMIC_CLASSES
    if (item && item->IsKindOf(CLASSINFO(wxControl)))
    {
        return ((wxControl *)item)->MSWOnDraw(itemStruct);
    }
    else
#endif
        return FALSE;
}

bool wxWindow::MSWOnMeasureItem(int id, WXMEASUREITEMSTRUCT *itemStruct)
{
#if wxUSE_OWNER_DRAWN
    if ( id == 0 ) {    // is it a menu item?
        MEASUREITEMSTRUCT *pMeasureStruct = (MEASUREITEMSTRUCT *)itemStruct;
        wxMenuItem *pMenuItem = (wxMenuItem *)(pMeasureStruct->itemData);
        wxCHECK( pMenuItem->IsKindOf(CLASSINFO(wxMenuItem)), FALSE );

        return pMenuItem->OnMeasureItem(&pMeasureStruct->itemWidth,
            &pMeasureStruct->itemHeight);
    }
#endif  // owner-drawn menus

    wxWindow *item = FindItem(id);
#if wxUSE_DYNAMIC_CLASSES
    if (item && item->IsKindOf(CLASSINFO(wxControl)))
    {
        return ((wxControl *)item)->MSWOnMeasure(itemStruct);
    }
    else
#endif
        return FALSE;
}

WXHBRUSH wxWindow::MSWOnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
                                 WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
#if WXDEBUG > 1
    wxDebugMsg("wxWindow::MSWOnCtlColour %d\n", m_hWnd);
#endif
    if (nCtlColor == CTLCOLOR_DLG)
    {
        return OnCtlColor(pDC, pWnd, nCtlColor, message, wParam, lParam);
    }

    wxControl *item = (wxControl *)FindItemByHWND(pWnd, TRUE);

    WXHBRUSH hBrush = 0;

    if ( item )
        hBrush = item->OnCtlColor(pDC, pWnd, nCtlColor, message, wParam, lParam);

    // I think that even for dialogs, we may need to call DefWindowProc (?)
    // Or maybe just rely on the usual default behaviour.
    if ( !hBrush )
        hBrush = (WXHBRUSH) MSWDefWindowProc(message, wParam, lParam);

    return hBrush ;
}

// Define for each class of dialog and control
WXHBRUSH wxWindow::OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
                              WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
    return (WXHBRUSH) MSWDefWindowProc(message, wParam, lParam);
}

bool wxWindow::MSWOnColorChange(WXHWND hWnd, WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
    wxSysColourChangedEvent event;
    event.SetEventObject(this);

    // Check if app handles this.
    if (GetEventHandler()->ProcessEvent(event))
        return 0;

    // We didn't process it
    return 1;
}

long wxWindow::MSWOnPaletteChanged(WXHWND hWndPalChange)
{
    wxPaletteChangedEvent event(GetId());
    event.SetEventObject(this);
    event.SetChangedWindow(wxFindWinFromHandle(hWndPalChange));
    GetEventHandler()->ProcessEvent(event);
    return 0;
}

long wxWindow::MSWOnQueryNewPalette()
{
    wxQueryNewPaletteEvent event(GetId());
    event.SetEventObject(this);
    if (!GetEventHandler()->ProcessEvent(event) || !event.GetPaletteRealized())
    {
        return (long) FALSE;
    }
    else
        return (long) TRUE;
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

long wxWindow::MSWDefWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    if ( m_oldWndProc )
        return ::CallWindowProc(CASTWNDPROC m_oldWndProc, (HWND) GetHWND(), (UINT) nMsg, (WPARAM) wParam, (LPARAM) lParam);
    else
        return ::DefWindowProc((HWND) GetHWND(), nMsg, wParam, lParam);
}

long wxWindow::Default()
{
    // Ignore 'fake' events (perhaps generated as a result of a separate real event)
    if (m_lastMsg == 0)
        return 0;

#ifdef __WXDEBUG__
    wxLogTrace(wxTraceMessages, "Forwarding %s to DefWindowProc.",
        wxGetMessageName(m_lastMsg));
#endif // WXDEBUG

    return this->MSWDefWindowProc(m_lastMsg, m_lastWParam, m_lastLParam);
}

bool wxWindow::MSWProcessMessage(WXMSG* pMsg)
{
    if ( m_hWnd != 0 && (GetWindowStyleFlag() & wxTAB_TRAVERSAL) ) {
        // intercept dialog navigation keys
        MSG *msg = (MSG *)pMsg;
        bool bProcess = TRUE;
        if ( msg->message != WM_KEYDOWN )
            bProcess = FALSE;

        if ( (HIWORD(msg->lParam) & KF_ALTDOWN) == KF_ALTDOWN )
            bProcess = FALSE;

        bool bCtrlDown = (::GetKeyState(VK_CONTROL) & 0x100) != 0;

        // WM_GETDLGCODE: if the control wants it for itself, don't process it
        // (except for Ctrl-Tab combination which is always processed)
        LONG lDlgCode = 0;
        if ( bProcess && !bCtrlDown ) {
            lDlgCode = ::SendMessage(msg->hwnd, WM_GETDLGCODE, 0, 0);
        }

        bool bForward;
        if ( bProcess ) {
            switch ( msg->wParam ) {
            case VK_TAB:
                if ( lDlgCode & DLGC_WANTTAB )  // this is FALSE for Ctrl-Tab
                    bProcess = FALSE;
                else
                    bForward = !(::GetKeyState(VK_SHIFT) & 0x100);
                break;

            case VK_UP:
            case VK_LEFT:
                if ( (lDlgCode & DLGC_WANTARROWS) || bCtrlDown )
                    bProcess = FALSE;
                else
                    bForward = FALSE;
                break;

            case VK_DOWN:
            case VK_RIGHT:
                if ( (lDlgCode & DLGC_WANTARROWS) || bCtrlDown )
                    bProcess = FALSE;
                else
                    bForward = TRUE;
                break;

            default:
                bProcess = FALSE;
            }
        }

        if ( bProcess ) {
            wxNavigationKeyEvent event;
            event.SetDirection(bForward);
            event.SetWindowChange(bCtrlDown);
            event.SetEventObject(this);

            if ( GetEventHandler()->ProcessEvent(event) )
                return TRUE;
        }

        return ::IsDialogMessage((HWND)GetHWND(), msg) != 0;
    }

    return FALSE;
}

bool wxWindow::MSWTranslateMessage(WXMSG* pMsg)
{
    if (m_acceleratorTable.Ok() &&
        ::TranslateAccelerator((HWND) GetHWND(), (HACCEL) m_acceleratorTable.GetHACCEL(), (MSG *)pMsg))
        return TRUE;
    else
        return FALSE;
}

long wxWindow::MSWOnMDIActivate(long WXUNUSED(flag), WXHWND WXUNUSED(activate), WXHWND WXUNUSED(deactivate))
{
#if WXDEBUG > 1
    wxDebugMsg("wxWindow::MSWOnMDIActivate %d\n", m_hWnd);
#endif
    return 1;
}

void wxWindow::MSWDetachWindowMenu(void)
{
    if (m_hMenu)
    {
        int N = GetMenuItemCount((HMENU) m_hMenu);
        int i;
        for (i = 0; i < N; i++)
        {
            char buf[100];
            int chars = GetMenuString((HMENU) m_hMenu, i, buf, 100, MF_BYPOSITION);
            if ((chars > 0) && (strcmp(buf, "&Window") == 0))
            {
                RemoveMenu((HMENU) m_hMenu, i, MF_BYPOSITION);
                break;
            }
        }
    }
}

bool wxWindow::MSWOnPaint(void)
{
#ifdef __WIN32__
    HRGN hRegion = ::CreateRectRgn(0, 0, 0, 0); // Dummy call to get a handle
    ::GetUpdateRgn((HWND) GetHWND(), hRegion, FALSE);

    m_updateRegion = wxRegion((WXHRGN) hRegion);
#else
    RECT updateRect;
    ::GetUpdateRect((HWND) GetHWND(), & updateRect, FALSE);

    m_updateRegion = wxRegion(updateRect.left, updateRect.top,
        updateRect.right - updateRect.left, updateRect.bottom - updateRect.top);
#endif

    wxPaintEvent event(m_windowId);
    event.SetEventObject(this);
    if (!GetEventHandler()->ProcessEvent(event))
        Default();
    return TRUE;
}

void wxWindow::MSWOnSize(int w, int h, WXUINT WXUNUSED(flag))
{
    if (m_inOnSize)
        return;

#if WXDEBUG > 1
    wxDebugMsg("wxWindow::MSWOnSize %d\n", m_hWnd);
#endif
    if (!m_hWnd)
        return;

    m_inOnSize = TRUE;

    wxSizeEvent event(wxSize(w, h), m_windowId);
    event.SetEventObject(this);
    if (!GetEventHandler()->ProcessEvent(event))
        Default();

    m_inOnSize = FALSE;
}

void wxWindow::MSWOnWindowPosChanging(void *WXUNUSED(lpPos))
{
    Default();
}

// Deal with child commands from buttons etc.
bool wxWindow::MSWOnCommand(WXWORD id, WXWORD cmd, WXHWND control)
{
#if WXDEBUG > 1
    wxDebugMsg("wxWindow::MSWOnCommand\n");
#endif
    if (wxCurrentPopupMenu)
    {
        wxMenu *popupMenu = wxCurrentPopupMenu;
        wxCurrentPopupMenu = NULL;
        bool succ = popupMenu->MSWCommand(cmd, id);
        return succ;
    }
#if WXDEBUG > 1
    char buf[80];
    sprintf(buf, "Looking for item %d...\n", id);
    wxDebugMsg(buf);
#endif

    wxWindow *item = FindItem(id);
    if (item)
    {
        bool value = item->MSWCommand(cmd, id);
#if WXDEBUG > 1
        if (value)
            wxDebugMsg("MSWCommand succeeded\n");
        else
            wxDebugMsg("MSWCommand failed\n");
#endif
        return value;
    }
    else
    {
        wxWindow *win = wxFindWinFromHandle(control);
        if (win)
            return win->MSWCommand(cmd, id);
    }
    return FALSE;
}

long wxWindow::MSWOnSysCommand(WXWPARAM wParam, WXLPARAM lParam)
{
    switch (wParam)
    {
    case SC_MAXIMIZE:
        {
            wxMaximizeEvent event(m_windowId);
            event.SetEventObject(this);
            if (!GetEventHandler()->ProcessEvent(event))
                return Default();
            else
                return 0;
            break;
        }
    case SC_MINIMIZE:
        {
            wxIconizeEvent event(m_windowId);
            event.SetEventObject(this);
            if (!GetEventHandler()->ProcessEvent(event))
                return Default();
            else
                return 0;
            break;
        }
    default:
        return Default();
    }
    return 0;
}

void wxWindow::MSWOnLButtonDown(int x, int y, WXUINT flags)
{
    wxMouseEvent event(wxEVT_LEFT_DOWN);

    event.m_x = x; event.m_y = y;
    event.m_shiftDown = ((flags & MK_SHIFT) != 0);
    event.m_controlDown = ((flags & MK_CONTROL) != 0);
    event.m_leftDown = ((flags & MK_LBUTTON) != 0);
    event.m_middleDown = ((flags & MK_MBUTTON) != 0);
    event.m_rightDown = ((flags & MK_RBUTTON) != 0);
    event.SetTimestamp(wxApp::sm_lastMessageTime);
    event.m_eventObject = this;

    m_lastXPos = event.m_x; m_lastYPos = event.m_y; m_lastEvent = wxEVENT_TYPE_LEFT_DOWN;

    if (!GetEventHandler()->ProcessEvent(event))
        Default();
}

void wxWindow::MSWOnLButtonUp(int x, int y, WXUINT flags)
{
    wxMouseEvent event(wxEVT_LEFT_UP);

    event.m_x = x; event.m_y = y;
    event.m_shiftDown = ((flags & MK_SHIFT) != 0);
    event.m_controlDown = ((flags & MK_CONTROL) != 0);
    event.m_leftDown = ((flags & MK_LBUTTON) != 0);
    event.m_middleDown = ((flags & MK_MBUTTON) != 0);
    event.m_rightDown = ((flags & MK_RBUTTON) != 0);
    event.SetTimestamp(wxApp::sm_lastMessageTime);
    event.m_eventObject = this;

    m_lastXPos = event.m_x; m_lastYPos = event.m_y; m_lastEvent = wxEVT_LEFT_UP;

    if (!GetEventHandler()->ProcessEvent(event))
        Default();
}

void wxWindow::MSWOnLButtonDClick(int x, int y, WXUINT flags)
{
    wxMouseEvent event(wxEVT_LEFT_DCLICK);

    event.m_x = x; event.m_y = y;
    event.m_shiftDown = ((flags & MK_SHIFT) != 0);
    event.m_controlDown = ((flags & MK_CONTROL) != 0);
    event.m_leftDown = ((flags & MK_LBUTTON) != 0);
    event.m_middleDown = ((flags & MK_MBUTTON) != 0);
    event.m_rightDown = ((flags & MK_RBUTTON) != 0);
    event.SetTimestamp(wxApp::sm_lastMessageTime);
    event.m_eventObject = this;

    m_lastXPos = event.m_x; m_lastYPos = event.m_y; m_lastEvent = wxEVT_LEFT_DCLICK;

    if (!GetEventHandler()->ProcessEvent(event))
        Default();
}

void wxWindow::MSWOnMButtonDown(int x, int y, WXUINT flags)
{
    wxMouseEvent event(wxEVT_MIDDLE_DOWN);

    event.m_x = x; event.m_y = y;
    event.m_shiftDown = ((flags & MK_SHIFT) != 0);
    event.m_controlDown = ((flags & MK_CONTROL) != 0);
    event.m_leftDown = ((flags & MK_LBUTTON) != 0);
    event.m_middleDown = ((flags & MK_MBUTTON) != 0);
    event.m_rightDown = ((flags & MK_RBUTTON) != 0);
    event.SetTimestamp(wxApp::sm_lastMessageTime);
    event.m_eventObject = this;

    m_lastXPos = event.m_x; m_lastYPos = event.m_y; m_lastEvent = wxEVT_MIDDLE_DOWN;

    if (!GetEventHandler()->ProcessEvent(event))
        Default();
}

void wxWindow::MSWOnMButtonUp(int x, int y, WXUINT flags)
{
    //wxDebugMsg("MButtonUp\n") ;
    wxMouseEvent event(wxEVT_MIDDLE_UP);

    event.m_x = x; event.m_y = y;
    event.m_shiftDown = ((flags & MK_SHIFT) != 0);
    event.m_controlDown = ((flags & MK_CONTROL) != 0);
    event.m_leftDown = ((flags & MK_LBUTTON) != 0);
    event.m_middleDown = ((flags & MK_MBUTTON) != 0);
    event.m_rightDown = ((flags & MK_RBUTTON) != 0);
    event.SetTimestamp(wxApp::sm_lastMessageTime);
    event.m_eventObject = this;

    m_lastXPos = event.m_x; m_lastYPos = event.m_y; m_lastEvent = wxEVT_MIDDLE_UP;

    if (!GetEventHandler()->ProcessEvent(event))
        Default();
}

void wxWindow::MSWOnMButtonDClick(int x, int y, WXUINT flags)
{
    wxMouseEvent event(wxEVT_MIDDLE_DCLICK);

    event.m_x = x; event.m_y = y;
    event.m_shiftDown = ((flags & MK_SHIFT) != 0);
    event.m_controlDown = ((flags & MK_CONTROL) != 0);
    event.m_leftDown = ((flags & MK_LBUTTON) != 0);
    event.m_middleDown = ((flags & MK_MBUTTON) != 0);
    event.m_rightDown = ((flags & MK_RBUTTON) != 0);
    event.SetTimestamp(wxApp::sm_lastMessageTime);
    event.m_eventObject = this;

    m_lastXPos = event.m_x; m_lastYPos = event.m_y; m_lastEvent = wxEVT_MIDDLE_DCLICK;

    if (!GetEventHandler()->ProcessEvent(event))
        Default();
}

void wxWindow::MSWOnRButtonDown(int x, int y, WXUINT flags)
{
    wxMouseEvent event(wxEVT_RIGHT_DOWN);

    event.m_x = x; event.m_y = y;
    event.m_shiftDown = ((flags & MK_SHIFT) != 0);
    event.m_controlDown = ((flags & MK_CONTROL) != 0);
    event.m_leftDown = ((flags & MK_LBUTTON) != 0);
    event.m_middleDown = ((flags & MK_MBUTTON) != 0);
    event.m_rightDown = ((flags & MK_RBUTTON) != 0);
    event.SetTimestamp(wxApp::sm_lastMessageTime);
    event.m_eventObject = this;

    m_lastXPos = event.m_x; m_lastYPos = event.m_y; m_lastEvent = wxEVT_RIGHT_DOWN;

    if (!GetEventHandler()->ProcessEvent(event))
        Default();
}

void wxWindow::MSWOnRButtonUp(int x, int y, WXUINT flags)
{
    wxMouseEvent event(wxEVT_RIGHT_UP);

    event.m_x = x; event.m_y = y;
    event.m_shiftDown = ((flags & MK_SHIFT) != 0);
    event.m_controlDown = ((flags & MK_CONTROL) != 0);
    event.m_leftDown = ((flags & MK_LBUTTON) != 0);
    event.m_middleDown = ((flags & MK_MBUTTON) != 0);
    event.m_rightDown = ((flags & MK_RBUTTON) != 0);
    event.m_eventObject = this;
    event.SetTimestamp(wxApp::sm_lastMessageTime);

    m_lastXPos = event.m_x; m_lastYPos = event.m_y; m_lastEvent = wxEVT_RIGHT_UP;

    if (!GetEventHandler()->ProcessEvent(event))
        Default();
}

void wxWindow::MSWOnRButtonDClick(int x, int y, WXUINT flags)
{
    wxMouseEvent event(wxEVT_RIGHT_DCLICK);

    event.m_x = x; event.m_y = y;
    event.m_shiftDown = ((flags & MK_SHIFT) != 0);
    event.m_controlDown = ((flags & MK_CONTROL) != 0);
    event.m_leftDown = ((flags & MK_LBUTTON) != 0);
    event.m_middleDown = ((flags & MK_MBUTTON) != 0);
    event.m_rightDown = ((flags & MK_RBUTTON) != 0);
    event.SetTimestamp(wxApp::sm_lastMessageTime);
    event.m_eventObject = this;

    m_lastXPos = event.m_x; m_lastYPos = event.m_y; m_lastEvent = wxEVT_RIGHT_DCLICK;

    if (!GetEventHandler()->ProcessEvent(event))
        Default();
}

void wxWindow::MSWOnMouseMove(int x, int y, WXUINT flags)
{
    // 'normal' move event...
    // Set cursor, but only if we're not in 'busy' mode

    // Trouble with this is that it sets the cursor for controls too :-(
    if (m_windowCursor.Ok() && !wxIsBusy())
        ::SetCursor((HCURSOR) m_windowCursor.GetHCURSOR());

    if (!m_mouseInWindow)
    {
        // Generate an ENTER event
        m_mouseInWindow = TRUE;
        MSWOnMouseEnter(x, y, flags);
    }

    wxMouseEvent event(wxEVT_MOTION);

    event.m_x = x; event.m_y = y;
    event.m_shiftDown = ((flags & MK_SHIFT) != 0);
    event.m_controlDown = ((flags & MK_CONTROL) != 0);
    event.m_leftDown = ((flags & MK_LBUTTON) != 0);
    event.m_middleDown = ((flags & MK_MBUTTON) != 0);
    event.m_rightDown = ((flags & MK_RBUTTON) != 0);
    event.SetTimestamp(wxApp::sm_lastMessageTime);
    event.m_eventObject = this;

    // Window gets a click down message followed by a mouse move
    // message even if position isn't changed!  We want to discard
    // the trailing move event if x and y are the same.
    if ((m_lastEvent == wxEVT_RIGHT_DOWN || m_lastEvent == wxEVT_LEFT_DOWN ||
        m_lastEvent == wxEVT_MIDDLE_DOWN) &&
        (m_lastXPos == event.m_x && m_lastYPos == event.m_y))
    {
        m_lastXPos = event.m_x; m_lastYPos = event.m_y;
        m_lastEvent = wxEVT_MOTION;
        return;
    }

    m_lastEvent = wxEVT_MOTION;
    m_lastXPos = event.m_x; m_lastYPos = event.m_y;

    if (!GetEventHandler()->ProcessEvent(event))
        Default();
}

void wxWindow::MSWOnMouseEnter(int x, int y, WXUINT flags)
{
    wxMouseEvent event(wxEVT_ENTER_WINDOW);

    event.m_x = x; event.m_y = y;
    event.m_shiftDown = ((flags & MK_SHIFT) != 0);
    event.m_controlDown = ((flags & MK_CONTROL) != 0);
    event.m_leftDown = ((flags & MK_LBUTTON) != 0);
    event.m_middleDown = ((flags & MK_MBUTTON) != 0);
    event.m_rightDown = ((flags & MK_RBUTTON) != 0);
    event.SetTimestamp(wxApp::sm_lastMessageTime);
    event.m_eventObject = this;

    m_lastEvent = wxEVT_ENTER_WINDOW;
    m_lastXPos = event.m_x; m_lastYPos = event.m_y;
    // No message - ensure we don't try to call the default behaviour accidentally.
    m_lastMsg = 0;
    GetEventHandler()->ProcessEvent(event);
}

void wxWindow::MSWOnMouseLeave(int x, int y, WXUINT flags)
{
    wxMouseEvent event(wxEVT_LEAVE_WINDOW);

    event.m_x = x; event.m_y = y;
    event.m_shiftDown = ((flags & MK_SHIFT) != 0);
    event.m_controlDown = ((flags & MK_CONTROL) != 0);
    event.m_leftDown = ((flags & MK_LBUTTON) != 0);
    event.m_middleDown = ((flags & MK_MBUTTON) != 0);
    event.m_rightDown = ((flags & MK_RBUTTON) != 0);
    event.SetTimestamp(wxApp::sm_lastMessageTime);
    event.m_eventObject = this;

    m_lastEvent = wxEVT_LEAVE_WINDOW;
    m_lastXPos = event.m_x; m_lastYPos = event.m_y;
    // No message - ensure we don't try to call the default behaviour accidentally.
    m_lastMsg = 0;
    GetEventHandler()->ProcessEvent(event);
}

void wxWindow::MSWOnChar(WXWORD wParam, WXLPARAM lParam, bool isASCII)
{
    int id;
    bool tempControlDown = FALSE;
    if (isASCII)
    {
        // If 1 -> 26, translate to CTRL plus a letter.
        id = wParam;
        if ((id > 0) && (id < 27))
        {
            switch (id)
            {
            case 13:
                {
                    id = WXK_RETURN;
                    break;
                }
            case 8:
                {
                    id = WXK_BACK;
                    break;
                }
            case 9:
                {
                    id = WXK_TAB;
                    break;
                }
            default:
                {
                    tempControlDown = TRUE;
                    id = id + 96;
                }
            }
        }
    }
    else if ((id = wxCharCodeMSWToWX(wParam)) == 0) {
        // it's ASCII and will be processed here only when called from
        // WM_CHAR (i.e. when isASCII = TRUE)
        id = -1;
    }

    if (id != -1)
    {
        wxKeyEvent event(wxEVT_CHAR);
        event.m_shiftDown = (::GetKeyState(VK_SHIFT)&0x100?TRUE:FALSE);
        event.m_controlDown = (::GetKeyState(VK_CONTROL)&0x100?TRUE:FALSE);
        if ((HIWORD(lParam) & KF_ALTDOWN) == KF_ALTDOWN)
            event.m_altDown = TRUE;

        event.m_eventObject = this;
        event.m_keyCode = id;
        event.SetTimestamp(wxApp::sm_lastMessageTime);

        POINT pt ;
        GetCursorPos(&pt) ;
        RECT rect ;
        GetWindowRect((HWND) GetHWND(),&rect) ;
        pt.x -= rect.left ;
        pt.y -= rect.top ;

        event.m_x = pt.x; event.m_y = pt.y;

        if (!GetEventHandler()->ProcessEvent(event))
            Default();
    }
}

void wxWindow::MSWOnJoyDown(int joystick, int x, int y, WXUINT flags)
{
    int buttons = 0;
    int change = 0;
    if (flags & JOY_BUTTON1CHG)
        change = wxJOY_BUTTON1;
    if (flags & JOY_BUTTON2CHG)
        change = wxJOY_BUTTON2;
    if (flags & JOY_BUTTON3CHG)
        change = wxJOY_BUTTON3;
    if (flags & JOY_BUTTON4CHG)
        change = wxJOY_BUTTON4;

    if (flags & JOY_BUTTON1)
        buttons |= wxJOY_BUTTON1;
    if (flags & JOY_BUTTON2)
        buttons |= wxJOY_BUTTON2;
    if (flags & JOY_BUTTON3)
        buttons |= wxJOY_BUTTON3;
    if (flags & JOY_BUTTON4)
        buttons |= wxJOY_BUTTON4;

    wxJoystickEvent event(wxEVT_JOY_BUTTON_DOWN, buttons, joystick, change);
    event.SetPosition(wxPoint(x, y));
    event.SetEventObject(this);

    GetEventHandler()->ProcessEvent(event);
}

void wxWindow::MSWOnJoyUp(int joystick, int x, int y, WXUINT flags)
{
    int buttons = 0;
    int change = 0;
    if (flags & JOY_BUTTON1CHG)
        change = wxJOY_BUTTON1;
    if (flags & JOY_BUTTON2CHG)
        change = wxJOY_BUTTON2;
    if (flags & JOY_BUTTON3CHG)
        change = wxJOY_BUTTON3;
    if (flags & JOY_BUTTON4CHG)
        change = wxJOY_BUTTON4;

    if (flags & JOY_BUTTON1)
        buttons |= wxJOY_BUTTON1;
    if (flags & JOY_BUTTON2)
        buttons |= wxJOY_BUTTON2;
    if (flags & JOY_BUTTON3)
        buttons |= wxJOY_BUTTON3;
    if (flags & JOY_BUTTON4)
        buttons |= wxJOY_BUTTON4;

    wxJoystickEvent event(wxEVT_JOY_BUTTON_UP, buttons, joystick, change);
    event.SetPosition(wxPoint(x, y));
    event.SetEventObject(this);

    GetEventHandler()->ProcessEvent(event);
}

void wxWindow::MSWOnJoyMove(int joystick, int x, int y, WXUINT flags)
{
    int buttons = 0;
    if (flags & JOY_BUTTON1)
        buttons |= wxJOY_BUTTON1;
    if (flags & JOY_BUTTON2)
        buttons |= wxJOY_BUTTON2;
    if (flags & JOY_BUTTON3)
        buttons |= wxJOY_BUTTON3;
    if (flags & JOY_BUTTON4)
        buttons |= wxJOY_BUTTON4;

    wxJoystickEvent event(wxEVT_JOY_MOVE, buttons, joystick, 0);
    event.SetPosition(wxPoint(x, y));
    event.SetEventObject(this);

    GetEventHandler()->ProcessEvent(event);
}

void wxWindow::MSWOnJoyZMove(int joystick, int z, WXUINT flags)
{
    int buttons = 0;
    if (flags & JOY_BUTTON1)
        buttons |= wxJOY_BUTTON1;
    if (flags & JOY_BUTTON2)
        buttons |= wxJOY_BUTTON2;
    if (flags & JOY_BUTTON3)
        buttons |= wxJOY_BUTTON3;
    if (flags & JOY_BUTTON4)
        buttons |= wxJOY_BUTTON4;

    wxJoystickEvent event(wxEVT_JOY_ZMOVE, buttons, joystick, 0);
    event.SetZPosition(z);
    event.SetEventObject(this);

    GetEventHandler()->ProcessEvent(event);
}

void wxWindow::MSWOnVScroll(WXWORD wParam, WXWORD pos, WXHWND control)
{
    if (control)
    {
        wxWindow *child = wxFindWinFromHandle(control);
        if ( child )
            child->MSWOnVScroll(wParam, pos, control);
        return;
    }

    wxScrollEvent event;
    event.SetPosition(pos);
    event.SetOrientation(wxVERTICAL);
    event.m_eventObject = this;

    switch ( wParam )
    {
    case SB_TOP:
        event.m_eventType = wxEVT_SCROLL_TOP;
        break;

    case SB_BOTTOM:
        event.m_eventType = wxEVT_SCROLL_BOTTOM;
        break;

    case SB_LINEUP:
        event.m_eventType = wxEVT_SCROLL_LINEUP;
        break;

    case SB_LINEDOWN:
        event.m_eventType = wxEVT_SCROLL_LINEDOWN;
        break;

    case SB_PAGEUP:
        event.m_eventType = wxEVT_SCROLL_PAGEUP;
        break;

    case SB_PAGEDOWN:
        event.m_eventType = wxEVT_SCROLL_PAGEDOWN;
        break;

    case SB_THUMBTRACK:
    case SB_THUMBPOSITION:
        event.m_eventType = wxEVT_SCROLL_THUMBTRACK;
        break;

    default:
        return;
        break;
    }

    if (!GetEventHandler()->ProcessEvent(event))
        Default();
}

void wxWindow::MSWOnHScroll( WXWORD wParam, WXWORD pos, WXHWND control)
{
    if (control)
    {
        wxWindow *child = wxFindWinFromHandle(control);
        if ( child ) {
            child->MSWOnHScroll(wParam, pos, control);

            return;
        }
    }
    else {
        wxScrollEvent event;
        event.SetPosition(pos);
        event.SetOrientation(wxHORIZONTAL);
        event.m_eventObject = this;

        switch ( wParam )
        {
        case SB_TOP:
            event.m_eventType = wxEVT_SCROLL_TOP;
            break;

        case SB_BOTTOM:
            event.m_eventType = wxEVT_SCROLL_BOTTOM;
            break;

        case SB_LINEUP:
            event.m_eventType = wxEVT_SCROLL_LINEUP;
            break;

        case SB_LINEDOWN:
            event.m_eventType = wxEVT_SCROLL_LINEDOWN;
            break;

        case SB_PAGEUP:
            event.m_eventType = wxEVT_SCROLL_PAGEUP;
            break;

        case SB_PAGEDOWN:
            event.m_eventType = wxEVT_SCROLL_PAGEDOWN;
            break;

        case SB_THUMBTRACK:
        case SB_THUMBPOSITION:
            event.m_eventType = wxEVT_SCROLL_THUMBTRACK;
            break;

        default:
            return;
        }

        if ( GetEventHandler()->ProcessEvent(event) )
            return;
    }

    // call the default WM_HSCROLL handler: it's non trivial in some common
    // controls (up-down control for example)
    Default();
}

void wxWindow::MSWOnShow(bool show, int status)
{
    wxShowEvent event(GetId(), show);
    event.m_eventObject = this;
    GetEventHandler()->ProcessEvent(event);
}

bool wxWindow::MSWOnInitDialog(WXHWND WXUNUSED(hWndFocus))
{
    wxInitDialogEvent event(GetId());
    event.m_eventObject = this;
    GetEventHandler()->ProcessEvent(event);
    return TRUE;
}

void wxWindow::InitDialog(void)
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

void wxGetCharSize(WXHWND wnd, int *x, int *y,wxFont *the_font)
{
    TEXTMETRIC tm;
    HDC dc = ::GetDC((HWND) wnd);
    HFONT fnt =0;
    HFONT was = 0;
    if (the_font)
    {
#if WXDEBUG > 1
        wxDebugMsg("wxGetCharSize: Selecting HFONT %X\n", fnt);
#endif
        //    the_font->UseResource();
        //    the_font->RealizeResource();
        if ((fnt=(HFONT) the_font->GetResourceHandle()))
            was = (HFONT) SelectObject(dc,fnt) ;
    }
    GetTextMetrics(dc, &tm);
    if (the_font && fnt && was)
    {
#if WXDEBUG > 1
        wxDebugMsg("wxGetCharSize: Selecting old HFONT %X\n", was);
#endif
        SelectObject(dc,was) ;
    }
    ReleaseDC((HWND)wnd, dc);
    *x = tm.tmAveCharWidth;
    *y = tm.tmHeight + tm.tmExternalLeading;

    //  if (the_font)
    //    the_font->ReleaseResource();
}

// Returns 0 if was a normal ASCII value, not a special key. This indicates that
// the key should be ignored by WM_KEYDOWN and processed by WM_CHAR instead.
int wxCharCodeMSWToWX(int keySym)
{
    int id = 0;
    switch (keySym)
    {
    case VK_CANCEL:     id = WXK_CANCEL; break;
    case VK_BACK:       id = WXK_BACK; break;
    case VK_TAB:        id = WXK_TAB; break;
    case VK_CLEAR:      id = WXK_CLEAR; break;
    case VK_RETURN:     id = WXK_RETURN; break;
    case VK_SHIFT:      id = WXK_SHIFT; break;
    case VK_CONTROL:    id = WXK_CONTROL; break;
    case VK_MENU :      id = WXK_MENU; break;
    case VK_PAUSE:      id = WXK_PAUSE; break;
    case VK_SPACE:      id = WXK_SPACE; break;
    case VK_ESCAPE:     id = WXK_ESCAPE; break;
    case VK_PRIOR:      id = WXK_PRIOR; break;
    case VK_NEXT :      id = WXK_NEXT; break;
    case VK_END:        id = WXK_END; break;
    case VK_HOME :      id = WXK_HOME; break;
    case VK_LEFT :      id = WXK_LEFT; break;
    case VK_UP:         id = WXK_UP; break;
    case VK_RIGHT:      id = WXK_RIGHT; break;
    case VK_DOWN :      id = WXK_DOWN; break;
    case VK_SELECT:     id = WXK_SELECT; break;
    case VK_PRINT:      id = WXK_PRINT; break;
    case VK_EXECUTE:    id = WXK_EXECUTE; break;
    case VK_INSERT:     id = WXK_INSERT; break;
    case VK_DELETE:     id = WXK_DELETE; break;
    case VK_HELP :      id = WXK_HELP; break;
    case VK_NUMPAD0:    id = WXK_NUMPAD0; break;
    case VK_NUMPAD1:    id = WXK_NUMPAD1; break;
    case VK_NUMPAD2:    id = WXK_NUMPAD2; break;
    case VK_NUMPAD3:    id = WXK_NUMPAD3; break;
    case VK_NUMPAD4:    id = WXK_NUMPAD4; break;
    case VK_NUMPAD5:    id = WXK_NUMPAD5; break;
    case VK_NUMPAD6:    id = WXK_NUMPAD6; break;
    case VK_NUMPAD7:    id = WXK_NUMPAD7; break;
    case VK_NUMPAD8:    id = WXK_NUMPAD8; break;
    case VK_NUMPAD9:    id = WXK_NUMPAD9; break;
    case VK_MULTIPLY:   id = WXK_MULTIPLY; break;
    case VK_ADD:        id = WXK_ADD; break;
    case VK_SUBTRACT:   id = WXK_SUBTRACT; break;
    case VK_DECIMAL:    id = WXK_DECIMAL; break;
    case VK_DIVIDE:     id = WXK_DIVIDE; break;
    case VK_F1:         id = WXK_F1; break;
    case VK_F2:         id = WXK_F2; break;
    case VK_F3:         id = WXK_F3; break;
    case VK_F4:         id = WXK_F4; break;
    case VK_F5:         id = WXK_F5; break;
    case VK_F6:         id = WXK_F6; break;
    case VK_F7:         id = WXK_F7; break;
    case VK_F8:         id = WXK_F8; break;
    case VK_F9:         id = WXK_F9; break;
    case VK_F10:        id = WXK_F10; break;
    case VK_F11:        id = WXK_F11; break;
    case VK_F12:        id = WXK_F12; break;
    case VK_F13:        id = WXK_F13; break;
    case VK_F14:        id = WXK_F14; break;
    case VK_F15:        id = WXK_F15; break;
    case VK_F16:        id = WXK_F16; break;
    case VK_F17:        id = WXK_F17; break;
    case VK_F18:        id = WXK_F18; break;
    case VK_F19:        id = WXK_F19; break;
    case VK_F20:        id = WXK_F20; break;
    case VK_F21:        id = WXK_F21; break;
    case VK_F22:        id = WXK_F22; break;
    case VK_F23:        id = WXK_F23; break;
    case VK_F24:        id = WXK_F24; break;
    case VK_NUMLOCK:    id = WXK_NUMLOCK; break;
    case VK_SCROLL:     id = WXK_SCROLL; break;
    default:
        {
            return 0;
        }
    }
    return id;
}

int wxCharCodeWXToMSW(int id, bool *isVirtual)
{
    *isVirtual = TRUE;
    int keySym = 0;
    switch (id)
    {
    case WXK_CANCEL:    keySym = VK_CANCEL; break;
    case WXK_CLEAR:     keySym = VK_CLEAR; break;
    case WXK_SHIFT:     keySym = VK_SHIFT; break;
    case WXK_CONTROL:   keySym = VK_CONTROL; break;
    case WXK_MENU :     keySym = VK_MENU; break;
    case WXK_PAUSE:     keySym = VK_PAUSE; break;
    case WXK_PRIOR:     keySym = VK_PRIOR; break;
    case WXK_NEXT :     keySym = VK_NEXT; break;
    case WXK_END:       keySym = VK_END; break;
    case WXK_HOME :     keySym = VK_HOME; break;
    case WXK_LEFT :     keySym = VK_LEFT; break;
    case WXK_UP:        keySym = VK_UP; break;
    case WXK_RIGHT:     keySym = VK_RIGHT; break;
    case WXK_DOWN :     keySym = VK_DOWN; break;
    case WXK_SELECT:    keySym = VK_SELECT; break;
    case WXK_PRINT:     keySym = VK_PRINT; break;
    case WXK_EXECUTE:   keySym = VK_EXECUTE; break;
    case WXK_INSERT:    keySym = VK_INSERT; break;
    case WXK_DELETE:    keySym = VK_DELETE; break;
    case WXK_HELP :     keySym = VK_HELP; break;
    case WXK_NUMPAD0:   keySym = VK_NUMPAD0; break;
    case WXK_NUMPAD1:   keySym = VK_NUMPAD1; break;
    case WXK_NUMPAD2:   keySym = VK_NUMPAD2; break;
    case WXK_NUMPAD3:   keySym = VK_NUMPAD3; break;
    case WXK_NUMPAD4:   keySym = VK_NUMPAD4; break;
    case WXK_NUMPAD5:   keySym = VK_NUMPAD5; break;
    case WXK_NUMPAD6:   keySym = VK_NUMPAD6; break;
    case WXK_NUMPAD7:   keySym = VK_NUMPAD7; break;
    case WXK_NUMPAD8:   keySym = VK_NUMPAD8; break;
    case WXK_NUMPAD9:   keySym = VK_NUMPAD9; break;
    case WXK_MULTIPLY:  keySym = VK_MULTIPLY; break;
    case WXK_ADD:       keySym = VK_ADD; break;
    case WXK_SUBTRACT:  keySym = VK_SUBTRACT; break;
    case WXK_DECIMAL:   keySym = VK_DECIMAL; break;
    case WXK_DIVIDE:    keySym = VK_DIVIDE; break;
    case WXK_F1:        keySym = VK_F1; break;
    case WXK_F2:        keySym = VK_F2; break;
    case WXK_F3:        keySym = VK_F3; break;
    case WXK_F4:        keySym = VK_F4; break;
    case WXK_F5:        keySym = VK_F5; break;
    case WXK_F6:        keySym = VK_F6; break;
    case WXK_F7:        keySym = VK_F7; break;
    case WXK_F8:        keySym = VK_F8; break;
    case WXK_F9:        keySym = VK_F9; break;
    case WXK_F10:       keySym = VK_F10; break;
    case WXK_F11:       keySym = VK_F11; break;
    case WXK_F12:       keySym = VK_F12; break;
    case WXK_F13:       keySym = VK_F13; break;
    case WXK_F14:       keySym = VK_F14; break;
    case WXK_F15:       keySym = VK_F15; break;
    case WXK_F16:       keySym = VK_F16; break;
    case WXK_F17:       keySym = VK_F17; break;
    case WXK_F18:       keySym = VK_F18; break;
    case WXK_F19:       keySym = VK_F19; break;
    case WXK_F20:       keySym = VK_F20; break;
    case WXK_F21:       keySym = VK_F21; break;
    case WXK_F22:       keySym = VK_F22; break;
    case WXK_F23:       keySym = VK_F23; break;
    case WXK_F24:       keySym = VK_F24; break;
    case WXK_NUMLOCK:   keySym = VK_NUMLOCK; break;
    case WXK_SCROLL:    keySym = VK_SCROLL; break;
    default:
        {
            *isVirtual = FALSE;
            keySym = id;
            break;
        }
    }
    return keySym;
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
    // Not implemented
}

void wxWindow::ShowCaret(bool show)
{
    if (m_caretEnabled)
    {
        if (show)
            ::ShowCaret((HWND) GetHWND());
        else
            ::HideCaret((HWND) GetHWND());
        m_caretShown = show;
    }
}

void wxWindow::DestroyCaret(void)
{
    m_caretEnabled = FALSE;
}

void wxWindow::SetCaretPos(int x, int y)
{
    ::SetCaretPos(x, y);
}

void wxWindow::GetCaretPos(int *x, int *y) const
{
    POINT point;
    ::GetCaretPos(&point);
    *x = point.x;
    *y = point.y;
}

wxWindow *wxGetActiveWindow(void)
{
    HWND hWnd = GetActiveWindow();
    if (hWnd != 0)
    {
        return wxFindWinFromHandle((WXHWND) hWnd);
    }
    return NULL;
}

// Windows keyboard hook. Allows interception of e.g. F1, ESCAPE
// in active frames and dialogs, regardless of where the focus is.
static HHOOK wxTheKeyboardHook = 0;
static FARPROC wxTheKeyboardHookProc = 0;
int APIENTRY _EXPORT
wxKeyboardHook(int nCode, WORD wParam, DWORD lParam);

void wxSetKeyboardHook(bool doIt)
{
    if (doIt)
    {
        wxTheKeyboardHookProc = MakeProcInstance((FARPROC) wxKeyboardHook, wxGetInstance());
        wxTheKeyboardHook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC) wxTheKeyboardHookProc, wxGetInstance(),
#ifdef __WIN32__
            GetCurrentThreadId());
        //      (DWORD)GetCurrentProcess()); // This is another possibility. Which is right?
#else
        GetCurrentTask());
#endif
    }
    else
    {
        UnhookWindowsHookEx(wxTheKeyboardHook);
        FreeProcInstance(wxTheKeyboardHookProc);
    }
}

int APIENTRY _EXPORT
wxKeyboardHook(int nCode, WORD wParam, DWORD lParam)
{
    DWORD hiWord = HIWORD(lParam);
    if (nCode != HC_NOREMOVE && ((hiWord & KF_UP) == 0))
    {
        int id;
        if ((id = wxCharCodeMSWToWX(wParam)) != 0)
        {
            wxKeyEvent event(wxEVT_CHAR_HOOK);
            if ((HIWORD(lParam) & KF_ALTDOWN) == KF_ALTDOWN)
                event.m_altDown = TRUE;

            event.m_eventObject = NULL;
            event.m_keyCode = id;
            /* begin Albert's fix for control and shift key 26.5 */
            event.m_shiftDown = (::GetKeyState(VK_SHIFT)&0x100?TRUE:FALSE);
            event.m_controlDown = (::GetKeyState(VK_CONTROL)&0x100?TRUE:FALSE);
            /* end Albert's fix for control and shift key 26.5 */
            event.SetTimestamp(wxApp::sm_lastMessageTime);

            wxWindow *win = wxGetActiveWindow();
            if (win)
            {
                if (win->GetEventHandler()->ProcessEvent(event))
                    return 1;
            }
            else
            {
                if ( wxTheApp && wxTheApp->ProcessEvent(event) )
                    return 1;
            }
        }
    }
    return (int)CallNextHookEx(wxTheKeyboardHook, nCode, wParam, lParam);
}

void wxWindow::SetSizeHints(int minW, int minH, int maxW, int maxH, int WXUNUSED(incW), int WXUNUSED(incH))
{
    m_minSizeX = minW;
    m_minSizeY = minH;
    m_maxSizeX = maxW;
    m_maxSizeY = maxH;
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

/* TODO (maybe)
void wxWindow::OnPaint(void)
{
PaintSelectionHandles();
}
*/

void wxWindow::WarpPointer (int x_pos, int y_pos)
{
    // Move the pointer to (x_pos,y_pos) coordinates. They are expressed in
    // pixel coordinates, relatives to the canvas -- So, we first need to
    // substract origin of the window, then convert to screen position

    int x = x_pos; int y = y_pos;
    RECT rect;
    GetWindowRect ((HWND) GetHWND(), &rect);

    x += rect.left;
    y += rect.top;

    SetCursorPos (x, y);
}

void wxWindow::MSWDeviceToLogical (float *x, float *y) const
{
}

bool wxWindow::MSWOnEraseBkgnd (WXHDC pDC)
{
    wxDC dc ;

    dc.SetHDC(pDC);
    dc.SetWindow(this);
    dc.BeginDrawing();

    wxEraseEvent event(m_windowId, &dc);
    event.m_eventObject = this;
    if (!GetEventHandler()->ProcessEvent(event))
    {
        dc.EndDrawing();
        dc.SelectOldObjects(pDC);
        return FALSE;
    }
    else
    {
        dc.EndDrawing();
        dc.SelectOldObjects(pDC);
    }

    dc.SetHDC((WXHDC) NULL);
    return TRUE;
}

void wxWindow::OnEraseBackground(wxEraseEvent& event)
{
    RECT rect;
    ::GetClientRect((HWND) GetHWND(), &rect);

    HBRUSH hBrush = ::CreateSolidBrush(PALETTERGB(GetBackgroundColour().Red(), GetBackgroundColour().Green(), GetBackgroundColour().Blue()));
    int mode = ::SetMapMode((HDC) event.GetDC()->GetHDC(), MM_TEXT);

    //  ::GetClipBox((HDC) event.GetDC()->GetHDC(), &rect);
    ::FillRect ((HDC) event.GetDC()->GetHDC(), &rect, hBrush);
    ::DeleteObject(hBrush);
    ::SetMapMode((HDC) event.GetDC()->GetHDC(), mode);
    /*
    // Less efficient version (and doesn't account for scrolling)
    int w, h;
    GetClientSize(& w, & h);
    wxBrush *brush = wxTheBrushList->FindOrCreateBrush(& GetBackgroundColour(), wxSOLID);
    event.GetDC()->SetBrush(brush);
    event.GetDC()->SetPen(wxTRANSPARENT_PEN);

      event.GetDC()->DrawRectangle(0, 0, w+1, h+1);
    */
}

#if WXWIN_COMPATIBILITY
void wxWindow::SetScrollRange(int orient, int range, bool refresh)
{
#if defined(__WIN95__)

    int range1 = range;

    // Try to adjust the range to cope with page size > 1
    // - a Windows API quirk
    int pageSize = GetScrollPage(orient);
    if ( pageSize > 1 && range > 0)
    {
        range1 += (pageSize - 1);
    }

    SCROLLINFO info;
    int dir;

    if (orient == wxHORIZONTAL) {
        dir = SB_HORZ;
    } else {
        dir = SB_VERT;
    }

    info.cbSize = sizeof(SCROLLINFO);
    info.nPage = pageSize; // Have to set this, or scrollbar goes awry
    info.nMin = 0;
    info.nMax = range1;
    info.nPos = 0;
    info.fMask = SIF_RANGE | SIF_PAGE;

    HWND hWnd = (HWND) GetHWND();
    if (hWnd)
        ::SetScrollInfo(hWnd, dir, &info, refresh);
#else
    int wOrient ;
    if (orient == wxHORIZONTAL)
        wOrient = SB_HORZ;
    else
        wOrient = SB_VERT;

    HWND hWnd = (HWND) GetHWND();
    if (hWnd)
        ::SetScrollRange(hWnd, wOrient, 0, range, refresh);
#endif
}

void wxWindow::SetScrollPage(int orient, int page, bool refresh)
{
#if defined(__WIN95__)
    SCROLLINFO info;
    int dir;

    if (orient == wxHORIZONTAL) {
        dir = SB_HORZ;
        m_xThumbSize = page;
    } else {
        dir = SB_VERT;
        m_yThumbSize = page;
    }

    info.cbSize = sizeof(SCROLLINFO);
    info.nPage = page;
    info.nMin = 0;
    info.fMask = SIF_PAGE ;

    HWND hWnd = (HWND) GetHWND();
    if (hWnd)
        ::SetScrollInfo(hWnd, dir, &info, refresh);
#else
    if (orient == wxHORIZONTAL)
        m_xThumbSize = page;
    else
        m_yThumbSize = page;
#endif
}

int wxWindow::OldGetScrollRange(int orient) const
{
    int wOrient ;
    if (orient == wxHORIZONTAL)
        wOrient = SB_HORZ;
    else
        wOrient = SB_VERT;

#if __WATCOMC__ && defined(__WINDOWS_386__)
    short minPos, maxPos;
#else
    int minPos, maxPos;
#endif
    HWND hWnd = (HWND) GetHWND();
    if (hWnd)
    {
        ::GetScrollRange(hWnd, wOrient, &minPos, &maxPos);
#if defined(__WIN95__)
        // Try to adjust the range to cope with page size > 1
        // - a Windows API quirk
        int pageSize = GetScrollPage(orient);
        if ( pageSize > 1 )
        {
            maxPos -= (pageSize - 1);
        }
#endif
        return maxPos;
    }
    else
        return 0;
}

int wxWindow::GetScrollPage(int orient) const
{
    if (orient == wxHORIZONTAL)
        return m_xThumbSize;
    else
        return m_yThumbSize;
}
#endif

int wxWindow::GetScrollPos(int orient) const
{
    int wOrient ;
    if (orient == wxHORIZONTAL)
        wOrient = SB_HORZ;
    else
        wOrient = SB_VERT;
    HWND hWnd = (HWND) GetHWND();
    if (hWnd)
    {
        return ::GetScrollPos(hWnd, wOrient);
    }
    else
        return 0;
}

// This now returns the whole range, not just the number
// of positions that we can scroll.
int wxWindow::GetScrollRange(int orient) const
{
    int wOrient ;
    if (orient == wxHORIZONTAL)
        wOrient = SB_HORZ;
    else
        wOrient = SB_VERT;

#if __WATCOMC__ && defined(__WINDOWS_386__)
    short minPos, maxPos;
#else
    int minPos, maxPos;
#endif
    HWND hWnd = (HWND) GetHWND();
    if (hWnd)
    {
        ::GetScrollRange(hWnd, wOrient, &minPos, &maxPos);
#if defined(__WIN95__)
        // Try to adjust the range to cope with page size > 1
        // - a Windows API quirk
        int pageSize = GetScrollPage(orient);
        if ( pageSize > 1 )
        {
            maxPos -= (pageSize - 1);
        }
        // October 10th: new range concept.
        maxPos += pageSize;
#endif

        return maxPos;
    }
    else
        return 0;
}

int wxWindow::GetScrollThumb(int orient) const
{
    if (orient == wxHORIZONTAL)
        return m_xThumbSize;
    else
        return m_yThumbSize;
}

void wxWindow::SetScrollPos(int orient, int pos, bool refresh)
{
#if defined(__WIN95__)
    SCROLLINFO info;
    int dir;

    if (orient == wxHORIZONTAL) {
        dir = SB_HORZ;
    } else {
        dir = SB_VERT;
    }

    info.cbSize = sizeof(SCROLLINFO);
    info.nPage = 0;
    info.nMin = 0;
    info.nPos = pos;
    info.fMask = SIF_POS ;

    HWND hWnd = (HWND) GetHWND();
    if (hWnd)
        ::SetScrollInfo(hWnd, dir, &info, refresh);
#else
    int wOrient ;
    if (orient == wxHORIZONTAL)
        wOrient = SB_HORZ;
    else
        wOrient = SB_VERT;

    HWND hWnd = (HWND) GetHWND();
    if (hWnd)
        ::SetScrollPos(hWnd, wOrient, pos, refresh);
#endif
}

// New function that will replace some of the above.
void wxWindow::SetScrollbar(int orient, int pos, int thumbVisible,
                            int range, bool refresh)
{
/*
SetScrollPage(orient, thumbVisible, FALSE);

  int oldRange = range - thumbVisible ;
  SetScrollRange(orient, oldRange, FALSE);

    SetScrollPos(orient, pos, refresh);
    */
#if defined(__WIN95__)
    int oldRange = range - thumbVisible ;

    int range1 = oldRange;

    // Try to adjust the range to cope with page size > 1
    // - a Windows API quirk
    int pageSize = thumbVisible;
    if ( pageSize > 1 && range > 0)
    {
        range1 += (pageSize - 1);
    }

    SCROLLINFO info;
    int dir;

    if (orient == wxHORIZONTAL) {
        dir = SB_HORZ;
    } else {
        dir = SB_VERT;
    }

    info.cbSize = sizeof(SCROLLINFO);
    info.nPage = pageSize; // Have to set this, or scrollbar goes awry
    info.nMin = 0;
    info.nMax = range1;
    info.nPos = pos;
    info.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;

    HWND hWnd = (HWND) GetHWND();
    if (hWnd)
        ::SetScrollInfo(hWnd, dir, &info, refresh);
#else
    int wOrient ;
    if (orient == wxHORIZONTAL)
        wOrient = SB_HORZ;
    else
        wOrient = SB_VERT;

    HWND hWnd = (HWND) GetHWND();
    if (hWnd)
    {
        ::SetScrollRange(hWnd, wOrient, 0, range, FALSE);
        ::SetScrollPos(hWnd, wOrient, pos, refresh);
    }
#endif
    if (orient == wxHORIZONTAL) {
        m_xThumbSize = thumbVisible;
    } else {
        m_yThumbSize = thumbVisible;
    }
}

void wxWindow::ScrollWindow(int dx, int dy, const wxRectangle *rect)
{
    RECT rect2;
    if ( rect )
    {
        rect2.left = rect->x;
        rect2.top = rect->y;
        rect2.right = rect->x + rect->width;
        rect2.bottom = rect->y + rect->height;
    }

    if ( rect )
        ::ScrollWindow((HWND) GetHWND(), dx, dy, &rect2, NULL);
    else
        ::ScrollWindow((HWND) GetHWND(), dx, dy, NULL, NULL);
}

void wxWindow::SetFont(const wxFont& font)
{
    m_windowFont = font;

    if (!m_windowFont.Ok())
        return;

    HWND hWnd = (HWND) GetHWND();
    if (hWnd != 0)
    {
        if (m_windowFont.GetResourceHandle())
            SendMessage(hWnd, WM_SETFONT,
            (WPARAM)m_windowFont.GetResourceHandle(),TRUE);
    }
}

void wxWindow::SubclassWin(WXHWND hWnd)
{
    wxASSERT_MSG( !m_oldWndProc, "subclassing window twice?" );

    wxAssociateWinWithHandle((HWND)hWnd, this);

    m_oldWndProc = (WXFARPROC) GetWindowLong((HWND) hWnd, GWL_WNDPROC);
    SetWindowLong((HWND) hWnd, GWL_WNDPROC, (LONG) wxWndProc);
}

void wxWindow::UnsubclassWin(void)
{
    wxRemoveHandleAssociation(this);

    // Restore old Window proc
    if ((HWND) GetHWND())
    {
        FARPROC farProc = (FARPROC) GetWindowLong((HWND) GetHWND(), GWL_WNDPROC);
        if ((m_oldWndProc != 0) && (farProc != (FARPROC) m_oldWndProc))
        {
            SetWindowLong((HWND) GetHWND(), GWL_WNDPROC, (LONG) m_oldWndProc);
            m_oldWndProc = 0;
        }
    }
}

// Make a Windows extended style from the given wxWindows window style
WXDWORD wxWindow::MakeExtendedStyle(long style, bool eliminateBorders)
{
    WXDWORD exStyle = 0;
    if ( style & wxTRANSPARENT_WINDOW )
        exStyle |= WS_EX_TRANSPARENT ;

    if ( !eliminateBorders )
    {
        if ( style & wxSUNKEN_BORDER )
            exStyle |= WS_EX_CLIENTEDGE ;
        if ( style & wxDOUBLE_BORDER )
            exStyle |= WS_EX_DLGMODALFRAME ;
#if defined(__WIN95__)
        if ( style & wxRAISED_BORDER )
            exStyle |= WS_EX_WINDOWEDGE ;
        if ( style & wxSTATIC_BORDER )
            exStyle |= WS_EX_STATICEDGE ;
#endif
    }
    return exStyle;
}

// Determines whether native 3D effects or CTL3D should be used,
// applying a default border style if required, and returning an extended
// style to pass to CreateWindowEx.
WXDWORD wxWindow::Determine3DEffects(WXDWORD defaultBorderStyle, bool *want3D)
{
    // If matches certain criteria, then assume no 3D effects
    // unless specifically requested (dealt with in MakeExtendedStyle)
    if ( !GetParent() || !IsKindOf(CLASSINFO(wxControl)) || (m_windowStyle & wxNO_BORDER) )
    {
        *want3D = FALSE;
        return MakeExtendedStyle(m_windowStyle, FALSE);
    }

    // Determine whether we should be using 3D effects or not.
    bool nativeBorder = FALSE; // by default, we don't want a Win95 effect

    // 1) App can specify global 3D effects
    *want3D = wxTheApp->GetAuto3D();

    // 2) If the parent is being drawn with user colours, or simple border specified,
    // switch effects off. TODO: replace wxUSER_COLOURS with wxNO_3D
    if (GetParent() && (GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS) || (m_windowStyle & wxSIMPLE_BORDER))
        *want3D = FALSE;

    // 3) Control can override this global setting by defining
    // a border style, e.g. wxSUNKEN_BORDER
    if (m_windowStyle & wxSUNKEN_BORDER )
        *want3D = TRUE;

    // 4) If it's a special border, CTL3D can't cope so we want a native border
    if ( (m_windowStyle & wxDOUBLE_BORDER) || (m_windowStyle & wxRAISED_BORDER) ||
        (m_windowStyle & wxSTATIC_BORDER) )
    {
        *want3D = TRUE;
        nativeBorder = TRUE;
    }

    // 5) If this isn't a Win95 app, and we are using CTL3D, remove border
    // effects from extended style
#if CTL3D
    if ( *want3D )
        nativeBorder = FALSE;
#endif

    DWORD exStyle = MakeExtendedStyle(m_windowStyle, !nativeBorder);

    // If we want 3D, but haven't specified a border here,
    // apply the default border style specified.
    // TODO what about non-Win95 WIN32? Does it have borders?
#if defined(__WIN95__) && !CTL3D
    if (defaultBorderStyle && (*want3D) && ! ((m_windowStyle & wxDOUBLE_BORDER) || (m_windowStyle & wxRAISED_BORDER) ||
        (m_windowStyle & wxSTATIC_BORDER) || (m_windowStyle & wxSIMPLE_BORDER) ))
        exStyle |= defaultBorderStyle; // WS_EX_CLIENTEDGE ;
#endif

    return exStyle;
}

void wxWindow::OnChar(wxKeyEvent& event)
{
    if ( event.KeyCode() == WXK_TAB ) {
        // propagate the TABs to the parent - it's up to it to decide what
        // to do with it
        if ( GetParent() ) {
            if ( GetParent()->GetEventHandler()->ProcessEvent(event) )
                return;
        }
    }

    bool isVirtual;
    int id = wxCharCodeWXToMSW((int)event.KeyCode(), &isVirtual);

    if ( id == -1 )
        id= m_lastWParam;

    if ( !event.ControlDown() )
        (void) MSWDefWindowProc(m_lastMsg, (WPARAM) id, m_lastLParam);
}

void wxWindow::OnPaint(wxPaintEvent& event)
{
    Default();
}

bool wxWindow::IsEnabled(void) const
{
    return (::IsWindowEnabled((HWND) GetHWND()) != 0);
}

// Dialog support: override these and call
// base class members to add functionality
// that can't be done using validators.
// NOTE: these functions assume that controls
// are direct children of this window, not grandchildren
// or other levels of descendant.

// Transfer values to controls. If returns FALSE,
// it's an application error (pops up a dialog)
bool wxWindow::TransferDataToWindow(void)
{
    wxNode *node = GetChildren()->First();
    while ( node )
    {
        wxWindow *child = (wxWindow *)node->Data();
        if ( child->GetValidator() && /* child->GetValidator()->Ok() && */
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
bool wxWindow::TransferDataFromWindow(void)
{
    wxNode *node = GetChildren()->First();
    while ( node )
    {
        wxWindow *child = (wxWindow *)node->Data();
        if ( child->GetValidator() && /* child->GetValidator()->Ok() && */ !child->GetValidator()->TransferFromWindow() )
        {
            return FALSE;
        }

        node = node->Next();
    }
    return TRUE;
}

bool wxWindow::Validate(void)
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
wxWindow *wxWindow::FindFocus(void)
{
    HWND hWnd = ::GetFocus();
    if ( hWnd )
    {
        return wxFindWinFromHandle((WXHWND) hWnd);
    }
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

void wxWindow::DestroyChildren(void)
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

// If nothing defined for this, try the parent.
// E.g. we may be a button loaded from a resource, with no callback function
// defined.
void wxWindow::OnCommand(wxWindow& win, wxCommandEvent& event)
{
    if (GetEventHandler()->ProcessEvent(event) )
        return;
    if (m_windowParent)
        m_windowParent->GetEventHandler()->OnCommand(win, event);
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
void wxWindow::DeleteRelatedConstraints(void)
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

bool wxWindow::Layout(void)
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

void wxWindow::ResetConstraints(void)
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
    event.SetCanVeto(!force);

    return (GetEventHandler()->ProcessEvent(event) && !event.GetVeto());
}

wxObject* wxWindow::GetChild(int number) const
{
    // Return a pointer to the Nth object in the Panel
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
/* This is obsolete now; if we wish to intercept listbox double-clicks,
* we explicitly intercept the wxEVT_COMMAND_LISTBOX_DOUBLECLICKED
* event.

  if (initiatingItem->IsKindOf(CLASSINFO(wxListBox)))
  {
  wxListBox *lbox = (wxListBox *)initiatingItem;
  wxCommandEvent event(wxEVT_COMMAND_LEFT_DCLICK);
  event.m_commandInt = -1;
  if ((lbox->GetWindowStyleFlag() & wxLB_MULTIPLE) == 0)
  {
  event.m_commandString = copystring(lbox->GetStringSelection());
  event.m_commandInt = lbox->GetSelection();
  event.m_clientData = lbox->wxListBox::GetClientData(event.m_commandInt);
  }
  event.m_eventObject = lbox;

    lbox->ProcessCommand(event);

      if (event.m_commandString)
      delete[] event.m_commandString;
      return;
      }

        wxButton *but = GetDefaultItem();
        if (but)
        {
        wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED);
        event.SetEventObject(but);
        but->Command(event);
        }
    */
}

void wxWindow::Clear(void)
{
    wxClientDC dc(this);
    wxBrush brush(GetBackgroundColour(), wxSOLID);
    dc.SetBackground(brush);
    dc.Clear();
}

// Fits the panel around the items
void wxWindow::Fit(void)
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

/* TODO
// Default input behaviour for a scrolling canvas should be to scroll
// according to the cursor keys pressed
void wxWindow::OnChar(wxKeyEvent& event)
{
int x_page = 0;
int y_page = 0;
int start_x = 0;
int start_y = 0;
// Bugfix Begin
int v_width = 0;
int v_height = 0;
int y_pages = 0;
// Bugfix End

  GetScrollUnitsPerPage(&x_page, &y_page);
  // Bugfix Begin
  GetVirtualSize(&v_width,&v_height);
  // Bugfix End
  ViewStart(&start_x, &start_y);
  // Bugfix begin
  if (vert_units)
  y_pages = (int)(v_height/vert_units) - y_page;

    #ifdef __WXMSW__
    int y = 0;
    #else
    int y = y_page-1;
    #endif
    // Bugfix End
    switch (event.keyCode)
    {
    case WXK_PRIOR:
    {
    // BugFix Begin
    if (y_page > 0)
    {
    if (start_y - y_page > 0)
    Scroll(start_x, start_y - y_page);
    else
    Scroll(start_x, 0);
    }
    // Bugfix End
    break;
    }
    case WXK_NEXT:
    {
    // Bugfix Begin
    if ((y_page > 0)  && (start_y <= y_pages-y-1))
    {
    if (y_pages + y < start_y + y_page)
    Scroll(start_x, y_pages + y);
    else
    Scroll(start_x, start_y + y_page);
    }
    // Bugfix End
    break;
    }
    case WXK_UP:
    {
    if ((y_page > 0) && (start_y >= 1))
    Scroll(start_x, start_y - 1);
    break;
    }
    case WXK_DOWN:
    {
    // Bugfix Begin
    if ((y_page > 0) && (start_y <= y_pages-y-1))
    // Bugfix End
    {
    Scroll(start_x, start_y + 1);
    }
    break;
    }
    case WXK_LEFT:
    {
    if ((x_page > 0) && (start_x >= 1))
    Scroll(start_x - 1, start_y);
    break;
    }
    case WXK_RIGHT:
    {
    if (x_page > 0)
    Scroll(start_x + 1, start_y);
    break;
    }
    case WXK_HOME:
    {
    Scroll(0, 0);
    break;
    }
    // This is new
    case WXK_END:
    {
    Scroll(start_x, y_pages+y);
    break;
    }
    // end
    }
    }
*/

// Setup background and foreground colours correctly
void wxWindow::SetupColours(void)
{
    if (GetParent())
        SetBackgroundColour(GetParent()->GetBackgroundColour());
}

void wxWindow::OnIdle(wxIdleEvent& event)
{
    // Check if we need to send a LEAVE event
    if (m_mouseInWindow)
    {
        POINT pt;
        ::GetCursorPos(&pt);
        if (::WindowFromPoint(pt) != (HWND) GetHWND())
        {
            // Generate a LEAVE event
            m_mouseInWindow = FALSE;

            int state = 0;
            if (::GetKeyState(VK_SHIFT) != 0)
                state |= MK_SHIFT;
            if (::GetKeyState(VK_CONTROL) != 0)
                state |= MK_CONTROL;

            // Unfortunately the mouse button and keyboard state may have changed
            // by the time the OnIdle function is called, so 'state' may be
            // meaningless.

            MSWOnMouseLeave(pt.x, pt.y, state);
        }
    }
    UpdateWindowUI();
}

// Raise the window to the top of the Z order
void wxWindow::Raise(void)
{
    ::BringWindowToTop((HWND) GetHWND());
}

// Lower the window to the bottom of the Z order
void wxWindow::Lower(void)
{
    ::SetWindowPos((HWND) GetHWND(), HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
}

long wxWindow::MSWGetDlgCode()
{
    // default: just forward to def window proc (the msg has no parameters)
    return MSWDefWindowProc(WM_GETDLGCODE, 0, 0);
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

#ifdef __WXDEBUG__
const char *wxGetMessageName(int message)
{
    switch ( message ) {
    case 0x0000: return "WM_NULL";
    case 0x0001: return "WM_CREATE";
    case 0x0002: return "WM_DESTROY";
    case 0x0003: return "WM_MOVE";
    case 0x0005: return "WM_SIZE";
    case 0x0006: return "WM_ACTIVATE";
    case 0x0007: return "WM_SETFOCUS";
    case 0x0008: return "WM_KILLFOCUS";
    case 0x000A: return "WM_ENABLE";
    case 0x000B: return "WM_SETREDRAW";
    case 0x000C: return "WM_SETTEXT";
    case 0x000D: return "WM_GETTEXT";
    case 0x000E: return "WM_GETTEXTLENGTH";
    case 0x000F: return "WM_PAINT";
    case 0x0010: return "WM_CLOSE";
    case 0x0011: return "WM_QUERYENDSESSION";
    case 0x0012: return "WM_QUIT";
    case 0x0013: return "WM_QUERYOPEN";
    case 0x0014: return "WM_ERASEBKGND";
    case 0x0015: return "WM_SYSCOLORCHANGE";
    case 0x0016: return "WM_ENDSESSION";
    case 0x0017: return "WM_SYSTEMERROR";
    case 0x0018: return "WM_SHOWWINDOW";
    case 0x0019: return "WM_CTLCOLOR";
    case 0x001A: return "WM_WININICHANGE";
    case 0x001B: return "WM_DEVMODECHANGE";
    case 0x001C: return "WM_ACTIVATEAPP";
    case 0x001D: return "WM_FONTCHANGE";
    case 0x001E: return "WM_TIMECHANGE";
    case 0x001F: return "WM_CANCELMODE";
    case 0x0020: return "WM_SETCURSOR";
    case 0x0021: return "WM_MOUSEACTIVATE";
    case 0x0022: return "WM_CHILDACTIVATE";
    case 0x0023: return "WM_QUEUESYNC";
    case 0x0024: return "WM_GETMINMAXINFO";
    case 0x0026: return "WM_PAINTICON";
    case 0x0027: return "WM_ICONERASEBKGND";
    case 0x0028: return "WM_NEXTDLGCTL";
    case 0x002A: return "WM_SPOOLERSTATUS";
    case 0x002B: return "WM_DRAWITEM";
    case 0x002C: return "WM_MEASUREITEM";
    case 0x002D: return "WM_DELETEITEM";
    case 0x002E: return "WM_VKEYTOITEM";
    case 0x002F: return "WM_CHARTOITEM";
    case 0x0030: return "WM_SETFONT";
    case 0x0031: return "WM_GETFONT";
    case 0x0037: return "WM_QUERYDRAGICON";
    case 0x0039: return "WM_COMPAREITEM";
    case 0x0041: return "WM_COMPACTING";
    case 0x0044: return "WM_COMMNOTIFY";
    case 0x0046: return "WM_WINDOWPOSCHANGING";
    case 0x0047: return "WM_WINDOWPOSCHANGED";
    case 0x0048: return "WM_POWER";

#ifdef  __WIN32__
    case 0x004A: return "WM_COPYDATA";
    case 0x004B: return "WM_CANCELJOURNAL";
    case 0x004E: return "WM_NOTIFY";
    case 0x0050: return "WM_INPUTLANGCHANGEREQUEST";
    case 0x0051: return "WM_INPUTLANGCHANGE";
    case 0x0052: return "WM_TCARD";
    case 0x0053: return "WM_HELP";
    case 0x0054: return "WM_USERCHANGED";
    case 0x0055: return "WM_NOTIFYFORMAT";
    case 0x007B: return "WM_CONTEXTMENU";
    case 0x007C: return "WM_STYLECHANGING";
    case 0x007D: return "WM_STYLECHANGED";
    case 0x007E: return "WM_DISPLAYCHANGE";
    case 0x007F: return "WM_GETICON";
    case 0x0080: return "WM_SETICON";
#endif  //WIN32

    case 0x0081: return "WM_NCCREATE";
    case 0x0082: return "WM_NCDESTROY";
    case 0x0083: return "WM_NCCALCSIZE";
    case 0x0084: return "WM_NCHITTEST";
    case 0x0085: return "WM_NCPAINT";
    case 0x0086: return "WM_NCACTIVATE";
    case 0x0087: return "WM_GETDLGCODE";
    case 0x00A0: return "WM_NCMOUSEMOVE";
    case 0x00A1: return "WM_NCLBUTTONDOWN";
    case 0x00A2: return "WM_NCLBUTTONUP";
    case 0x00A3: return "WM_NCLBUTTONDBLCLK";
    case 0x00A4: return "WM_NCRBUTTONDOWN";
    case 0x00A5: return "WM_NCRBUTTONUP";
    case 0x00A6: return "WM_NCRBUTTONDBLCLK";
    case 0x00A7: return "WM_NCMBUTTONDOWN";
    case 0x00A8: return "WM_NCMBUTTONUP";
    case 0x00A9: return "WM_NCMBUTTONDBLCLK";
    case 0x0100: return "WM_KEYDOWN";
    case 0x0101: return "WM_KEYUP";
    case 0x0102: return "WM_CHAR";
    case 0x0103: return "WM_DEADCHAR";
    case 0x0104: return "WM_SYSKEYDOWN";
    case 0x0105: return "WM_SYSKEYUP";
    case 0x0106: return "WM_SYSCHAR";
    case 0x0107: return "WM_SYSDEADCHAR";
    case 0x0108: return "WM_KEYLAST";

#ifdef  __WIN32__
    case 0x010D: return "WM_IME_STARTCOMPOSITION";
    case 0x010E: return "WM_IME_ENDCOMPOSITION";
    case 0x010F: return "WM_IME_COMPOSITION";
#endif  //WIN32

    case 0x0110: return "WM_INITDIALOG";
    case 0x0111: return "WM_COMMAND";
    case 0x0112: return "WM_SYSCOMMAND";
    case 0x0113: return "WM_TIMER";
    case 0x0114: return "WM_HSCROLL";
    case 0x0115: return "WM_VSCROLL";
    case 0x0116: return "WM_INITMENU";
    case 0x0117: return "WM_INITMENUPOPUP";
    case 0x011F: return "WM_MENUSELECT";
    case 0x0120: return "WM_MENUCHAR";
    case 0x0121: return "WM_ENTERIDLE";
    case 0x0200: return "WM_MOUSEMOVE";
    case 0x0201: return "WM_LBUTTONDOWN";
    case 0x0202: return "WM_LBUTTONUP";
    case 0x0203: return "WM_LBUTTONDBLCLK";
    case 0x0204: return "WM_RBUTTONDOWN";
    case 0x0205: return "WM_RBUTTONUP";
    case 0x0206: return "WM_RBUTTONDBLCLK";
    case 0x0207: return "WM_MBUTTONDOWN";
    case 0x0208: return "WM_MBUTTONUP";
    case 0x0209: return "WM_MBUTTONDBLCLK";
    case 0x0210: return "WM_PARENTNOTIFY";
    case 0x0211: return "WM_ENTERMENULOOP";
    case 0x0212: return "WM_EXITMENULOOP";

#ifdef  __WIN32__
    case 0x0213: return "WM_NEXTMENU";
    case 0x0214: return "WM_SIZING";
    case 0x0215: return "WM_CAPTURECHANGED";
    case 0x0216: return "WM_MOVING";
    case 0x0218: return "WM_POWERBROADCAST";
    case 0x0219: return "WM_DEVICECHANGE";
#endif  //WIN32

    case 0x0220: return "WM_MDICREATE";
    case 0x0221: return "WM_MDIDESTROY";
    case 0x0222: return "WM_MDIACTIVATE";
    case 0x0223: return "WM_MDIRESTORE";
    case 0x0224: return "WM_MDINEXT";
    case 0x0225: return "WM_MDIMAXIMIZE";
    case 0x0226: return "WM_MDITILE";
    case 0x0227: return "WM_MDICASCADE";
    case 0x0228: return "WM_MDIICONARRANGE";
    case 0x0229: return "WM_MDIGETACTIVE";
    case 0x0230: return "WM_MDISETMENU";
    case 0x0233: return "WM_DROPFILES";

#ifdef  __WIN32__
    case 0x0281: return "WM_IME_SETCONTEXT";
    case 0x0282: return "WM_IME_NOTIFY";
    case 0x0283: return "WM_IME_CONTROL";
    case 0x0284: return "WM_IME_COMPOSITIONFULL";
    case 0x0285: return "WM_IME_SELECT";
    case 0x0286: return "WM_IME_CHAR";
    case 0x0290: return "WM_IME_KEYDOWN";
    case 0x0291: return "WM_IME_KEYUP";
#endif  //WIN32

    case 0x0300: return "WM_CUT";
    case 0x0301: return "WM_COPY";
    case 0x0302: return "WM_PASTE";
    case 0x0303: return "WM_CLEAR";
    case 0x0304: return "WM_UNDO";
    case 0x0305: return "WM_RENDERFORMAT";
    case 0x0306: return "WM_RENDERALLFORMATS";
    case 0x0307: return "WM_DESTROYCLIPBOARD";
    case 0x0308: return "WM_DRAWCLIPBOARD";
    case 0x0309: return "WM_PAINTCLIPBOARD";
    case 0x030A: return "WM_VSCROLLCLIPBOARD";
    case 0x030B: return "WM_SIZECLIPBOARD";
    case 0x030C: return "WM_ASKCBFORMATNAME";
    case 0x030D: return "WM_CHANGECBCHAIN";
    case 0x030E: return "WM_HSCROLLCLIPBOARD";
    case 0x030F: return "WM_QUERYNEWPALETTE";
    case 0x0310: return "WM_PALETTEISCHANGING";
    case 0x0311: return "WM_PALETTECHANGED";

#ifdef __WIN32__
        // common controls messages - although they're not strictly speaking
        // standard, it's nice to decode them nevertheless

        // listview
    case 0x1000 + 0: return "LVM_GETBKCOLOR";
    case 0x1000 + 1: return "LVM_SETBKCOLOR";
    case 0x1000 + 2: return "LVM_GETIMAGELIST";
    case 0x1000 + 3: return "LVM_SETIMAGELIST";
    case 0x1000 + 4: return "LVM_GETITEMCOUNT";
    case 0x1000 + 5: return "LVM_GETITEMA";
    case 0x1000 + 75: return "LVM_GETITEMW";
    case 0x1000 + 6: return "LVM_SETITEMA";
    case 0x1000 + 76: return "LVM_SETITEMW";
    case 0x1000 + 7: return "LVM_INSERTITEMA";
    case 0x1000 + 77: return "LVM_INSERTITEMW";
    case 0x1000 + 8: return "LVM_DELETEITEM";
    case 0x1000 + 9: return "LVM_DELETEALLITEMS";
    case 0x1000 + 10: return "LVM_GETCALLBACKMASK";
    case 0x1000 + 11: return "LVM_SETCALLBACKMASK";
    case 0x1000 + 12: return "LVM_GETNEXTITEM";
    case 0x1000 + 13: return "LVM_FINDITEMA";
    case 0x1000 + 83: return "LVM_FINDITEMW";
    case 0x1000 + 14: return "LVM_GETITEMRECT";
    case 0x1000 + 15: return "LVM_SETITEMPOSITION";
    case 0x1000 + 16: return "LVM_GETITEMPOSITION";
    case 0x1000 + 17: return "LVM_GETSTRINGWIDTHA";
    case 0x1000 + 87: return "LVM_GETSTRINGWIDTHW";
    case 0x1000 + 18: return "LVM_HITTEST";
    case 0x1000 + 19: return "LVM_ENSUREVISIBLE";
    case 0x1000 + 20: return "LVM_SCROLL";
    case 0x1000 + 21: return "LVM_REDRAWITEMS";
    case 0x1000 + 22: return "LVM_ARRANGE";
    case 0x1000 + 23: return "LVM_EDITLABELA";
    case 0x1000 + 118: return "LVM_EDITLABELW";
    case 0x1000 + 24: return "LVM_GETEDITCONTROL";
    case 0x1000 + 25: return "LVM_GETCOLUMNA";
    case 0x1000 + 95: return "LVM_GETCOLUMNW";
    case 0x1000 + 26: return "LVM_SETCOLUMNA";
    case 0x1000 + 96: return "LVM_SETCOLUMNW";
    case 0x1000 + 27: return "LVM_INSERTCOLUMNA";
    case 0x1000 + 97: return "LVM_INSERTCOLUMNW";
    case 0x1000 + 28: return "LVM_DELETECOLUMN";
    case 0x1000 + 29: return "LVM_GETCOLUMNWIDTH";
    case 0x1000 + 30: return "LVM_SETCOLUMNWIDTH";
    case 0x1000 + 31: return "LVM_GETHEADER";
    case 0x1000 + 33: return "LVM_CREATEDRAGIMAGE";
    case 0x1000 + 34: return "LVM_GETVIEWRECT";
    case 0x1000 + 35: return "LVM_GETTEXTCOLOR";
    case 0x1000 + 36: return "LVM_SETTEXTCOLOR";
    case 0x1000 + 37: return "LVM_GETTEXTBKCOLOR";
    case 0x1000 + 38: return "LVM_SETTEXTBKCOLOR";
    case 0x1000 + 39: return "LVM_GETTOPINDEX";
    case 0x1000 + 40: return "LVM_GETCOUNTPERPAGE";
    case 0x1000 + 41: return "LVM_GETORIGIN";
    case 0x1000 + 42: return "LVM_UPDATE";
    case 0x1000 + 43: return "LVM_SETITEMSTATE";
    case 0x1000 + 44: return "LVM_GETITEMSTATE";
    case 0x1000 + 45: return "LVM_GETITEMTEXTA";
    case 0x1000 + 115: return "LVM_GETITEMTEXTW";
    case 0x1000 + 46: return "LVM_SETITEMTEXTA";
    case 0x1000 + 116: return "LVM_SETITEMTEXTW";
    case 0x1000 + 47: return "LVM_SETITEMCOUNT";
    case 0x1000 + 48: return "LVM_SORTITEMS";
    case 0x1000 + 49: return "LVM_SETITEMPOSITION32";
    case 0x1000 + 50: return "LVM_GETSELECTEDCOUNT";
    case 0x1000 + 51: return "LVM_GETITEMSPACING";
    case 0x1000 + 52: return "LVM_GETISEARCHSTRINGA";
    case 0x1000 + 117: return "LVM_GETISEARCHSTRINGW";
    case 0x1000 + 53: return "LVM_SETICONSPACING";
    case 0x1000 + 54: return "LVM_SETEXTENDEDLISTVIEWSTYLE";
    case 0x1000 + 55: return "LVM_GETEXTENDEDLISTVIEWSTYLE";
    case 0x1000 + 56: return "LVM_GETSUBITEMRECT";
    case 0x1000 + 57: return "LVM_SUBITEMHITTEST";
    case 0x1000 + 58: return "LVM_SETCOLUMNORDERARRAY";
    case 0x1000 + 59: return "LVM_GETCOLUMNORDERARRAY";
    case 0x1000 + 60: return "LVM_SETHOTITEM";
    case 0x1000 + 61: return "LVM_GETHOTITEM";
    case 0x1000 + 62: return "LVM_SETHOTCURSOR";
    case 0x1000 + 63: return "LVM_GETHOTCURSOR";
    case 0x1000 + 64: return "LVM_APPROXIMATEVIEWRECT";
    case 0x1000 + 65: return "LVM_SETWORKAREA";

        // tree view
    case 0x1100 + 0: return "TVM_INSERTITEMA";
    case 0x1100 + 50: return "TVM_INSERTITEMW";
    case 0x1100 + 1: return "TVM_DELETEITEM";
    case 0x1100 + 2: return "TVM_EXPAND";
    case 0x1100 + 4: return "TVM_GETITEMRECT";
    case 0x1100 + 5: return "TVM_GETCOUNT";
    case 0x1100 + 6: return "TVM_GETINDENT";
    case 0x1100 + 7: return "TVM_SETINDENT";
    case 0x1100 + 8: return "TVM_GETIMAGELIST";
    case 0x1100 + 9: return "TVM_SETIMAGELIST";
    case 0x1100 + 10: return "TVM_GETNEXTITEM";
    case 0x1100 + 11: return "TVM_SELECTITEM";
    case 0x1100 + 12: return "TVM_GETITEMA";
    case 0x1100 + 62: return "TVM_GETITEMW";
    case 0x1100 + 13: return "TVM_SETITEMA";
    case 0x1100 + 63: return "TVM_SETITEMW";
    case 0x1100 + 14: return "TVM_EDITLABELA";
    case 0x1100 + 65: return "TVM_EDITLABELW";
    case 0x1100 + 15: return "TVM_GETEDITCONTROL";
    case 0x1100 + 16: return "TVM_GETVISIBLECOUNT";
    case 0x1100 + 17: return "TVM_HITTEST";
    case 0x1100 + 18: return "TVM_CREATEDRAGIMAGE";
    case 0x1100 + 19: return "TVM_SORTCHILDREN";
    case 0x1100 + 20: return "TVM_ENSUREVISIBLE";
    case 0x1100 + 21: return "TVM_SORTCHILDRENCB";
    case 0x1100 + 22: return "TVM_ENDEDITLABELNOW";
    case 0x1100 + 23: return "TVM_GETISEARCHSTRINGA";
    case 0x1100 + 64: return "TVM_GETISEARCHSTRINGW";
    case 0x1100 + 24: return "TVM_SETTOOLTIPS";
    case 0x1100 + 25: return "TVM_GETTOOLTIPS";

        // header
    case 0x1200 + 0: return "HDM_GETITEMCOUNT";
    case 0x1200 + 1: return "HDM_INSERTITEMA";
    case 0x1200 + 10: return "HDM_INSERTITEMW";
    case 0x1200 + 2: return "HDM_DELETEITEM";
    case 0x1200 + 3: return "HDM_GETITEMA";
    case 0x1200 + 11: return "HDM_GETITEMW";
    case 0x1200 + 4: return "HDM_SETITEMA";
    case 0x1200 + 12: return "HDM_SETITEMW";
    case 0x1200 + 5: return "HDM_LAYOUT";
    case 0x1200 + 6: return "HDM_HITTEST";
    case 0x1200 + 7: return "HDM_GETITEMRECT";
    case 0x1200 + 8: return "HDM_SETIMAGELIST";
    case 0x1200 + 9: return "HDM_GETIMAGELIST";
    case 0x1200 + 15: return "HDM_ORDERTOINDEX";
    case 0x1200 + 16: return "HDM_CREATEDRAGIMAGE";
    case 0x1200 + 17: return "HDM_GETORDERARRAY";
    case 0x1200 + 18: return "HDM_SETORDERARRAY";
    case 0x1200 + 19: return "HDM_SETHOTDIVIDER";

        // tab control
    case 0x1300 + 2: return "TCM_GETIMAGELIST";
    case 0x1300 + 3: return "TCM_SETIMAGELIST";
    case 0x1300 + 4: return "TCM_GETITEMCOUNT";
    case 0x1300 + 5: return "TCM_GETITEMA";
    case 0x1300 + 60: return "TCM_GETITEMW";
    case 0x1300 + 6: return "TCM_SETITEMA";
    case 0x1300 + 61: return "TCM_SETITEMW";
    case 0x1300 + 7: return "TCM_INSERTITEMA";
    case 0x1300 + 62: return "TCM_INSERTITEMW";
    case 0x1300 + 8: return "TCM_DELETEITEM";
    case 0x1300 + 9: return "TCM_DELETEALLITEMS";
    case 0x1300 + 10: return "TCM_GETITEMRECT";
    case 0x1300 + 11: return "TCM_GETCURSEL";
    case 0x1300 + 12: return "TCM_SETCURSEL";
    case 0x1300 + 13: return "TCM_HITTEST";
    case 0x1300 + 14: return "TCM_SETITEMEXTRA";
    case 0x1300 + 40: return "TCM_ADJUSTRECT";
    case 0x1300 + 41: return "TCM_SETITEMSIZE";
    case 0x1300 + 42: return "TCM_REMOVEIMAGE";
    case 0x1300 + 43: return "TCM_SETPADDING";
    case 0x1300 + 44: return "TCM_GETROWCOUNT";
    case 0x1300 + 45: return "TCM_GETTOOLTIPS";
    case 0x1300 + 46: return "TCM_SETTOOLTIPS";
    case 0x1300 + 47: return "TCM_GETCURFOCUS";
    case 0x1300 + 48: return "TCM_SETCURFOCUS";
    case 0x1300 + 49: return "TCM_SETMINTABWIDTH";
    case 0x1300 + 50: return "TCM_DESELECTALL";

        // toolbar
    case WM_USER+1: return "TB_ENABLEBUTTON";
    case WM_USER+2: return "TB_CHECKBUTTON";
    case WM_USER+3: return "TB_PRESSBUTTON";
    case WM_USER+4: return "TB_HIDEBUTTON";
    case WM_USER+5: return "TB_INDETERMINATE";
    case WM_USER+9: return "TB_ISBUTTONENABLED";
    case WM_USER+10: return "TB_ISBUTTONCHECKED";
    case WM_USER+11: return "TB_ISBUTTONPRESSED";
    case WM_USER+12: return "TB_ISBUTTONHIDDEN";
    case WM_USER+13: return "TB_ISBUTTONINDETERMINATE";
    case WM_USER+17: return "TB_SETSTATE";
    case WM_USER+18: return "TB_GETSTATE";
    case WM_USER+19: return "TB_ADDBITMAP";
    case WM_USER+20: return "TB_ADDBUTTONS";
    case WM_USER+21: return "TB_INSERTBUTTON";
    case WM_USER+22: return "TB_DELETEBUTTON";
    case WM_USER+23: return "TB_GETBUTTON";
    case WM_USER+24: return "TB_BUTTONCOUNT";
    case WM_USER+25: return "TB_COMMANDTOINDEX";
    case WM_USER+26: return "TB_SAVERESTOREA";
    case WM_USER+76: return "TB_SAVERESTOREW";
    case WM_USER+27: return "TB_CUSTOMIZE";
    case WM_USER+28: return "TB_ADDSTRINGA";
    case WM_USER+77: return "TB_ADDSTRINGW";
    case WM_USER+29: return "TB_GETITEMRECT";
    case WM_USER+30: return "TB_BUTTONSTRUCTSIZE";
    case WM_USER+31: return "TB_SETBUTTONSIZE";
    case WM_USER+32: return "TB_SETBITMAPSIZE";
    case WM_USER+33: return "TB_AUTOSIZE";
    case WM_USER+35: return "TB_GETTOOLTIPS";
    case WM_USER+36: return "TB_SETTOOLTIPS";
    case WM_USER+37: return "TB_SETPARENT";
    case WM_USER+39: return "TB_SETROWS";
    case WM_USER+40: return "TB_GETROWS";
    case WM_USER+42: return "TB_SETCMDID";
    case WM_USER+43: return "TB_CHANGEBITMAP";
    case WM_USER+44: return "TB_GETBITMAP";
    case WM_USER+45: return "TB_GETBUTTONTEXTA";
    case WM_USER+75: return "TB_GETBUTTONTEXTW";
    case WM_USER+46: return "TB_REPLACEBITMAP";
    case WM_USER+47: return "TB_SETINDENT";
    case WM_USER+48: return "TB_SETIMAGELIST";
    case WM_USER+49: return "TB_GETIMAGELIST";
    case WM_USER+50: return "TB_LOADIMAGES";
    case WM_USER+51: return "TB_GETRECT";
    case WM_USER+52: return "TB_SETHOTIMAGELIST";
    case WM_USER+53: return "TB_GETHOTIMAGELIST";
    case WM_USER+54: return "TB_SETDISABLEDIMAGELIST";
    case WM_USER+55: return "TB_GETDISABLEDIMAGELIST";
    case WM_USER+56: return "TB_SETSTYLE";
    case WM_USER+57: return "TB_GETSTYLE";
    case WM_USER+58: return "TB_GETBUTTONSIZE";
    case WM_USER+59: return "TB_SETBUTTONWIDTH";
    case WM_USER+60: return "TB_SETMAXTEXTROWS";
    case WM_USER+61: return "TB_GETTEXTROWS";
    case WM_USER+41: return "TB_GETBITMAPFLAGS";

#endif //WIN32

    default:
        static char s_szBuf[128];
        sprintf(s_szBuf, "<unknown message = %d>", message);
        return s_szBuf;
  }
}
#endif //WXDEBUG
