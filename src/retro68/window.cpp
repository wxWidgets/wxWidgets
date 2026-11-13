/////////////////////////////////////////////////////////////////////////////
// Name:        src/retro68/window.cpp
// Purpose:     wxWindowRetro68 implementation
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/window.h"

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/log.h"
    #include "wx/nonownedwnd.h"
    #include "wx/toplevel.h"
    #include "wx/utils.h"
#endif

#include "wx/caret.h"

#include "wx/retro68/private/qd.h"

#define TRACE_EVENTS "events"
#define TRACE_PAINT  "paint"

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// global variables
// ---------------------------------------------------------------------------

// the window that has keyboard focus:
static wxWindowRetro68 *gs_focusedWindow = nullptr;

// the window that is about to be focused after currently focused
// one looses focus:
static wxWindow *gs_toBeFocusedWindow = nullptr;

// the window that has mouse capture
static wxWindowRetro68 *gs_mouseCapture = nullptr;

// the window under mouse, defined in evtloop.cpp
extern wxWindow *wxRetro68GetWindowUnderMouse();
extern void wxRetro68ForgetWindow(wxWindow* win);

// ---------------------------------------------------------------------------
// event tables
// ---------------------------------------------------------------------------

// in wxUniv this class is abstract because it doesn't have DoPopupMenu()
wxIMPLEMENT_ABSTRACT_CLASS(wxWindowRetro68, wxWindowBase);

wxBEGIN_EVENT_TABLE(wxWindowRetro68, wxWindowBase)
wxEND_EVENT_TABLE()

//-----------------------------------------------------------------------------
// global functions
//-----------------------------------------------------------------------------

wxWindow *wxGetActiveWindow()
{
    return wxMacFindWindow(FrontWindow());
}

// ----------------------------------------------------------------------------
// constructors and such
// ----------------------------------------------------------------------------

void wxWindowRetro68::Init()
{
    m_isShown = true;
    m_tlw = nullptr;
}

// Destructor
wxWindowRetro68::~wxWindowRetro68()
{
    SendDestroyEvent();

    if ( gs_mouseCapture == this )
        ReleaseMouse();

    if ( gs_focusedWindow == this )
        KillFocus();

    wxRetro68ForgetWindow(static_cast<wxWindow*>(this));

    DestroyChildren();
}

