/////////////////////////////////////////////////////////////////////////////
// Name:        src/dfb/toplevel.cpp
// Purpose:     Top level window, abstraction of wxFrame and wxDialog
// Author:      Vaclav Slavik
// Created:     2006-08-10
// RCS-ID:      $Id$
// Copyright:   (c) 2006 REA Elektronik GmbH
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/toplevel.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/hashmap.h"
#include "wx/evtloop.h"
#include "wx/dfb/private.h"

#define TRACE_EVENTS _T("events")
#define TRACE_PAINT  _T("paint")

// ============================================================================
// globals
// ============================================================================

// mapping of DirectFB windows to wxTLWs:
WX_DECLARE_HASH_MAP(DFBWindowID, wxTopLevelWindowDFB*,
                    wxIntegerHash, wxIntegerEqual,
                    wxDfbWindowsMap);
static wxDfbWindowsMap gs_dfbWindowsMap;

// ============================================================================
// helpers
// ============================================================================

// Queue of paint requests
class wxDfbQueuedPaintRequests
{
public:
    ~wxDfbQueuedPaintRequests() { Clear(); }

    // Adds paint request to the queue
    void Add(const wxRect& rect)
    {
        // We use a simple implementation here for now: all refresh requests
        // are merged together into single rectangle that is superset of
        // all the requested rectangles. This wastes some blitting and painting
        // time, but OTOH, EVT_PAINT handler is called only once per window.
        m_invalidated.Union(rect);
    }

    // Is the queue empty?
    bool IsEmpty() const { return m_invalidated.IsEmpty(); }

    // Empties the queue
    void Clear() { m_invalidated = wxRect(); }

    // Gets the next request in the queue, returns true if there was one,
    // false if the queue was empty
    bool GetNext(wxRect& rect)
    {
        if ( m_invalidated.IsEmpty() )
            return false;

        rect = m_invalidated;
        Clear(); // there's only one item in the queue
        return true;
    }

private:
    // currently invalidated region
    wxRect m_invalidated;
};

// ============================================================================
// wxTopLevelWindowDFB
// ============================================================================

// ----------------------------------------------------------------------------
// creation & destruction
// ----------------------------------------------------------------------------

void wxTopLevelWindowDFB::Init()
{
    m_isShown = false;
    m_isMaximized = false;
    m_fsIsShowing = false;
    m_sizeSet = false;
    m_opacity = 255;
    m_toPaint = new wxDfbQueuedPaintRequests;
    m_isPainting = false;
}

bool wxTopLevelWindowDFB::Create(wxWindow *parent,
                                 wxWindowID id,
                                 const wxString& title,
                                 const wxPoint& posOrig,
                                 const wxSize& sizeOrig,
                                 long style,
                                 const wxString &name)
{
    m_tlw = this;

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

    wxPoint pos(posOrig);
    if ( pos.x == wxDefaultCoord )
        pos.x = 0;
    if ( pos.y == wxDefaultCoord )
        pos.y = 0;

    // create DirectFB window:
    wxIDirectFBDisplayLayerPtr layer(wxIDirectFB::Get()->GetDisplayLayer());
    wxCHECK_MSG( layer, false, _T("no display layer") );

    DFBWindowDescription desc;
    desc.flags = (DFBWindowDescriptionFlags)
                   (DWDESC_CAPS |
                    DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_POSX | DWDESC_POSY);
    desc.caps = DWCAPS_DOUBLEBUFFER;
    desc.posx = pos.x;
    desc.posy = pos.y;
    desc.width = size.x;
    desc.height = size.y;
    m_dfbwin = layer->CreateWindow(&desc);
    if ( !m_dfbwin )
        return false;

    // add the new TLW to DFBWindowID->wxTLW map:
    DFBWindowID winid;
    if ( !m_dfbwin->GetID(&winid) )
        return false;
    gs_dfbWindowsMap[winid] = this;

    // TLWs are created initially hidden:
    if ( !m_dfbwin->SetOpacity(wxALPHA_TRANSPARENT) )
        return false;

    if ( !wxWindow::Create(NULL, id, pos, size, style, name) )
        return false;

    SetParent(parent);
    if ( parent )
        parent->AddChild(this);

    wxTopLevelWindows.Append(this);
    m_title = title;

    if ( style & (wxSTAY_ON_TOP | wxPOPUP_WINDOW) )
    {
        m_dfbwin->SetStackingClass(DWSC_UPPER);
    }

    // direct events in this window to the global event buffer:
    m_dfbwin->AttachEventBuffer(wxEventLoop::GetDirectFBEventBuffer());

    return true;
}

