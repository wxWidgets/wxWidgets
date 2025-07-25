/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/toplvcmn.cpp
// Purpose:     common (for all platforms) wxTopLevelWindow functions
// Author:      Julian Smart, Vadim Zeitlin
// Created:     01/02/97
// Copyright:   (c) 1998 Robert Roebling and Julian Smart
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
    #include "wx/dcclient.h"
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/display.h"
#include "wx/modalhook.h"

#include "wx/private/tlwgeom.h"

// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(wxTopLevelWindowBase, wxWindow)
    EVT_CLOSE(wxTopLevelWindowBase::OnCloseWindow)
    EVT_SIZE(wxTopLevelWindowBase::OnSize)
wxEND_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

wxIMPLEMENT_ABSTRACT_CLASS(wxTopLevelWindow, wxWindow);

// ----------------------------------------------------------------------------
// construction/destruction
// ----------------------------------------------------------------------------

wxTopLevelWindowBase::wxTopLevelWindowBase()
{
    // Unlike windows, top level windows are created hidden by default.
    m_isShown = false;
}

wxTopLevelWindowBase::~wxTopLevelWindowBase()
{
    // don't let wxTheApp keep any stale pointers to us
    if ( wxTheApp && wxTheApp->GetTopWindow() == this )
        wxTheApp->SetTopWindow(nullptr);

    wxTopLevelWindows.DeleteObject(this);

    // delete any our top level children which are still pending for deletion
    // immediately: this could happen if a child (e.g. a temporary dialog
    // created with this window as parent) was Destroy()'d) while this window
    // was deleted directly (with delete, or maybe just because it was created
    // on the stack) immediately afterwards and before the child TLW was really
    // destroyed -- not destroying it now would leave it alive with a dangling
    // parent pointer and result in a crash later
    for ( wxObjectList::iterator i = wxPendingDelete.begin();
          i != wxPendingDelete.end();
          )
    {
        wxWindow * const win = wxDynamicCast(*i, wxWindow);
        if ( win && wxGetTopLevelParent(win->GetParent()) == this )
        {
            wxPendingDelete.erase(i);

            delete win;

            // deleting it invalidated the list (and not only one node because
            // it could have resulted in deletion of other objects to)
            i = wxPendingDelete.begin();
        }
        else
        {
            ++i;
        }
    }

    if ( IsLastBeforeExit() )
    {
        // no other (important) windows left, quit the app
        wxTheApp->ExitMainLoop();
    }
}

bool wxTopLevelWindowBase::Destroy()
{
    // We can't delay the destruction if our parent is being already destroyed
    // as we will be deleted anyhow during its destruction and the pointer
    // stored in wxPendingDelete would become invalid, so just delete ourselves
    // immediately in this case.
    wxWindow* parent = GetParent();
    if ( (parent && parent->IsBeingDeleted()) || !GetHandle() )
    {
        return wxNonOwnedWindow::Destroy();
    }

    // delayed destruction: the frame will be deleted during the next idle
    // loop iteration
    if ( !wxPendingDelete.Member(this) )
        wxPendingDelete.Append(this);

    // normally we want to hide the window immediately so that it doesn't get
    // stuck on the screen while it's being destroyed, however we shouldn't
    // hide the last visible window as then we might not get any idle events
    // any more as no events will be sent to the hidden window and without idle
    // events we won't prune wxPendingDelete list and the application won't
    // terminate
    for ( wxWindowList::const_iterator i = wxTopLevelWindows.begin(),
                                     end = wxTopLevelWindows.end();
          i != end;
          ++i )
    {
        wxTopLevelWindow * const win = static_cast<wxTopLevelWindow *>(*i);
        if ( win != this && win->IsShown() )
        {
            // there remains at least one other visible TLW, we can hide this
            // one
            Hide();

            break;
        }
    }

    return true;
}

