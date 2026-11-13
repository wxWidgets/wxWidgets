/////////////////////////////////////////////////////////////////////////////
// Name:        src/retro68/nonownedwnd.cpp
// Purpose:     implementation of wxNonOwnedWindow
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/toplevel.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/dialog.h"
#endif // WX_PRECOMP

#include "wx/retro68/private/qd.h"
#include "wx/retro68/dc.h"

#include <vector>
#include <algorithm>

#define TRACE_PAINT  "paint"

// ============================================================================
// globals
// ============================================================================

namespace
{

// All existing native windows.
std::vector<wxNonOwnedWindow*> gs_allWindows;

// Size of the size box in the bottom right corner of resizable windows.
const int wxMAC_SIZE_BOX_SIZE = 15;

} // anonymous namespace

wxNonOwnedWindow* wxMacFindWindow(WindowRef win)
{
    if ( !win )
        return nullptr;

    for ( wxNonOwnedWindow* const w : gs_allWindows )
    {
        if ( w->GetWindowRef() == win )
            return w;
    }

    return nullptr;
}

Rect wxMacGetWindowStructureRect(WindowRef win)
{
    Rect r;
#if TARGET_API_MAC_CARBON
    GetWindowBounds(win, kWindowStructureRgn, &r);
#else
    r = (**reinterpret_cast<WindowPeek>(win)->strucRgn).rgnBBox;
#endif
    return r;
}

Rect wxMacGetWindowContentRect(WindowRef win)
{
    Rect r;
#if TARGET_API_MAC_CARBON
    GetWindowBounds(win, kWindowContentRgn, &r);
#else
    // Don't use the content region as it may not be computed for the hidden
    // windows, but compute the content rectangle from the port rectangle,
    // which is always valid.
    wxMacPortSaver saver;
    wxMacSetWindowPort(wxMacGetWindowGrafPtr(win));

    r = wxMacGetPortRect(wxMacGetWindowGrafPtr(win));
    LocalToGlobal(reinterpret_cast<Point*>(&r.top));
    LocalToGlobal(reinterpret_cast<Point*>(&r.bottom));
#endif
    return r;
}

// ============================================================================
// wxNonOwnedWindow implementation
// ============================================================================

// ----------------------------------------------------------------------------
// creation & destruction
// ----------------------------------------------------------------------------

void wxNonOwnedWindow::Init()
{
    m_isShown = false;
    m_macWindow = nullptr;
    m_isPainting = false;
    m_sizeSet = false;
}

short wxNonOwnedWindow::GetWindowProcID() const
{
    const long style = GetWindowStyleFlag();

    if ( !(style & wxCAPTION) )
    {
        // No title bar: use a plain box with a simple border.
        return plainDBox;
    }

    const bool resizable = (style & wxRESIZE_BORDER) != 0;

    if ( wxDynamicCast(this, wxDialog) && !resizable )
    {
        // Use the standard movable modal dialog appearance for dialogs.
        return movableDBoxProc;
    }

    if ( style & wxMAXIMIZE_BOX )
        return resizable ? wxMAC_ZOOM_DOC_PROC : wxMAC_ZOOM_NO_GROW_PROC;

    return resizable ? documentProc : noGrowDocProc;
}

bool wxNonOwnedWindow::Create(wxWindow *parent,
                              wxWindowID id,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              const wxString &name)
{
    m_tlw = this;

    // Set the style now as it is used by GetWindowProcID().
    SetWindowStyleFlag(style);

    // Create the window at the given position, we'll adjust it to take
    // into account the decorations later, when we can determine their size.
    const Rect bounds = wxMacRectFromWX(pos.x, pos.y + 20,
                                        wxMax(size.x, 1), wxMax(size.y, 1));

    const bool hasCloseBox = (style & wxCLOSE_BOX) != 0;
    const short procID = GetWindowProcID();
    const unsigned char emptyTitle[1] = { 0 };

    // NewCWindow() initializes the window port using the current device, so
    // make sure it's the screen and not some offscreen GWorld device used for
    // drawing on a bitmap before.
    wxMacPortSaver saver;

    WindowRef win;
    if ( wxMacHasColorQD() )
    {
        SetGDevice(GetMainDevice());

        win = NewCWindow(nullptr, &bounds, emptyTitle, false, procID,
                         reinterpret_cast<WindowRef>(-1), hasCloseBox, 0);
    }
    else
    {
        win = NewWindow(nullptr, &bounds, emptyTitle, false, procID,
                        reinterpret_cast<WindowRef>(-1), hasCloseBox, 0);
    }

    if ( !win )
        return false;

    m_macWindow = win;
    gs_allWindows.push_back(this);

    if ( !wxWindow::Create(nullptr, id, pos, size, style, name) )
        return false;

    SetParent(parent);
    if ( parent )
        parent->AddChild(this);

    return true;
}