wxTopLevelWindowDFB::~wxTopLevelWindowDFB()
{
    m_isBeingDeleted = true;

    // destroy all children before we destroy the underlying DirectFB window,
    // so that if any of them does something with the TLW, it will still work:
    DestroyChildren();

    // it's safe to delete the underlying DirectFB window now:
    wxDELETE(m_toPaint);

    if ( !m_dfbwin )
        return;

    // remove the TLW from DFBWindowID->wxTLW map:
    DFBWindowID winid;
    if ( m_dfbwin->GetID(&winid) )
        gs_dfbWindowsMap.erase(winid);

    m_dfbwin->Destroy();
    m_dfbwin.Reset();
}

// ----------------------------------------------------------------------------
// window size & position
// ----------------------------------------------------------------------------

void wxTopLevelWindowDFB::DoGetPosition(int *x, int *y) const
{
    m_dfbwin->GetPosition(x, y);
}

void wxTopLevelWindowDFB::DoGetSize(int *width, int *height) const
{
    m_dfbwin->GetSize(width, height);
}

void wxTopLevelWindowDFB::DoMoveWindow(int x, int y, int width, int height)
{
    wxPoint curpos = GetPosition();
    if ( curpos.x != x || curpos.y != y )
    {
        m_dfbwin->MoveTo(x, y);
    }

    wxSize cursize = GetSize();
    if ( cursize.x != width || cursize.y != height )
    {
        // changing window's size changes its surface:
        InvalidateDfbSurface();

        m_dfbwin->Resize(width, height);

        // we must repaint the window after it changed size:
        if ( IsShown() )
            DoRefreshWindow();
    }
}

// ----------------------------------------------------------------------------
// showing and hiding
// ----------------------------------------------------------------------------

#warning "FIXME: the rest of this file is almost same as for MGL, merge it"
bool wxTopLevelWindowDFB::ShowFullScreen(bool show, long style)
{
    if (show == m_fsIsShowing) return false; // return what?

    m_fsIsShowing = show;

    if (show)
    {
        m_fsSaveStyle = m_windowStyle;
        m_fsSaveFlag = style;
        GetPosition(&m_fsSaveFrame.x, &m_fsSaveFrame.y);
        GetSize(&m_fsSaveFrame.width, &m_fsSaveFrame.height);

        if ( style & wxFULLSCREEN_NOCAPTION )
            m_windowStyle &= ~wxCAPTION;
        if ( style & wxFULLSCREEN_NOBORDER )
            m_windowStyle = wxSIMPLE_BORDER;

        int x, y;
        wxDisplaySize(&x, &y);
        SetSize(0, 0, x, y);
    }
    else
    {
        m_windowStyle = m_fsSaveStyle;
        SetSize(m_fsSaveFrame.x, m_fsSaveFrame.y,
                m_fsSaveFrame.width, m_fsSaveFrame.height);
    }

    return true;
}

bool wxTopLevelWindowDFB::Show(bool show)
{
    if ( !wxTopLevelWindowBase::Show(show) )
        return false;

    // hide/show the window by setting its opacity to 0/full:
    m_dfbwin->SetOpacity(show ? m_opacity : 0);

    // If this is the first time Show was called, send size event,
    // so that the frame can adjust itself (think auto layout or single child)
    if ( !m_sizeSet )
    {
        m_sizeSet = true;
        wxSizeEvent event(GetSize(), GetId());
        event.SetEventObject(this);
        GetEventHandler()->ProcessEvent(event);
    }

    if ( show )
    {
        wxWindow *focused = wxWindow::FindFocus();
        if ( focused && focused->GetTLW() == this )
        {
            SetDfbFocus();
        }
        else if ( AcceptsFocus() )
        {
            // FIXME: we should probably always call SetDfbFocus instead
            // and call SetFocus() from wxActivateEvent/DWET_GOTFOCUS
            // handler
            SetFocus();
        }
    }

    return true;
}

