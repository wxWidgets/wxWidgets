///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/bmpbndl.cpp
// Purpose:     Common methods of wxBitmapBundle class.
// Author:      Vadim Zeitlin
// Created:     2021-09-22
// Copyright:   (c) 2021 Vadim Zeitlin <vadim@wxwidgets.org>
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
#endif // WX_PRECOMP

#include "wx/bmpbndl.h"
#include "wx/icon.h"
#include "wx/window.h"
#include "wx/filename.h"

#include "wx/private/bmpbndl.h"

#include <algorithm>

#ifdef __WXOSX__
#include "wx/osx/private.h"
#endif

// ----------------------------------------------------------------------------
// private helpers
// ----------------------------------------------------------------------------

namespace
{

// Simplest possible bundle implementation storing a collection of bitmaps
class wxBitmapBundleImplSet : public wxBitmapBundleImpl
{
public:
    // The vector must not be empty, caller is supposed to have checked for it.
    explicit wxBitmapBundleImplSet(const wxVector<wxBitmap>& bitmaps)
    {
        Init(&bitmaps[0], bitmaps.size());
    }

    // Convenience ctor from a single bitmap.
    explicit wxBitmapBundleImplSet(const wxBitmap& bitmap)
    {
        Init(&bitmap, 1);
    }

    ~wxBitmapBundleImplSet()
    {
    }

    virtual wxSize GetDefaultSize() const wxOVERRIDE;
    virtual wxSize GetPreferredSizeAtScale(double scale) const wxOVERRIDE;
    virtual wxBitmap GetBitmap(const wxSize& size) wxOVERRIDE;

private:
    // Struct containing bitmap itself as well as a flag indicating whether we
    // generated it by rescaling the existing bitmap or not.
    struct Entry
    {
        // Create a new entry from the original bitmap.
        explicit Entry(const wxBitmap& bitmap_)
            : bitmap(bitmap_)
        {
            generated = false;
        }

        // Create a new entry of the given size by resizing the bitmap of an
        // existing one.
        Entry(const Entry& entry, const wxSize& size)
            : bitmap(entry.bitmap)
        {
            wxASSERT_MSG( !entry.generated, wxS("should use original bitmap") );

            generated = true;

            wxBitmap::Rescale(bitmap, size);
        }

        wxBitmap bitmap;
        bool generated;
    };

    // Comparator comparing entries by the bitmap size.
    struct BitmapSizeComparator
    {
        bool operator()(const Entry& entry1, const Entry& entry2) const
        {
            // We could compare the bitmaps areas too, but they're supposed to
            // all use different sizes anyhow, so keep things simple.
            return entry1.bitmap.GetHeight() < entry2.bitmap.GetHeight();
        }
    };

    typedef wxVector<Entry> Entries;

    // All bitmaps sorted by size.
    //
    // Note that this vector is never empty.
    Entries m_entries;


    // Common implementation of all ctors.
    void Init(const wxBitmap* bitmaps, size_t n);

#ifdef __WXOSX__
    void OSXCreateNSImage();
#endif

    wxDECLARE_NO_COPY_CLASS(wxBitmapBundleImplSet);
};

} // anonymous namespace

// ============================================================================
// wxBitmapBundleImplSet implementation
// ============================================================================

void wxBitmapBundleImplSet::Init(const wxBitmap* bitmaps, size_t n)
{
    m_entries.reserve(n);
    for ( size_t i = 0; i < n; ++i )
    {
        const wxBitmap& bitmap = bitmaps[i];

        wxASSERT_MSG( bitmap.IsOk(), wxS("all bundle bitmaps must be valid") );

        m_entries.push_back(Entry(bitmap));
    }

    std::sort(m_entries.begin(), m_entries.end(), BitmapSizeComparator());

    // Should we check that all bitmaps really have unique sizes here? For now,
    // don't bother with this, but we might want to do it later if it really
    // turns out to be a problem in practice.

#ifdef __WXOSX__
    OSXCreateNSImage();
#endif
}

wxSize wxBitmapBundleImplSet::GetDefaultSize() const
{
    // Default size is the size of the smallest bitmap in the bundle.
    return m_entries[0].bitmap.GetSize();
}