wxNonOwnedWindow::~wxNonOwnedWindow()
{
    SendDestroyEvent();

    DestroyChildren();

    if ( !m_macWindow )
        return;

    gs_allWindows.erase(std::remove(gs_allWindows.begin(),
                                    gs_allWindows.end(),
                                    this),
                        gs_allWindows.end());

    wxRetro68DCImpl::InvalidateCachedPortState();

    {
        // DisposeWindow() can change the current port if it was the window
        // being destroyed, so make sure we don't leave a dangling port.
        CGrafPtr port;
        GDHandle device;
        GetGWorld(&port, &device);
        if ( reinterpret_cast<GrafPtr>(port) == GetPort() )
        {
            GrafPtr wmPort;
            GetWMgrPort(&wmPort);
            wxMacSetWindowPort(wmPort);
        }
    }

    DisposeWindow(m_macWindow);
    m_macWindow = nullptr;
}

GrafPtr wxNonOwnedWindow::GetPort() const
{
    return wxMacGetWindowGrafPtr(m_macWindow);
}

// ----------------------------------------------------------------------------
// window geometry
// ----------------------------------------------------------------------------

wxRect wxNonOwnedWindow::GetDecorationsSize() const
{
    // Return the rectangle with x/y and width/height fields containing the
    // left/top and right/bottom decorations sizes respectively.
    wxRect deco;

    const Rect content = wxMacGetWindowContentRect(m_macWindow);
    const Rect structure = wxMacGetWindowStructureRect(m_macWindow);
    if ( !EmptyRect(&structure) &&
            structure.top <= content.top && structure.left <= content.left &&
                structure.bottom >= content.bottom &&
                    structure.right >= content.right )
    {
        deco.x = content.left - structure.left;
        deco.y = content.top - structure.top;
        deco.width = structure.right - content.right;
        deco.height = structure.bottom - content.bottom;
    }

    if ( deco.y == 0 && (GetWindowStyleFlag() & wxCAPTION) )
    {
        // The structure region is not computed yet, use the values for the
        // standard System 7 document window: 1 pixel frame, 18 pixel title
        // bar and 1 pixel shadow.
        deco.x = 1;
        deco.y = 19;
        deco.width = 2;
        deco.height = 2;
    }

    return deco;
}

void wxNonOwnedWindow::DoGetPosition(int *x, int *y) const
{
    const Rect content = wxMacGetWindowContentRect(m_macWindow);
    const wxRect deco = GetDecorationsSize();

    if ( x )
        *x = content.left - deco.x;
    if ( y )
        *y = content.top - deco.y;
}

void wxNonOwnedWindow::DoGetSize(int *width, int *height) const
{
    const Rect content = wxMacGetWindowContentRect(m_macWindow);
    const wxRect deco = GetDecorationsSize();

    if ( width )
        *width = content.right - content.left + deco.x + deco.width;
    if ( height )
        *height = content.bottom - content.top + deco.y + deco.height;
}

void wxNonOwnedWindow::DoGetClientSize(int *width, int *height) const
{
    const Rect r = wxMacGetPortRect(GetPort());

    if ( width )
        *width = r.right - r.left;
    if ( height )
        *height = r.bottom - r.top;
}