bool wxTopLevelWindowDFB::SetTransparent(wxByte alpha)
{
    if ( IsShown() )
    {
        if ( !m_dfbwin->SetOpacity(alpha) )
            return false;
    }

    m_opacity = alpha;
    return true;
}

// ----------------------------------------------------------------------------
// maximize, minimize etc.
// ----------------------------------------------------------------------------

void wxTopLevelWindowDFB::Maximize(bool maximize)
{
    int x, y, w, h;
    wxClientDisplayRect(&x, &y, &w, &h);

    if ( maximize && !m_isMaximized )
    {
        m_isMaximized = true;

        GetPosition(&m_savedFrame.x, &m_savedFrame.y);
        GetSize(&m_savedFrame.width, &m_savedFrame.height);

        SetSize(x, y, w, h);
    }
    else if ( !maximize && m_isMaximized )
    {
        m_isMaximized = false;
        SetSize(m_savedFrame.x, m_savedFrame.y,
                m_savedFrame.width, m_savedFrame.height);
    }
}

bool wxTopLevelWindowDFB::IsMaximized() const
{
    return m_isMaximized;
}

void wxTopLevelWindowDFB::Restore()
{
    if ( IsMaximized() )
    {
        Maximize(false);
    }
}

void wxTopLevelWindowDFB::Iconize(bool WXUNUSED(iconize))
{
    wxFAIL_MSG(wxT("Iconize not supported under wxDFB"));
}

bool wxTopLevelWindowDFB::IsIconized() const
{
    return false;
}


// ----------------------------------------------------------------------------
// surfaces and painting
// ----------------------------------------------------------------------------

wxIDirectFBSurfacePtr wxTopLevelWindowDFB::ObtainDfbSurface() const
{
    return m_dfbwin->GetSurface();
}

void wxTopLevelWindowDFB::HandleQueuedPaintRequests()
{
    if ( m_toPaint->IsEmpty() )
        return; // nothing to do

    if ( IsFrozen() || !IsShown() )
    {
        // nothing to do if the window is frozen or hidden; clear the queue
        // and return (note that it's OK to clear the queue even if the window
        // is frozen, because Thaw() calls Refresh()):
        m_toPaint->Clear();
        return;
    }

    // process queued paint requests:
    wxRect winRect(wxPoint(0, 0), GetSize());
    wxRect paintedRect;

    // important note: all DCs created from now until m_isPainting is reset to
    // false will not update the front buffer as this flag indicates that we'll
    // blit the entire back buffer to front soon
    m_isPainting = true;

#ifdef __WXDEBUG__
    int requestsCount = 0;
#endif

    wxRect request;
    while ( m_toPaint->GetNext(request) )
    {
#ifdef __WXDEBUG__
        requestsCount++;
#endif
        wxRect clipped(request);
        clipped.Intersect(winRect);
        if ( clipped.IsEmpty() )
            continue; // nothing to refresh

        wxLogTrace(TRACE_PAINT,
                   _T("%p ('%s'): processing paint request [%i,%i,%i,%i]"),
                   this, GetName().c_str(),
                   clipped.x, clipped.y, clipped.GetRight(), clipped.GetBottom());

        PaintWindow(clipped);

        // remember rectangle covering all repainted areas:
        if ( paintedRect.IsEmpty() )
            paintedRect = clipped;
        else
            paintedRect.Union(clipped);
    }

    m_isPainting = false;

    m_toPaint->Clear();

    if ( paintedRect.IsEmpty() )
        return; // no painting occurred, no need to flip

    // Flip the surface to make the changes visible. Note that the rectangle we
    // flip is *superset* of the union of repainted rectangles (created as
    // "rectangles union" by wxRect::Union) and so some parts of the back
    // buffer that we didn't touch in this HandleQueuedPaintRequests call will
    // be copied to the front buffer as well. This is safe/correct thing to do
    // *only* because wx always use wxIDirectFBSurface::FlipToFront() and so
    // the back and front buffers contain the same data.
    //
    // Note that we do _not_ split m_toPaint into disjoint rectangles and
    // do FlipToFront() for each of them, because that could result in visible
    // updating of the screen; instead, we prefer to flip everything at once.

    DFBRegion r = {paintedRect.GetLeft(), paintedRect.GetTop(),
                   paintedRect.GetRight(), paintedRect.GetBottom()};
    DFBRegion *rptr = (winRect == paintedRect) ? NULL : &r;

    GetDfbSurface()->FlipToFront(rptr);

    wxLogTrace(TRACE_PAINT,
               _T("%p ('%s'): processed %i paint requests, flipped surface: [%i,%i,%i,%i]"),
               this, GetName().c_str(),
               requestsCount,
               paintedRect.x, paintedRect.y,
               paintedRect.GetRight(), paintedRect.GetBottom());
}

