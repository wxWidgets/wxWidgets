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
    Factory().InvalidateCache();
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

    if ( m_impl->IsConnected() )
        return m_impl->GetGeometry();
    return wxRect();
}

wxRect wxDisplay::GetClientArea() const
{
    wxCHECK_MSG( IsOk(), wxRect(), wxT("invalid wxDisplay object") );

    if ( m_impl->IsConnected() )
        return m_impl->GetClientArea();
    return wxRect();
}

wxSize wxDisplay::GetPPI() const
{
    wxCHECK_MSG( IsOk(), wxSize(), wxT("invalid wxDisplay object") );

    if ( m_impl->IsConnected() )
        return m_impl->GetPPI();
    return wxSize();
}

double wxDisplay::GetScaleFactor() const
{
    wxCHECK_MSG( IsOk(), 0, wxT("invalid wxDisplay object") );

    if ( m_impl->IsConnected() )
        return m_impl->GetScaleFactor();
    return 0.0;
}

int wxDisplay::GetDepth() const
{
    wxCHECK_MSG( IsOk(), 0, wxT("invalid wxDisplay object") );

    if ( m_impl->IsConnected() )
        return m_impl->GetDepth();
    return 0;
}

wxString wxDisplay::GetName() const
{
    wxCHECK_MSG( IsOk(), wxString(), wxT("invalid wxDisplay object") );

    if ( m_impl->IsConnected() )
        return m_impl->GetName();
    return wxString();
}

bool wxDisplay::IsPrimary() const
{
    return m_impl && m_impl->IsConnected() && m_impl->IsPrimary();
}

#if wxUSE_DISPLAY

wxArrayVideoModes wxDisplay::GetModes(const wxVideoMode& mode) const
{
    wxCHECK_MSG( IsOk(), wxArrayVideoModes(), wxT("invalid wxDisplay object") );

    if ( m_impl->IsConnected() )
        return m_impl->GetModes(mode);
    return wxArrayVideoModes();
}

wxVideoMode wxDisplay::GetCurrentMode() const
{
    wxCHECK_MSG( IsOk(), wxVideoMode(), wxT("invalid wxDisplay object") );

    if ( m_impl->IsConnected() )
        return m_impl->GetCurrentMode();
    return wxVideoMode();
}

bool wxDisplay::ChangeMode(const wxVideoMode& mode)
{
    wxCHECK_MSG( IsOk(), false, wxT("invalid wxDisplay object") );

    if ( m_impl->IsConnected() )
        return m_impl->ChangeMode(mode);
    return false;
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

bool wxDisplayFactory::UpdateDisplayChange(wxDisplayImpl *impl) const
{
    // If the factory is not able to detect connection state of the
    // input display we assume that it is disconnected now.
    impl->Disconnect();
    return false;
}

void wxDisplayFactory::ClearImpls()
{
    m_impls.clear();
}

void wxDisplayFactory::UpdateDisplayChanges()
{
    const unsigned newCount = GetCount();
    wxVector<wxObjectDataPtr<wxDisplayImpl> > impls((size_t)newCount);

    for ( size_t n = 0; n < m_impls.size(); ++n )
    {
        wxObjectDataPtr<wxDisplayImpl> &impl = m_impls[n];

        // There are wxDisplay objects referring to impl still in scope.
        if ( impl && impl->GetRefCount() > 1 )
        {
            // Try to update display state or mark it as disconnected.
            if ( UpdateDisplayChange(impl.get()) && impl->GetIndex() < newCount )
                impls[impl->GetIndex()] = impl;
        }
    }
    m_impls = impls;
}

wxObjectDataPtr<wxDisplayImpl> wxDisplayFactory::GetPrimaryDisplay()
{
    // Just use dumb linear search -- there seems to be the most reliable way
    // to do this in general. In particular, primary monitor is not guaranteed
    // to be the first one and it's not obvious if it always contains (0, 0).
    const unsigned count = GetCount();
    for ( unsigned n = 0; n < count; ++n )
    {
        wxObjectDataPtr<wxDisplayImpl> const d = GetDisplay(n);
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
