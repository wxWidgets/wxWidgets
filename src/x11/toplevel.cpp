///////////////////////////////////////////////////////////////////////////////
// Name:        x11/toplevel.cpp
// Purpose:     implements wxTopLevelWindow for X11
// Author:      Julian Smart
// Modified by:
// Created:     24.09.01
// RCS-ID:      $Id$
// Copyright:   (c) 2002 Julian Smart
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "toplevel.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/toplevel.h"
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/frame.h"
#endif //WX_PRECOMP

#include "wx/x11/private.h"

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

// list of all frames and modeless dialogs
// wxWindowList wxModelessWindows;

// ----------------------------------------------------------------------------
// wxTopLevelWindowX11 creation
// ----------------------------------------------------------------------------

void wxTopLevelWindowX11::Init()
{
    m_iconized =
    m_maximizeOnShow = FALSE;

    // unlike (almost?) all other windows, frames are created hidden
    m_isShown = FALSE;

    // Data to save/restore when calling ShowFullScreen
    m_fsStyle = 0;
    m_fsIsMaximized = FALSE;
    m_fsIsShowing = FALSE;
}

bool wxTopLevelWindowX11::CreateDialog(const wxString& title,
                                       const wxPoint& pos,
                                       const wxSize& size)
{
    // TODO
    return FALSE;
}

bool wxTopLevelWindowX11::CreateFrame(const wxString& title,
                                      const wxPoint& pos,
                                      const wxSize& size)
{
    // TODO
    return FALSE;
}

bool wxTopLevelWindowX11::Create(wxWindow *parent,
                                 wxWindowID id,
                                 const wxString& title,
                                 const wxPoint& pos,
                                 const wxSize& size,
                                 long style,
                                 const wxString& name)
{
    // init our fields
    Init();

    m_windowStyle = style;

    SetName(name);

    m_windowId = id == -1 ? NewControlId() : id;

    wxTopLevelWindows.Append(this);

    if ( parent )
        parent->AddChild(this);

    if ( GetExtraStyle() & wxTOPLEVEL_EX_DIALOG )
    {
        return CreateDialog(title, pos, size);
    }
    else // !dialog
    {
        return CreateFrame(title, pos, size);
    }
}

wxTopLevelWindowX11::~wxTopLevelWindowX11()
{
    wxTopLevelWindows.DeleteObject(this);

    if ( wxModelessWindows.Find(this) )
        wxModelessWindows.DeleteObject(this);

    // If this is the last top-level window, exit.
    if ( wxTheApp && (wxTopLevelWindows.Number() == 0) )
    {
        wxTheApp->SetTopWindow(NULL);

        if (wxTheApp->GetExitOnFrameDelete())
        {
            // Signal to the app that we're going to close
            wxTheApp->ExitMainLoop();
        }
    }
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowX11 showing
// ----------------------------------------------------------------------------

bool wxTopLevelWindowX11::Show(bool show)
{
    if ( !wxWindowBase::Show(show) )
        return FALSE;

    // TODO

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowX11 maximize/minimize
// ----------------------------------------------------------------------------

void wxTopLevelWindowX11::Maximize(bool maximize)
{
    // TODO
}

bool wxTopLevelWindowX11::IsMaximized() const
{
    // TODO
    return TRUE;
}

void wxTopLevelWindowX11::Iconize(bool iconize)
{
    // TODO
}

bool wxTopLevelWindowX11::IsIconized() const
{
    // TODO
    return m_iconized;
}

void wxTopLevelWindowX11::Restore()
{
    // TODO
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowX11 fullscreen
// ----------------------------------------------------------------------------

bool wxTopLevelWindowX11::ShowFullScreen(bool show, long style)
{
    if (show)
    {
        if (IsFullScreen())
            return FALSE;

        m_fsIsShowing = TRUE;
        m_fsStyle = style;

        // TODO

        return TRUE;
    }
    else
    {
        if (!IsFullScreen())
            return FALSE;

        m_fsIsShowing = FALSE;

        // TODO
        return TRUE;
    }
}

// ----------------------------------------------------------------------------
// wxTopLevelWindowX11 misc
// ----------------------------------------------------------------------------

void wxTopLevelWindowX11::SetIcon(const wxIcon& icon)
{
    // this sets m_icon
    wxTopLevelWindowBase::SetIcon(icon);

    // TODO
}