void wxNonOwnedWindow::DoSetClientSize(int width, int height)
{
    const wxRect deco = GetDecorationsSize();

    SetSize(width + deco.x + deco.width, height + deco.y + deco.height);
}

wxPoint wxNonOwnedWindow::GetScreenPosOfClientOrigin() const
{
    const Rect content = wxMacGetWindowContentRect(m_macWindow);

    return wxPoint(content.left, content.top) + GetClientAreaOrigin();
}

void wxNonOwnedWindow::DoMoveWindow(int x, int y, int width, int height)
{
    const wxRect deco = GetDecorationsSize();

    wxRetro68DCImpl::InvalidateCachedPortState();

    const Rect content = wxMacGetWindowContentRect(m_macWindow);

    const int contentX = x + deco.x;
    const int contentY = y + deco.y;
    if ( content.left != contentX || content.top != contentY )
    {
        MoveWindow(m_macWindow,
                   wxMacClampCoord(contentX), wxMacClampCoord(contentY),
                   false);
    }

    const int contentW = wxMax(width - deco.x - deco.width, 1);
    const int contentH = wxMax(height - deco.y - deco.height, 1);
    if ( content.right - content.left != contentW ||
            content.bottom - content.top != contentH )
    {
        SizeWindow(m_macWindow,
                   wxMacClampCoord(contentW), wxMacClampCoord(contentH),
                   true);

        // We don't try to preserve the existing contents of the window
        // when it is resized, so just repaint everything.
        DoRefreshRect(wxRect(0, 0, contentW, contentH));
    }
}

bool wxNonOwnedWindow::IsMovable() const
{
    return HasFlag(wxCAPTION);
}

bool wxNonOwnedWindow::IsResizable() const
{
    return HasFlag(wxRESIZE_BORDER);
}

void wxNonOwnedWindow::HandleUserMove()
{
    wxMoveEvent event(GetPosition(), GetId());
    event.SetEventObject(this);
    HandleWindowEvent(event);
}

void wxNonOwnedWindow::HandleUserResize()
{
    wxRetro68DCImpl::InvalidateCachedPortState();

    DoRefreshRect(wxRect(GetClientSize()));

    wxSizeEvent event(GetSize(), GetId());
    event.SetEventObject(this);
    HandleWindowEvent(event);
}

// ----------------------------------------------------------------------------
// showing and hiding
// ----------------------------------------------------------------------------

bool wxNonOwnedWindow::Show(bool show)
{
    if ( !wxWindow::Show(show) )
        return false;

    if ( !m_sizeSet )
    {
        // Send the initial size event as the size of the window may have
        // been set before it was shown and its children may need to be laid
        // out.
        m_sizeSet = true;

        wxSizeEvent event(GetSize(), GetId());
        event.SetEventObject(this);
        HandleWindowEvent(event);
    }

    wxRetro68DCImpl::InvalidateCachedPortState();

    if ( show )
    {
        ShowWindow(m_macWindow);

        if ( ShouldActivateOnClick() )
            SelectWindow(m_macWindow);
        else
            BringToFront(m_macWindow);
    }
    else
    {
        HideWindow(m_macWindow);
    }

    return true;
}

void wxNonOwnedWindow::Raise()
{
    if ( !IsShown() )
        return;

    if ( ShouldActivateOnClick() )
        SelectWindow(m_macWindow);
    else
        BringToFront(m_macWindow);
}

void wxNonOwnedWindow::Lower()
{
    SendBehind(m_macWindow, nullptr);
}

void wxNonOwnedWindow::SetLabel(const wxString& label)
{
    SetNativeTitle(label);
}

void wxNonOwnedWindow::SetNativeTitle(const wxString& title)
{
    Str255 ptitle;
    wxMacStringToPascal(title, ptitle);
    SetWTitle(m_macWindow, ptitle);
}

// ----------------------------------------------------------------------------
// painting
// ----------------------------------------------------------------------------

