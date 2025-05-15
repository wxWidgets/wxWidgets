///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/dpycmn.cpp
// Purpose:     wxDisplay and wxDisplayImplSingle implementation
// Author:      Vadim Zeitlin
// Created:     01.03.03
// Copyright:   (c) 2003-2006 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/gdicmn.h"
    #include "wx/window.h"
    #include "wx/module.h"
#endif //WX_PRECOMP

#include "wx/math.h"

#include "wx/private/display.h"

#if wxUSE_DISPLAY

const wxVideoMode wxDefaultVideoMode;

#endif // wxUSE_DISPLAY

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

// the factory object used by wxDisplay
//
// created on demand and destroyed by wxDisplayModule
static wxDisplayFactory *gs_factory = nullptr;

// ----------------------------------------------------------------------------
// wxDisplayModule is used to cleanup gs_factory
// ----------------------------------------------------------------------------

class wxDisplayModule : public wxModule
{
public:
    virtual bool OnInit() override { return true; }
    virtual void OnExit() override
    {
        wxDELETE(gs_factory);
    }

    wxDECLARE_DYNAMIC_CLASS(wxDisplayModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxDisplayModule, wxModule);

// ============================================================================
// wxDisplay implementation
// ============================================================================

// ----------------------------------------------------------------------------
// ctor/dtor
// ----------------------------------------------------------------------------

wxDisplay::wxDisplay()
{
    m_impl = Factory().GetPrimaryDisplay();
}

wxDisplay::wxDisplay(unsigned n)
{
    wxASSERT_MSG( n == 0 || n < GetCount(),
                    wxT("An invalid index was passed to wxDisplay") );

    m_impl = Factory().GetDisplay(n);
}

wxDisplay::wxDisplay(const wxWindow* window)
{
    const int n = GetFromWindow(window);

    m_impl = n != wxNOT_FOUND ? Factory().GetDisplay(n)
                              : Factory().GetPrimaryDisplay();
}

// Note that this dtor must be implemented here, where wxDisplayImpl is fully
// declared, and not inline.
wxDisplay::~wxDisplay() = default;

// ----------------------------------------------------------------------------
// static functions forwarded to wxDisplayFactory
// ----------------------------------------------------------------------------

/* static */ unsigned wxDisplay::GetCount()
{
    return Factory().GetCount();
}

/* static */ int wxDisplay::GetFromPoint(const wxPoint& pt)
{
    return Factory().GetFromPoint(pt);
}

/* static */ int wxDisplay::GetFromRect(const wxRect& rect)
{
    return Factory().GetFromRect(rect);
}

/* static */ int wxDisplay::GetFromWindow(const wxWindow *window)
{
    wxCHECK_MSG( window, wxNOT_FOUND, wxT("invalid window") );

    return Factory().GetFromWindow(window);
}

/* static */ void wxDisplay::InvalidateCache()
{
    Factory().UpdateOnDisplayChange();
}

// ----------------------------------------------------------------------------
// functions forwarded to wxDisplayImpl
// ----------------------------------------------------------------------------

bool wxDisplay::IsConnected() const
{
    wxCHECK_MSG( IsOk(), false, wxT("invalid wxDisplay object") );

    return m_impl->IsConnected();
}

wxRect wxDisplay::GetGeometry() const
{
    wxCHECK_MSG( IsOk(), wxRect(), wxT("invalid wxDisplay object") );

    if ( !m_impl->IsConnected() )
        return wxRect();

    return m_impl->GetGeometry();
}

wxRect wxDisplay::GetClientArea() const
{
    wxCHECK_MSG( IsOk(), wxRect(), wxT("invalid wxDisplay object") );

    if ( !m_impl->IsConnected() )
        return wxRect();

    return m_impl->GetClientArea();
}

wxSize wxDisplay::GetPPI() const
{
    wxCHECK_MSG( IsOk(), wxSize(), wxT("invalid wxDisplay object") );

    if ( !m_impl->IsConnected() )
        return wxSize();

    return m_impl->GetPPI();
}

double wxDisplay::GetScaleFactor() const
{
    wxCHECK_MSG( IsOk(), 0, wxT("invalid wxDisplay object") );

    if ( !m_impl->IsConnected() )
        return 0.0;

    return m_impl->GetScaleFactor();
}

int wxDisplay::GetDepth() const
{
    wxCHECK_MSG( IsOk(), 0, wxT("invalid wxDisplay object") );

    if ( !m_impl->IsConnected() )
        return 0;

    return m_impl->GetDepth();
}

wxString wxDisplay::GetName() const
{
    wxCHECK_MSG( IsOk(), wxString(), wxT("invalid wxDisplay object") );

    if ( !m_impl->IsConnected() )
        return wxString();

    return m_impl->GetName();
}

bool wxDisplay::IsPrimary() const
{
    return m_impl && m_impl->IsConnected() && m_impl->IsPrimary();
}

#if wxUSE_DISPLAY

wxArrayVideoModes wxDisplay::GetModes(const wxVideoMode& mode) const
{
    wxCHECK_MSG( IsOk(), wxArrayVideoModes(), wxT("invalid wxDisplay object") );

    if ( !m_impl->IsConnected() )
        return wxArrayVideoModes();

    return m_impl->GetModes(mode);
}

wxVideoMode wxDisplay::GetCurrentMode() const
{
    wxCHECK_MSG( IsOk(), wxVideoMode(), wxT("invalid wxDisplay object") );

    if ( !m_impl->IsConnected() )
        return wxVideoMode();

    return m_impl->GetCurrentMode();
}

bool wxDisplay::ChangeMode(const wxVideoMode& mode)
{
    wxCHECK_MSG( IsOk(), false, wxT("invalid wxDisplay object") );

    if ( !m_impl->IsConnected() )
        return false;

    return m_impl->ChangeMode(mode);
}

#endif // wxUSE_DISPLAY

// ----------------------------------------------------------------------------
// static functions implementation
// ----------------------------------------------------------------------------

/* static */ wxDisplayFactory& wxDisplay::Factory()
{
    if ( !gs_factory )
    {
        gs_factory = CreateFactory();
    }

    return *gs_factory;
}

// ============================================================================
// wxDisplayFactory implementation
// ============================================================================

bool wxDisplayFactory::RefreshOnDisplayChange(wxDisplayImpl& impl) const
{
    // We have no means to detect if the display is still connected here, so
    // assume it isn't any more: this is safer than assuming that it still is.
    //
    // Of course, ideally this should be really implemented in port-specific
    // code.
    impl.Disconnect();

    return false;
}

void wxDisplayFactory::ClearImpls()
{
    m_impls.clear();
}

wxObjectDataPtr<wxDisplayImpl> wxDisplayFactory::GetDisplay(unsigned n)
{
    // Normally, m_impls should be cleared if the number of displays in the
    // system changes because InvalidateCache() must be called. However in
    // some ports, cache invalidation never happens, so we can end up with
    // m_impls size being out of sync with the actual number of monitors.
    // Compensate for this here by checking if the index is invalid and
    // invalidating the cache at least in this case.
    //
    // Note that this is still incorrect because we continue using outdated
    // information if the first monitor is disconnected, for example. The
    // only real solution is to ensure that InvalidateCache() is called,
    // but for now this at least avoids crashes when a new display is
    // connected.
    if ( n >= m_impls.size() )
    {
        // This strange two-step resize is done to clear all the existing
        // elements: they may not be valid any longer if the number of
        // displays has changed.
        m_impls.resize(0);
        m_impls.resize(GetCount());
    }

    // Return the existing display if we have it or create a new one.
    auto& impl = m_impls[n];

    if ( !impl )
        impl = CreateDisplay(n);

    return impl;
}

wxObjectDataPtr<wxDisplayImpl> wxDisplayFactory::GetPrimaryDisplay()
{
    // Just use dumb linear search -- there seems to be the most reliable way
    // to do this in general. In particular, primary monitor is not guaranteed
    // to be the first one and it's not obvious if it always contains (0, 0).
    const unsigned count = GetCount();
    for ( unsigned n = 0; n < count; ++n )
    {
        auto const d = GetDisplay(n);
        if ( d && d->IsPrimary() )
            return d;
    }

    // This is not supposed to happen, but what else can we do if it
    // somehow does?
    return wxObjectDataPtr<wxDisplayImpl>();
}

int wxDisplayFactory::GetFromRect(const wxRect& r)
{
    int display = wxNOT_FOUND;

    // Find the display with the biggest intersection with the given window.
    //
    // Note that just using GetFromPoint() with the center of the rectangle is
    // not correct in general, as the center might lie outside of the visible
    // area, while the rectangle itself could be partially visible. Moreover,
    // in some exotic (L-shaped) display layouts, the center might not actually
    // be on the display containing the biggest part of the rectangle even if
    // it is visible.
    int biggestOverlapArea = 0;
    const unsigned count = GetCount();
    for ( unsigned n = 0; n < count; ++n )
    {
        const auto overlap = GetDisplay(n)->GetGeometry().Intersect(r);
        const int overlapArea = overlap.width * overlap.height;
        if ( overlapArea > biggestOverlapArea )
        {
            biggestOverlapArea = overlapArea;
            display = n;
        }
    }

    return display;
}

int wxDisplayFactory::GetFromWindow(const wxWindow *window)
{
    wxCHECK_MSG( window, wxNOT_FOUND, "window can't be null" );

    // Check if the window is created: we can't find its display before this is
    // done anyhow, as we simply don't know on which display will it appear,
    // and trying to do this below would just result in assert failures inside
    // GetScreenRect() if the window doesn't yet exist, so return immediately.
    if ( !window->GetHandle() )
        return wxNOT_FOUND;

    return GetFromRect(window->GetScreenRect());
}

void wxDisplayFactory::UpdateOnDisplayChange()
{
    // Prepare the new vector by copying the still valid displays into it.
    //
    // Preserve the invariant that m_impls size is always equal to the number
    // of displays.
    wxVector<wxObjectDataPtr<wxDisplayImpl>> impls(GetCount());

    for ( auto& impl : m_impls )
    {
        // Object may be empty if not accessed yet.
        // Try to update display state or mark it as disconnected.
        if ( impl && RefreshOnDisplayChange(*impl) )
        {
            // If display is still connected put it at the new index position:
            // it must have been updated by RefreshOnDisplayChange().
            impls[impl->GetIndex()] = impl;
        }
    }

    m_impls = std::move(impls);
}

// ============================================================================
// wxDisplayFactorySingle implementation
// ============================================================================

wxDisplayImpl *wxDisplayFactorySingle::CreateDisplay(unsigned n)
{
    // we recognize the main display only
    return n != 0 ? nullptr : CreateSingleDisplay();
}

int wxDisplayFactorySingle::GetFromPoint(const wxPoint& pt)
{
    return wxDisplay().GetGeometry().Contains(pt) ? 0 : wxNOT_FOUND;
}