// real construction (Init() must have been called before!)
bool wxWindowRetro68::Create(wxWindow *parent,
                             wxWindowID id,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style,
                             const wxString& name)
{
    if ( !m_tlw && parent )
        m_tlw = parent->GetTLW();

    if ( !CreateBase(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    if ( parent )
        parent->AddChild(this);

    // set the size to something bogus initially, in case some code tries to
    // create wxWindowDC before SetSize() is called below:
    m_rect.width = m_rect.height = 1;

    int x, y, w, h;
    x = pos.x, y = pos.y;
    if ( x == -1  ) x = 0;
    if ( y == -1 ) y = 0;
    w = WidthDefault(size.x);
    h = HeightDefault(size.y);
    SetSize(x, y, w, h);

    return true;
}

// ---------------------------------------------------------------------------
// focus handling
// ---------------------------------------------------------------------------

void wxWindowRetro68::SetFocus()
{
    if ( gs_focusedWindow == this )
        return; // nothing to do, focused already

    wxWindowRetro68 *oldFocusedWindow = gs_focusedWindow;

    if ( gs_focusedWindow )
    {
        gs_toBeFocusedWindow = (wxWindow*)this;
        gs_focusedWindow->KillFocus();
        gs_toBeFocusedWindow = nullptr;
    }

    gs_focusedWindow = this;

    // notify the parent keeping track of focus for the kbd navigation
    // purposes that we got it
    wxChildFocusEvent eventFocus((wxWindow*)this);
    HandleWindowEvent(eventFocus);

    wxFocusEvent event(wxEVT_SET_FOCUS, GetId());
    event.SetEventObject(this);
    event.SetWindow((wxWindow*)oldFocusedWindow);
    HandleWindowEvent(event);

#if wxUSE_CARET
    // caret needs to be informed about focus change
    wxCaret *caret = GetCaret();
    if ( caret )
        caret->OnSetFocus();
#endif // wxUSE_CARET
}

void wxWindowRetro68::KillFocus()
{
    wxCHECK_RET( gs_focusedWindow == this,
                 "killing focus on window that doesn't have it" );

    gs_focusedWindow = nullptr;

    if ( m_isBeingDeleted )
        return; // don't send any events from dtor

#if wxUSE_CARET
    // caret needs to be informed about focus change
    wxCaret *caret = GetCaret();
    if ( caret )
        caret->OnKillFocus();
#endif // wxUSE_CARET

    wxFocusEvent event(wxEVT_KILL_FOCUS, GetId());
    event.SetEventObject(this);
    event.SetWindow(gs_toBeFocusedWindow);
    HandleWindowEvent(event);
}

// ----------------------------------------------------------------------------
// this wxWindowBase function is implemented here (in platform-specific file)
// because it is static and so couldn't be made virtual
// ----------------------------------------------------------------------------
wxWindow *wxWindowBase::DoFindFocus()
{
    return (wxWindow*)gs_focusedWindow;
}

// ---------------------------------------------------------------------------
// showing, hiding and Z-order
// ---------------------------------------------------------------------------

bool wxWindowRetro68::Show(bool show)
{
    if ( !wxWindowBase::Show(show) )
        return false;

    // Unlike Refresh(), DoRefreshWindow() doesn't check visibility, so
    // call it to force refresh of either this window (if showing) or its
    // parent area at the place of this window (if hiding):
    DoRefreshWindow();

    if ( !show && gs_focusedWindow )
    {
        // Don't leave the focus on a hidden window.
        for ( wxWindow* win = static_cast<wxWindow*>(gs_focusedWindow);
              win;
              win = win->GetParent() )
        {
            if ( win == this )
            {
                gs_focusedWindow->KillFocus();
                break;
            }

            if ( win->IsTopLevel() )
                break;
        }
    }

    return true;
}

// Raise the window to the top of the Z order
void wxWindowRetro68::Raise()
{
    wxWindow * const parent = GetParent();
    if ( !parent )
        return;

    wxWindowList& siblings = parent->GetChildren();
    siblings.DeleteObject(static_cast<wxWindow*>(this));
    siblings.Append(static_cast<wxWindow*>(this));

    DoRefreshWindow();
}

// Lower the window to the bottom of the Z order
void wxWindowRetro68::Lower()
{
    wxWindow * const parent = GetParent();
    if ( !parent )
        return;

    wxWindowList& siblings = parent->GetChildren();
    siblings.DeleteObject(static_cast<wxWindow*>(this));
    siblings.Insert(static_cast<wxWindow*>(this));

    DoRefreshWindow();
}

// ---------------------------------------------------------------------------
// mouse handling
// ---------------------------------------------------------------------------

void wxWindowRetro68::DoCaptureMouse()
{
    gs_mouseCapture = this;
}

void wxWindowRetro68::DoReleaseMouse()
{
    wxASSERT_MSG( gs_mouseCapture == this,
                  wxT("attempt to release mouse, but this window hasn't captured it") );

    gs_mouseCapture = nullptr;
}

/* static */ wxWindow *wxWindowBase::GetCapture()
{
    return (wxWindow*)gs_mouseCapture;
}

bool wxWindowRetro68::SetCursor(const wxCursor& cursor)
{
    if ( !wxWindowBase::SetCursor(cursor) )
    {
        // no change
        return false;
    }

    // Update the cursor immediately if the mouse is over this window.
    wxWindow* const winUnder = wxRetro68GetWindowUnderMouse();
    for ( wxWindow* win = winUnder; win; win = win->GetParent() )
    {
        if ( win == this )
        {
            winUnder->UpdateCursor();
            break;
        }

        if ( win->IsTopLevel() )
            break;
    }

    return true;
}

void wxWindowRetro68::UpdateCursor()
{
    if ( wxIsBusy() )
    {
        // Busy cursor overrides all the others.
        return;
    }

    for ( wxWindow* win = (wxWindow*)this; win; win = win->GetParent() )
    {
        const wxCursor& cursor = win->GetCursor();
        if ( cursor.IsOk() )
        {
            cursor.MacInstall();
            return;
        }

        if ( win->IsTopLevel() )
            break;
    }

    wxSTANDARD_CURSOR->MacInstall();
}

void wxWindowRetro68::WarpPointer(int WXUNUSED(x), int WXUNUSED(y))
{
    // There is no official way to move the mouse pointer under classic
    // Mac OS, so just don't do anything.
}

// Set this window to be the child of 'parent'.
bool wxWindowRetro68::Reparent(wxWindowBase *parent)
{
    if ( IsTopLevel() )
        return wxWindowBase::Reparent(parent);

    DoRefreshWindow();

    if ( !wxWindowBase::Reparent(parent) )
        return false;

    // Update the TLW pointer of this window and all its children.
    wxNonOwnedWindow* const tlw =
        parent ? static_cast<wxWindow*>(parent)->GetTLW() : nullptr;

    wxWindowList windows;
    windows.push_back(static_cast<wxWindow*>(this));
    while ( !windows.empty() )
    {
        wxWindow* const win = windows.front();
        windows.pop_front();

        if ( win->IsTopLevel() )
            continue;

        win->m_tlw = tlw;

        for ( wxWindow* child : win->GetChildren() )
            windows.push_back(child);
    }

    DoRefreshWindow();

    return true;
}

// ---------------------------------------------------------------------------
// moving and resizing
// ---------------------------------------------------------------------------

// Get total size
void wxWindowRetro68::DoGetSize(int *x, int *y) const
{
    if (x) *x = m_rect.width;
    if (y) *y = m_rect.height;
}

void wxWindowRetro68::DoGetPosition(int *x, int *y) const
{
    if (x) *x = m_rect.x;
    if (y) *y = m_rect.y;
}

wxPoint wxWindowRetro68::GetScreenPosOfClientOrigin() const
{
    wxCHECK_MSG( m_parent, wxPoint(), "non-TLW window without parent?" );

    return m_parent->GetScreenPosOfClientOrigin() +
            m_rect.GetPosition() +
            GetClientAreaOrigin();
}

wxPoint wxWindowRetro68::GetPortPosition() const
{
    if ( IsTopLevel() )
        return wxPoint(0, 0);

    wxCHECK_MSG( m_parent, wxPoint(), "non-TLW window without parent?" );

    return m_parent->GetPortPosition() +
            m_parent->GetClientAreaOrigin() +
            m_rect.GetPosition();
}

wxRect wxWindowRetro68::GetUncoveredRect() const
{
    wxRect r(GetSize());
    if ( IsTopLevel() )
        return r;

    wxWindow * const parent = GetParent();
    if ( !parent )
        return r;

    // intersect with parent's uncovered area, after offsetting it into our
    // coordinates; this will remove parts of 'r' that are outside of the
    // parent's area:
    wxRect rp(parent->GetUncoveredRect());

    // normal windows cannot extend out of its parent's client area:
    if ( !CanBeOutsideClientArea() )
        rp.Intersect(parent->GetClientRect());

    rp.Offset(-GetPosition());
    rp.Offset(-parent->GetClientAreaOrigin());

    r.Intersect(rp);

    return r;
}

// Return the screen position of the origin of the coordinates used by
// DoScreenToClient() and DoClientToScreen().
//
// For the top level windows, wxTopLevelWindowBase versions of these functions
// already account for the client area origin (i.e. the space taken by the
// menu bar, tool bar etc) themselves, so we must not do it here too, or it
// would be applied twice, resulting in wrong coordinates and, notably, in all
// the mouse clicks being dispatched to a wrong window.
wxPoint wxWindowRetro68::GetScreenOriginForConversion() const
{
    wxPoint o = GetScreenPosOfClientOrigin();
    if ( IsTopLevel() )
        o -= GetClientAreaOrigin();

    return o;
}

void wxWindowRetro68::DoScreenToClient(int *x, int *y) const
{
    const wxPoint o = GetScreenOriginForConversion();

    if (x) *x -= o.x;
    if (y) *y -= o.y;
}

void wxWindowRetro68::DoClientToScreen(int *x, int *y) const
{
    const wxPoint o = GetScreenOriginForConversion();

    if (x) *x += o.x;
    if (y) *y += o.y;
}

// Get size *available for subwindows* i.e. excluding menu bar etc.
void wxWindowRetro68::DoGetClientSize(int *x, int *y) const
{
    DoGetSize(x, y);
}

void wxWindowRetro68::DoMoveWindow(int x, int y, int width, int height)
{
    // NB: [x,y] arguments are in (parent's) window coordinates, while
    //     m_rect.{x,y} are in (parent's) client coordinates. That's why we
    //     offset by parentOrigin in some places below

    wxPoint parentOrigin(0, 0);
    AdjustForParentClientOrigin(parentOrigin.x, parentOrigin.y);

    wxRect oldpos(m_rect);
    oldpos.Offset(parentOrigin);

    wxRect newpos(x, y, width, height);

    // input [x,y] is in window coords, but we store client coords in m_rect:
    m_rect = newpos;
    m_rect.Offset(-parentOrigin);

    if ( IsShown() )
    {
        // queue both former and new position of the window for repainting:
        wxWindow *parent = GetParent();

        // only refresh the visible parts:
        if ( !CanBeOutsideClientArea() )
        {
            wxRect parentClient(parent->GetClientSize());
            parentClient.Offset(parent->GetClientAreaOrigin());
            oldpos.Intersect(parentClient);
            newpos.Intersect(parentClient);
        }

        parent->DoRefreshRect(oldpos);
        parent->DoRefreshRect(newpos);
    }
}

// set the size of the window: if the dimensions are positive, just use them,
// but if any of them is equal to -1, it means that we must find the value for
// it ourselves (unless sizeFlags contains wxSIZE_ALLOW_MINUS_ONE flag, in
// which case -1 is a valid value for x and y)
//
// If sizeFlags contains wxSIZE_AUTO_WIDTH/HEIGHT flags (default), we calculate
// the width/height to best suit our contents, otherwise we reuse the current
// width/height
void wxWindowRetro68::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    // get the current size and position...
    int currentX, currentY;
    GetPosition(&currentX, &currentY);
    int currentW,currentH;
    GetSize(&currentW, &currentH);

    if ( x == -1 && !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE) )
        x = currentX;
    if ( y == -1 && !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE) )
        y = currentY;

    // ... and don't do anything (avoiding flicker) if it's already ok
    if ( x == currentX && y == currentY &&
         width == currentW && height == currentH )
    {
        return;
    }

    wxSize size(-1, -1);
    if ( width == -1 )
    {
        if ( sizeFlags & wxSIZE_AUTO_WIDTH )
        {
            size = DoGetBestSize();
            width = size.x;
        }
        else
        {
            // just take the current one
            width = currentW;
        }
    }

    if ( height == -1 )
    {
        if ( sizeFlags & wxSIZE_AUTO_HEIGHT )
        {
            if ( size.x == -1 )
            {
                size = DoGetBestSize();
            }
            //else: already called DoGetBestSize() above

            height = size.y;
        }
        else
        {
            // just take the current one
            height = currentH;
        }
    }

    int maxWidth = GetMaxWidth(),
        minWidth = GetMinWidth(),
        maxHeight = GetMaxHeight(),
        minHeight = GetMinHeight();

    if ( minWidth != -1 && width < minWidth ) width = minWidth;
    if ( maxWidth != -1 && width > maxWidth ) width = maxWidth;
    if ( minHeight != -1 && height < minHeight ) height = minHeight;
    if ( maxHeight != -1 && height > maxHeight ) height = maxHeight;

    if ( x != currentX || y != currentY ||
         width != currentW || height != currentH )
    {
        AdjustForParentClientOrigin(x, y, sizeFlags);
        DoMoveWindow(x, y, width, height);

        wxSize newSize(width, height);
        wxSizeEvent event(newSize, GetId());
        event.SetEventObject(this);
        HandleWindowEvent(event);
    }
}

