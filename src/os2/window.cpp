/////////////////////////////////////////////////////////////////////////////
// Name:        windows.cpp
// Purpose:     wxWindow
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "window.h"
#endif

#ifndef WX_PRECOMP
    #define INCL_DOS
    #define INCL_PM
    #include <os2.h>
    #include "wx/window.h"
    #include "wx/accel.h"
    #include "wx/setup.h"
    #include "wx/menu.h"
    #include "wx/dc.h"
    #include "wx/dcclient.h"
    #include "wx/utils.h"
    #include "wx/app.h"
    #include "wx/panel.h"
    #include "wx/layout.h"
    #include "wx/dialog.h"
    #include "wx/frame.h"
    #include "wx/listbox.h"
    #include "wx/button.h"
    #include "wx/msgdlg.h"

    #include <stdio.h>
#endif

#if     wxUSE_OWNER_DRAWN
    #include "wx/ownerdrw.h"
#endif

#if     wxUSE_DRAG_AND_DROP
    #include "wx/dnd.h"
#endif

#include "wx/menuitem.h"
#include "wx/log.h"

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif

#if wxUSE_CARET
    #include "wx/caret.h"
#endif // wxUSE_CARET

#include "wx/intl.h"
#include "wx/log.h"


#include "wx/textctrl.h"

#include <string.h>

// place compiler, OS specific includes here


// standard macros -- these are for OS/2 PM, but most GUI's have something similar
#ifndef GET_X_LPARAM
//  SHORT1FROMMP -- LOWORD
    #define GET_X_LPARAM(mp) ((unsigned short)(unsigned long)(mp))
//  SHORT2FROMMP -- HIWORD
    #define GET_Y_LPARAM(mp) ((unsigned short)(unsigned long)(mp >> 16))
#endif // GET_X_LPARAM

// ---------------------------------------------------------------------------
// global variables
// ---------------------------------------------------------------------------

// the last Windows message we got (MT-UNSAFE)
extern WXMSGID s_currentMsg;
extern wxList WXDLLEXPORT wxPendingDelete;
extern wxChar wxCanvasClassName[];

wxMenu *wxCurrentPopupMenu = NULL;
wxList *wxWinHandleList = NULL;

// ---------------------------------------------------------------------------
// private functions
// ---------------------------------------------------------------------------
// the window proc for all our windows; most gui's have something similar
MRESULT wxWndProc( HWND hWnd
                  ,ULONG message
                  ,MPARAM mp1
                  ,MPARAM mp2
                 );
void wxRemoveHandleAssociation(wxWindow *win);
void wxAssociateWinWithHandle(HWND hWnd, wxWindow *win);
wxWindow *wxFindWinFromHandle(WXHWND hWnd);

// ---------------------------------------------------------------------------
// event tables
// ---------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
    IMPLEMENT_DYNAMIC_CLASS(wxWindow, wxWindowBase)
#endif

BEGIN_EVENT_TABLE(wxWindow, wxWindowBase)
    EVT_ERASE_BACKGROUND(wxWindow::OnEraseBackground)
    EVT_SYS_COLOUR_CHANGED(wxWindow::OnSysColourChanged)
    EVT_INIT_DIALOG(wxWindow::OnInitDialog)
    EVT_IDLE(wxWindow::OnIdle)
END_EVENT_TABLE()

// ===========================================================================
// implementation
// ===========================================================================

void wxWindow::Init()
{
    // generic
    InitBase();

    // PM specific
    m_winCaptured = FALSE;
    m_isBeingDeleted = FALSE;
    m_mouseInWindow = FALSE;
    m_backgroundTransparent = FALSE;

    // wxWnd
    m_hMenu = 0;

    m_hWnd = 0;

    // pass WM_GETDLGCODE to DefWindowProc()
    m_lDlgCode = 0;

    m_xThumbSize = 0;
    m_yThumbSize = 0;

    // as all windows are created with WS_VISIBLE style...
    m_isShown = TRUE;

}

bool wxWindow::Create(wxWindow *parent, wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxString& name)
{
    wxCHECK_MSG( parent, FALSE, wxT("can't create wxWindow without parent") );

    if ( !CreateBase(parent, id, pos, size, style, wxDefaultValidator, name) )
        return FALSE;

    // TODO: PM Specific initialization
    parent->AddChild(this);
    return TRUE;
}

// Destructor
wxWindow::~wxWindow()
{
    m_isBeingDeleted = TRUE;

    PMDetachWindowMenu();
    // delete handlers?
    if (m_parent)
        m_parent->RemoveChild(this);
    DestroyChildren();
    if (m_hWnd)
    {
        if(!WinDestroyWindow(GetHWND()))
            wxLogLastError(wxT("DestroyWindow"));
        // remove hWnd <-> wxWindow association
        wxRemoveHandleAssociation(this);
    }
}

// ---------------------------------------------------------------------------
// basic operations
// ---------------------------------------------------------------------------

void wxWindow::Raise()
{
    // TODO:
}

