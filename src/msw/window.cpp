/////////////////////////////////////////////////////////////////////////////
// Name:        windows.cpp
// Purpose:     wxWindow
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
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

#if     USE_OWNER_DRAWN
#include "wx/ownerdrw.h"
#endif

#if     USE_DRAG_AND_DROP
#include "wx/msw/ole/droptgt.h"
#endif

#include "wx/menuitem.h"
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

#define WINDOW_MARGIN 3	// This defines sensitivity of Leave events

wxMenu *wxCurrentPopupMenu = NULL;
extern wxList wxPendingDelete;

void wxRemoveHandleAssociation(wxWindow *win);
void wxAssociateWinWithHandle(HWND hWnd, wxWindow *win);
wxWindow *wxFindWinFromHandle(WXHWND hWnd);

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxWindow, wxEvtHandler)

BEGIN_EVENT_TABLE(wxWindow, wxEvtHandler)
  EVT_CHAR(wxWindow::OnChar)
  EVT_SIZE(wxWindow::OnSize)
  EVT_ERASE_BACKGROUND(wxWindow::OnEraseBackground)
  EVT_SYS_COLOUR_CHANGED(wxWindow::OnSysColourChanged)
  EVT_INIT_DIALOG(wxWindow::OnInitDialog)
  EVT_IDLE(wxWindow::OnIdle)
END_EVENT_TABLE()

#endif

// Find an item given the MS Windows id
wxWindow *wxWindow::FindItem(const int id) const
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
wxWindow *wxWindow::FindItemByHWND(const WXHWND hWnd, bool controlOnly) const
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
bool wxWindow::MSWCommand(const WXUINT WXUNUSED(param), const WXWORD WXUNUSED(id))
{
  return FALSE;
}

bool wxWindow::MSWNotify(const WXWPARAM WXUNUSED(wParam), const WXLPARAM WXUNUSED(lParam))
{
  return FALSE;
}

void wxWindow::PreDelete(const WXHDC WXUNUSED(dc))
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

  m_backgroundColour = settings.GetSystemColour(wxSYS_COLOUR_WINDOW) ; ;
  m_foregroundColour = *wxBLACK;
  m_defaultForegroundColour = *wxBLACK ;
  m_defaultBackgroundColour = settings.GetSystemColour(wxSYS_COLOUR_3DFACE) ;

/*
    wxColour(GetRValue(GetSysColor(COLOR_WINDOW)),
  	GetGValue(GetSysColor(COLOR_BTNFACE)), GetBValue(GetSysColor(COLOR_BTNFACE)));
*/

  // wxWnd
  m_lastMsg = 0;
  m_lastWParam = 0;
  m_lastLParam = 0;
  m_acceleratorTable = 0;
  m_hMenu = 0;

  m_xThumbSize = 0;
  m_yThumbSize = 0;
  m_backgroundTransparent = FALSE;

  m_lastXPos = (float)-1.0;
  m_lastYPos = (float)-1.0;
  m_lastEvent = -1;
  m_returnCode = 0;

#if  USE_DRAG_AND_DROP
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

  // wxWnd
  MSWDetachWindowMenu();

  wxRemoveHandleAssociation(this);

  // TODO for backward compatibility
#if 0
  // WX_CANVAS
  if (m_windowDC)
  {
    HWND hWnd = (HWND) GetHWND();
    HDC dc = ::GetDC(hWnd);
    m_windowDC->SelectOldObjects (dc);
    ReleaseDC(hWnd, dc);
    delete m_windowDC;
  }
#endif

  if (m_windowParent)
    m_windowParent->RemoveChild(this);

  DestroyChildren();

  if (m_hWnd)
    ::DestroyWindow((HWND)m_hWnd);
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

//  if (GetFont() && GetFont()->Ok())
//    GetFont()->ReleaseResource();

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
bool wxWindow::Create(wxWindow *parent, const wxWindowID id,
           const wxPoint& pos,
           const wxSize& size,
           const long style,
           const wxString& name)
{
  // Generic
  m_isBeingDeleted = FALSE;
  m_windowId = 0;
  m_isShown = TRUE;
  m_windowStyle = 0;
  m_windowParent = NULL;
  m_windowEventHandler = this;
//  m_windowFont = NULL;
  // We don't wish internal (potentially transient) fonts to be found
  // by FindOrCreate
//  wxTheFontList->RemoveFont(& m_windowFont);
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
#if  USE_DRAG_AND_DROP
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
//  m_paintHDC = 0;
//  m_tempHDC = 0;
  m_oldWndProc = 0;
#ifndef __WIN32__
  m_globalHandle = 0;
#endif
  m_useCtl3D = FALSE;
  m_defaultItem = NULL;
  m_windowParent = NULL;
//  m_windowDC = NULL;
  m_mouseInWindow = FALSE;
  if (!parent)
    return FALSE;

  if (parent) parent->AddChild(this);

  // wxWnd
  m_lastMsg = 0;
  m_lastWParam = 0;
  m_lastLParam = 0;
  m_acceleratorTable = 0;
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

  wxSystemSettings settings;

  m_backgroundColour = settings.GetSystemColour(wxSYS_COLOUR_WINDOW) ; ;
  m_foregroundColour = *wxBLACK;
  m_defaultForegroundColour = *wxBLACK ;
  m_defaultBackgroundColour = settings.GetSystemColour(wxSYS_COLOUR_3DFACE) ;
/*
  m_defaultBackgroundColour = wxColour(GetRValue(GetSysColor(COLOR_BTNFACE)),
  	GetGValue(GetSysColor(COLOR_BTNFACE)), GetBValue(GetSysColor(COLOR_BTNFACE)));
*/

  m_windowStyle = style;

  DWORD msflags = 0;
  if (style & wxBORDER)
    msflags |= WS_BORDER;
  if (style & wxTHICK_FRAME)
    msflags |= WS_THICKFRAME;
  // TODO: probably make WS_CLIPCHILDREN this a setting in wx/setup.h,
  // to reduce flicker with the trade-off that groupboxes must paint in a solid
  // colour (so your control order must be correct, and you can't easily draw a
  // transparent group).
  msflags |= WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN;

  bool want3D;
  WXDWORD exStyle = Determine3DEffects(WS_EX_CLIENTEDGE, &want3D) ;

  // Even with extended styles, need to combine with WS_BORDER
  // for them to look right.
  if (want3D || (m_windowStyle & wxSIMPLE_BORDER) || (m_windowStyle & wxRAISED_BORDER) ||
       (m_windowStyle & wxSUNKEN_BORDER) || (m_windowStyle & wxDOUBLE_BORDER))
    msflags |= WS_BORDER;

  m_mouseInWindow = FALSE ;

  if ( id == -1 )
  	m_windowId = (int)NewControlId();
  else
	m_windowId = id;

  MSWCreate(m_windowId, (wxWindow *)parent, wxCanvasClassName, this, NULL, x, y, width, height, msflags,
    NULL, exStyle);

  return TRUE;
}

void wxWindow::SetFocus(void)
{
  HWND hWnd = (HWND) GetHWND();
  if (hWnd)
    ::SetFocus(hWnd);
}

void wxWindow::Enable(const bool enable)
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
  DELETEP(m_pDropTarget);
  m_pDropTarget = pDropTarget;
  if ( m_pDropTarget != 0 )
    m_pDropTarget->Register(m_hWnd);
}

#endif

//old style file-manager drag&drop support
// I think we should retain the old-style
// DragAcceptFiles in parallel with SetDropTarget.
// JACS
void wxWindow::DragAcceptFiles(const bool accept)
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

  *x = pt.x;
  *y = pt.y;
}

void wxWindow::ClientToScreen(int *x, int *y) const
{
  HWND hWnd = (HWND) GetHWND();
  POINT pt;
  pt.x = *x;
  pt.y = *y;
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
// For XView, this is the same as GetSize
void wxWindow::GetClientSize(int *x, int *y) const
{
  HWND hWnd = (HWND) GetHWND();
  RECT rect;
  GetClientRect(hWnd, &rect);
  *x = rect.right;
  *y = rect.bottom;
}

void wxWindow::SetSize(const int x, const int y, const int width, const int height, const int sizeFlags)
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

  int currentW,currentH;
  GetSize(&currentW, &currentH);
  if (width == -1)
    actualWidth = currentW ;
  if (height == -1)
    actualHeight = currentH ;

  HWND hWnd = (HWND) GetHWND();
  if (hWnd)
    MoveWindow(hWnd, actualX, actualY, actualWidth, actualHeight, (BOOL)TRUE);

/* Not needed? should be called anyway via MoveWindow
  if (!(width == -1) && (height == -1))
  {
#if WXWIN_COMPATIBILITY
  GetEventHandler()->OldOnSize(width, height);
#else
  wxSizeEvent event(wxSize(width, height), m_windowId);
  event.eventObject = this;
  GetEventHandler()->ProcessEvent(event);
#endif
  }
*/
}

void wxWindow::SetClientSize(const int width, const int height)
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
#if WXWIN_COMPATIBILITY
  GetEventHandler()->OldOnSize(width, height);
#else
  wxSizeEvent event(wxSize(width, height), m_windowId);
  event.m_eventObject = this;
  GetEventHandler()->ProcessEvent(event);
#endif
}

bool wxWindow::Show(const bool show)
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
                           int *descent, int *externalLeading, const wxFont *theFont, const bool) const
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
//    fontToUse->UseResource();
    
//    fontToUse->RealizeResource();
    if ((fnt=(HFONT) fontToUse->GetResourceHandle()))
      was = SelectObject(dc,fnt) ;
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