void wxWindowRetro68::DoSetClientSize(int width, int height)
{
    SetSize(width, height);
}

// ---------------------------------------------------------------------------
// text metrics
// ---------------------------------------------------------------------------

int wxWindowRetro68::GetCharHeight() const
{
    wxClientDC dc((wxWindow*)this);
    return dc.GetCharHeight();
}

int wxWindowRetro68::GetCharWidth() const
{
    wxClientDC dc((wxWindow*)this);
    return dc.GetCharWidth();
}

void wxWindowRetro68::DoGetTextExtent(const wxString& string,
                                      int *x, int *y,
                                      int *descent,
                                      int *externalLeading,
                                      const wxFont *theFont) const
{
    wxClientDC dc((wxWindow*)this);
    dc.GetTextExtent(string, x, y, descent, externalLeading, (wxFont*)theFont);
}


// ---------------------------------------------------------------------------
// painting
// ---------------------------------------------------------------------------

void wxWindowRetro68::Refresh(bool WXUNUSED(eraseBack), const wxRect *rect)
{
    if ( !IsShown() || IsFrozen() )
        return;

    // NB[1]: We intentionally ignore the eraseBack argument here. This is
    //        because of the way painting is implemented: the refresh
    //        request is propagated up to wxTLW, which is then painted in
    //        top-down order. This means that this window's area is first
    //        painted by its parent and this window is then painted over it, so
    //        it's not safe to not paint this window's background even if
    //        eraseBack=false.
    // NB[2]: wxWindow::Refresh() takes the rectangle in client coords, but
    //        wxUniv translates it to window coords before passing it to
    //        wxWindowRetro68::Refresh(), so we can directly pass the rect to
    //        DoRefreshRect (which takes window, not client, coords) here.
    if ( rect )
        DoRefreshRect(*rect);
    else
        DoRefreshWindow();
}