bool wxTopLevelWindowBase::IsLastBeforeExit() const
{
    // first of all, automatically exiting the app on last window close can be
    // completely disabled at wxTheApp level
    if ( !wxTheApp || !wxTheApp->GetExitOnFrameDelete() )
        return false;

    // second, never terminate the application after closing a child TLW
    // because this would close its parent unexpectedly -- notice that this
    // check is not redundant with the loop below, as the parent might return
    // false from its ShouldPreventAppExit() -- except if the child is being
    // deleted as part of the parent destruction
    if ( GetParent() && !GetParent()->IsBeingDeleted() )
        return false;

    wxWindowList::const_iterator i;
    const wxWindowList::const_iterator end = wxTopLevelWindows.end();

    // then decide whether we should exit at all
    for ( i = wxTopLevelWindows.begin(); i != end; ++i )
    {
        wxTopLevelWindow * const win = static_cast<wxTopLevelWindow *>(*i);
        if ( win->ShouldPreventAppExit() )
        {
            // there remains at least one important TLW, don't exit
            return false;
        }
    }

    // if yes, close all the other windows: this could still fail
    for ( i = wxTopLevelWindows.begin(); i != end; ++i )
    {
        // don't close twice the windows which are already marked for deletion
        wxTopLevelWindow * const win = static_cast<wxTopLevelWindow *>(*i);
        if ( !wxPendingDelete.Member(win) && !win->Close() )
        {
            // one of the windows refused to close, don't exit
            //
            // NB: of course, by now some other windows could have been already
            //     closed but there is really nothing we can do about it as we
            //     have no way just to ask the window if it can close without
            //     forcing it to do it
            return false;
        }
    }

    return true;
}

// ----------------------------------------------------------------------------
// wxTopLevelWindow geometry
// ----------------------------------------------------------------------------

void wxTopLevelWindowBase::SetMinSize(const wxSize& minSize)
{
    SetSizeHints(minSize, GetMaxSize());
}

void wxTopLevelWindowBase::SetMaxSize(const wxSize& maxSize)
{
    SetSizeHints(GetMinSize(), maxSize);
}

void wxTopLevelWindowBase::GetRectForTopLevelChildren(int *x, int *y, int *w, int *h)
{
    GetPosition(x,y);
    GetSize(w,h);
}

/* static */
wxSize wxTopLevelWindowBase::GetDefaultSize()
{
    wxSize size = wxGetClientDisplayRect().GetSize();
#ifndef __WXOSX_IPHONE__
    // create proportionally bigger windows on small screens but also scale the
    // size with DPI on the large screens to avoid creating windows too small
    // to fit anything at all when using high DPI
    if ( size.x >= 1024 )
        size.x = FromDIP(400, nullptr /* no window */);
    else if ( size.x >= 800 )
        size.x = 300;
    else if ( size.x >= 320 )
        size.x = 240;

    if ( size.y >= 768 )
        size.y = FromDIP(250, nullptr /* no window */);
    else if ( size.y > 200 )
    {
        size.y *= 2;
        size.y /= 3;
    }
#endif
    return size;
}

void wxTopLevelWindowBase::DoCentre(int dir)
{
    // on some platforms centering top level windows is impossible
    // because they are always maximized by guidelines or limitations
    //
    // and centering a maximized window doesn't make sense as its position
    // can't change
    if ( IsAlwaysMaximized() || IsMaximized() )
        return;

    // we need the display rect anyhow so store it first: notice that we should
    // be centered on the same display as our parent window, the display of
    // this window itself is not really defined yet
    wxDisplay dpy(GetParent() ? GetParent() : this);
    const wxRect rectDisplay(dpy.GetClientArea());

    // what should we centre this window on?
    wxRect rectParent;
    if ( !(dir & wxCENTRE_ON_SCREEN) && GetParent() )
    {
        // centre on parent window: notice that we need screen coordinates for
        // positioning this TLW
        rectParent = GetParent()->GetScreenRect();

        // if the parent is entirely off screen (happens at least with MDI
        // parent frame under Mac but could happen elsewhere too if the frame
        // was hidden/moved away for some reason), don't use it as otherwise
        // this window wouldn't be visible at all
        if ( !rectParent.Intersects(rectDisplay) )
        {
            // just centre on screen then
            rectParent = rectDisplay;
        }
    }
    else
    {
        // we were explicitly asked to centre this window on the entire screen
        // or if we have no parent anyhow and so can't centre on it
        rectParent = rectDisplay;
    }

    if ( !(dir & wxBOTH) )
        dir |= wxBOTH; // if neither is specified, center in both directions

    // the new window rect candidate
    wxRect rect = GetRect().CentreIn(rectParent, dir & ~wxCENTRE_ON_SCREEN);

    // we don't want to place the window off screen if Centre() is called as
    // this is (almost?) never wanted and it would be very difficult to prevent
    // it from happening from the user code if we didn't check for it here
    if ( !rectDisplay.Contains(rect.GetTopLeft()) )
    {
        // move the window just enough to make the corner visible
        int dx = rectDisplay.GetLeft() - rect.GetLeft();
        int dy = rectDisplay.GetTop() - rect.GetTop();
        rect.Offset(dx > 0 ? dx : 0, dy > 0 ? dy : 0);
    }

    if ( !rectDisplay.Contains(rect.GetBottomRight()) )
    {
        // do the same for this corner too
        int dx = rectDisplay.GetRight() - rect.GetRight();
        int dy = rectDisplay.GetBottom() - rect.GetBottom();
        rect.Offset(dx < 0 ? dx : 0, dy < 0 ? dy : 0);
    }

    // the window top left and bottom right corner are both visible now and
    // although the window might still be not entirely on screen (with 2
    // staggered displays for example) we wouldn't be able to improve the
    // layout much in such case, so we stop here

    // -1 could be valid coordinate here if there are several displays
    SetSize(rect, wxSIZE_ALLOW_MINUS_ONE);
}

