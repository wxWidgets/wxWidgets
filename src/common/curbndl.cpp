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
    #include "wx/app.h"
    #include "wx/bitmap.h"
    #include "wx/bmpbndl.h"
    #include "wx/cursor.h"
    #include "wx/gdicmn.h"
    #include "wx/settings.h"
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "wx/display.h"

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

    virtual wxCursor GetCursorFor(const wxWindow* WXUNUSED(window)) const override
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
        // If the window is not specified, try to use the main window (note
        // that it can still be null, but this is not an error, we'll just use
        // the default system cursor size in this case).
        if ( !window )
            window = wxApp::GetMainTopWindow();

        // Just calling GetBitmapFor(window) here would be wrong as cursor size
        // is not always just the standard size scaled by DPI, for example, the
        // user may increase the cursor size even in standard DPI, so get the
        // size at the effectively used scale which is obtained by comparing
        // the currently used cursor size with the default one.
        int h = wxSystemSettings::GetMetric(wxSYS_CURSOR_Y, window);

        // We need the physical size of the cursor bitmap.
        h = wxWindow::ToPhys(h, window);

        // Check if we can reuse the last returned cursor.
        if ( h == m_lastCursorSize )
            return m_lastCursor;

        // Determining the default size is tricky because wxSystemSetting tries
        // hard to return the correct size for the current system, but this is
        // not what we want here, so hardcode the size instead.
#if defined(__WXGTK__) || defined(__WXX11__)
        constexpr int hDef = 24;
#else
        // This is the default size for Windows and macOS and hopefully should
        // be fine for everyone else too.
        constexpr int hDef = 32;
#endif

        const wxSize
            sizeDef = m_bitmaps.GetDefaultSize();

        // Note that we add 1 to the height to break the tie in favour of a
        // larger cursor: by default, wxBitmapBundle prefers smaller bitmaps,
        // but for the cursors choosing the larger one seems preferable and
        // MSW does it like this for its default cursor.
        const wxSize
            size = m_bitmaps.GetPreferredBitmapSizeAtScale(double(h + 1) / hDef);

        const wxPoint
            hotSpot = wxRescaleCoord(m_hotSpot).From(sizeDef).To(size);

        // The bitmap used for the cursor must always be use the current scale
        // factor to avoid being rescaled.
        wxBitmap bitmap = m_bitmaps.GetBitmap(size);
        bitmap.SetScaleFactor(window ? window->GetDPIScaleFactor()
                                     : wxDisplay().GetScaleFactor());

        m_lastCursor = wxCursor(bitmap, hotSpot);
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

wxCursorBundle::wxCursorBundle(wxStockCursor id)
    : m_impl(new wxCursorBundleSingleImpl(wxCursor(id)))
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

wxCursor wxCursorBundle::GetCursorForMainWindow() const
{
    return GetCursorFor(nullptr);
}

// ----------------------------------------------------------------------------
// Global functions
// ----------------------------------------------------------------------------

#if WXWIN_COMPATIBILITY_3_2

/* static */
wxCursor wxBusyCursor::GetStoredCursor()
{
    return wxCursor{};
}

/* static */
wxCursor wxBusyCursor::GetBusyCursor()
{
    return wxCursor{};
}

#endif // WXWIN_COMPATIBILITY_3_2

void wxBeginBusyCursor(const wxCursorBundle& cursors)
{
    const wxCursor cursor = cursors.GetCursorForMainWindow();
    wxBeginBusyCursor(cursor.IsOk() ? &cursor : nullptr);
}