#if WXWIN_COMPATIBILITY
void wxWindow::GetTextExtent(const wxString& string, float *x, float *y,
                             float *descent,
                             float *externalLeading,
                             const wxFont *theFont, const bool use16) const
  {
    int x1, y1, descent1, externalLeading1;
    GetTextExtent(string, &x1, &y1, &descent1, &externalLeading1, theFont, use16);
    *x = x1; *y = y1;
    if ( descent )
        *descent = descent1;
    if ( externalLeading )
        *externalLeading = externalLeading1;
  }
#endif

void wxWindow::Refresh(const bool eraseBack, const wxRectangle *rect)
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

// TODO: Are these really necessary now?
/*
WXHDC wxWindow::GetHDC(void) const
{
  wxWindow *nonConst = (wxWindow *)this;
  if (m_paintHDC)
    return(m_paintHDC) ;
  nonConst->m_tempHDC = (WXHDC) ::GetDC((HWND) GetHWND()) ;
  return(m_tempHDC) ;
}

void wxWindow::ReleaseHDC(void)
{
  // We're within an OnPaint: it'll be released.
  if (m_paintHDC)
    return ;

  ::ReleaseDC((HWND) GetHWND(),(HDC) m_tempHDC) ;
}
*/

// Hook for new window just as it's being created,
// when the window isn't yet associated with the handle
wxWindow *wxWndHook = NULL;

/*
#if HAVE_SOCKET
// DDE Interface Handler
extern	"C" {
  long	ddeWindowProc(HWND hwnd,UINT message,WPARAM wparam,LPARAM lparam);
  void __ddeUnblock(HWND hWnd, WPARAM wParam);
};
#endif
*/

// Main Windows 3 window proc
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
#if (DEBUG > 1)
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
/* Don't know why this was here
    if (message == WM_SETFONT)
      return 0;
    else if (message == WM_INITDIALOG)
      return TRUE;
*/
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
#if 0
  switch (message)
  {
        case WM_INITDIALOG:
        case WM_ACTIVATE:
        case WM_SETFOCUS:
        case WM_KILLFOCUS:
	case WM_CREATE:
	case WM_PAINT:
	case WM_QUERYDRAGICON:
        case WM_SIZE:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDBLCLK:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
        case WM_MOUSEMOVE:
//        case WM_DESTROY:
        case WM_COMMAND:
        case WM_NOTIFY:
        case WM_MENUSELECT:
        case WM_INITMENUPOPUP:
        case WM_DRAWITEM:
        case WM_MEASUREITEM:
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_CHAR: // Always an ASCII character
        case WM_HSCROLL:
        case WM_VSCROLL:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORMSGBOX:
        case WM_CTLCOLORSCROLLBAR:
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLOREDIT:
        case WM_SYSCOLORCHANGE:
        case WM_ERASEBKGND:
        case WM_MDIACTIVATE:
        case WM_DROPFILES:
        case WM_QUERYENDSESSION:
//        case WM_CLOSE:
        case WM_GETMINMAXINFO:
            return MSWDefWindowProc(message, wParam, lParam );
    }
#endif


  HWND hWnd = (HWND)m_hWnd;

  switch (message)
  {
/*
        case WM_SETFONT:
        {
          return 0;
        }
*/
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

        case WM_WINDOWPOSCHANGING:
        {
          WINDOWPOS *pos = (WINDOWPOS *)lParam;
          MSWOnWindowPosChanging((void *)pos);
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
        {
//            return Default();

            if (wParam == VK_SHIFT)
            	return Default();

            else if (wParam == VK_CONTROL)
            	return Default();

            // Avoid duplicate messages to OnChar
            else if ((wParam != VK_ESCAPE) && (wParam != VK_SPACE) && (wParam != VK_RETURN) && (wParam != VK_BACK) && (wParam != VK_TAB))
	    	{
              MSWOnChar((WORD)wParam, lParam);
              if (::GetKeyState(VK_CONTROL)&0x100?TRUE:FALSE)
			    return Default();
	    	}
			else
				return Default();
            break;
        }
        case WM_KEYUP:
        {
/*
            if (wParam == VK_SHIFT)
              wxShiftDown = FALSE;
            else if (wParam == VK_CONTROL)
              wxControlDown = FALSE;
*/
            break;
        }
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
        case WM_ERASEBKGND:
        {
          // Prevents flicker when dragging
          if (IsIconic(hWnd)) return 1;

          // EXPERIMENTAL
//          return 1;
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
            return MSWOnClose();
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

/*
#if HAVE_SOCKET
        case WM_TIMER:
	{
          __ddeUnblock(hWnd, wParam);
          break;
	}

 	case ASYNC_SELECT_MESSAGE:
 	    return ddeWindowProc(hWnd,message,wParam,lParam);
#endif
*/

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
#if 0

#if DEBUG > 1
  wxDebugMsg("wxWindow::MSWDestroyWindow %d\n", handle);
#endif
  MSWDetachWindowMenu();
//  SetWindowLong(handle, 0, (long)0);
  HWND oldHandle = handle;
  handle = NULL;

  ::DestroyWindow(oldHandle);

  // Menu is destroyed explicitly by wxMDIChild::DestroyWindow,
  // or when Windows HWND is deleted if MDI parent or
  // SDI frame.
/*
  if (m_hMenu)
  {
    ::DestroyMenu(m_hMenu);
    m_hMenu = 0;
  }
 */
#endif
}

void wxWindow::MSWCreate(const int id, wxWindow *parent, const char *wclass, wxWindow *wx_win, const char *title,
                    const int x, const int y, const int width, const int height,
                    const WXDWORD style, const char *dialog_template, const WXDWORD extendedStyle)
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
      
    m_hWnd = (WXHWND) CreateWindowEx(extendedStyle, wclass,
                title,
                style,
                x1, y1,
                width1, height1,
//                hParent, NULL, wxGetInstance(),
                hParent, (HMENU)controlId, wxGetInstance(),
                NULL);

    if (m_hWnd == 0)
    {
       char buf[300];
       sprintf(buf, "Can't create window of class %s! Weird.\nPossible Windows 3.x compatibility problem?",
         wclass);
       wxFatalError(buf,
                  "Fatal wxWindows Error");
    }
  }
  wxWndHook = NULL;
  wxWinHandleList->Append((long)m_hWnd, this);

#if DEBUG > 1
  wxDebugMsg("wxWindow::MSWCreate %d\n", m_hWnd);
#endif
}

void wxWindow::MSWOnCreate(WXLPCREATESTRUCT WXUNUSED(cs))
{
}

bool wxWindow::MSWOnClose(void)
{
#if DEBUG > 1
  wxDebugMsg("wxWindow::MSWOnClose %d\n", handle);
#endif
  return FALSE;
}

bool wxWindow::MSWOnDestroy(void)
{
#if DEBUG > 1
  wxDebugMsg("wxWindow::MSWOnDestroy %d\n", handle);
#endif
  // delete our log target if we've got one
#if USE_DRAG_AND_DROP
    if ( m_pDropTarget != 0 ) {
      m_pDropTarget->Revoke(m_hWnd);

      delete m_pDropTarget;
      m_pDropTarget = NULL;
    }
#endif

  return TRUE;
}

// Deal with child commands from buttons etc.

bool wxWindow::MSWOnNotify(const WXWPARAM wParam, const WXLPARAM lParam)
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

void wxWindow::MSWOnMenuHighlight(const WXWORD WXUNUSED(item), const WXWORD WXUNUSED(flags), const WXHMENU WXUNUSED(sysmenu))
{
#if DEBUG > 1
  wxDebugMsg("wxWindow::MSWOnMenuHighlight %d\n", handle);
#endif
}

void wxWindow::MSWOnInitMenuPopup(const WXHMENU menu, const int pos, const bool isSystem)
{
  if (!isSystem)
    OldOnInitMenuPopup(pos);
}

bool wxWindow::MSWOnActivate(const int state, const bool WXUNUSED(minimized), const WXHWND WXUNUSED(activate))
{
#if DEBUG > 1
  wxDebugMsg("wxWindow::MSWOnActivate %d\n", handle);
#endif

#if WXWIN_COMPATIBILITY
  GetEventHandler()->OldOnActivate(((state == WA_ACTIVE) || (state == WA_CLICKACTIVE)));
#else
  wxActivateEvent event(wxEVT_ACTIVATE, ((state == WA_ACTIVE) || (state == WA_CLICKACTIVE)),
	  m_windowId);
  event.m_eventObject = this;
  GetEventHandler()->ProcessEvent(event);
#endif
  return 0;
}

bool wxWindow::MSWOnSetFocus(const WXHWND WXUNUSED(hwnd))
{
#if DEBUG > 1
  wxDebugMsg("wxWindow::MSWOnSetFocus %d\n", m_hWnd);
#endif
    // Deal with caret
    if (m_caretEnabled && (m_caretWidth > 0) && (m_caretHeight > 0))
    {
      ::CreateCaret((HWND) GetHWND(), NULL, m_caretWidth, m_caretHeight);
      if (m_caretShown)
        ::ShowCaret((HWND) GetHWND());
    }

#if WXWIN_COMPATIBILITY
  GetEventHandler()->OldOnSetFocus();
#else
  wxFocusEvent event(wxEVT_SET_FOCUS, m_windowId);
  event.m_eventObject = this;
  GetEventHandler()->ProcessEvent(event);
#endif
    return TRUE;
}

