/////////////////////////////////////////////////////////////////////////////
// Name:        common/toplvcmn.cpp
// Purpose:     common (for all platforms) wxTopLevelWindow functions
// Author:      Julian Smart, Vadim Zeitlin
// Created:     01/02/97
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling and Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "toplevelbase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/toplevel.h"
    #include "wx/dcclient.h"
    #include "wx/app.h"
#endif // WX_PRECOMP

// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxTopLevelWindowBase, wxWindow)
    EVT_CLOSE(wxTopLevelWindowBase::OnCloseWindow)
    EVT_SIZE(wxTopLevelWindowBase::OnSize)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxTopLevelWindow, wxWindow)

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
        wxTheApp->SetTopWindow(NULL);

    bool shouldExit = IsLastBeforeExit();

    wxTopLevelWindows.DeleteObject(this);

    if ( shouldExit )
    {
        // then do it
        wxTheApp->ExitMainLoop();
    }
}

bool wxTopLevelWindowBase::Destroy()
{
    // delayed destruction: the frame will be deleted during the next idle
    // loop iteration
    if ( !wxPendingDelete.Member(this) )
        wxPendingDelete.Append(this);

    if (wxTopLevelWindows.GetCount() > 1)
    {
        // Hide it immediately. This should
        // not be done if this TLW is the
        // only one left since we then would
        // risk not to get any idle events
        // at all anymore during which we
        // could delete any pending events.
        Hide();
    }

    return true;
}

bool wxTopLevelWindowBase::IsLastBeforeExit() const
{
    // we exit the application if there are no more top level windows left
    // normally but wxApp can prevent this from happening
    return wxTopLevelWindows.GetCount() == 1 &&
            wxTopLevelWindows.GetFirst()->GetData() == (wxWindow *)this &&
            wxTheApp && wxTheApp->GetExitOnFrameDelete();
}

// ----------------------------------------------------------------------------
// wxTopLevelWindow geometry
// ----------------------------------------------------------------------------

wxSize wxTopLevelWindowBase::GetMaxSize() const
{
    wxSize  size( GetMaxWidth(), GetMaxHeight() );
    int     w, h;

    wxClientDisplayRect( 0, 0, &w, &h );

    if( size.GetWidth() == wxDefaultCoord )
        size.SetWidth( w );

    if( size.GetHeight() == wxDefaultCoord )
        size.SetHeight( h );

    return size;
}

/* static */
wxSize wxTopLevelWindowBase::GetDefaultSize()
{
    wxSize size = wxGetClientDisplayRect().GetSize();

    // create proportionally bigger windows on small screens
    if ( size.x >= 1024 )
        size.x = 400;
    else if ( size.x >= 800 )
        size.x = 300;
    else if ( size.x >= 320 )
        size.x = 240;

    if ( size.y >= 768 )
        size.y = 250;
    else if ( size.y > 200 )
    {
        size.y *= 2;
        size.y /= 3;
    }

    return size;
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


// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

// default resizing behaviour - if only ONE subwindow, resize to fill the
// whole client area
void wxTopLevelWindowBase::DoLayout()
{
    // if we're using constraints or sizers - do use them
    if ( GetAutoLayout() )
    {
        Layout();
    }
    else
    {
        // do we have _exactly_ one child?
        wxWindow *child = (wxWindow *)NULL;
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
                if ( child )
                {
                    return;     // it's our second subwindow - nothing to do
                }

                child = win;
            }
        }

        // do we have any children at all?
        if ( child )
        {
            // exactly one child - set it's size to fill the whole frame
            int clientW, clientH;
            DoGetClientSize(&clientW, &clientH);

            // for whatever reasons, wxGTK wants to have a small offset - it
            // probably looks better with it?
#ifdef __WXGTK__
            static const int ofs = 1;
#else
            static const int ofs = 0;
#endif

            child->SetSize(ofs, ofs, clientW - 2*ofs, clientH - 2*ofs);
        }
    }
}

// The default implementation for the close window event.
void wxTopLevelWindowBase::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
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
    if ( event.GetSetEnabled() )
        Enable(event.GetEnabled());

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