// ----------------------------------------------------------------------------
// Default item management
// ----------------------------------------------------------------------------

wxWindow* wxTopLevelWindowBase::SetDefaultItem(wxWindow* win)
{
    wxWindow* const old = GetDefaultItem();
    m_winDefault = win;
    return old;
}

wxWindow* wxTopLevelWindowBase::SetTmpDefaultItem(wxWindow* win)
{
    wxWindow* const old = GetDefaultItem();
    m_winTmpDefault = win;
    return old;
}

// ----------------------------------------------------------------------------
// Saving/restoring geometry
// ----------------------------------------------------------------------------

bool wxTopLevelWindowBase::SaveGeometry(const GeometrySerializer& ser) const
{
    wxTLWGeometry geom;
    if ( !geom.GetFrom(static_cast<const wxTopLevelWindow*>(this)) )
        return false;

    return geom.Save(ser);
}

bool wxTopLevelWindowBase::RestoreToGeometry(GeometrySerializer& ser)
{
    wxTLWGeometry geom;
    if ( !geom.Restore(ser) )
        return false;

    return geom.ApplyTo(static_cast<wxTopLevelWindow*>(this));
}

// ----------------------------------------------------------------------------
// wxTopLevelWindow size management: we exclude the areas taken by
// menu/status/toolbars from the client area, so the client area is what's
// really available for the frame contents
// ----------------------------------------------------------------------------

void wxTopLevelWindowBase::DoScreenToClient(int *x, int *y) const
{
    wxWindow::DoScreenToClient(x, y);

    // translate the wxWindow client coords to our client coords
    wxPoint pt(GetClientAreaOrigin());
    if ( x )
        *x -= pt.x;
    if ( y )
        *y -= pt.y;
}

void wxTopLevelWindowBase::DoClientToScreen(int *x, int *y) const
{
    // our client area origin (0, 0) may be really something like (0, 30) for
    // wxWindow if we have a toolbar, account for it before translating
    wxPoint pt(GetClientAreaOrigin());
    if ( x )
        *x += pt.x;
    if ( y )
        *y += pt.y;

    wxWindow::DoClientToScreen(x, y);
}

bool wxTopLevelWindowBase::IsAlwaysMaximized() const
{
    return false;
}

// ----------------------------------------------------------------------------
// icons
// ----------------------------------------------------------------------------

wxIcon wxTopLevelWindowBase::GetIcon() const
{
    return m_icons.IsEmpty() ? wxIcon() : m_icons.GetIcon( -1 );
}