void wxWindowRetro68::DoRefreshWindow()
{
    // NB: DoRefreshRect() takes window coords, not client, so this is correct
    DoRefreshRect(wxRect(GetSize()));
}

void wxWindowRetro68::DoRefreshRect(const wxRect& rect)
{
    wxWindow *parent = GetParent();
    wxCHECK_RET( parent, "no parent" );

    // don't overlap outside of the window (NB: 'rect' is in window coords):
    wxRect r(rect);
    r.Intersect(wxRect(GetSize()));
    if ( r.IsEmpty() )
        return;

    wxLogTrace(TRACE_PAINT,
               "%p ('%s'): refresh rect [%i,%i,%i,%i]",
               this, GetName(),
               rect.x, rect.y, rect.GetRight(), rect.GetBottom());

    // convert the refresh rectangle to parent's coordinates and
    // recursively refresh the parent:
    r.Offset(GetPosition());
    r.Offset(parent->GetClientAreaOrigin());

    // normal windows cannot extend out of its parent's client area, so don't
    // refresh any hidden parts:
    if ( !CanBeOutsideClientArea() )
        r.Intersect(parent->GetClientRect());

    parent->DoRefreshRect(r);
}

void wxWindowRetro68::Update()
{
    if ( !IsShown() || IsFrozen() )
        return;

    if ( m_tlw && m_tlw != this )
        m_tlw->Update();
}

