/////////////////////////////////////////////////////////////////////////////
// Name:        common/toplvcmn.cpp
// Purpose:     common (for all platforms) wxTopLevelWindow functions
// Author:      Julian Smart, Vadim Zeitlin
// Created:     01/02/97
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
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
#endif // WX_PRECOMP

#include "wx/evtloop.h"

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

// FIXME: some platforms don't have wxTopLevelWindow yet
#ifdef wxTopLevelWindowNative
    IMPLEMENT_DYNAMIC_CLASS(wxTopLevelWindow, wxWindow)
#endif

// ----------------------------------------------------------------------------
// construction/destruction
// ----------------------------------------------------------------------------

wxTopLevelWindowBase::wxTopLevelWindowBase()
{
}

wxTopLevelWindowBase::~wxTopLevelWindowBase()
{
    // this destructor is required for Darwin
}

bool wxTopLevelWindowBase::Destroy()
{
    // delayed destruction: the frame will be deleted during the next idle
    // loop iteration
    if ( !wxPendingDelete.Member(this) )
        wxPendingDelete.Append(this);

    return TRUE;
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
void wxTopLevelWindowBase::OnSize(wxSizeEvent& WXUNUSED(event))
{
    // if we're using constraints - do use them
#if wxUSE_CONSTRAINTS
    if ( GetAutoLayout() )
    {
        Layout();
    }
    else
#endif // wxUSE_CONSTRAINTS
    {
        // do we have _exactly_ one child?
        wxWindow *child = (wxWindow *)NULL;
        for ( wxWindowList::Node *node = GetChildren().GetFirst();
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
#ifdef __WXPM__
                    AlterChildPos();
#endif
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
#ifdef __WXPM__
            UpdateInternalSize(child, clientH);
#endif
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

// ----------------------------------------------------------------------------
// interactive manipulation
// ----------------------------------------------------------------------------

#ifdef __WXUNIVERSAL__

#define wxINTERACTIVE_RESIZE_DIR \
          (wxINTERACTIVE_RESIZE_W | wxINTERACTIVE_RESIZE_E | \
           wxINTERACTIVE_RESIZE_S | wxINTERACTIVE_RESIZE_N)

struct wxInteractiveMoveData
{
    wxTopLevelWindowBase *m_window;
    wxEventLoop          *m_evtLoop;
    int                   m_flags;
    wxRect                m_rect;
    wxRect                m_rectOrig;
    wxPoint               m_pos;
    wxSize                m_minSize, m_maxSize;
};

class wxInteractiveMoveHandler : public wxEvtHandler
{
public:
    wxInteractiveMoveHandler(wxInteractiveMoveData& data) : m_data(data) {}

private:
    DECLARE_EVENT_TABLE()
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent& event);

    wxInteractiveMoveData& m_data;
};

BEGIN_EVENT_TABLE(wxInteractiveMoveHandler, wxEvtHandler)
    EVT_MOTION(wxInteractiveMoveHandler::OnMouseMove)
    EVT_LEFT_DOWN(wxInteractiveMoveHandler::OnMouseDown)
    EVT_LEFT_UP(wxInteractiveMoveHandler::OnMouseUp)
    EVT_KEY_DOWN(wxInteractiveMoveHandler::OnKeyDown)
END_EVENT_TABLE()


static inline LINKAGEMODE
void wxApplyResize(wxInteractiveMoveData& data, const wxPoint& diff)
{
    if ( data.m_flags & wxINTERACTIVE_RESIZE_W )
    {
        data.m_rect.x += diff.x;
        data.m_rect.width -= diff.x;
    }
    else if ( data.m_flags & wxINTERACTIVE_RESIZE_E )
    {
        data.m_rect.width += diff.x;
    }
    if ( data.m_flags & wxINTERACTIVE_RESIZE_N )
    {
        data.m_rect.y += diff.y;
        data.m_rect.height -= diff.y;
    }
    else if ( data.m_flags & wxINTERACTIVE_RESIZE_S )
    {
        data.m_rect.height += diff.y;
    }

    if ( data.m_minSize.x != -1 && data.m_rect.width < data.m_minSize.x )
    {
        if ( data.m_flags & wxINTERACTIVE_RESIZE_W )
            data.m_rect.x -= data.m_minSize.x - data.m_rect.width;
        data.m_rect.width = data.m_minSize.x;
    }
    if ( data.m_maxSize.x != -1 && data.m_rect.width > data.m_maxSize.x )
    {
        if ( data.m_flags & wxINTERACTIVE_RESIZE_W )
            data.m_rect.x -= data.m_minSize.x - data.m_rect.width;
        data.m_rect.width = data.m_maxSize.x;
    }
    if ( data.m_minSize.y != -1 && data.m_rect.height < data.m_minSize.y )
    {
        if ( data.m_flags & wxINTERACTIVE_RESIZE_N )
            data.m_rect.y -= data.m_minSize.y - data.m_rect.height;
        data.m_rect.height = data.m_minSize.y;
    }
    if ( data.m_maxSize.y != -1 && data.m_rect.height > data.m_maxSize.y )
    {
        if ( data.m_flags & wxINTERACTIVE_RESIZE_N )
            data.m_rect.y -= data.m_minSize.y - data.m_rect.height;
        data.m_rect.height = data.m_maxSize.y;
    }
}

void wxInteractiveMoveHandler::OnMouseMove(wxMouseEvent& event)
{
    if ( m_data.m_flags & wxINTERACTIVE_WAIT_FOR_INPUT )
        event.Skip();

    else if ( m_data.m_flags & wxINTERACTIVE_MOVE )
    {
        wxPoint diff = wxGetMousePosition() - m_data.m_pos;
        m_data.m_rect = m_data.m_rectOrig;
        m_data.m_rect.Offset(diff);
        m_data.m_window->Move(m_data.m_rect.GetPosition());
    }

    else if ( m_data.m_flags & wxINTERACTIVE_RESIZE )
    {
        wxPoint diff = wxGetMousePosition() - m_data.m_pos;
        m_data.m_rect = m_data.m_rectOrig;
        wxApplyResize(m_data, diff);
        m_data.m_window->SetSize(m_data.m_rect);
    }
}

void wxInteractiveMoveHandler::OnMouseDown(wxMouseEvent& event)
{
    if ( m_data.m_flags & wxINTERACTIVE_WAIT_FOR_INPUT )
    {
        m_data.m_flags &= ~wxINTERACTIVE_WAIT_FOR_INPUT;
        m_data.m_pos = wxGetMousePosition();
    }
}

void wxInteractiveMoveHandler::OnKeyDown(wxKeyEvent& event)
{
    if ( m_data.m_flags & wxINTERACTIVE_WAIT_FOR_INPUT )
    {
        m_data.m_flags &= ~wxINTERACTIVE_WAIT_FOR_INPUT;
        m_data.m_pos = wxGetMousePosition();
    }

    wxPoint diff(-1,-1);

    switch ( event.GetKeyCode() )
    {
        case WXK_UP:    diff = wxPoint(0, -16); break;
        case WXK_DOWN:  diff = wxPoint(0, 16);  break;
        case WXK_LEFT:  diff = wxPoint(-16, 0); break;
        case WXK_RIGHT: diff = wxPoint(16, 0);  break;
        case WXK_ESCAPE:
            m_data.m_window->SetSize(m_data.m_rectOrig);
            m_data.m_evtLoop->Exit();
            return;
        case WXK_RETURN:
            m_data.m_evtLoop->Exit();
            return;
    }

    if ( diff.x != -1 )
    {
        if ( m_data.m_flags & wxINTERACTIVE_MOVE )
        {
            m_data.m_rect.Offset(diff);
            m_data.m_window->Move(m_data.m_rect.GetPosition());
        }
        else /* wxINTERACTIVE_RESIZE */
        {
            if ( !(m_data.m_flags & wxINTERACTIVE_RESIZE_DIR) )
            {
                if ( diff.y < 0 )
                    m_data.m_flags |= wxINTERACTIVE_RESIZE_N;
                else if ( diff.y > 0 )
                    m_data.m_flags |= wxINTERACTIVE_RESIZE_S;
                if ( diff.x < 0 )
                    m_data.m_flags |= wxINTERACTIVE_RESIZE_W;
                else if ( diff.x > 0 )
                    m_data.m_flags |= wxINTERACTIVE_RESIZE_E;
            }

            wxApplyResize(m_data, diff);
            m_data.m_window->SetSize(m_data.m_rect);
        }
    }
}

void wxInteractiveMoveHandler::OnMouseUp(wxMouseEvent& event)
{
    m_data.m_evtLoop->Exit();
}


void wxTopLevelWindowBase::InteractiveMove(int flags)
{
    wxASSERT_MSG( !((flags & wxINTERACTIVE_MOVE) && (flags & wxINTERACTIVE_RESIZE)),
                  wxT("can't move and resize window at the same time") );

    wxASSERT_MSG( !(flags & wxINTERACTIVE_RESIZE) ||
                  (flags & wxINTERACTIVE_WAIT_FOR_INPUT) ||
                  (flags & wxINTERACTIVE_RESIZE_DIR),
                  wxT("direction of resizing not specified") );

    wxInteractiveMoveData data;
    wxEventLoop loop;
    wxWindow *focus = FindFocus();

    // FIXME - display resize cursor if waiting for initial input

    data.m_window = this;
    data.m_evtLoop = &loop;
    data.m_flags = flags;
    data.m_rect = data.m_rectOrig = GetRect();
    data.m_pos = wxGetMousePosition();
    data.m_minSize = wxSize(GetMinWidth(), GetMinHeight());
    data.m_maxSize = wxSize(GetMaxWidth(), GetMaxHeight());

    this->PushEventHandler(new wxInteractiveMoveHandler(data));
    if ( focus )
        focus->PushEventHandler(new wxInteractiveMoveHandler(data));

    CaptureMouse();
    loop.Run();
    ReleaseMouse();

    this->PopEventHandler(TRUE/*delete*/);
    if ( focus )
        focus->PopEventHandler(TRUE/*delete*/);
}

#endif // __WXUNIVERSAL__
