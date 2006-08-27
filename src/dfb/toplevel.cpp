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
    #include "wx/dynarray.h"
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

struct wxDfbPaintRequest
{
    wxDfbPaintRequest(const wxRect& rect, bool eraseBackground)
        : m_rect(rect), m_eraseBackground(eraseBackground) {}
    wxDfbPaintRequest(const wxDfbPaintRequest& r)
        : m_rect(r.m_rect), m_eraseBackground(r.m_eraseBackground) {}

    wxRect m_rect;
    bool   m_eraseBackground;
};

WX_DEFINE_ARRAY_PTR(wxDfbPaintRequest*, wxDfbQueuedPaintRequests);

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
    IDirectFBDisplayLayerPtr layer = wxDfbGetDisplayLayer();
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
    if ( !DFB_CALL( layer->CreateWindow(layer, &desc, &m_dfbwin) ) )
        return false;

    // add the new TLW to DFBWindowID->wxTLW map:
    DFBWindowID winid;
    if ( !DFB_CALL( m_dfbwin->GetID(m_dfbwin, &winid) ) )
        return false;
    gs_dfbWindowsMap[winid] = this;

    // TLWs are created initially hidden:
    if ( !DFB_CALL( m_dfbwin->SetOpacity(m_dfbwin, 0) ) )
        return false;

    wxWindow::Create(NULL, id, pos, size, style, name);

    SetParent(parent);
    if ( parent )
        parent->AddChild(this);

    wxTopLevelWindows.Append(this);
    m_title = title;

    if ( style & (wxSTAY_ON_TOP | wxPOPUP_WINDOW) )
    {
        DFB_CALL( m_dfbwin->SetStackingClass(m_dfbwin, DWSC_UPPER) );
    }

    // direct events in this window to the global event buffer:
    DFB_CALL( m_dfbwin->AttachEventBuffer(
                            m_dfbwin, wxEventLoop::GetDirectFBEventBuffer()) );

    return true;
}

wxTopLevelWindowDFB::~wxTopLevelWindowDFB()
{
    m_isBeingDeleted = true;

    wxTopLevelWindows.DeleteObject(this);

    if ( wxTheApp->GetTopWindow() == this )
        wxTheApp->SetTopWindow(NULL);

    if ( wxTopLevelWindows.empty() && wxTheApp->GetExitOnFrameDelete() )
    {
        wxTheApp->ExitMainLoop();
    }

    WX_CLEAR_ARRAY(*m_toPaint);
    wxDELETE(m_toPaint);

    // remove the TLW from DFBWindowID->wxTLW map:
    DFBWindowID winid;
    if ( DFB_CALL( m_dfbwin->GetID(m_dfbwin, &winid) ) )
        gs_dfbWindowsMap.erase(winid);
}

// ----------------------------------------------------------------------------
// window size & position
// ----------------------------------------------------------------------------

void wxTopLevelWindowDFB::DoGetPosition(int *x, int *y) const
{
    DFB_CALL( m_dfbwin->GetPosition(m_dfbwin, x, y) );
}

void wxTopLevelWindowDFB::DoGetSize(int *width, int *height) const
{
    DFB_CALL( m_dfbwin->GetSize(m_dfbwin, width, height) );
}

void wxTopLevelWindowDFB::DoMoveWindow(int x, int y, int width, int height)
{
    wxPoint curpos = GetPosition();
    if ( curpos.x != x || curpos.y != y )
    {
        DFB_CALL( m_dfbwin->MoveTo(m_dfbwin, x, y) );
    }

    wxSize cursize = GetSize();
    if ( cursize.x != width || cursize.y != height )
    {
        DFB_CALL( m_dfbwin->Resize(m_dfbwin, width, height) );
        // we must repaint the window after it changed size:
        Refresh();
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
    DFB_CALL( m_dfbwin->SetOpacity(m_dfbwin, show ? m_opacity : 0) );

    // If this is the first time Show was called, send size event,
    // so that the frame can adjust itself (think auto layout or single child)
    if ( !m_sizeSet )
    {
        m_sizeSet = true;
        wxSizeEvent event(GetSize(), GetId());
        event.SetEventObject(this);
        GetEventHandler()->ProcessEvent(event);
    }

    // FIXME_DFB: do this at all?
    if ( show && AcceptsFocus() )
        SetFocus();
        // FIXME_DFB -- don't do this for popup windows?

    return true;
}

bool wxTopLevelWindowDFB::SetTransparent(wxByte alpha)
{
    if ( IsShown() )
    {
        if ( !DFB_CALL( m_dfbwin->SetOpacity(m_dfbwin, alpha) ) )
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

IDirectFBSurfacePtr wxTopLevelWindowDFB::ObtainDfbSurface() const
{
    IDirectFBSurfacePtr surface;
    DFB_CALL( m_dfbwin->GetSurface(m_dfbwin, &surface) );
    return surface;
}

void wxTopLevelWindowDFB::HandleQueuedPaintRequests()
{
    wxDfbQueuedPaintRequests& toPaint = *m_toPaint;
    if ( toPaint.empty() )
        return; // nothing to do

    // process queued paint requests:
    wxRect winRect(wxPoint(0, 0), GetSize());
    wxRect paintedRect;

    size_t cnt = toPaint.size();
    for ( size_t i = 0; i < cnt; ++i )
    {
        const wxDfbPaintRequest& request = *toPaint[i];

        wxRect clipped(request.m_rect);

        wxLogTrace(TRACE_PAINT,
                   _T("%p ('%s'): processing paint request [x=%i,y=%i,w=%i,h=%i]"),
                   this, GetName().c_str(),
                   clipped.x, clipped.y, clipped.width, clipped.height);

        clipped.Intersect(winRect);
        if ( clipped.IsEmpty() )
            continue; // nothing to refresh

        PaintWindow(clipped, request.m_eraseBackground);

        // remember rectangle covering all repainted areas:
        if ( paintedRect.IsEmpty() )
            paintedRect = clipped;
        else
            paintedRect.Union(clipped);
    }

    WX_CLEAR_ARRAY(toPaint);

    if ( paintedRect.IsEmpty() )
        return; // no painting occurred, no need to flip

    // flip the surface to make the changes visible:
    DFBRegion r = {paintedRect.GetLeft(), paintedRect.GetTop(),
                   paintedRect.GetRight(), paintedRect.GetBottom()};
    DFBRegion *rptr = (winRect == paintedRect) ? NULL : &r;

    IDirectFBSurfacePtr surface(GetDfbSurface());
    DFB_CALL( surface->Flip(surface, rptr, DSFLIP_NONE) );
}

void wxTopLevelWindowDFB::DoRefreshRect(const wxRect& rect, bool eraseBack)
{
    // defer paiting until idle time or until Update() is called:
    m_toPaint->push_back(new wxDfbPaintRequest(rect, eraseBack));
}

void wxTopLevelWindowDFB::Update()
{
    HandleQueuedPaintRequests();
}

// ---------------------------------------------------------------------------
// events handling
// ---------------------------------------------------------------------------

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