bool wxWindow::MSWOnKillFocus(const WXHWND WXUNUSED(hwnd))
{
#if DEBUG > 1
  wxDebugMsg("wxWindow::MSWOnKillFocus %d\n", m_hWnd);
#endif
    // Deal with caret
    if (m_caretEnabled)
    {
      ::DestroyCaret();
    }

#if WXWIN_COMPATIBILITY
  GetEventHandler()->OldOnKillFocus();
#else
  wxFocusEvent event(wxEVT_KILL_FOCUS, m_windowId);
  event.m_eventObject = this;
  GetEventHandler()->ProcessEvent(event);
#endif
  return TRUE;
}

void wxWindow::MSWOnDropFiles(const WXWPARAM wParam)
{
#if DEBUG > 1
  wxDebugMsg("wxWindow::MSWOnDropFiles %d\n", m_hWnd);
#endif

  HANDLE hFilesInfo = (HANDLE)wParam;
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

  GetEventHandler()->ProcessEvent(event);

  delete[] files;
}

bool wxWindow::MSWOnDrawItem(const int id, WXDRAWITEMSTRUCT *itemStruct)
{
#if USE_OWNER_DRAWN
    if ( id == 0 ) {    // is it a menu item?
      DRAWITEMSTRUCT *pDrawStruct = (DRAWITEMSTRUCT *)itemStruct;
      wxMenuItem *pMenuItem = (wxMenuItem *)(pDrawStruct->itemData);
      wxCHECK_RET( pMenuItem->IsKindOf(CLASSINFO(wxMenuItem)), FALSE );

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
#if USE_DYNAMIC_CLASSES
  if (item && item->IsKindOf(CLASSINFO(wxControl)))
  {
    return ((wxControl *)item)->MSWOnDraw(itemStruct);
  }
  else
#endif
    return FALSE;
}

bool wxWindow::MSWOnMeasureItem(const int id, WXMEASUREITEMSTRUCT *itemStruct)
{
#if USE_OWNER_DRAWN
    if ( id == 0 ) {    // is it a menu item?
      MEASUREITEMSTRUCT *pMeasureStruct = (MEASUREITEMSTRUCT *)itemStruct;
      wxMenuItem *pMenuItem = (wxMenuItem *)(pMeasureStruct->itemData);
      wxCHECK_RET( pMenuItem->IsKindOf(CLASSINFO(wxMenuItem)), FALSE );

      return pMenuItem->OnMeasureItem(&pMeasureStruct->itemWidth, 
                                      &pMeasureStruct->itemHeight);
    }
#endif  // owner-drawn menus

  wxWindow *item = FindItem(id);
#if USE_DYNAMIC_CLASSES
  if (item && item->IsKindOf(CLASSINFO(wxControl)))
  {
    return ((wxControl *)item)->MSWOnMeasure(itemStruct);
  }
  else
#endif
    return FALSE;
}

WXHBRUSH wxWindow::MSWOnCtlColor(const WXHDC pDC, const WXHWND pWnd, const WXUINT nCtlColor,
                         const WXUINT message, const WXWPARAM wParam, const WXLPARAM lParam)
{
#if DEBUG > 1
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
WXHBRUSH wxWindow::OnCtlColor(const WXHDC pDC, const WXHWND pWnd, const WXUINT nCtlColor,
			WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
    return (WXHBRUSH) MSWDefWindowProc(message, wParam, lParam);
}

bool wxWindow::MSWOnColorChange(const WXHWND hWnd, const WXUINT message, const WXWPARAM wParam, const WXLPARAM lParam)
{
  wxSysColourChangedEvent event;
  event.m_eventObject = this;

  // Check if app handles this.
  if (GetEventHandler()->ProcessEvent(event))
    return 0;

#if WXWIN_COMPATIBILITY
  if (GetEventHandler()->OldOnSysColourChange())
    return 0;
#endif

  // We didn't process it
  return 1;
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
  	return ::CallWindowProc(CASTWNDPROC (FARPROC) m_oldWndProc, (HWND) GetHWND(), (UINT) nMsg, (WPARAM) wParam, (LPARAM) lParam);
  else
  	return ::DefWindowProc((HWND) GetHWND(), nMsg, wParam, lParam);
}

long wxWindow::Default()
{
  return this->MSWDefWindowProc(m_lastMsg, m_lastWParam, m_lastLParam);
}

bool wxWindow::MSWProcessMessage(WXMSG* pMsg)
{
  if (!m_hWnd)
    return FALSE;
  else
  {
    // Suggestion by Andrew Davison to allow
    // a panel to accept character input in user edit mode
    
    // OK, what we probably want to do here for wxWin 2.0
    // is have a window style to indicate whether the window
    // should process dialog-style input, since we can't
    // otherwise tell whether it's supposed to do tab traversal
    // or not.
    if (GetWindowStyleFlag() & wxTAB_TRAVERSAL)
      return (::IsDialogMessage((HWND) m_hWnd, (MSG *)pMsg) != 0);
    else
      return FALSE;
  }
}

long wxWindow::MSWOnMDIActivate(const long WXUNUSED(flag), const WXHWND WXUNUSED(activate), const WXHWND WXUNUSED(deactivate))
{
#if DEBUG > 1
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
#if WXWIN_COMPATIBILITY
  GetEventHandler()->OldOnPaint();
#else
  wxPaintEvent event(m_windowId);
  event.m_eventObject = this;
  GetEventHandler()->ProcessEvent(event);
#endif
  return TRUE;
  
#if 0

#if DEBUG > 1
  wxDebugMsg("wxWindow::MSWOnPaint %d\n", m_hWnd);
#endif
#ifdef __WIN32__
  HRGN	tRgn=CreateRectRgn(0,0,0,0);	//Dummy call to get a handle!
  if (GetUpdateRgn(m_hWnd, tRgn, FALSE))
#else
  RECT tRect;
  if (GetUpdateRect((HWND) m_hWnd, &tRect, FALSE))
#endif
  {
    PAINTSTRUCT ps;
    // Hold a pointer to the dc so long as the OnPaint() message
    // is being processed
    HDC dc = BeginPaint(m_hWnd, &ps);
    bool isPanel = IsKindOf(CLASSINFO(wxWindow));
    m_paintHDC = (WXHDC) dc;
    RECT updateRect1 = ps.rcPaint;
	m_updateRect.x = updateRect1.left;
	m_updateRect.y = updateRect1.top;
	m_updateRect.width = updateRect1.right - updateRect1.left;
	m_updateRect.height = updateRect1.bottom - updateRect1.top;

    GetEventHandler()->OldOnPaint();

    m_paintHDC = 0;
    EndPaint((HWND) m_hWnd, &ps);
#ifdef __WIN32__
    DeleteObject(tRgn);
#endif

    if (isPanel)
      // Do default processing
      return FALSE;
    else
      return TRUE;
  }
#ifdef __WIN32__
  DeleteObject(tRgn);
#endif
  return FALSE;
#endif
}

void wxWindow::MSWOnSize(const int w, const int h, const WXUINT WXUNUSED(flag))
{
  if (m_inOnSize)
    return;
    
#if DEBUG > 1
  wxDebugMsg("wxWindow::MSWOnSize %d\n", m_hWnd);
#endif
  if (!m_hWnd)
    return;

  m_inOnSize = TRUE;

#if WXWIN_COMPATIBILITY
  GetEventHandler()->OldOnSize(w, h);
#else
  wxSizeEvent event(wxSize(w, h), m_windowId);
  event.m_eventObject = this;
  GetEventHandler()->ProcessEvent(event);
#endif

  m_inOnSize = FALSE;
}

void wxWindow::MSWOnWindowPosChanging(void *WXUNUSED(lpPos))
{
    Default();
}

// Deal with child commands from buttons etc.
bool wxWindow::MSWOnCommand(const WXWORD id, const WXWORD cmd, const WXHWND WXUNUSED(control))
{
#if DEBUG > 1
  wxDebugMsg("wxWindow::MSWOnCommand\n");
#endif
  if (wxCurrentPopupMenu)
  {
    wxMenu *popupMenu = wxCurrentPopupMenu;
    wxCurrentPopupMenu = NULL;
    bool succ = popupMenu->MSWCommand(cmd, id);
    return succ;
  }
#if DEBUG > 1
  char buf[80];
  sprintf(buf, "Looking for item %d...\n", id);
  wxDebugMsg(buf);
#endif

  wxWindow *item = FindItem(id);
  if (item)
  {
    bool value = item->MSWCommand(cmd, id);
#if DEBUG > 1
    if (value)
      wxDebugMsg("MSWCommand succeeded\n");
    else
      wxDebugMsg("MSWCommand failed\n");
#endif
    return value;
  }
  else
  {
#if DEBUG > 1
    wxDebugMsg("Could not find item!\n");
    char buf[100];
    wxDebugMsg("Item ids for this panel:\n");

    wxNode *current = GetChildren()->First();
    while (current)
    {
      wxObject *obj = (wxObject *)current->Data() ;
      if (obj->IsKindOf(CLASSINFO(wxControl)))
      {
        wxControl *item = (wxControl *)current->Data();
        sprintf(buf, "  %d\n", (int)item->m_windowId);
        wxDebugMsg(buf);
      }
      current = current->Next();  
    }
    wxYield();
#endif
    return FALSE;
  }
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

void wxWindow::MSWOnLButtonDown(const int x, const int y, const WXUINT flags)
{
#if 0 // defined(__WIN32__) && !defined(WIN95)
  // DClick not clean supported on Win3.1, except if someone know
  // how to emulate Sleep()...
  // This means that your app will receive Down-Up-Dclick sequences
  // rather than Dclick
  if (m_doubleClickAllowed)
  {
    UINT time = GetDoubleClickTime() ;
    Sleep(time) ;
    MSG dummy ;
    if (PeekMessage(&dummy,m_hWnd,
                    WM_LBUTTONDBLCLK,WM_LBUTTONDBLCLK,
                    PM_NOREMOVE)
       )
    {
      PeekMessage(&dummy,m_hWnd,WM_LBUTTONUP,WM_LBUTTONUP,PM_REMOVE);
      return;
    }
  }
#endif

//wxDebugMsg("LButtonDown\n") ;
  wxMouseEvent event(wxEVENT_TYPE_LEFT_DOWN);

/*
  float px = (float)x;
  float py = (float)y;

  MSWDeviceToLogical(&px, &py);

  CalcUnscrolledPosition((int)px, (int)py, &event.m_x, &event.m_y);
*/

  event.m_x = x; event.m_y = y;
  event.m_shiftDown = ((flags & MK_SHIFT) != 0);
  event.m_controlDown = ((flags & MK_CONTROL) != 0);
  event.m_leftDown = ((flags & MK_LBUTTON) != 0);
  event.m_middleDown = ((flags & MK_MBUTTON) != 0);
  event.m_rightDown = ((flags & MK_RBUTTON) != 0);
  event.SetTimestamp(wxApp::sm_lastMessageTime); /* MATTHEW: timeStamp */
  event.m_eventObject = this;

  m_lastXPos = event.m_x; m_lastYPos = event.m_y; m_lastEvent = wxEVENT_TYPE_LEFT_DOWN;
  GetEventHandler()->OldOnMouseEvent(event);
}

void wxWindow::MSWOnLButtonUp(const int x, const int y, const WXUINT flags)
{
//wxDebugMsg("LButtonUp\n") ;
  wxMouseEvent event(wxEVENT_TYPE_LEFT_UP);

/*
  float px = (float)x;
  float py = (float)y;

  MSWDeviceToLogical(&px, &py);

  CalcUnscrolledPosition((int)px, (int)py, &event.m_x, &event.m_y);
*/

  event.m_x = x; event.m_y = y;
  event.m_shiftDown = ((flags & MK_SHIFT) != 0);
  event.m_controlDown = ((flags & MK_CONTROL) != 0);
  event.m_leftDown = ((flags & MK_LBUTTON) != 0);
  event.m_middleDown = ((flags & MK_MBUTTON) != 0);
  event.m_rightDown = ((flags & MK_RBUTTON) != 0);
  event.SetTimestamp(wxApp::sm_lastMessageTime); /* MATTHEW: timeStamp */
  event.m_eventObject = this;

  m_lastXPos = event.m_x; m_lastYPos = event.m_y; m_lastEvent = wxEVENT_TYPE_LEFT_UP;

  GetEventHandler()->OldOnMouseEvent(event);
}

void wxWindow::MSWOnLButtonDClick(const int x, const int y, const WXUINT flags)
{
//wxDebugMsg("LButtonDClick\n") ;
  /* MATTHEW: If dclick not allowed, generate another single-click */
  wxMouseEvent event(m_doubleClickAllowed ?
		     wxEVENT_TYPE_LEFT_DCLICK : wxEVENT_TYPE_LEFT_DOWN);

/*
  float px = (float)x;
  float py = (float)y;

  MSWDeviceToLogical(&px, &py);

  CalcUnscrolledPosition((int)px, (int)py, &event.m_x, &event.m_y);
*/

  event.m_x = x; event.m_y = y;
  event.m_shiftDown = ((flags & MK_SHIFT) != 0);
  event.m_controlDown = ((flags & MK_CONTROL) != 0);
  event.m_leftDown = ((flags & MK_LBUTTON != 0));
  event.m_middleDown = ((flags & MK_MBUTTON) != 0);
  event.m_rightDown = ((flags & MK_RBUTTON) != 0);
  event.SetTimestamp(wxApp::sm_lastMessageTime); /* MATTHEW: timeStamp */
  event.m_eventObject = this;

  m_lastXPos = event.m_x; m_lastYPos = event.m_y; m_lastEvent = wxEVENT_TYPE_LEFT_DCLICK;

//  if (m_doubleClickAllowed)
     GetEventHandler()->OldOnMouseEvent(event);
}

void wxWindow::MSWOnMButtonDown(const int x, const int y, const WXUINT flags)
{
#if 0 // defined(__WIN32__) && !defined(__WIN95__)
  // DClick not clean supported on Win3.1, except if someone know
  // how to emulate Sleep()...
  // This means that your app will receive Down-Up-Dclick sequences
  // rather than Dclick
  if (m_doubleClickAllowed)
  {
    UINT time = GetDoubleClickTime() ;
    Sleep(time) ;
    MSG dummy ;
    if (PeekMessage(&dummy,m_hWnd,
                    WM_MBUTTONDBLCLK,WM_MBUTTONDBLCLK,
                    PM_NOREMOVE)
       )
    {
      PeekMessage(&dummy,m_hWnd,WM_MBUTTONUP,WM_MBUTTONUP,PM_REMOVE);
      return;
    }
  }
#endif

//wxDebugMsg("MButtonDown\n") ;
  wxMouseEvent event(wxEVENT_TYPE_MIDDLE_DOWN);

/*
  float px = (float)x;
  float py = (float)y;

  MSWDeviceToLogical(&px, &py);

  CalcUnscrolledPosition((int)px, (int)py, &event.m_x, &event.m_y);
*/

  event.m_x = x; event.m_y = y;
  event.m_shiftDown = ((flags & MK_SHIFT) != 0);
  event.m_controlDown = ((flags & MK_CONTROL) != 0);
  event.m_leftDown = ((flags & MK_LBUTTON) != 0);
  event.m_middleDown = ((flags & MK_MBUTTON) != 0);
  event.m_rightDown = ((flags & MK_RBUTTON) != 0);
  event.SetTimestamp(wxApp::sm_lastMessageTime); /* MATTHEW: timeStamp */
  event.m_eventObject = this;

  m_lastXPos = event.m_x; m_lastYPos = event.m_y; m_lastEvent = wxEVENT_TYPE_LEFT_DOWN;
  GetEventHandler()->OldOnMouseEvent(event);
}

void wxWindow::MSWOnMButtonUp(const int x, const int y, const WXUINT flags)
{
//wxDebugMsg("MButtonUp\n") ;
  wxMouseEvent event(wxEVENT_TYPE_MIDDLE_UP);

/*
  float px = (float)x;
  float py = (float)y;

  MSWDeviceToLogical(&px, &py);

  CalcUnscrolledPosition((int)px, (int)py, &event.m_x, &event.m_y);
*/

  event.m_x = x; event.m_y = y;
  event.m_shiftDown = ((flags & MK_SHIFT) != 0);
  event.m_controlDown = ((flags & MK_CONTROL) != 0);
  event.m_leftDown = ((flags & MK_LBUTTON) != 0);
  event.m_middleDown = ((flags & MK_MBUTTON) != 0);
  event.m_rightDown = ((flags & MK_RBUTTON) != 0);
  event.SetTimestamp(wxApp::sm_lastMessageTime); /* MATTHEW: timeStamp */
  event.m_eventObject = this;

  m_lastXPos = event.m_x; m_lastYPos = event.m_y; m_lastEvent = wxEVENT_TYPE_LEFT_UP;
  GetEventHandler()->OldOnMouseEvent(event);
}

void wxWindow::MSWOnMButtonDClick(const int x, const int y, const WXUINT flags)
{
//wxDebugMsg("MButtonDClick\n") ;
  /* MATTHEW: If dclick not allowed, generate another single-click */
  wxMouseEvent event((m_doubleClickAllowed) ?
		     wxEVENT_TYPE_MIDDLE_DCLICK : wxEVENT_TYPE_MIDDLE_DOWN);

/*
  float px = (float)x;
  float py = (float)y;

  MSWDeviceToLogical(&px, &py);

  CalcUnscrolledPosition((int)px, (int)py, &event.m_x, &event.m_y);
*/

  event.m_x = x; event.m_y = y;
  event.m_shiftDown = ((flags & MK_SHIFT) != 0);
  event.m_controlDown = ((flags & MK_CONTROL) != 0);
  event.m_leftDown = ((flags & MK_LBUTTON) != 0);
  event.m_middleDown = ((flags & MK_MBUTTON) != 0);
  event.m_rightDown = ((flags & MK_RBUTTON) != 0);
  event.SetTimestamp(wxApp::sm_lastMessageTime); /* MATTHEW: timeStamp */
  event.m_eventObject = this;

  m_lastXPos = event.m_x; m_lastYPos = event.m_y; m_lastEvent = wxEVENT_TYPE_LEFT_DCLICK;
//  if (m_doubleClickAllowed)
     GetEventHandler()->OldOnMouseEvent(event);
}

void wxWindow::MSWOnRButtonDown(const int x, const int y, const WXUINT flags)
{
#if 0 // defined(__WIN32__) && !defined(__WIN95__)
  // DClick not clean supported on Win3.1, except if someone know
  // how to emulate Sleep()...
  // This means that your app will receive Down-Up-Dclick sequences
  // rather than Dclick
  if (m_doubleClickAllowed)
  {
    UINT time = GetDoubleClickTime() ;
    Sleep(time) ;
    MSG dummy ;
    if (PeekMessage(&dummy,m_hWnd,
                    WM_RBUTTONDBLCLK,WM_RBUTTONDBLCLK,
                    PM_NOREMOVE)
       )
    {
      PeekMessage(&dummy,m_hWnd,WM_RBUTTONUP,WM_RBUTTONUP,PM_REMOVE);
      return;
    }
  }
#endif

//wxDebugMsg("RButtonDown\n") ;
  wxMouseEvent event(wxEVENT_TYPE_RIGHT_DOWN);

/*
  float px = (float)x;
  float py = (float)y;

  MSWDeviceToLogical(&px, &py);

  CalcUnscrolledPosition((int)px, (int)py, &event.m_x, &event.m_y);
*/

  event.m_x = x; event.m_y = y;
  event.m_shiftDown = ((flags & MK_SHIFT) != 0);
  event.m_controlDown = ((flags & MK_CONTROL) != 0);
  event.m_leftDown = ((flags & MK_LBUTTON) != 0);
  event.m_middleDown = ((flags & MK_MBUTTON) != 0);
  event.m_rightDown = ((flags & MK_RBUTTON) != 0);
  event.SetTimestamp(wxApp::sm_lastMessageTime); /* MATTHEW: timeStamp */
  event.m_eventObject = this;

  m_lastXPos = event.m_x; m_lastYPos = event.m_y; m_lastEvent = wxEVENT_TYPE_RIGHT_DOWN;
  GetEventHandler()->OldOnMouseEvent(event);
}

void wxWindow::MSWOnRButtonUp(const int x, const int y, const WXUINT flags)
{
//wxDebugMsg("RButtonUp\n") ;
  wxMouseEvent event(wxEVENT_TYPE_RIGHT_UP);

/*
  float px = (float)x;
  float py = (float)y;

  MSWDeviceToLogical(&px, &py);

  CalcUnscrolledPosition((int)px, (int)py, &event.m_x, &event.m_y);
*/

  event.m_x = x; event.m_y = y;
  event.m_shiftDown = ((flags & MK_SHIFT) != 0);
  event.m_controlDown = ((flags & MK_CONTROL) != 0);
  event.m_leftDown = ((flags & MK_LBUTTON) != 0);
  event.m_middleDown = ((flags & MK_MBUTTON) != 0);
  event.m_rightDown = ((flags & MK_RBUTTON) != 0);
  event.m_eventObject = this;
  event.SetTimestamp(wxApp::sm_lastMessageTime); /* MATTHEW: timeStamp */

  m_lastXPos = event.m_x; m_lastYPos = event.m_y; m_lastEvent = wxEVENT_TYPE_RIGHT_UP;
  GetEventHandler()->OldOnMouseEvent(event);
}

void wxWindow::MSWOnRButtonDClick(const int x, const int y, const WXUINT flags)
{
//wxDebugMsg("RButtonDClick\n") ;
  /* MATTHEW: If dclick not allowed, generate another single-click */
  wxMouseEvent event((m_doubleClickAllowed) ?
		     wxEVENT_TYPE_RIGHT_DCLICK : wxEVENT_TYPE_RIGHT_DOWN);

/*
  float px = (float)x;
  float py = (float)y;

  MSWDeviceToLogical(&px, &py);

  CalcUnscrolledPosition((int)px, (int)py, &event.m_x, &event.m_y);
*/

  event.m_x = x; event.m_y = y;
  event.m_shiftDown = ((flags & MK_SHIFT) != 0);
  event.m_controlDown = ((flags & MK_CONTROL) != 0);
  event.m_leftDown = ((flags & MK_LBUTTON) != 0);
  event.m_middleDown = ((flags & MK_MBUTTON) != 0);
  event.m_rightDown = ((flags & MK_RBUTTON) != 0);
  event.SetTimestamp(wxApp::sm_lastMessageTime); /* MATTHEW: timeStamp */
  event.m_eventObject = this;

  m_lastXPos = event.m_x; m_lastYPos = event.m_y; m_lastEvent = wxEVENT_TYPE_RIGHT_DCLICK;
//  if (m_doubleClickAllowed)
     GetEventHandler()->OldOnMouseEvent(event);
}

void wxWindow::MSWOnMouseMove(const int x, const int y, const WXUINT flags)
{
  // 'normal' move event...
  // Set cursor, but only if we're not in 'busy' mode
  
  // Trouble with this is that it sets the cursor for controls too :-(
  if (m_windowCursor.Ok() && !wxIsBusy())
    ::SetCursor((HCURSOR) m_windowCursor.GetHCURSOR());

  wxMouseEvent event(wxEVENT_TYPE_MOTION);

/*
  float px = (float)x;
  float py = (float)y;

  MSWDeviceToLogical(&px, &py);

  CalcUnscrolledPosition((int)px, (int)py, &event.m_x, &event.m_y);
*/

  event.m_x = x; event.m_y = y;
  event.m_shiftDown = ((flags & MK_SHIFT) != 0);
  event.m_controlDown = ((flags & MK_CONTROL) != 0);
  event.m_leftDown = ((flags & MK_LBUTTON) != 0);
  event.m_middleDown = ((flags & MK_MBUTTON) != 0);
  event.m_rightDown = ((flags & MK_RBUTTON) != 0);
  event.SetTimestamp(wxApp::sm_lastMessageTime); /* MATTHEW: timeStamp */
  event.m_eventObject = this;

  // Window gets a click down message followed by a mouse move
  // message even if position isn't changed!  We want to discard
  // the trailing move event if x and y are the same.
  if ((m_lastEvent == wxEVENT_TYPE_RIGHT_DOWN || m_lastEvent == wxEVENT_TYPE_LEFT_DOWN ||
       m_lastEvent == wxEVENT_TYPE_MIDDLE_DOWN) &&
      (m_lastXPos == event.m_x && m_lastYPos == event.m_y))
  {
    m_lastXPos = event.m_x; m_lastYPos = event.m_y;
    m_lastEvent = wxEVENT_TYPE_MOTION;
    return;
  }

  m_lastEvent = wxEVENT_TYPE_MOTION;
  m_lastXPos = event.m_x; m_lastYPos = event.m_y;
  GetEventHandler()->OldOnMouseEvent(event);
}

/* TODO put back leave/enter code if required
 */
#if 0
void wxWindow::MSWOnMouseMove(int x, int y, WXUINT flags)
{
//wxDebugMsg("Client 0x%08x Move Msg %d,%d\n",this,x,y) ;

// #if MOUSE_EXIT_FIX //Should work now!!

  // Don't do the Leave/Enter fix if we've captured the window,
  // or SetCapture won't work properly.
  if (!m_winCaptured)
  {
    HWND hunder ;
    POINT pt ;
    // See if we Leave/Enter the window.
    GetCursorPos(&pt) ;
    hunder = WindowFromPoint(pt) ;
    if (hunder==m_hWnd)
    {
      // I'm in the Window, but perhaps in NC area.
      RECT wind ;
      RECT nc ;
      GetClientRect(m_hWnd,&wind) ;
      GetWindowRect(m_hWnd,&nc) ;
      pt.x -= nc.left ;
      pt.y -= nc.top ;
      wind.left    += WINDOW_MARGIN ; // to be able to 'see' leave
      wind.top     += WINDOW_MARGIN ; // to be able to 'see' leave
      wind.right   -= WINDOW_MARGIN ; // to be able to 'see' leave
      wind.bottom  -= WINDOW_MARGIN ; // to be able to 'see' leave

      if (!PtInRect(&wind,pt))
        hunder = NULL ; // So, I can simulate a Leave event...
    }

    if (hunder!=m_hWnd)
    {
      if (m_mouseInWindow)
      {
        m_mouseInWindow = FALSE ;
        // Capture/Release is no more needed...
        //ReleaseCapture() ;
        MSWOnMouseLeave(x,y,flags) ;
        return ;
      }
      // We never want to see Enter or Motion in this part of the Window...
      return ;
    }
    else
    {
      // Event was triggered while I'm really into my client area.
      // Do an Enter if not done.
      if (!m_mouseInWindow)
      {
        m_mouseInWindow = TRUE ;
        // Capture/Release is no more needed...
        //SetCapture(m_hWnd) ;
        // Set cursor, but only if we're not in 'busy' mode
        if (m_windowCursor.Ok() && !wxIsBusy())
          ::SetCursor(m_windowCursor.ms_cursor);
        MSWOnMouseEnter(x,y,flags) ;
        return ;
      }
    }
  }
// #endif //MOUSE_EXIT_FIX
    
  // 'normal' move event...
  // Set cursor, but only if we're not in 'busy' mode
  if (m_windowCursor.Ok() && !wxIsBusy())
    ::SetCursor(m_windowCursor.ms_cursor);

  wxMouseEvent event(wxEVENT_TYPE_MOTION);

/*
  float px = (float)x;
  float py = (float)y;

  MSWDeviceToLogical(&px, &py);

  CalcUnscrolledPosition((int)px, (int)py, &event.m_x, &event.m_y);
*/

  event.m_x = x; event.m_y = y;
  event.m_shiftDown = ((flags & MK_SHIFT) != 0);
  event.m_controlDown = ((flags & MK_CONTROL) != 0);
  event.m_leftDown = ((flags & MK_LBUTTON) != 0);
  event.m_middleDown = ((flags & MK_MBUTTON) != 0);
  event.m_rightDown = ((flags & MK_RBUTTON) != 0);
  event.SetTimestamp(wxApp::sm_lastMessageTime); /* MATTHEW: timeStamp */
  event.m_eventObject = this;

  // Window gets a click down message followed by a mouse move
  // message even if position isn't changed!  We want to discard
  // the trailing move event if x and y are the same.
  if ((m_lastEvent == wxEVENT_TYPE_RIGHT_DOWN || m_lastEvent == wxEVENT_TYPE_LEFT_DOWN ||
       m_lastEvent == wxEVENT_TYPE_MIDDLE_DOWN) &&
      (m_lastXPos == event.m_x && m_lastYPos == event.m_y))
  {
    m_lastXPos = event.m_x; m_lastYPos = event.m_y;
    m_lastEvent = wxEVENT_TYPE_MOTION;
    return;
  }

  m_lastEvent = wxEVENT_TYPE_MOTION;
  m_lastXPos = event.m_x; m_lastYPos = event.m_y;
  GetEventHandler()->OldOnMouseEvent(event);
}
#endif

void wxWindow::MSWOnMouseEnter(const int x, const int y, const WXUINT flags)
{
//wxDebugMsg("Client 0x%08x Enter %d,%d\n",this,x,y) ;

  // Set cursor, but only if we're not in 'busy' mode
  if (m_windowCursor.Ok() && !wxIsBusy())
    ::SetCursor((HCURSOR) m_windowCursor.GetHCURSOR());

  wxMouseEvent event(wxEVENT_TYPE_ENTER_WINDOW);

/*
  float px = (float)x;
  float py = (float)y;

  MSWDeviceToLogical(&px, &py);

  CalcUnscrolledPosition((int)px, (int)py, &event.m_x, &event.m_y);
*/

  event.m_x = x; event.m_y = y;
  event.m_shiftDown = ((flags & MK_SHIFT) != 0);
  event.m_controlDown = ((flags & MK_CONTROL) != 0);
  event.m_leftDown = ((flags & MK_LBUTTON) != 0);
  event.m_middleDown = ((flags & MK_MBUTTON) != 0);
  event.m_rightDown = ((flags & MK_RBUTTON) != 0);
  event.SetTimestamp(wxApp::sm_lastMessageTime); /* MATTHEW: timeStamp */
  event.m_eventObject = this;

  m_lastEvent = wxEVENT_TYPE_ENTER_WINDOW;
  m_lastXPos = event.m_x; m_lastYPos = event.m_y;
  GetEventHandler()->OldOnMouseEvent(event);
}

void wxWindow::MSWOnMouseLeave(const int x, const int y, const WXUINT flags)
{
//wxDebugMsg("Client 0x%08x Leave %d,%d\n",this,x,y) ;

  // Set cursor, but only if we're not in 'busy' mode
  if (m_windowCursor.Ok() && !wxIsBusy())
    ::SetCursor((HCURSOR) m_windowCursor.GetHCURSOR());

  wxMouseEvent event(wxEVENT_TYPE_LEAVE_WINDOW);

/*
  float px = (float)x;
  float py = (float)y;

  MSWDeviceToLogical(&px, &py);

  CalcUnscrolledPosition((int)px, (int)py, &event.m_x, &event.m_y);
*/

  event.m_x = x; event.m_y = y;
  event.m_shiftDown = ((flags & MK_SHIFT) != 0);
  event.m_controlDown = ((flags & MK_CONTROL) != 0);
  event.m_leftDown = ((flags & MK_LBUTTON) != 0);
  event.m_middleDown = ((flags & MK_MBUTTON) != 0);
  event.m_rightDown = ((flags & MK_RBUTTON) != 0);
  event.SetTimestamp(wxApp::sm_lastMessageTime); /* MATTHEW: timeStamp */
  event.m_eventObject = this;

  m_lastEvent = wxEVENT_TYPE_LEAVE_WINDOW;
  m_lastXPos = event.m_x; m_lastYPos = event.m_y;
  GetEventHandler()->OldOnMouseEvent(event);
}

void wxWindow::MSWOnChar(const WXWORD wParam, const WXLPARAM lParam, const bool isASCII)
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
  else
    if ((id = wxCharCodeMSWToWX(wParam)) == 0)
      id = -1;

  if (id > -1)
  {
    wxKeyEvent event(wxEVT_CHAR);
    event.m_shiftDown = (::GetKeyState(VK_SHIFT)&0x100?TRUE:FALSE);
    event.m_controlDown = (::GetKeyState(VK_CONTROL)&0x100?TRUE:FALSE);
    if ((HIWORD(lParam) & KF_ALTDOWN) == KF_ALTDOWN)
      event.m_altDown = TRUE;

    event.m_eventObject = this;
    event.m_keyCode = id;
    event.SetTimestamp(wxApp::sm_lastMessageTime); /* MATTHEW: timeStamp */

    POINT pt ;
    GetCursorPos(&pt) ;
    RECT rect ;
    GetWindowRect((HWND) GetHWND(),&rect) ;
    pt.x -= rect.left ;
    pt.y -= rect.top ;

/*
    float fx,fy ;
    fx = (float)pt.x ;
    fy = (float)pt.y ;
    MSWDeviceToLogical(&fx,&fy) ;
    CalcUnscrolledPosition((int)fx,(int)fy,&event.m_x,&event.m_y) ;
*/
    event.m_x = pt.x; event.m_y = pt.y;

#if WXWIN_COMPATIBILITY
    GetEventHandler()->OldOnChar(event);
#else
    if (!GetEventHandler()->ProcessEvent(event))
		Default();
#endif
  }
}

void wxWindow::MSWOnJoyDown(const int joystick, const int x, const int y, const WXUINT flags)
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

void wxWindow::MSWOnJoyUp(const int joystick, const int x, const int y, const WXUINT flags)
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

void wxWindow::MSWOnJoyMove(const int joystick, const int x, const int y, const WXUINT flags)
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

void wxWindow::MSWOnJoyZMove(const int joystick, const int z, const WXUINT flags)
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

void wxWindow::MSWOnVScroll(const WXWORD wParam, const WXWORD pos, const WXHWND control)
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
			event.m_eventType = wxEVENT_TYPE_SCROLL_TOP;
			break;

		case SB_BOTTOM:
			event.m_eventType = wxEVENT_TYPE_SCROLL_BOTTOM;
			break;

		case SB_LINEUP:
			event.m_eventType = wxEVENT_TYPE_SCROLL_LINEUP;
			break;

		case SB_LINEDOWN:
		        event.m_eventType = wxEVENT_TYPE_SCROLL_LINEDOWN;
			break;

		case SB_PAGEUP:
                        event.m_eventType = wxEVENT_TYPE_SCROLL_PAGEUP;
			break;

		case SB_PAGEDOWN:
                        event.m_eventType = wxEVENT_TYPE_SCROLL_PAGEDOWN;
			break;

        case SB_THUMBTRACK:
        case SB_THUMBPOSITION:
                        event.m_eventType = wxEVENT_TYPE_SCROLL_THUMBTRACK;
			break;

		default:
                        return;
                        break;
	}

  if (!GetEventHandler()->ProcessEvent(event))
#if WXWIN_COMPATIBILITY
    GetEventHandler()->OldOnScroll(event);
#else
    Default();
#endif
}