void wxWindowRetro68::DoThaw()
{
    if ( IsShown() )
        DoRefreshWindow();
}

void wxWindowRetro68::PaintWindow(const wxRect& rect)
{
    wxCHECK_RET( !IsFrozen() && IsShown(), "shouldn't be called" );

    wxLogTrace(TRACE_PAINT,
               "%p ('%s'): painting region [%i,%i,%i,%i]",
               this, GetName(),
               rect.x, rect.y, rect.GetRight(), rect.GetBottom());

    m_updateRegion = rect;

    // NB: unconditionally send wxEraseEvent, because our implementation of
    //     wxWindow::Refresh() ignores the eraseBack argument
    {
        wxWindowDC dc((wxWindow*)this);
        wxEraseEvent eventEr(m_windowId, &dc);
        eventEr.SetEventObject(this);
        HandleWindowEvent(eventEr);
    }

    wxRect clientRect(GetClientRect());

    // only send wxNcPaintEvent if drawing at least part of nonclient area:
    if ( !clientRect.Contains(rect) )
    {
        wxNcPaintEvent eventNc(this);
        HandleWindowEvent(eventNc);
    }

    // only send wxPaintEvent if drawing at least part of client area:
    if ( rect.Intersects(clientRect) )
    {
        wxPaintEvent eventPt(this);
        HandleWindowEvent(eventPt);
    }

    m_updateRegion.Clear();

    // client area portion of 'rect':
    wxRect rectClientOnly(rect);
    rectClientOnly.Intersect(clientRect);

    // paint the children:
    wxPoint origin = GetClientAreaOrigin();
    wxWindowList& children = GetChildren();
    for ( wxWindowList::iterator i = children.begin();
          i != children.end(); ++i )
    {
        wxWindow *child = *i;

        // top level children are separate native windows painted on their own
        if ( child->IsTopLevel() )
            continue;

        if ( child->IsFrozen() || !child->IsShown() )
            continue; // don't paint anything if the window is frozen or hidden

        // compute child's area to repaint
        wxRect childrect(child->GetRect());
        childrect.Offset(origin);

        if ( child->CanBeOutsideClientArea() )
            childrect.Intersect(rect);
        else
            childrect.Intersect(rectClientOnly);

        if ( childrect.IsEmpty() )
            continue;

        // and repaint it:
        childrect.Offset(-child->GetPosition());
        childrect.Offset(-origin);
        child->PaintWindow(childrect);
    }
}

