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
#include "wx/filename.h"
#include "wx/icon.h"
#include "wx/iconbndl.h"
#include "wx/imaglist.h"
#include "wx/scopeguard.h"
#include "wx/window.h"

#include "wx/private/bmpbndl.h"

#include <algorithm>

#ifdef __WXOSX__
#include "wx/osx/private.h"
#endif

#if wxUSE_VARIANT

#include "wx/variant.h"

// We can't use the macros from wx/variant.h because they only work for
// wxObject-derived classes, so define our own wxVariantData.

class wxBitmapBundleVariantData: public wxVariantData
{
public:
    explicit wxBitmapBundleVariantData(const wxBitmapBundle& value)
        : m_value(value)
    {
    }

    virtual bool Eq(wxVariantData& data) const wxOVERRIDE
    {
        // We're only called with the objects of the same type, so the cast is
        // safe.
        return static_cast<wxBitmapBundleVariantData&>(data).m_value.IsSameAs(m_value);
    }

    virtual wxString GetType() const wxOVERRIDE
    {
        return wxASCII_STR("wxBitmapBundle");
    }

    virtual wxClassInfo* GetValueClassInfo() wxOVERRIDE
    {
        return NULL;
    }

    virtual wxVariantData* Clone() const wxOVERRIDE
    {
        return new wxBitmapBundleVariantData(m_value);
    }

    wxBitmapBundle m_value;

    DECLARE_WXANY_CONVERSION()
};

IMPLEMENT_TRIVIAL_WXANY_CONVERSION(wxBitmapBundle, wxBitmapBundleVariantData)

WXDLLIMPEXP_CORE
wxBitmapBundle& operator<<(wxBitmapBundle& value, const wxVariant& variant)
{
    wxASSERT( variant.GetType() == wxASCII_STR("wxBitmapBundle") );

    wxBitmapBundleVariantData* const
        data = static_cast<wxBitmapBundleVariantData*>(variant.GetData());

    value = data->m_value;
    return value;
}

WXDLLIMPEXP_CORE
wxVariant& operator<<(wxVariant& variant, const wxBitmapBundle& value)
{
    variant.SetData(new wxBitmapBundleVariantData(value));
    return variant;
}

#endif // wxUSE_VARIANT

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
    virtual wxSize GetPreferredBitmapSizeAtScale(double scale) const wxOVERRIDE;
    virtual wxBitmap GetBitmap(const wxSize& size) wxOVERRIDE;

protected:
    virtual double GetNextAvailableScale(size_t& i) const wxOVERRIDE;

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

    // The size of the bitmap at the default size.
    //
    // Note that it may be different from the size of the first entry if we
    // only have high resolution bitmap and no bitmap for 100% DPI.
    wxSize m_sizeDefault;

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

    // This is not normally the case, but it could happen that even the
    // smallest bitmap has scale factor > 1, so use its size in DIPs (this can
    // notably be the case when there is only a single high resolution bitmap
    // provided, e.g. in the code predating wxBitmapBundle introduction but now
    // using it due to implicit conversion to it from wxBitmap).
    m_sizeDefault = m_entries[0].bitmap.GetDIPSize();

    // Should we check that all bitmaps really have unique sizes here? For now,
    // don't bother with this, but we might want to do it later if it really
    // turns out to be a problem in practice.

#ifdef __WXOSX__
    OSXCreateNSImage();
#endif
}

wxSize wxBitmapBundleImplSet::GetDefaultSize() const
{
    return m_sizeDefault;
}

double wxBitmapBundleImplSet::GetNextAvailableScale(size_t& i) const
{
    while ( i < m_entries.size() )
    {
        const Entry& entry = m_entries[i++];

        if ( entry.generated )
            continue;

        return static_cast<double>(entry.bitmap.GetSize().y) / GetDefaultSize().y;
    }

    return 0.0;
}

wxSize wxBitmapBundleImplSet::GetPreferredBitmapSizeAtScale(double scale) const
{
    return DoGetPreferredSize(scale);
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
    // of the bitmaps, so find the most appropriate one for doing it.
    const size_t i = GetIndexToUpscale(size);

    const Entry entryNew(m_entries[i], size);

    m_entries.push_back(entryNew);

    return entryNew.bitmap;
}