void wxWindow::MSWOnHScroll( const WXWORD wParam, const WXWORD pos, const WXHWND control)
{
  if (control)
  {
    wxWindow *child = wxFindWinFromHandle(control);
	if ( child )
    	child->MSWOnHScroll(wParam, pos, control);
    return;
  }

    wxScrollEvent event;
    event.SetPosition(pos);
    event.SetOrientation(wxHORIZONTAL);
    event.m_eventObject = this;

	switch ( wParam )
	{
		case SB_TOP:
			event.m_eventType = wxEVENT_TYPE_SCROLL_TOP;
			break;

		case SB_BOTTOM:
			event.m_eventType = wxEVENT_TYPE_SCROLL_BOTTOM;
			break;

		case SB_LINEUP:
			event.m_eventType = wxEVENT_TYPE_SCROLL_LINEUP;
			break;

		case SB_LINEDOWN:
		        event.m_eventType = wxEVENT_TYPE_SCROLL_LINEDOWN;
			break;

		case SB_PAGEUP:
                        event.m_eventType = wxEVENT_TYPE_SCROLL_PAGEUP;
			break;

		case SB_PAGEDOWN:
                        event.m_eventType = wxEVENT_TYPE_SCROLL_PAGEDOWN;
			break;

        case SB_THUMBTRACK:
        case SB_THUMBPOSITION:
                        event.m_eventType = wxEVENT_TYPE_SCROLL_THUMBTRACK;
			break;

		default:
                        return;
                        break;
	}
  if (!GetEventHandler()->ProcessEvent(event))
#if WXWIN_COMPATIBILITY
    GetEventHandler()->OldOnScroll(event);
#else
    Default();
#endif
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
#if DEBUG > 1
    wxDebugMsg("wxGetCharSize: Selecting HFONT %X\n", fnt);
#endif
//    the_font->UseResource();
//    the_font->RealizeResource();
    if ((fnt=(HFONT) the_font->GetResourceHandle()))
      was = SelectObject(dc,fnt) ;
  }
  GetTextMetrics(dc, &tm);
  if (the_font && fnt && was)
  {
#if DEBUG > 1
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
    case VK_CANCEL:             id = WXK_CANCEL; break;
    case VK_BACK:               id = WXK_BACK; break;
    case VK_TAB:	        id = WXK_TAB; break;
    case VK_CLEAR:		id = WXK_CLEAR; break;
    case VK_RETURN:		id = WXK_RETURN; break;
    case VK_SHIFT:		id = WXK_SHIFT; break;
    case VK_CONTROL:		id = WXK_CONTROL; break;
    case VK_MENU :		id = WXK_MENU; break;
    case VK_PAUSE:		id = WXK_PAUSE; break;
    case VK_SPACE:		id = WXK_SPACE; break;
    case VK_ESCAPE:		id = WXK_ESCAPE; break;
    case VK_PRIOR:		id = WXK_PRIOR; break;
    case VK_NEXT :		id = WXK_NEXT; break;
    case VK_END:		id = WXK_END; break;
    case VK_HOME :		id = WXK_HOME; break;
    case VK_LEFT :		id = WXK_LEFT; break;
    case VK_UP:		        id = WXK_UP; break;
    case VK_RIGHT:		id = WXK_RIGHT; break;
    case VK_DOWN :		id = WXK_DOWN; break;
    case VK_SELECT:		id = WXK_SELECT; break;
    case VK_PRINT:		id = WXK_PRINT; break;
    case VK_EXECUTE:		id = WXK_EXECUTE; break;
    case VK_INSERT:		id = WXK_INSERT; break;
    case VK_DELETE:		id = WXK_DELETE; break;
    case VK_HELP :		id = WXK_HELP; break;
    case VK_NUMPAD0:		id = WXK_NUMPAD0; break;
    case VK_NUMPAD1:		id = WXK_NUMPAD1; break;
    case VK_NUMPAD2:		id = WXK_NUMPAD2; break;
    case VK_NUMPAD3:		id = WXK_NUMPAD3; break;
    case VK_NUMPAD4:		id = WXK_NUMPAD4; break;
    case VK_NUMPAD5:		id = WXK_NUMPAD5; break;
    case VK_NUMPAD6:		id = WXK_NUMPAD6; break;
    case VK_NUMPAD7:		id = WXK_NUMPAD7; break;
    case VK_NUMPAD8:		id = WXK_NUMPAD8; break;
    case VK_NUMPAD9:		id = WXK_NUMPAD9; break;
    case VK_MULTIPLY:		id = WXK_MULTIPLY; break;
    case VK_ADD:		id = WXK_ADD; break;
    case VK_SUBTRACT:		id = WXK_SUBTRACT; break;
    case VK_DECIMAL:		id = WXK_DECIMAL; break;
    case VK_DIVIDE:		id = WXK_DIVIDE; break;
    case VK_F1:		id = WXK_F1; break;
    case VK_F2:		id = WXK_F2; break;
    case VK_F3:		id = WXK_F3; break;
    case VK_F4:		id = WXK_F4; break;
    case VK_F5:		id = WXK_F5; break;
    case VK_F6:		id = WXK_F6; break;
    case VK_F7:		id = WXK_F7; break;
    case VK_F8:		id = WXK_F8; break;
    case VK_F9:		id = WXK_F9; break;
    case VK_F10:		id = WXK_F10; break;
    case VK_F11:		id = WXK_F11; break;
    case VK_F12:		id = WXK_F12; break;
    case VK_F13:		id = WXK_F13; break;
    case VK_F14:		id = WXK_F14; break;
    case VK_F15:		id = WXK_F15; break;
    case VK_F16:		id = WXK_F16; break;
    case VK_F17:		id = WXK_F17; break;
    case VK_F18:		id = WXK_F18; break;
    case VK_F19:		id = WXK_F19; break;
    case VK_F20:		id = WXK_F20; break;
    case VK_F21:		id = WXK_F21; break;
    case VK_F22:		id = WXK_F22; break;
    case VK_F23:		id = WXK_F23; break;
    case VK_F24:		id = WXK_F24; break;
    case VK_NUMLOCK:		id = WXK_NUMLOCK; break;
    case VK_SCROLL:		id = WXK_SCROLL; break;
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
    case WXK_CANCEL:            keySym = VK_CANCEL; break;
    case WXK_CLEAR:		keySym = VK_CLEAR; break;
    case WXK_SHIFT:		keySym = VK_SHIFT; break;
    case WXK_CONTROL:		keySym = VK_CONTROL; break;
    case WXK_MENU :		keySym = VK_MENU; break;
    case WXK_PAUSE:		keySym = VK_PAUSE; break;
    case WXK_PRIOR:		keySym = VK_PRIOR; break;
    case WXK_NEXT :		keySym = VK_NEXT; break;
    case WXK_END:		keySym = VK_END; break;
    case WXK_HOME :		keySym = VK_HOME; break;
    case WXK_LEFT :		keySym = VK_LEFT; break;
    case WXK_UP:		keySym = VK_UP; break;
    case WXK_RIGHT:		keySym = VK_RIGHT; break;
    case WXK_DOWN :		keySym = VK_DOWN; break;
    case WXK_SELECT:		keySym = VK_SELECT; break;
    case WXK_PRINT:		keySym = VK_PRINT; break;
    case WXK_EXECUTE:		keySym = VK_EXECUTE; break;
    case WXK_INSERT:		keySym = VK_INSERT; break;
    case WXK_DELETE:		keySym = VK_DELETE; break;
    case WXK_HELP :		keySym = VK_HELP; break;
    case WXK_NUMPAD0:		keySym = VK_NUMPAD0; break;
    case WXK_NUMPAD1:		keySym = VK_NUMPAD1; break;
    case WXK_NUMPAD2:		keySym = VK_NUMPAD2; break;
    case WXK_NUMPAD3:		keySym = VK_NUMPAD3; break;
    case WXK_NUMPAD4:		keySym = VK_NUMPAD4; break;
    case WXK_NUMPAD5:		keySym = VK_NUMPAD5; break;
    case WXK_NUMPAD6:		keySym = VK_NUMPAD6; break;
    case WXK_NUMPAD7:		keySym = VK_NUMPAD7; break;
    case WXK_NUMPAD8:		keySym = VK_NUMPAD8; break;
    case WXK_NUMPAD9:		keySym = VK_NUMPAD9; break;
    case WXK_MULTIPLY:		keySym = VK_MULTIPLY; break;
    case WXK_ADD:		keySym = VK_ADD; break;
    case WXK_SUBTRACT:		keySym = VK_SUBTRACT; break;
    case WXK_DECIMAL:		keySym = VK_DECIMAL; break;
    case WXK_DIVIDE:		keySym = VK_DIVIDE; break;
    case WXK_F1:		keySym = VK_F1; break;
    case WXK_F2:		keySym = VK_F2; break;
    case WXK_F3:		keySym = VK_F3; break;
    case WXK_F4:		keySym = VK_F4; break;
    case WXK_F5:		keySym = VK_F5; break;
    case WXK_F6:		keySym = VK_F6; break;
    case WXK_F7:		keySym = VK_F7; break;
    case WXK_F8:		keySym = VK_F8; break;
    case WXK_F9:		keySym = VK_F9; break;
    case WXK_F10:		keySym = VK_F10; break;
    case WXK_F11:		keySym = VK_F11; break;
    case WXK_F12:		keySym = VK_F12; break;
    case WXK_F13:		keySym = VK_F13; break;
    case WXK_F14:		keySym = VK_F14; break;
    case WXK_F15:		keySym = VK_F15; break;
    case WXK_F16:		keySym = VK_F16; break;
    case WXK_F17:		keySym = VK_F17; break;
    case WXK_F18:		keySym = VK_F18; break;
    case WXK_F19:		keySym = VK_F19; break;
    case WXK_F20:		keySym = VK_F20; break;
    case WXK_F21:		keySym = VK_F21; break;
    case WXK_F22:		keySym = VK_F22; break;
    case WXK_F23:		keySym = VK_F23; break;
    case WXK_F24:		keySym = VK_F24; break;
    case WXK_NUMLOCK:		keySym = VK_NUMLOCK; break;
    case WXK_SCROLL:		keySym = VK_SCROLL; break;
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
void wxWindow::CreateCaret(const int w, const int h)
{
  m_caretWidth = w;
  m_caretHeight = h;
  m_caretEnabled = TRUE;
}

void wxWindow::CreateCaret(const wxBitmap *WXUNUSED(bitmap))
{
  // Not implemented
}

void wxWindow::ShowCaret(const bool show)
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

void wxWindow::SetCaretPos(const int x, const int y)
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

/*
 * Update iterator. Use from within OnPaint.
 */

static RECT gs_UpdateRect;

wxUpdateIterator::wxUpdateIterator(wxWindow* wnd)
{
  current = 0;					//start somewhere...
#if defined(__WIN32__) && !defined(__win32s__)
  rlist = NULL;					//make sure I don't free randomly
  int len = GetRegionData((HRGN) wnd->m_updateRgn,0,NULL);	//Get buffer size
  if (len)
  {
    rlist = (WXRGNDATA *) (RGNDATA *)new char[len];
    GetRegionData((HRGN) wnd->m_updateRgn,len, (RGNDATA *)rlist);
    rp = (void *)(RECT*) ((RGNDATA *)rlist)->Buffer;
    rects = ((RGNDATA *)rlist)->rdh.nCount;
  }
  else
#endif
  {
	gs_UpdateRect.left = wnd->m_updateRect.x;
	gs_UpdateRect.top = wnd->m_updateRect.y;
	gs_UpdateRect.right = wnd->m_updateRect.x + wnd->m_updateRect.width;
	gs_UpdateRect.bottom = wnd->m_updateRect.y + wnd->m_updateRect.height;
    rects = 1;
    rp = (void *)&gs_UpdateRect;			//Only one available in Win16,32s
  }
}

wxUpdateIterator::~wxUpdateIterator(void)
{
#ifdef __WIN32__
#ifndef __win32s__
  if (rlist) delete (RGNDATA *) rlist;
#endif
#endif
}

wxUpdateIterator::operator int (void)
{
  if (current < rects)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

wxUpdateIterator* wxUpdateIterator::operator ++(int)
{
  current++;
  return this;
}

void wxUpdateIterator::GetRect(wxRect *rect)
{
  RECT *mswRect = ((RECT *)rp)+current;	//ought to error check this...
  rect->x = mswRect->left;
  rect->y = mswRect->top;
  rect->width = mswRect->right - mswRect->left;
  rect->height = mswRect->bottom - mswRect->top;
}

int wxUpdateIterator::GetX()
{
  return ((RECT*)rp)[current].left;
}

int wxUpdateIterator::GetY()
{
  return ((RECT *)rp)[current].top;
}

int wxUpdateIterator::GetW()
{
 return ((RECT *)rp)[current].right-GetX();
}

int wxUpdateIterator::GetH()
{
  return ((RECT *)rp)[current].bottom-GetY();
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
      event.SetTimestamp(wxApp::sm_lastMessageTime); /* MATTHEW: timeStamp */

#if WXWIN_COMPATIBILITY
		if ( wxTheApp && wxTheApp->OldOnCharHook(event) )
			return 1;
#endif
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

void wxWindow::SetSizeHints(const int minW, const int minH, const int maxW, const int maxH, const int WXUNUSED(incW), const int WXUNUSED(incH))
{
  m_minSizeX = minW;
  m_minSizeY = minH;
  m_maxSizeX = maxW;
  m_maxSizeY = maxH;
}

void wxWindow::Centre(const int direction)
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

void wxWindow::WarpPointer (const int x_pos, const int y_pos)
{
  // Move the pointer to (x_pos,y_pos) coordinates. They are expressed in
  // pixel coordinates, relatives to the canvas -- So, we first need to
  // substract origin of the window, then convert to screen position

  int x = x_pos; int y = y_pos;
/* Leave this to the app to decide (and/or wxScrolledWindow)
      x -= m_xScrollPosition * m_xScrollPixelsPerLine;
      y -= m_yScrollPosition * m_yScrollPixelsPerLine;
*/
      RECT rect;
      GetWindowRect ((HWND) GetHWND(), &rect);

      x += rect.left;
      y += rect.top;

      SetCursorPos (x, y);
}

void wxWindow::MSWDeviceToLogical (float *x, float *y) const
{
    // TODO
    // Do we have a SetUserScale in wxWindow too, so we can
    // get mouse events scaled?
/*
      if (m_windowDC)
      {
        *x = m_windowDC->DeviceToLogicalX ((int) *x);
        *y = m_windowDC->DeviceToLogicalY ((int) *y);
      }
*/
}

bool wxWindow::MSWOnEraseBkgnd (const WXHDC pDC)
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
void wxWindow::SetScrollRange(const int orient, const int range, const bool refresh)
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

void wxWindow::SetScrollPage(const int orient, const int page, const bool refresh)
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

int wxWindow::OldGetScrollRange(const int orient) const
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

int wxWindow::GetScrollPage(const int orient) const
{
  if (orient == wxHORIZONTAL)
    return m_xThumbSize;
  else
    return m_yThumbSize;
}
#endif

int wxWindow::GetScrollPos(const int orient) const
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
int wxWindow::GetScrollRange(const int orient) const
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

int wxWindow::GetScrollThumb(const int orient) const
{
  if (orient == wxHORIZONTAL)
    return m_xThumbSize;
  else
    return m_yThumbSize;
}

void wxWindow::SetScrollPos(const int orient, const int pos, const bool refresh)
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
void wxWindow::SetScrollbar(const int orient, const int pos, const int thumbVisible,
    const int range, const bool refresh)
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

void wxWindow::ScrollWindow(const int dx, const int dy, const wxRectangle *rect)
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

void wxWindow::OnSize(wxSizeEvent& event)
{
	Default();
#if USE_CONSTRAINTS
  if (GetAutoLayout())
    Layout();
#endif
}

/*
void wxWindow::CalcScrolledPosition(const int x, const int y, int *xx, int *yy) const
{
  *xx = x;
  *yy = y;
}

void wxWindow::CalcUnscrolledPosition(const int x, const int y, float *xx, float *yy) const
{
  *xx = x;
  *yy = y;
}
*/

void wxWindow::SetFont(const wxFont& font)
{
  // Decrement the usage count of the old label font
  // (we may be able to free it up)
//  if (GetFont()->Ok())
//    GetFont()->ReleaseResource();
    
  m_windowFont = font;

  if (!m_windowFont.Ok())
	return;

//  m_windowFont.UseResource();

  HWND hWnd = (HWND) GetHWND();
  if (hWnd != 0)
  {
//      m_windowFont.RealizeResource();

      if (m_windowFont.GetResourceHandle())
        SendMessage(hWnd, WM_SETFONT,
                  (WPARAM)m_windowFont.GetResourceHandle(),TRUE);
  }
}

void wxWindow::SubclassWin(WXHWND hWnd)
{
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

#if WXWIN_COMPATIBILITY
void wxWindow::OldOnPaint(void)
{
  wxPaintEvent event(m_windowId);
  event.m_eventObject = this;
  if (!GetEventHandler()->ProcessEvent(event))
    Default();
};

void wxWindow::OldOnSize(int w, int h)
{
  wxSizeEvent event(wxSize(w, h), m_windowId);
  event.m_eventObject = this;
  if (!GetEventHandler()->ProcessEvent(event))
    Default();
};

void wxWindow::OldOnMouseEvent(wxMouseEvent& event)
{
  if (!GetEventHandler()->ProcessEvent(event))
    Default();
};

void wxWindow::OldOnChar(wxKeyEvent& event)
{
  if (!GetEventHandler()->ProcessEvent(event))
    Default();
};

void wxWindow::OldOnSetFocus(void)
{
  wxFocusEvent event(wxEVT_SET_FOCUS, m_windowId);
  event.m_eventObject = this;
  if (!GetEventHandler()->ProcessEvent(event))
    Default();
};

void wxWindow::OldOnKillFocus(void)
{
  wxFocusEvent event(wxEVT_KILL_FOCUS, m_windowId);
  event.m_eventObject = this;
  if (!GetEventHandler()->ProcessEvent(event))
    Default();
};
#endif

void wxWindow::OnChar(wxKeyEvent& event)
{
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

void wxWindow::MakeModal(const bool modal)
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
bool wxWindow::DoPhase(const int phase)
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
void wxWindow::SetConstraintSizes(const bool recurse)
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

void wxWindow::SizerSetSize(const int x, const int y, const int w, const int h)
{
	int xx = x;
	int yy = y;
  TransformSizerToActual(&xx, &yy);
  SetSize(xx, yy, w, h);
}

void wxWindow::SizerMove(const int x, const int y)
{
	int xx = x;
	int yy = y;
  TransformSizerToActual(&xx, &yy);
  Move(xx, yy);
}

// Only set the size/position of the constraint (if any)
void wxWindow::SetSizeConstraint(const int x, const int y, const int w, const int h)
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

void wxWindow::MoveConstraint(const int x, const int y)
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

bool wxWindow::Close(const bool force)
{
  // Let's generalise it to work the same for any window.
/*
  if (!IsKindOf(CLASSINFO(wxDialog)) && !IsKindOf(CLASSINFO(wxFrame)))
  {
    this->Destroy();
    return TRUE;
  }
*/

  wxCloseEvent event(wxEVT_CLOSE_WINDOW, m_windowId);
  event.SetEventObject(this);
  event.SetForce(force);

  return GetEventHandler()->ProcessEvent(event);

/*
  if ( !force && event.GetVeto() )
    return FALSE;

  Show(FALSE);

  if (!wxPendingDelete.Member(this))
    wxPendingDelete.Append(this);

  return TRUE;
*/
}

wxObject* wxWindow::GetChild(const int number) const
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
  if (initiatingItem->IsKindOf(CLASSINFO(wxListBox)) && initiatingItem->GetCallback())
  {
    wxListBox *lbox = (wxListBox *)initiatingItem;
    wxCommandEvent event(wxEVENT_TYPE_LISTBOX_DCLICK_COMMAND);
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
    wxCommandEvent event(wxEVENT_TYPE_BUTTON_COMMAND);
    but->Command(event);
  }
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
wxWindow *wxWindow::FindWindow(const long id)
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

#ifdef __WINDOWS__
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

// Do Update UI processing for child controls

// TODO: should this be implemented for the child window rather
// than the parent? Then you can override it e.g. for wxCheckBox
// to do the Right Thing rather than having to assume a fixed number
// of control classes.

void wxWindow::UpdateWindowUI(void)
{
		wxWindowID id = GetId();
		if (id > 0)
		{
			wxUpdateUIEvent event(id);
			event.m_eventObject = this;

			if (this->GetEventHandler()->ProcessEvent(event))
			{
				if (event.GetSetEnabled())
					this->Enable(event.GetEnabled());

				if (event.GetSetText() && this->IsKindOf(CLASSINFO(wxControl)))
					((wxControl*)this)->SetLabel(event.GetText());

				if (this->IsKindOf(CLASSINFO(wxCheckBox)))
				{
					if (event.GetSetChecked())
						((wxCheckBox *) this)->SetValue(event.GetChecked());
				}
				else if (this->IsKindOf(CLASSINFO(wxRadioButton)))
				{
					if (event.GetSetChecked())
						((wxRadioButton *) this)->SetValue(event.GetChecked());
				}
			}
		}

}

void wxWindow::OnIdle(wxIdleEvent& event)
{
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