wxWindow* wxWindowRetro68::FindDeepestChildAt(const wxPoint& pt)
{
    const wxPoint origin = GetClientAreaOrigin();

    // Iterate in the reverse order as the last children are on top.
    wxWindowList& children = GetChildren();
    for ( wxWindowList::reverse_iterator i = children.rbegin();
          i != children.rend(); ++i )
    {
        wxWindow * const child = *i;

        if ( child->IsTopLevel() || !child->IsShown() )
            continue;

        wxRect childRect(child->GetRect());
        childRect.Offset(origin);
        if ( childRect.Contains(pt) )
        {
            return child->FindDeepestChildAt(pt - childRect.GetPosition());
        }
    }

    return static_cast<wxWindow*>(this);
}

// ---------------------------------------------------------------------------
// keyboard events handling
// ---------------------------------------------------------------------------

namespace
{

// Virtual key codes of the Apple keyboards.
enum
{
    kVK_Return          = 0x24,
    kVK_Tab             = 0x30,
    kVK_Space           = 0x31,
    kVK_Delete          = 0x33,
    kVK_Escape          = 0x35,
    kVK_Command         = 0x37,
    kVK_Shift           = 0x38,
    kVK_CapsLock        = 0x39,
    kVK_Option          = 0x3A,
    kVK_Control         = 0x3B,
    kVK_KeypadDecimal   = 0x41,
    kVK_KeypadMultiply  = 0x43,
    kVK_KeypadPlus      = 0x45,
    kVK_KeypadClear     = 0x47,
    kVK_KeypadDivide    = 0x4B,
    kVK_KeypadEnter     = 0x4C,
    kVK_KeypadMinus     = 0x4E,
    kVK_KeypadEquals    = 0x51,
    kVK_Keypad0         = 0x52,
    kVK_Keypad1         = 0x53,
    kVK_Keypad2         = 0x54,
    kVK_Keypad3         = 0x55,
    kVK_Keypad4         = 0x56,
    kVK_Keypad5         = 0x57,
    kVK_Keypad6         = 0x58,
    kVK_Keypad7         = 0x59,
    kVK_Keypad8         = 0x5B,
    kVK_Keypad9         = 0x5C,
    kVK_F5              = 0x60,
    kVK_F6              = 0x61,
    kVK_F7              = 0x62,
    kVK_F3              = 0x63,
    kVK_F8              = 0x64,
    kVK_F9              = 0x65,
    kVK_F11             = 0x67,
    kVK_F13             = 0x69,
    kVK_F14             = 0x6B,
    kVK_F10             = 0x6D,
    kVK_F12             = 0x6F,
    kVK_F15             = 0x71,
    kVK_Help            = 0x72,
    kVK_Home            = 0x73,
    kVK_PageUp          = 0x74,
    kVK_ForwardDelete   = 0x75,
    kVK_F4              = 0x76,
    kVK_End             = 0x77,
    kVK_F2              = 0x78,
    kVK_PageDown        = 0x79,
    kVK_F1              = 0x7A,
    kVK_LeftArrow       = 0x7B,
    kVK_RightArrow      = 0x7C,
    kVK_DownArrow       = 0x7D,
    kVK_UpArrow         = 0x7E
};

// Translate the virtual key code to wx key code for special keys, return 0
// for the other ones.
int GetSpecialKeyCode(int vk)
{
    switch ( vk )
    {
        case kVK_Return:            return WXK_RETURN;
        case kVK_Tab:               return WXK_TAB;
        case kVK_Space:             return WXK_SPACE;
        case kVK_Delete:            return WXK_BACK;
        case kVK_Escape:            return WXK_ESCAPE;
        case kVK_Command:           return WXK_CONTROL;
        case kVK_Shift:             return WXK_SHIFT;
        case kVK_CapsLock:          return WXK_CAPITAL;
        case kVK_Option:            return WXK_ALT;
        case kVK_Control:           return WXK_RAW_CONTROL;
        case kVK_KeypadDecimal:     return WXK_NUMPAD_DECIMAL;
        case kVK_KeypadMultiply:    return WXK_NUMPAD_MULTIPLY;
        case kVK_KeypadPlus:        return WXK_NUMPAD_ADD;
        case kVK_KeypadClear:       return WXK_CLEAR;
        case kVK_KeypadDivide:      return WXK_NUMPAD_DIVIDE;
        case kVK_KeypadEnter:       return WXK_NUMPAD_ENTER;
        case kVK_KeypadMinus:       return WXK_NUMPAD_SUBTRACT;
        case kVK_KeypadEquals:      return WXK_NUMPAD_EQUAL;
        case kVK_Keypad0:           return WXK_NUMPAD0;
        case kVK_Keypad1:           return WXK_NUMPAD1;
        case kVK_Keypad2:           return WXK_NUMPAD2;
        case kVK_Keypad3:           return WXK_NUMPAD3;
        case kVK_Keypad4:           return WXK_NUMPAD4;
        case kVK_Keypad5:           return WXK_NUMPAD5;
        case kVK_Keypad6:           return WXK_NUMPAD6;
        case kVK_Keypad7:           return WXK_NUMPAD7;
        case kVK_Keypad8:           return WXK_NUMPAD8;
        case kVK_Keypad9:           return WXK_NUMPAD9;
        case kVK_F1:                return WXK_F1;
        case kVK_F2:                return WXK_F2;
        case kVK_F3:                return WXK_F3;
        case kVK_F4:                return WXK_F4;
        case kVK_F5:                return WXK_F5;
        case kVK_F6:                return WXK_F6;
        case kVK_F7:                return WXK_F7;
        case kVK_F8:                return WXK_F8;
        case kVK_F9:                return WXK_F9;
        case kVK_F10:               return WXK_F10;
        case kVK_F11:               return WXK_F11;
        case kVK_F12:               return WXK_F12;
        case kVK_F13:               return WXK_F13;
        case kVK_F14:               return WXK_F14;
        case kVK_F15:               return WXK_F15;
        case kVK_Help:              return WXK_HELP;
        case kVK_Home:              return WXK_HOME;
        case kVK_PageUp:            return WXK_PAGEUP;
        case kVK_ForwardDelete:     return WXK_DELETE;
        case kVK_End:               return WXK_END;
        case kVK_PageDown:          return WXK_PAGEDOWN;
        case kVK_LeftArrow:         return WXK_LEFT;
        case kVK_RightArrow:        return WXK_RIGHT;
        case kVK_DownArrow:         return WXK_DOWN;
        case kVK_UpArrow:           return WXK_UP;
    }

    return 0;
}

// Convert the character in the system encoding to Unicode.
wxChar32 CharToUnicode(unsigned char ch)
{
    if ( ch < 0x80 )
        return ch;

    const char buf[2] = { static_cast<char>(ch), '\0' };
    const wxString s(buf, wxConvLibc);

    return s.empty() ? 0 : static_cast<wxChar32>(s[0].GetValue());
}

} // anonymous namespace