void wxWindow::Lower()
{
    // TODO:
}

void wxWindow::SetFocus()
{
    // TODO:
}

void wxWindow::WarpPointer(int x_pos, int y_pos)
{
    // TODO:
}

void wxWindow::CaptureMouse()
{
    // TODO:
}

void wxWindow::ReleaseMouse()
{
    // TODO:
}

void wxWindow::Refresh(bool eraseBack, const wxRect *rect)
{
    // TODO:
}

void wxWindow::Clear()
{
    // TODO:
}

bool wxWindow::SetFont(const wxFont& f)
{
    // TODO:
    return(TRUE);
}

int  wxWindow::GetCharHeight() const
{
    // TODO:
    return(1);
}

int  wxWindow::GetCharWidth() const
{
    // TODO:
    return(1);
}

void wxWindow::GetTextExtent( const wxString& string
                             ,int*            x
                             ,int*            y
                             ,int*            descent
                             ,int*            externalLeading
                             ,const wxFont*   theFont
                            ) const
{
    // TODO:
}

void wxWindow::SetScrollbar( int  orient
                            ,int  pos
                            ,int  thumbVisible
                            ,int  range
                            ,bool refresh
                           )
{
    // TODO:
}

void wxWindow::SetScrollPos( int  orient
                            ,int  pos
                            ,bool refresh
                           )
{
    // TODO:
}

int  wxWindow::GetScrollPos(int orient) const
{
    // TODO:
    return(1);
}

int  wxWindow::GetScrollRange(int orient) const
{
    // TODO:
    return(1);
}

int  wxWindow::GetScrollThumb(int orient) const
{
    // TODO:
    return(1);
}

void wxWindow::ScrollWindow( int           dx
                            ,int           dy
                            ,const wxRect* rect
                           )
{
    // TODO:
}

#if wxUSE_DRAG_AND_DROP
void wxWindow::SetDropTarget(wxDropTarget *pDropTarget)
{
    // TODO:
}
#endif

void wxWindow::DoClientToScreen( int *x, int *y ) const
{
    // TODO:
}

void wxWindow::DoScreenToClient( int *x, int *y ) const
{
    // TODO:
}

void wxWindow::DoGetPosition( int *x, int *y ) const
{
    // TODO:
}

void wxWindow::DoGetSize( int *width, int *height ) const
{
    // TODO:
}

void wxWindow::DoGetClientSize( int *width, int *height ) const
{
    // TODO:
}

void wxWindow::DoSetSize(int x, int y,
                         int width, int height,
                         int sizeFlags)
{
    // TODO:
}

void wxWindow::DoSetClientSize(int width, int height)
{
    // TODO:
}

bool wxWindow::DoPopupMenu( wxMenu *menu, int x, int y )
{
    // TODO:
    return(TRUE);
}

void wxWindow::Fit()
{
    // TODO:
}

bool wxWindow::Show(bool show) // check if base implementation is OK
{
    // TODO:
    return(TRUE);
}

bool wxWindow::Enable(bool enable) // check if base implementation is OK
{
    // TODO:
    return(TRUE);
}

bool wxWindow::SetCursor(const wxCursor& cursor) // check if base implementation is OK
{
    // TODO:
    return(TRUE);
}

bool wxWindow::Validate()
{
    // TODO:
    return(TRUE);
}

wxWindow* wxWindow::FindFocus()
{
    wxWindow*                       window = NULL;
    // TODO:
    return(window);
}

void wxWindow::DragAcceptFiles(bool accept)
{
    // TODO:
}

#if wxUSE_CARET && WXWIN_COMPATIBILITY
void wxWindow::CreateCaret(int w, int h)
{
    // TODO:
}

void wxWindow::CreateCaret(const wxBitmap *bitmap)
{
    // TODO:
}

void wxWindow::DestroyCaret()
{
    // TODO:
}

void wxWindow::ShowCaret(bool show)
{
    // TODO:
}

void wxWindow::SetCaretPos(int x, int y)
{
    // TODO:
}

void wxWindow::GetCaretPos(int *x, int *y) const
{
    // TODO:
}

#endif

void wxWindow::OnCommand(wxWindow& win, wxCommandEvent& event)
{
    // TODO:
}

void wxWindow::OnDefaultAction(wxControl *initiatingItem)
{
    // TODO:
}

void wxWindow::OnEraseBackground(wxEraseEvent& event)
{
    // TODO:
}

void wxWindow::OnChar(wxKeyEvent& event)
{
    // TODO:
}

void wxWindow::OnKeyDown(wxKeyEvent& event)
{
    // TODO:
}

void wxWindow::OnKeyUp(wxKeyEvent& event)
{
    // TODO:
}

void wxWindow::OnPaint(wxPaintEvent& event)
{
    // TODO:
}

void wxWindow::OnIdle(wxIdleEvent& event)
{
    // TODO:
}

void wxWindow::OnInitDialog(wxInitDialogEvent& event)
{
    // TODO:
}