wxSize wxBitmapBundleImplSet::GetPreferredSizeAtScale(double scale) const
{
    // Target size is the ideal size we'd like the bitmap to have at this scale.
    const wxSize sizeTarget = GetDefaultSize()*scale;

    const size_t n = m_entries.size();
    for ( size_t i = 0; i < n; ++i )
    {
        const wxSize sizeThis = m_entries[i].bitmap.GetSize();

        // Keep looking for the exact match which we still can hope to find
        // while the current bitmap is smaller.
        if ( sizeThis.y < sizeTarget.y )
            continue;

        // If we've found the exact match, just return it.
        if ( sizeThis.y == sizeTarget.y )
            return sizeThis;

        // We've found the closest bigger bitmap.

        // If there is no smaller bitmap, we have no choice but to use this one.
        if ( i == 0 )
            return sizeThis;

        // Decide whether we should use this one or the previous smaller one
        // depending on which of them is closer to the target size, breaking
        // the tie in favour of the bigger size.
        const wxSize sizeLast = m_entries[i - 1].bitmap.GetSize();

        return sizeThis.y - sizeTarget.y <= sizeTarget.y - sizeLast.y
                ? sizeThis
                : sizeLast;

    }

    // We only get here if the target size is bigger than all the available
    // sizes, in which case we have no choice but to use the biggest bitmap.
    return m_entries.back().bitmap.GetSize();
}

wxBitmap wxBitmapBundleImplSet::GetBitmap(const wxSize& size)
{
    // We use linear search instead if binary one because it's simpler and the
    // vector size is small enough (< 10) for it not to matter in practice.
    const size_t n = m_entries.size();
    size_t lastSmaller = 0;
    for ( size_t i = 0; i < n; ++i )
    {
        const Entry& entry = m_entries[i];

        const wxSize sizeThis = entry.bitmap.GetSize();
        if ( sizeThis.y == size.y )
        {
            // Exact match, just use it.
            return entry.bitmap;
        }

        if ( sizeThis.y < size.y )
        {
            // Don't rescale this one, we prefer to downscale rather than
            // upscale as it results in better-looking bitmaps.
            lastSmaller = i;
            continue;
        }

        if ( sizeThis.y > size.y && !entry.generated )
        {
            // We know that we don't have any exact match and we've found the
            // next bigger bitmap, so rescale it to the desired size.
            const Entry entryNew(entry, size);

            m_entries.insert(m_entries.begin() + lastSmaller + 1, entryNew);

            return entryNew.bitmap;
        }
    }

    // We only get here if the requested size is larger than the size of all
    // the bitmaps we have, in which case we have no choice but to upscale one
    // of the bitmaps, so find the largest available non-generated bitmap.
    for ( size_t i = n; n > 0; --i )
    {
        const Entry& entry = m_entries[i - 1];
        if ( !entry.generated )
        {
            const Entry entryNew(entry, size);

            m_entries.push_back(entryNew);

            return entryNew.bitmap;
        }
    }

    // We should have at least one non-generated bitmap.
    wxFAIL_MSG( wxS("unreachable") );

    return wxBitmap();
}

#ifdef __WXOSX__
void wxBitmapBundleImplSet::OSXCreateNSImage()
{
    WXImage image = NULL;
#if wxOSX_USE_COCOA
    image = wxOSXImageFromBitmap(m_entries[0].bitmap);
    const size_t n = m_entries.size();
    for ( size_t i = 1; i < n; ++i )
    {
        const Entry& entry = m_entries[i];
        wxOSXAddBitmapToImage(image, entry.bitmap);
    }
#else
    image = wxOSXImageFromBitmap(m_entries[0].bitmap);
    // TODO determine best bitmap for device scale factor, and use that
    // with wxOSXImageFromBitmap as on iOS there is only one bitmap in a UIImage
#endif
    if ( image )
        wxOSXSetImageForBundleImpl(this, image);
}
#endif

// ============================================================================
// wxBitmapBundle implementation
// ============================================================================

wxBitmapBundle::wxBitmapBundle()
{
}

wxBitmapBundle::wxBitmapBundle(wxBitmapBundleImpl* impl)
    : m_impl(impl)
{
}

wxBitmapBundle::wxBitmapBundle(const wxBitmap& bitmap)
    : m_impl(bitmap.IsOk() ? new wxBitmapBundleImplSet(bitmap) : NULL)
{
}

