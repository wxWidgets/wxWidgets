/////////////////////////////////////////////////////////////////////////////
// Name:        src/retro68/toplevel.cpp
// Purpose:     wxTopLevelWindowRetro68 implementation
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/toplevel.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/display.h"

#include "wx/retro68/private/qd.h"

#define TRACE_EVENTS "events"

// ============================================================================
// wxTopLevelWindowRetro68
// ============================================================================

// ----------------------------------------------------------------------------
// creation & destruction
// ----------------------------------------------------------------------------

void wxTopLevelWindowRetro68::Init()
{
    m_isMaximized = false;
    m_fsIsShowing = false;
}

bool wxTopLevelWindowRetro68::Create(wxWindow *parent,
                                     wxWindowID id,
                                     const wxString& title,
                                     const wxPoint& posOrig,
                                     const wxSize& sizeOrig,
                                     long style,
                                     const wxString &name)
{
    // always create a frame of some reasonable, even if arbitrary, size (at
    // least for MSW compatibility)
    wxSize size(sizeOrig);
    if ( size.x == wxDefaultCoord || size.y == wxDefaultCoord )
    {
        wxSize sizeDefault = GetDefaultSize();
        if ( size.x == wxDefaultCoord )
            size.x = sizeDefault.x;
        if ( size.y == wxDefaultCoord )
            size.y = sizeDefault.y;
    }

    // Use staggered positions for the windows without explicitly specified
    // position, as is customary under Mac OS.
    wxPoint pos(posOrig);
    if ( pos.x == wxDefaultCoord || pos.y == wxDefaultCoord )
    {
        static int s_nextPos = 0;

        const int offset = 20 * (s_nextPos++ % 8);
        if ( pos.x == wxDefaultCoord )
            pos.x = 10 + offset;
        if ( pos.y == wxDefaultCoord )
            pos.y = wxMacGetMenuBarHeight() + 4 + offset;
    }

    // Ensure that the title bar is always visible below the menu bar.
    if ( pos.y < wxMacGetMenuBarHeight() )
        pos.y = wxMacGetMenuBarHeight();

    // This must be done before creating the window as wxWindowBase uses the
    // presence in this list to check if the window is a top level one and,
    // if it is not, sets the minimal window size to its initial size, which
    // would prevent the window from ever being made smaller than that.
    // Notice that wxTopLevelWindowBase dtor removes the window from the list
    // if creating it fails.
    wxTopLevelWindows.Append(this);

    if ( !wxNonOwnedWindow::Create(parent, id, pos, size, style, name) )
        return false;

    SetTitle(title);

    return true;
}

wxTopLevelWindowRetro68::~wxTopLevelWindowRetro68()
{
}

// ----------------------------------------------------------------------------
// title
// ----------------------------------------------------------------------------

void wxTopLevelWindowRetro68::SetTitle(const wxString& title)
{
    m_title = title;
    SetNativeTitle(title);
}

// ----------------------------------------------------------------------------
// showing and hiding
// ----------------------------------------------------------------------------

bool wxTopLevelWindowRetro68::ShowFullScreen(bool show, long WXUNUSED(style))
{
    if ( show == m_fsIsShowing )
        return true;

    m_fsIsShowing = show;

    if (show)
    {
        m_fsSaveFrame = GetRect();

        // Note that we don't hide the menu bar, as this is not supported in
        // all the systems, and so just make the window as big as possible.
        SetSize(wxGetClientDisplayRect());
    }
    else
    {
        SetSize(m_fsSaveFrame);
    }

    return true;
}

// ----------------------------------------------------------------------------
// maximize, minimize etc.
// ----------------------------------------------------------------------------

void wxTopLevelWindowRetro68::Maximize(bool maximize)
{
    if ( maximize && !m_isMaximized )
    {
        m_isMaximized = true;

        m_savedFrame = GetRect();

        // Leave a small margin around the window as is customary under Mac.
        wxRect r = wxGetClientDisplayRect();
        r.Deflate(2, 0);
        r.height -= 2;
        SetSize(r);
    }
    else if ( !maximize && m_isMaximized )
    {
        m_isMaximized = false;
        SetSize(m_savedFrame);
    }
}

bool wxTopLevelWindowRetro68::IsMaximized() const
{
    return m_isMaximized;
}

void wxTopLevelWindowRetro68::Restore()
{
    if ( IsMaximized() )
    {
        Maximize(false);
    }
}

void wxTopLevelWindowRetro68::Iconize(bool WXUNUSED(iconize))
{
    // There is no way to minimize windows under classic Mac OS (window
    // shading is controlled by the user only).
}

bool wxTopLevelWindowRetro68::IsIconized() const
{
    return false;
}

// ----------------------------------------------------------------------------
// activation and focus handling
// ----------------------------------------------------------------------------

bool wxTopLevelWindowRetro68::IsActive()
{
    return FrontWindow() == GetWindowRef() &&
            (!wxTheApp || wxTheApp->IsInForeground());
}

void wxTopLevelWindowRetro68::HandleActivateEvent(bool activate)
{
    wxNonOwnedWindow::HandleActivateEvent(activate);

    wxLogTrace(TRACE_EVENTS,
               "toplevel window %p ('%s') %s",
               this, GetName(),
               activate ? "activated" : "deactivated");

    wxActivateEvent event(wxEVT_ACTIVATE, activate, GetId());
    event.SetEventObject(this);
    HandleWindowEvent(event);

    // if a frame that doesn't have wx focus inside it was just activated, we
    // need to set focus to it (or its child):
    if ( activate )
    {
        wxWindow *focused = wxWindow::FindFocus();
        if ( !focused || focused->GetTLW() != this )
        {
            wxLogTrace(TRACE_EVENTS,
                       "setting wx focus to toplevel window %p ('%s')",
                       this, GetName());

            if ( CanAcceptFocus() )
                SetFocus();
            else
                wxLogTrace(TRACE_EVENTS, "...which doesn't accept it");
        }
    }
}