void wxTopLevelWindowDFB::DoRefreshRect(const wxRect& rect)
{
    // don't overlap outside of the window (NB: 'rect' is in window coords):
    wxRect r(rect);
    r.Intersect(wxRect(GetSize()));
    if ( r.IsEmpty() )
        return;

    wxLogTrace(TRACE_PAINT,
               _T("%p ('%s'): [TLW] refresh rect [%i,%i,%i,%i]"),
               this, GetName().c_str(),
               rect.x, rect.y, rect.GetRight(), rect.GetBottom());

    // defer painting until idle time or until Update() is called:
    m_toPaint->Add(rect);
}

void wxTopLevelWindowDFB::Update()
{
    HandleQueuedPaintRequests();
}

// ---------------------------------------------------------------------------
// events handling
// ---------------------------------------------------------------------------

void wxTopLevelWindowDFB::SetDfbFocus()
{
    wxCHECK_RET( IsShown(), _T("cannot set focus to hidden window") );
    wxASSERT_MSG( FindFocus() && FindFocus()->GetTLW() == this,
                  _T("setting DirectFB focus to unexpected window") );

    GetDirectFBWindow()->RequestFocus();
}

/* static */
void wxTopLevelWindowDFB::HandleDFBWindowEvent(const wxDFBWindowEvent& event_)
{
    const DFBWindowEvent& event = event_;

    if ( gs_dfbWindowsMap.find(event.window_id) == gs_dfbWindowsMap.end() )
    {
        wxLogTrace(TRACE_EVENTS,
                   _T("received event for unknown DirectFB window, ignoring"));
        return;
    }

    wxTopLevelWindowDFB *tlw = gs_dfbWindowsMap[event.window_id];
    wxWindow *recipient = NULL;
    void (wxWindow::*handlerFunc)(const wxDFBWindowEvent&) = NULL;

    switch ( event.type )
    {
        case DWET_KEYDOWN:
        case DWET_KEYUP:
        {
            recipient = wxWindow::FindFocus();
            handlerFunc = &wxWindowDFB::HandleKeyEvent;
            break;
        }

        case DWET_NONE:
        case DWET_ALL:
        {
            wxFAIL_MSG( _T("invalid event type") );
            break;
        }

        default:
            // we're not interested in them here
            break;
    }

    if ( !recipient )
    {
        wxLogTrace(TRACE_EVENTS, _T("ignoring event: no recipient window"));
        return;
    }

    wxCHECK_RET( recipient && recipient->GetTLW() == tlw,
                 _T("event recipient not in TLW which received the event") );

    // process the event:
    (recipient->*handlerFunc)(event_);
}

// ---------------------------------------------------------------------------
// idle events processing
// ---------------------------------------------------------------------------

void wxTopLevelWindowDFB::OnInternalIdle()
{
    wxTopLevelWindowBase::OnInternalIdle();
    HandleQueuedPaintRequests();
}
