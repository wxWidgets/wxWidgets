///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/curbndl.cpp
// Purpose:     Implementation of wxCursorBundle.
// Author:      Vadim Zeitlin
// Created:     2025-05-03
// Copyright:   (c) 2025 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
    #include "wx/bmpbndl.h"
    #include "wx/cursor.h"
    #include "wx/settings.h"
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "wx/private/rescale.h"

// ----------------------------------------------------------------------------
// wxCursorBundleImpl
// ----------------------------------------------------------------------------

// Base class for possible wxCursorBundle implementations.
class wxCursorBundleImpl : public wxRefCounter
{
public:
    wxCursorBundleImpl() = default;

    virtual wxCursor GetCursorFor(const wxWindow* window) const = 0;

private:
    // This class uses ref counting and is not copied normally.
    wxDECLARE_NO_COPY_CLASS(wxCursorBundleImpl);
};

namespace
{

// Implementation using a single cursor.
class wxCursorBundleSingleImpl : public wxCursorBundleImpl
{
public:
    explicit wxCursorBundleSingleImpl(const wxCursor& cursor)
        : m_cursor(cursor)
    {
    }

    virtual wxCursor GetCursorFor(const wxWindow* WXUNUSED(window)) const
    {
        return m_cursor;
    }

private:
    wxCursor m_cursor;
};

// Implementation using multiple bitmaps.
class wxCursorBundleMultiImpl : public wxCursorBundleImpl
{
public:
    wxCursorBundleMultiImpl(const wxBitmapBundle& bitmaps,
                            const wxPoint& hotSpot)
        : m_bitmaps(bitmaps),
          m_hotSpot(hotSpot)
    {
    }

    virtual wxCursor GetCursorFor(const wxWindow* window) const override
    {
        // Just calling GetBitmapFor(window) here would be wrong as cursor size
        // is not always just the standard size scaled by DPI, for example, the
        // user may increase the cursor size even in standard DPI, so get the
        // size that we need (or rather just one of its components as cursors
        // are always square anyhow).
        const int
            h = wxSystemSettings::GetMetric(wxSYS_CURSOR_Y, window);

        // Check if we can reuse the last returned cursor.
        if ( h == m_lastCursorSize )
            return m_lastCursor;

        const wxSize
            sizeDef = m_bitmaps.GetDefaultSize();

        const wxSize
            size = m_bitmaps.GetPreferredBitmapSizeAtScale(double(h) / sizeDef.y);

        const wxPoint
            hotSpot = wxRescaleCoord(m_hotSpot).From(sizeDef).To(size);

        m_lastCursor = wxCursor(m_bitmaps.GetBitmap(size), hotSpot);
        m_lastCursorSize = h;

        return m_lastCursor;
    }

private:
    const wxBitmapBundle m_bitmaps;

    const wxPoint m_hotSpot;

    // Cache the last returned cursor: at least in wxMSW we don't always keep
    // it alive and just use its handle, which would become invalid if it is
    // destroyed too soon.
    mutable wxCursor m_lastCursor;

    mutable int m_lastCursorSize = 0;
};

} // anonymous namespace

// ============================================================================
// implementation
// ============================================================================

wxCursorBundle::wxCursorBundle() = default;

wxCursorBundle::wxCursorBundle(const ImplPtr& impl)
    : m_impl(impl)
{
}

wxCursorBundle::wxCursorBundle(const wxBitmapBundle& bitmaps,
                               const wxPoint& hotSpot)
    : m_impl(new wxCursorBundleMultiImpl(bitmaps, hotSpot))
{
}

wxCursorBundle::wxCursorBundle(const wxCursor& cursor)
    : m_impl(new wxCursorBundleSingleImpl(cursor))
{
}

wxCursorBundle::wxCursorBundle(const wxCursorBundle&) = default;

wxCursorBundle& wxCursorBundle::operator=(const wxCursorBundle&) = default;

wxCursorBundle::~wxCursorBundle() = default;

void wxCursorBundle::Clear()
{
    m_impl.reset(nullptr);
}

wxCursor wxCursorBundle::GetCursorFor(const wxWindow* window) const
{
    // We can be asked for cursor even if we don't have any, just return an
    // invalid cursor in this case, but don't assert as this happens all the
    // time (e.g. when wxGTK updates the cursor for a window for which no
    // special cursor had been set).
    return m_impl ? m_impl->GetCursorFor(window) : wxCursor{};
}