#ifdef __WXOSX__
void wxBitmapBundleImplSet::OSXCreateNSImage()
{
    WXImage image = NULL;
    const size_t n = m_entries.size();
    if ( n == 1 )
    {
        // The special case of only a single bitmap in the bundle is common and
        // occurs in older code using wxBitmap instead of wxBitmapBundle. We want
        // to avoid creating a new NSImage as is done below in this case for two
        // reasons:
        //
        // - performance - returning existing NSImage is much cheaper
        // - backward compatibility - using existing NSImage preserves existing
        //   metadata such as image name or, importantly, its isTemplate property
        //
        // The OSXGetImage() method will either return NSImage representation
        // if it already exists, which takes care of the two above points.
        // Otherwise, it will create NSImage using wxOSXGetImageFromCGImage,
        // which is essentially identical to wxOSXImageFromBitmap below.
        image = m_entries[0].bitmap.OSXGetImage();
    }
    else
    {
        // Generate NSImage with multiple scale factors from individual bitmaps
#if wxOSX_USE_COCOA
        image = wxOSXImageFromBitmap(m_entries[0].bitmap);
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
    }

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

wxBitmapBundle::wxBitmapBundle(const char* const* xpm)
    : m_impl(new wxBitmapBundleImplSet(wxBitmap(xpm)))
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

void wxBitmapBundle::Clear()
{
    m_impl.reset(NULL);
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

/* static */
wxBitmapBundle wxBitmapBundle::FromIconBundle(const wxIconBundle& iconBundle)
{
    if ( !iconBundle.IsOk() || iconBundle.IsEmpty() )
        return wxBitmapBundle();

    wxVector<wxBitmap> bitmaps;
    for ( size_t n = 0; n < iconBundle.GetIconCount(); ++n )
    {
        bitmaps.push_back(iconBundle.GetIconByIndex(n));
    }

    return FromBitmaps(bitmaps);
}

// MSW and MacOS have their own, actually working, version, in their platform-specific code.
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

#ifdef wxHAS_SVG

/* static */
wxBitmapBundle wxBitmapBundle::FromSVGResource(const wxString& WXUNUSED(name), const wxSize& WXUNUSED(sizeDef))
{
    wxFAIL_MSG
    (
        "Loading an SVG from a resource not available on this platform, "
        "don't use this function and call wxBitmapBundle::FromSVG(File)() "
        "instead."
    );

    return wxBitmapBundle();
}

#endif // wxHAS_SVG

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

wxSize wxBitmapBundle::GetPreferredBitmapSizeFor(const wxWindow* window) const
{
    wxCHECK_MSG( window, wxDefaultSize, "window must be valid" );

    return GetPreferredBitmapSizeAtScale(window->GetDPIScaleFactor());
}

wxSize wxBitmapBundle::GetPreferredLogicalSizeFor(const wxWindow* window) const
{
    wxCHECK_MSG( window, wxDefaultSize, "window must be valid" );

    return window->FromPhys(GetPreferredBitmapSizeAtScale(window->GetDPIScaleFactor()));
}

wxSize wxBitmapBundle::GetPreferredBitmapSizeAtScale(double scale) const
{
    if ( !m_impl )
        return wxDefaultSize;

    return m_impl->GetPreferredBitmapSizeAtScale(scale);
}

wxBitmap wxBitmapBundle::GetBitmap(const wxSize& size) const
{
    if ( !m_impl )
        return wxBitmap();

    const wxSize sizeDef = GetDefaultSize();

    wxBitmap bmp = m_impl->GetBitmap(size == wxDefaultSize ? sizeDef : size);

    // Ensure that the returned bitmap uses the scale factor such that it takes
    // the same space, in logical pixels, as the bitmap in the default size.
    if ( size != wxDefaultSize )
        bmp.SetScaleFactor(static_cast<double>(size.y)/sizeDef.y);

    return bmp;
}

wxIcon wxBitmapBundle::GetIcon(const wxSize& size) const
{
    wxIcon icon;

    const wxBitmap bmp = GetBitmap(size);
    if ( bmp.IsOk() )
        icon.CopyFromBitmap(bmp);

    return icon;
}

wxBitmap wxBitmapBundle::GetBitmapFor(const wxWindow* window) const
{
    return GetBitmap(GetPreferredBitmapSizeFor(window));
}

wxIcon wxBitmapBundle::GetIconFor(const wxWindow* window) const
{
    return GetIcon(GetPreferredBitmapSizeFor(window));
}

namespace
{

// Struct containing the number of tools preferring to use the given size.
struct SizePrefWithCount
{
    SizePrefWithCount() : count(0) { }

    wxSize size;
    int count;
};

typedef wxVector<SizePrefWithCount> SizePrefs;

void RecordSizePref(SizePrefs& prefs, const wxSize& size)
{
    for ( size_t n = 0; n < prefs.size(); ++n )
    {
        if ( prefs[n].size == size )
        {
            prefs[n].count++;
            return;
        }
    }

    SizePrefWithCount pref;
    pref.size = size;
    pref.count++;
    prefs.push_back(pref);
}

} // anonymous namespace

/* static */
wxSize
wxBitmapBundle::GetConsensusSizeFor(wxWindow* win,
                                    const wxVector<wxBitmapBundle>& bundles)
{
    return GetConsensusSizeFor(win->GetDPIScaleFactor(), bundles);
}

/* static */
wxSize
wxBitmapBundle::GetConsensusSizeFor(double scale,
                                    const wxVector<wxBitmapBundle>& bundles)
{
    // We want to use preferred bitmap size, but the preferred sizes can be
    // different for different bitmap bundles, so record all their preferences
    // first.
    SizePrefs prefs;
    for ( size_t n = 0; n < bundles.size(); ++n )
    {
        RecordSizePref(prefs, bundles[n].GetPreferredBitmapSizeAtScale(scale));
    }

    // Now find the size preferred by most tools.
    int countMax = 0;
    wxSize sizePreferred;
    for ( size_t n = 0; n < prefs.size(); ++n )
    {
        const int countThis = prefs[n].count;
        const wxSize sizeThis = prefs[n].size;

        if ( countThis > countMax )
        {
            countMax = countThis;
            sizePreferred = sizeThis;
        }
        else if ( countThis == countMax )
        {
            // We have a tie between different sizes.

            // Choose the larger one as like this some bitmaps will be
            // downscaled, which should look better than upscaling some
            // (other) ones.
            if ( sizeThis.y > sizePreferred.y )
                sizePreferred = sizeThis;
        }
    }

    return sizePreferred;
}

/* static */
wxImageList*
wxBitmapBundle::CreateImageList(wxWindow* win,
                                const wxVector<wxBitmapBundle>& bundles)
{
    wxCHECK_MSG( win, NULL, "must have a valid window" );
    wxCHECK_MSG( !bundles.empty(), NULL, "should have some images" );

    wxSize size = GetConsensusSizeFor(win, bundles);

    // wxImageList wants the logical size for the platforms where logical and
    // physical pixels are different.
    size /= win->GetContentScaleFactor();

    wxImageList* const iml = new wxImageList(size.x, size.y);

    for ( size_t n = 0; n < bundles.size(); ++n )
    {
        iml->Add(bundles[n].GetBitmap(size));
    }

    return iml;
}

// ============================================================================
// wxBitmapBundleImpl implementation
// ============================================================================

double
wxBitmapBundleImpl::GetNextAvailableScale(size_t& WXUNUSED(i)) const
{
    wxFAIL_MSG( wxS("must be overridden if called") );

    return 0.0;
}

wxSize
wxBitmapBundleImpl::DoGetPreferredSize(double scaleTarget) const
{
    double scaleBest = 0.0;
    double scaleLast = 0.0;

    for ( size_t i = 0;; )
    {
        const double scaleThis = GetNextAvailableScale(i);
        if ( scaleThis == 0.0 )
        {
            // We only get here if the target scale is bigger than all the
            // available scales, in which case we have no choice but to use the
            // biggest bitmap, which corresponds to the last used scale that we
            // should have by now.
            wxASSERT_MSG( scaleLast != 0.0, "must have some available scales" );

            // But check how far is it from the requested scale: if it's more than
            // 1.5 times larger, we should still scale it, notably to ensure that
            // bitmaps of standard size are scaled when 2x DPI scaling is used.
            if ( scaleTarget > 1.5*scaleLast )
            {
                // However scaling by non-integer scales doesn't work well at
                // all, so try to find a bitmap that we may rescale by an
                // integer factor.
                //
                // Note that this is similar to GetIndexToUpscale(), but we
                // don't want to fall back on the largest bitmap here, so we
                // can't reuse it.
                //
                // Also, while we reenter GetNextAvailableScale() here, it
                // doesn't matter because we're not going to continue using it
                // in the outer loop any more.
                for ( i = 0;; )
                {
                    const double scale = GetNextAvailableScale(i);
                    if ( scale == 0.0 )
                        break;

                    const double factor = scaleTarget / scale;
                    if ( wxRound(factor) == factor )
                    {
                        scaleBest = scaleTarget;

                        // We don't need to keep going: if there is a bigger
                        // bitmap which can be scaled using an integer factor
                        // to the target scale, our GetIndexToUpscale() will
                        // find it, we don't need to do it here.
                        break;
                    }
                }

                // If none of the bitmaps can be upscaled by an integer factor,
                // round the target scale itself, as we can be sure to be able
                // to scale at least the base bitmap to it using an integer
                // factor then.
                if ( scaleBest == 0.0 )
                    scaleBest = wxRound(scaleTarget);
            }
            else // Target scale is not much greater than the biggest one we have.
            {
                scaleBest = scaleLast;
            }

            break;
        }

        // Ensure we remember the last used scale value.
        wxON_BLOCK_EXIT_SET(scaleLast, scaleThis);

        // Keep looking for the exact match which we still can hope to find
        // while the current bitmap is smaller.
        if ( scaleThis < scaleTarget )
            continue;

        // If we've found the exact match, just return it.
        if ( scaleThis == scaleTarget )
        {
            scaleBest = scaleThis;
            break;
        }

        // We've found the closest bigger bitmap.

        // If there is no smaller bitmap, we have no choice but to use this one.
        if ( scaleLast == 0.0 )
        {
            scaleBest = scaleThis;
            break;
        }

        // Decide whether we should use this one or the previous smaller one
        // depending on which of them is closer to the target size, breaking
        // the tie in favour of the smaller size as it's arguably better to use
        // slightly smaller bitmaps than too big ones.
        scaleBest = scaleThis - scaleTarget < scaleTarget - scaleLast
                        ? scaleThis
                        : scaleLast;
        break;
    }

    return GetDefaultSize()*scaleBest;
}

size_t wxBitmapBundleImpl::GetIndexToUpscale(const wxSize& size) const
{
    // Our best hope is to find a scale dividing the given one evenly.
    size_t indexBest = (size_t)-1;

    // In the worst case, we will use the largest index, as it should hopefully
    // result in the least bad results.
    size_t indexLast = 0;

    const wxSize sizeDef = GetDefaultSize();
    for ( size_t i = 0;; )
    {
        // Save it before it's updated by GetNextAvailableScale().
        size_t indexPrev = i;

        const double scaleThis = GetNextAvailableScale(i);
        if ( scaleThis == 0.0 )
            break;

        // Only update it now, knowing that this index could have been used.
        indexLast = indexPrev;

        const double scale = size.y / (sizeDef.y*scaleThis);
        if (wxRound(scale) == scale)
            indexBest = indexLast;
    }

    return indexBest != (size_t)-1 ? indexBest : indexLast;
}

wxBitmapBundleImpl::~wxBitmapBundleImpl()
{
#ifdef __WXOSX__
    wxOSXBundleImplDestroyed(this);
#endif
}