void wxTopLevelWindowBase::SetIcon(const wxIcon& icon)
{
    // passing wxNullIcon to SetIcon() is possible (it means that we shouldn't
    // have any icon), but adding an invalid icon to wxIconBundle is not
    wxIconBundle icons;
    if ( icon.IsOk() )
        icons.AddIcon(icon);

    SetIcons(icons);
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

bool wxTopLevelWindowBase::IsTopNavigationDomain(NavigationKind kind) const
{
    // This switch only exists to generate a compiler warning and force us to
    // revisit this code if any new kinds of navigation are added in the
    // future, but for now we block of them by default (some derived classes
    // relax this however).
    switch ( kind )
    {
        case Navigation_Tab:
        case Navigation_Accel:
            break;
    }

    return true;
}

wxWindow* wxTopLevelWindowBase::GetUniqueChild() const
{
    // do we have _exactly_ one child?
    wxWindow *child = nullptr;
    for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxWindow *win = node->GetData();

        // exclude top level and managed windows (status bar isn't
        // currently in the children list except under wxMac anyhow, but
        // it makes no harm to test for it)
        if ( !win->IsTopLevel() && !IsOneOfBars(win) )
        {
            // We don't take hidden children into account and we also consider
            // that something more complicated than just the default resizing
            // behaviour is necessary if there are any hidden windows, so give
            // up immediately in this case.
            if ( !win->IsShown() )
                return nullptr;

            // Also stop if it's not our first child.
            if ( child )
                return nullptr;

            child = win;
        }
    }

    return child;
}

bool wxTopLevelWindowBase::UsesAutoLayout() const
{
    return GetAutoLayout()
                || GetSizer()
#if wxUSE_CONSTRAINTS
                    || GetConstraints()
#endif
        ;
}

// default resizing behaviour - if only ONE subwindow, resize to fill the
// whole client area
bool wxTopLevelWindowBase::Layout()
{
    // We are called during the window destruction several times, e.g. as
    // wxFrame tries to adjust to its tool/status bars disappearing. But
    // actually doing the layout is pretty useless in this case as the window
    // will disappear anyhow -- so just don't bother.
    if ( IsBeingDeleted() )
        return false;


    // if we're using sizers or constraints - do use them
    if ( UsesAutoLayout() )
    {
        return wxNonOwnedWindow::Layout();
    }
    else
    {
        // do we have _exactly_ one child?
        if ( wxWindow* const child = GetUniqueChild() )
        {
            // yes - set its size to fill the whole frame
            int clientW, clientH;
            DoGetClientSize(&clientW, &clientH);

            child->SetSize(0, 0, clientW, clientH);

            return true;
        }
    }

    return false;
}

void wxTopLevelWindowBase::Fit()
{
    if ( !UsesAutoLayout() )
    {
        if ( wxWindow* const child = GetUniqueChild() )
        {
            SetClientSize(child->GetBestSize());
            return;
        }
    }

    return wxNonOwnedWindow::Fit();
}

wxSize wxTopLevelWindowBase::DoGetBestClientSize() const
{
    // The logic here parallels that of Layout() above.
    if ( !UsesAutoLayout() )
    {
        if ( wxWindow* const child = GetUniqueChild() )
            return child->GetBestSize();
    }

    return wxNonOwnedWindow::DoGetBestClientSize();
}

// The default implementation for the close window event.
void wxTopLevelWindowBase::OnCloseWindow(wxCloseEvent& event)
{
    if ( event.CanVeto() && wxModalDialogHook::GetOpenCount() )
    {
        // We can't close the window if there are any app-modal dialogs still
        // shown.
        event.Veto();
        return;
    }

    Destroy();
}

bool wxTopLevelWindowBase::SendIconizeEvent(bool iconized)
{
    wxIconizeEvent event(GetId(), iconized);
    event.SetEventObject(this);

    return GetEventHandler()->ProcessEvent(event);
}

// do the window-specific processing after processing the update event
void wxTopLevelWindowBase::DoUpdateWindowUI(wxUpdateUIEvent& event)
{
    // call inherited, but skip the wxControl's version, and call directly the
    // wxWindow's one instead, because the only reason why we are overriding this
    // function is that we want to use SetTitle() instead of wxControl::SetLabel()
    wxWindowBase::DoUpdateWindowUI(event);

    // update title
    if ( event.GetSetText() )
    {
        if ( event.GetText() != GetTitle() )
            SetTitle(event.GetText());
    }
}

void wxTopLevelWindowBase::RequestUserAttention(int WXUNUSED(flags))
{
    // it's probably better than do nothing, isn't it?
    Raise();
}