wxBitmapBundle::wxBitmapBundle(const wxIcon& icon)
    : m_impl(icon.IsOk() ? new wxBitmapBundleImplSet(wxBitmap(icon)) : NULL)
{
}

wxBitmapBundle::wxBitmapBundle(const wxImage& image)
    : m_impl(image.IsOk() ? new wxBitmapBundleImplSet(wxBitmap(image)) : NULL)
{
}

wxBitmapBundle::wxBitmapBundle(const wxBitmapBundle& other)
    : m_impl(other.m_impl)
{
}

wxBitmapBundle& wxBitmapBundle::operator=(const wxBitmapBundle& other)
{
    // No need to check for self-assignment because m_impl already does.
    m_impl = other.m_impl;
    return *this;
}

wxBitmapBundle::~wxBitmapBundle()
{
}

/* static */
wxBitmapBundle wxBitmapBundle::FromBitmaps(const wxVector<wxBitmap>& bitmaps)
{
    if ( bitmaps.empty() )
        return wxBitmapBundle();

    return wxBitmapBundle(new wxBitmapBundleImplSet(bitmaps));
}

/* static */
wxBitmapBundle wxBitmapBundle::FromImpl(wxBitmapBundleImpl* impl)
{
    return wxBitmapBundle(impl);
}


// MSW has its own, actually working, version, in MSW-specific code.
#if !defined( __WXMSW__ ) && !defined( __WXOSX__ )

/* static */
wxBitmapBundle wxBitmapBundle::FromResources(const wxString& WXUNUSED(name))
{
    wxFAIL_MSG
    (
        "Loading bitmaps from resources not available on this platform, "
        "don't use this function and call wxBitmapBundle::FromBitmaps() "
        "instead."
    );

    return wxBitmapBundle();
}

#endif // !__WXMSW__ && !__WXOSX__

wxBitmapBundle wxBitmapBundle::FromFiles(const wxString& filename)
{
    wxFileName fn(filename);
    return FromFiles(fn.GetPath(wxPATH_GET_VOLUME), fn.GetName(), fn.GetExt());
}

#if !defined( __WXOSX__ )

/* static */
wxBitmapBundle wxBitmapBundle::FromFiles(const wxString& path, const wxString& filename, const wxString& extension)
{
    wxVector<wxBitmap> bitmaps;

    wxFileName fn(path, filename, extension);
    wxString ext = extension.Lower();

    for ( int dpiFactor = 1 ; dpiFactor <= 2 ; ++dpiFactor)
    {
        if ( dpiFactor == 1 )
            fn.SetName(filename);
        else
            fn.SetName(wxString::Format("%s@%dx", filename, dpiFactor));

        if ( !fn.FileExists() && dpiFactor != 1 )
        {
            // try alternate naming scheme
            fn.SetName(wxString::Format("%s_%dx", filename, dpiFactor));
        }

        if ( fn.FileExists() )
        {
            wxBitmap bmp(fn.GetFullPath(), wxBITMAP_TYPE_ANY);
            // TODO Decide: SetScaleFactor to dpiFactor ? only on GTK3 ?

            if ( bmp.IsOk() )
            {
                bitmaps.push_back(bmp);
            }
        }
    }

    return wxBitmapBundle::FromBitmaps(bitmaps);
}

#endif

wxSize wxBitmapBundle::GetDefaultSize() const
{
    if ( !m_impl )
        return wxDefaultSize;

    return m_impl->GetDefaultSize();
}

wxSize wxBitmapBundle::GetPreferredSizeFor(wxWindow* window) const
{
    wxCHECK_MSG( window, wxDefaultSize, "window must be valid" );

    return GetPreferredSizeAtScale(window->GetDPIScaleFactor());
}

wxSize wxBitmapBundle::GetPreferredSizeAtScale(double scale) const
{
    if ( !m_impl )
        return wxDefaultSize;

    return m_impl->GetPreferredSizeAtScale(scale);
}

wxBitmap wxBitmapBundle::GetBitmap(const wxSize& size) const
{
    if ( !m_impl )
        return wxBitmap();

    return m_impl->GetBitmap(size == wxDefaultSize ? GetDefaultSize() : size);
}

// ============================================================================
// wxBitmapBundleImpl implementation
// ============================================================================

wxBitmapBundleImpl::~wxBitmapBundleImpl()
{
#ifdef __WXOSX__
    wxOSXBundleImplDestroyed(this);
#endif
}
