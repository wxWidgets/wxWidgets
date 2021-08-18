///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/dpycmn.cpp
// Purpose:     wxDisplay and wxDisplayImplSingle implementation
// Author:      Vadim Zeitlin
// Modified by:
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

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(wxArrayVideoModes)

const wxVideoMode wxDefaultVideoMode;

#endif // wxUSE_DISPLAY

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

// the factory object used by wxDisplay
//
// created on demand and destroyed by wxDisplayModule
static wxDisplayFactory *gs_factory = NULL;

// ----------------------------------------------------------------------------
// wxDisplayModule is used to cleanup gs_factory
// ----------------------------------------------------------------------------

class wxDisplayModule : public wxModule
{
public:
    virtual bool OnInit() wxOVERRIDE { return true; }
    virtual void OnExit() wxOVERRIDE
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

wxRect wxDisplay::GetGeometry() const
{
    wxCHECK_MSG( IsOk(), wxRect(), wxT("invalid wxDisplay object") );

    return m_impl->GetGeometry();
}

wxRect wxDisplay::GetClientArea() const
{
    wxCHECK_MSG( IsOk(), wxRect(), wxT("invalid wxDisplay object") );

    return m_impl->GetClientArea();
}

wxSize wxDisplay::GetPPI() const
{
    wxCHECK_MSG( IsOk(), wxSize(), wxT("invalid wxDisplay object") );

    return m_impl->GetPPI();
}

double wxDisplay::GetScaleFactor() const
{
    wxCHECK_MSG( IsOk(), 0, wxT("invalid wxDisplay object") );

    return m_impl->GetScaleFactor();
}

int wxDisplay::GetDepth() const
{
    wxCHECK_MSG( IsOk(), 0, wxT("invalid wxDisplay object") );

    return m_impl->GetDepth();
}

wxString wxDisplay::GetName() const
{
    wxCHECK_MSG( IsOk(), wxString(), wxT("invalid wxDisplay object") );

    return m_impl->GetName();
}

bool wxDisplay::IsPrimary() const
{
    return m_impl && m_impl->IsPrimary();
}

#if wxUSE_DISPLAY

wxArrayVideoModes wxDisplay::GetModes(const wxVideoMode& mode) const
{
    wxCHECK_MSG( IsOk(), wxArrayVideoModes(), wxT("invalid wxDisplay object") );

    return m_impl->GetModes(mode);
}

wxVideoMode wxDisplay::GetCurrentMode() const
{
    wxCHECK_MSG( IsOk(), wxVideoMode(), wxT("invalid wxDisplay object") );

    return m_impl->GetCurrentMode();
}

bool wxDisplay::ChangeMode(const wxVideoMode& mode)
{
    wxCHECK_MSG( IsOk(), false, wxT("invalid wxDisplay object") );

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

void wxDisplayFactory::ClearImpls()
{
    for ( size_t n = 0; n < m_impls.size(); ++n )
    {
        // It can be null, that's ok.
        delete m_impls[n];
    }

    m_impls.clear();
}

wxDisplayImpl* wxDisplayFactory::GetPrimaryDisplay()
{
    // Just use dumb linear search -- there seems to be the most reliable way
    // to do this in general. In particular, primary monitor is not guaranteed
    // to be the first one and it's not obvious if it always contains (0, 0).
    const unsigned count = GetCount();
    for ( unsigned n = 0; n < count; ++n )
    {
        wxDisplayImpl* const d = GetDisplay(n);
        if ( d && d->IsPrimary() )
            return d;
    }

    // This is not supposed to happen, but what else can we do if it
    // somehow does?
    return NULL;
}

int wxDisplayFactory::GetFromWindow(const wxWindow *window)
{
    wxCHECK_MSG( window, wxNOT_FOUND, "window can't be NULL" );

    // Check if the window is created: we can't find its display before this is
    // done anyhow, as we simply don't know on which display will it appear,
    // and trying to do this below would just result in assert failures inside
    // GetScreenRect() if the window doesn't yet exist, so return immediately.
    if ( !window->GetHandle() )
        return wxNOT_FOUND;

    // consider that the window belongs to the display containing its centre
    const wxRect r(window->GetScreenRect());
    return GetFromPoint(wxPoint(r.x + r.width/2, r.y + r.height/2));
}

// ============================================================================
// wxDisplayFactorySingle implementation
// ============================================================================

wxDisplayImpl *wxDisplayFactorySingle::CreateDisplay(unsigned n)
{
    // we recognize the main display only
    return n != 0 ? NULL : CreateSingleDisplay();
}

int wxDisplayFactorySingle::GetFromPoint(const wxPoint& pt)
{
    return wxDisplay().GetGeometry().Contains(pt) ? 0 : wxNOT_FOUND;
}