wxPoint wxWindow::GetClientAreaOrigin() const
{
    // TODO:
    return wxPoint(0, 0);
}

void wxWindow::AdjustForParentClientOrigin(int& x, int& y, int sizeFlags)
{
    // TODO:
}

long wxWindow::Default()
{
    // TODO:
    return(1);
}

void wxWindow::UnsetConstraints(wxLayoutConstraints *c)
{
    // TODO:
}

wxObject* wxWindow::GetChild(int number) const
{
    // TODO:
    return((wxObject*)this);
}

void wxWindow::PMDetachWindowMenu()
{
    if ( m_hMenu )
    {
        HMENU hMenu = (HMENU)m_hMenu;

        int N = (int)WinSendMsg(hMenu, MM_QUERYITEMCOUNT, 0, 0);
        int i;
        for (i = 0; i < N; i++)
        {
            wxChar buf[100];
            int chars = (int)WinSendMsg(hMenu, MM_QUERYITEMTEXT, MPFROM2SHORT(i, N), buf);
            if ( !chars )
            {
                wxLogLastError(wxT("GetMenuString"));

                continue;
            }

            if ( wxStrcmp(buf, wxT("&Window")) == 0 )
            {
                WinSendMsg(hMenu, MM_DELETEITEM, MPFROM2SHORT(i, TRUE), 0);
                break;
            }
        }
    }
}

void wxAssociateWinWithHandle(HWND hWnd, wxWindow *win)
{
    // adding NULL hWnd is (first) surely a result of an error and
    // (secondly) breaks menu command processing
    wxCHECK_RET( hWnd != (HWND)NULL,
                 wxT("attempt to add a NULL hWnd to window list ignored") );

    if ( !wxWinHandleList->Find((long)hWnd) )
        wxWinHandleList->Append((long)hWnd, win);
}

wxWindow *wxFindWinFromHandle(WXHWND hWnd)
{
    wxNode *node = wxWinHandleList->Find((long)hWnd);
    if ( !node )
        return NULL;
    return (wxWindow *)node->Data();
}

void wxRemoveHandleAssociation(wxWindow *win)
{
    wxWinHandleList->DeleteObject(win);
}

void wxWindow::SubclassWin(WXHWND hWnd)
{
    wxASSERT_MSG( !m_oldWndProc, wxT("subclassing window twice?") );

    HWND hwnd = (HWND)hWnd;
/*
* TODO: implement something like this:
*   wxCHECK_RET( ::IsWindow(hwnd), wxT("invalid HWND in SubclassWin") );
*
*   wxAssociateWinWithHandle(hwnd, this);
*
*   m_oldWndProc = (WXFARPROC) GetWindowLong(hwnd, GWL_WNDPROC);
*   SetWindowLong(hwnd, GWL_WNDPROC, (LONG) wxWndProc);
*/
}

void wxWindow::UnsubclassWin()
{
/*
* TODO:

    wxRemoveHandleAssociation(this);

    // Restore old Window proc
    HWND hwnd = GetHwnd();
    if ( hwnd )
    {
        m_hWnd = 0;

        wxCHECK_RET( ::IsWindow(hwnd), wxT("invalid HWND in UnsubclassWin") );

        FARPROC farProc = (FARPROC) GetWindowLong(hwnd, GWL_WNDPROC);
        if ( (m_oldWndProc != 0) && (farProc != (FARPROC) m_oldWndProc) )
        {
            SetWindowLong(hwnd, GWL_WNDPROC, (LONG) m_oldWndProc);
            m_oldWndProc = 0;
        }
    }
*/
}

WXDWORD wxWindow::Determine3DEffects(WXDWORD defaultBorderStyle,
                                     bool *want3D) const
{
   DWORD exStyle; // remove after implementation doe
/* TODO:  this ought to be fun
*
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
    if ( GetParent() && (GetParent()->GetWindowStyleFlag() & wxUSER_COLOURS) || (m_windowStyle & wxSIMPLE_BORDER) )
        *want3D = FALSE;

    // 3) Control can override this global setting by defining
    // a border style, e.g. wxSUNKEN_BORDER
    if ( m_windowStyle & wxSUNKEN_BORDER  )
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
#if wxUSE_CTL3D
    if ( *want3D )
        nativeBorder = FALSE;
#endif

    DWORD exStyle = MakeExtendedStyle(m_windowStyle, !nativeBorder);

    // If we want 3D, but haven't specified a border here,
    // apply the default border style specified.
    // TODO what about non-Win95 WIN32? Does it have borders?
#if defined(__WIN95__) && !wxUSE_CTL3D
    if ( defaultBorderStyle && (*want3D) && ! ((m_windowStyle & wxDOUBLE_BORDER) || (m_windowStyle & wxRAISED_BORDER ) ||
        (m_windowStyle & wxSTATIC_BORDER) || (m_windowStyle & wxSIMPLE_BORDER) ))
        exStyle |= defaultBorderStyle; // WS_EX_CLIENTEDGE;
#endif
*/
    return exStyle;
}