bool wxWindowRetro68::HandleKeyEvent(const EventRecord& e, bool isKeyUp)
{
    if ( !IsEnabled() )
        return false;

    const unsigned char ch = e.message & charCodeMask;
    const int vk = (e.message & keyCodeMask) >> 8;

    // fill in wxKeyEvent fields:
    wxKeyEvent event;
    event.SetEventObject(this);
    event.SetTimestamp(wxMacTicksToMillis(e.when));
    event.m_rawCode = vk;
    event.m_rawFlags = e.modifiers;

    // Map Command key to wx "control" modifier, as this is what is used for
    // the keyboard shortcuts in the other ports, and use "meta" for the
    // real Control key.
    event.m_shiftDown = (e.modifiers & shiftKey) != 0;
    event.m_controlDown = (e.modifiers & cmdKey) != 0;
    event.m_altDown = (e.modifiers & optionKey) != 0;
    event.m_metaDown = (e.modifiers & wxMAC_CONTROL_KEY) != 0;

    int keyCode = GetSpecialKeyCode(vk);
    wxChar32 uniChar = 0;
    if ( !keyCode )
    {
        // Control key changes the letters to control characters, undo it.
        unsigned char c = ch;
        if ( c > 0 && c < 0x20 && (e.modifiers & wxMAC_CONTROL_KEY) )
            c += '@';

        uniChar = CharToUnicode(c);

        keyCode = uniChar < 0x80 ? static_cast<int>(wxToupper(uniChar))
                                 : WXK_NONE;
    }
    else if ( keyCode == WXK_SPACE || keyCode == WXK_RETURN ||
                keyCode == WXK_TAB || keyCode == WXK_BACK ||
                    keyCode == WXK_ESCAPE || keyCode == WXK_DELETE )
    {
        uniChar = keyCode;
    }

    event.m_keyCode = keyCode;
    event.m_uniChar = uniChar;

    // the event doesn't carry mouse position, use the current one
    wxPoint pt = wxGetMousePosition();
    ScreenToClient(&pt.x, &pt.y);
    event.m_x = pt.x;
    event.m_y = pt.y;

    if ( isKeyUp )
    {
        event.SetEventType(wxEVT_KEY_UP);
        return HandleWindowEvent(event);
    }

    const bool isTab = (keyCode == WXK_TAB);

    event.SetEventType(wxEVT_KEY_DOWN);

    if ( HandleWindowEvent(event) )
        return true;

    // only send wxEVT_CHAR event if not processed yet:
    if ( uniChar && !(e.modifiers & cmdKey) )
    {
        // wxEVT_CHAR uses the untranslated character code
        event.m_keyCode = uniChar < 0x80 ? static_cast<int>(uniChar) : WXK_NONE;
        if ( !GetSpecialKeyCode(vk) && ch < 0x20 && (e.modifiers & wxMAC_CONTROL_KEY) )
            event.m_keyCode = ch;

        event.SetEventType(wxEVT_CHAR);
        if ( HandleWindowEvent(event) )
            return true;
    }
    else if ( GetSpecialKeyCode(vk) )
    {
        // generate wxEVT_CHAR for the special keys too, as other ports do
        event.SetEventType(wxEVT_CHAR);
        if ( HandleWindowEvent(event) )
            return true;
    }

    // Synthesize navigation key event, but do it only if the TAB key
    // wasn't handled yet:
    if ( isTab && GetParent() && GetParent()->HasFlag(wxTAB_TRAVERSAL) )
    {
        wxNavigationKeyEvent navEvent;
        navEvent.SetEventObject(GetParent());
        // Shift-TAB goes in reverse direction:
        navEvent.SetDirection(!event.m_shiftDown);
        // Ctrl-TAB changes the (parent) window, i.e. switch notebook page:
        navEvent.SetWindowChange(event.m_controlDown);
        navEvent.SetCurrentFocus(wxStaticCast(this, wxWindow));
        return GetParent()->HandleWindowEvent(navEvent);
    }

    return false;
}

// ---------------------------------------------------------------------------
// global functions
// ---------------------------------------------------------------------------

wxWindow* wxFindWindowAtPoint(const wxPoint& pt)
{
    Point where;
    where.h = wxMacClampCoord(pt.x);
    where.v = wxMacClampCoord(pt.y);

    WindowRef win;
    if ( FindWindow(where, &win) != inContent )
        return nullptr;

    wxNonOwnedWindow* const tlw = wxMacFindWindow(win);
    if ( !tlw )
        return nullptr;

    wxPoint ptClient(pt);
    tlw->ScreenToClient(&ptClient.x, &ptClient.y);

    return tlw->FindDeepestChildAt(ptClient + tlw->GetClientAreaOrigin());
}