void wxNonOwnedWindow::DoRefreshRect(const wxRect& rect)
{
    wxRect r(rect);
    r.Intersect(wxRect(GetClientSize()));
    if ( r.IsEmpty() )
        return;

    wxLogTrace(TRACE_PAINT,
               "%p ('%s'): [TLW] refresh rect [%i,%i,%i,%i]",
               this, GetName(),
               rect.x, rect.y, rect.GetRight(), rect.GetBottom());

    const Rect macRect = wxMacRectFromWX(r);

#if TARGET_API_MAC_CARBON
    InvalWindowRect(m_macWindow, &macRect);
#else
    wxMacPortSaver saver;
    wxMacSetWindowPort(GetPort());
    InvalRect(&macRect);
#endif

    wxRetro68DCImpl::InvalidateCachedPortState();
}

void wxNonOwnedWindow::Update()
{
    if ( !IsShown() || IsFrozen() )
        return;

#if !TARGET_API_MAC_CARBON
    // Don't do anything if there is nothing to update.
    if ( EmptyRgn(reinterpret_cast<WindowPeek>(m_macWindow)->updateRgn) )
        return;
#endif

    HandleUpdateEvent();
}

void wxNonOwnedWindow::HandleUpdateEvent()
{
    wxMacPortSaver saver;
    wxMacSetWindowPort(GetPort());

    wxRetro68DCImpl::InvalidateCachedPortState();

    BeginUpdate(m_macWindow);

    // The visible region now contains only the part of the window which
    // needs to be repainted.
    Rect bbox;
#if TARGET_API_MAC_CARBON
    RgnHandle visRgn = NewRgn();
    GetPortVisibleRegion(GetWindowPort(m_macWindow), visRgn);
    GetRegionBounds(visRgn, &bbox);
    DisposeRgn(visRgn);
#else
    bbox = (**GetPort()->visRgn).rgnBBox;
#endif

    if ( !EmptyRect(&bbox) && IsShown() && !IsFrozen() )
    {
        m_isPainting = true;

        PaintWindow(wxRectFromMac(bbox));

        m_isPainting = false;

        DrawSizeBox();
    }

    wxRetro68DCImpl::InvalidateCachedPortState();

    // The drawing code may have changed the port, so restore it.
    wxMacSetWindowPort(GetPort());
    EndUpdate(m_macWindow);
}

void wxNonOwnedWindow::DrawSizeBox()
{
    if ( !IsResizable() || !HasFlag(wxCAPTION) )
        return;

    wxMacPortSaver saver;
    wxMacSetWindowPort(GetPort());

    wxRetro68DCImpl::InvalidateCachedPortState();

    const Rect portRect = wxMacGetPortRect(GetPort());
    Rect box;
    box.right = portRect.right;
    box.bottom = portRect.bottom;
    box.left = box.right - wxMAC_SIZE_BOX_SIZE;
    box.top = box.bottom - wxMAC_SIZE_BOX_SIZE;

    RgnHandle oldClip = NewRgn();
    GetClip(oldClip);
    ClipRect(&box);

    PenNormal();
    RGBColor black = { 0, 0, 0 };
    RGBColor white = { 0xffff, 0xffff, 0xffff };
    if ( wxMacHasColorQD() )
    {
        RGBForeColor(&black);
        RGBBackColor(&white);
    }

    EraseRect(&box);
    FrameRect(&box);

    if ( FrontWindow() == m_macWindow )
    {
        // Draw the classic size box glyph: two overlapping squares.
        Rect small = { short(box.top + 3), short(box.left + 3),
                       short(box.top + 10), short(box.left + 10) };
        Rect big = { short(box.top + 5), short(box.left + 5),
                     short(box.top + 13), short(box.left + 13) };

        FrameRect(&big);
        EraseRect(&small);
        FrameRect(&small);
    }

    SetClip(oldClip);
    DisposeRgn(oldClip);
}

void wxNonOwnedWindow::HandleActivateEvent(bool WXUNUSED(activate))
{
    // Redraw the size box to show it in the appropriate state.
    DrawSizeBox();
}

void wxNonOwnedWindow::OnInternalIdle()
{
    wxWindow::OnInternalIdle();
}
