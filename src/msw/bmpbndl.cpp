///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/bmpbndl.cpp
// Purpose:     MSW-specific part of wxBitmapBundle class.
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

#include "wx/utils.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"

    #include "wx/msw/private.h"
    #include "wx/msw/wrapwin.h"
#endif // WX_PRECOMP

#include "wx/bmpbndl.h"

#include "wx/private/bmpbndl.h"

#include <algorithm>

// ----------------------------------------------------------------------------
// private helpers
// ----------------------------------------------------------------------------

namespace
{

// Helper function, which will probably need to be factored out as it's likely
// to be useful elsewhere, returning the scale determined by the given suffix,
// i.e. it returns 2 for the suffix of "_2x" or 1.5 for "_1_5x".
//
// Returns 1 if the suffix is empty or 0 if it doesn't look like a valid scale
// suffix at all.
double GetScaleFromSuffix(const wxString& suffix)
{
    if ( suffix.empty() )
        return 1.0;

    if ( *suffix.begin() != '_' )
        return 0;

    if ( *suffix.rbegin() != 'x' )
        return 0;

    wxString factor = suffix.substr(1, suffix.length() - 2);
    if ( factor.empty() )
        return 0;

    factor.Replace(wxS("_"), wxS("."));

    double d;
    if ( !factor.ToCDouble(&d) )
        return 0;

    return d;
}

// Just a bitmap name and the corresponding scale for some resource.
struct ResourceInfo
{
    ResourceInfo(const wxString& name_, double scale_)
        : name(name_), scale(scale_)
    {
    }

    wxString name;
    double scale;
};

// Predicate for std::sort() comparing resource infos using their scale.
struct ScaleComparator
{
    bool operator()(const ResourceInfo& info1, const ResourceInfo& info2) const
    {
        return info1.scale < info2.scale;
    }
};

typedef wxVector<ResourceInfo> ResourceInfos;

// Used to pass information to EnumRCBitmaps() callback.
struct RCEnumCallbackData
{
    explicit RCEnumCallbackData(const wxString& name_)
        : name(name_.Lower())
    {
    }

    // The base name of all resources, passed to the callback.
    wxString name;

    // All the existing bitmaps starting with this name and using a valid scale
    // suffix, filled by the callback.
    ResourceInfos resourceInfos;
};

BOOL CALLBACK EnumRCBitmaps(HMODULE WXUNUSED(hModule),
                            LPCTSTR WXUNUSED(lpszType),
                            LPTSTR lpszName,
                            LONG_PTR lParam)
{
    wxString name(lpszName);
    name.MakeLower(); // resource names are case insensitive

    RCEnumCallbackData *data = reinterpret_cast<RCEnumCallbackData*>(lParam);

    wxString suffix;
    if ( name.StartsWith(data->name, &suffix) )
    {
        const double scale = GetScaleFromSuffix(suffix);
        if ( scale )
            data->resourceInfos.push_back(ResourceInfo(name, scale));
    }

    return TRUE; // continue enumeration
}

// Bundle implementation using PNG bitmaps from Windows resources.
class wxBitmapBundleImplRC : public wxBitmapBundleImpl
{
public:
    // Ctor takes the vector containing all the existing resources starting
    // with the given name and the valid bitmap corresponding to the base name
    // resource itself.
    wxBitmapBundleImplRC(const ResourceInfos& resourceInfos,
                         const wxBitmap& bitmap);

    virtual wxSize GetDefaultSize() const wxOVERRIDE;
    virtual wxSize GetPreferredBitmapSizeAtScale(double scale) const wxOVERRIDE;
    virtual wxBitmap GetBitmap(const wxSize& size) wxOVERRIDE;

protected:
    virtual double GetNextAvailableScale(size_t& i) const wxOVERRIDE;

private:
    // Load the bitmap from the given resource and add it m_bitmaps, after
    // rescaling it to the given size if necessary, i.e. if the needed size is
    // different from the size of this bitmap itself.
    //
    // The expected size is used to confirm that the resource really has the
    // size we expect it to have.
    //
    // Return the just loaded bitmap.
    wxBitmap
    AddBitmap(const ResourceInfo& info,
              const wxSize& sizeExpected,
              const wxSize& sizeNeeded);


    // All the available resources.
    const ResourceInfos m_resourceInfos;

    // All already loaded bitmaps sorted by their size.
    //
    // This vector is never empty and its first element is the bitmap using the
    // default size.
    wxVector<wxBitmap> m_bitmaps;

    wxDECLARE_NO_COPY_CLASS(wxBitmapBundleImplRC);
};

} // anonymous namespace

// ============================================================================
// wxBitmapBundleImplRC implementation
// ============================================================================

wxBitmapBundleImplRC::wxBitmapBundleImplRC(const ResourceInfos& resourceInfos,
                                           const wxBitmap& bitmap)
    : m_resourceInfos(resourceInfos)
{
    m_bitmaps.push_back(bitmap);
}

wxSize wxBitmapBundleImplRC::GetDefaultSize() const
{
    return m_bitmaps[0].GetSize();
}

double wxBitmapBundleImplRC::GetNextAvailableScale(size_t& i) const
{
    return i < m_resourceInfos.size() ? m_resourceInfos[i++].scale : 0.0;
}

wxSize wxBitmapBundleImplRC::GetPreferredBitmapSizeAtScale(double scale) const
{
    const size_t n = m_resourceInfos.size();
    wxVector<double> scales(n);
    for ( size_t i = 0; i < n; ++i )
    {
        scales[i] = m_resourceInfos[i].scale;
    }

    return DoGetPreferredSize(scale);
}

wxBitmap
wxBitmapBundleImplRC::AddBitmap(const ResourceInfo& info,
                                const wxSize& sizeExpected,
                                const wxSize& sizeNeeded)
{
    // First load the bitmap, it shouldn't fail, but we can't do much if it
    // does fail -- we're just going to return an invalid bitmap in this case.
    wxBitmap bitmap(info.name, wxBITMAP_TYPE_PNG_RESOURCE);

    // We rely on suffixes really corresponding to the bitmap sizes and bad
    // things will happen if they don't.
    wxASSERT_MSG
    (
        bitmap.GetSize() == sizeExpected,
        wxString::Format(wxS("Bitmap \"%s\" should have size %d*%d."),
                         info.name, sizeExpected.x, sizeExpected.y)
    );

    if ( sizeNeeded != sizeExpected )
        wxBitmap::Rescale(bitmap, sizeNeeded);


    // Now cache the bitmap for future use by inserting it into our sorted
    // vector of bitmaps. We don't bother with the binary search here because
    // the vector is typically very small (< 10 elements).
    const wxSize sizeThis = bitmap.GetSize();
    for ( size_t i = 0; ; ++i )
    {
        if ( i == m_bitmaps.size() )
        {
            // This is the biggest bitmap we have, put it at the end.
            m_bitmaps.push_back(bitmap);
            break;
        }

        if ( m_bitmaps[i].GetSize().y > sizeThis.y )
        {
            m_bitmaps.insert(m_bitmaps.begin() + i, bitmap);
            break;
        }
    }

    return bitmap;
}

wxBitmap wxBitmapBundleImplRC::GetBitmap(const wxSize& size)
{
    // First check if we already have the bitmap of this size: we're only
    // interested in the exact match here.
    for ( size_t i = 0; i < m_bitmaps.size(); ++i )
    {
        const wxBitmap& bitmap = m_bitmaps[i];

        if ( bitmap.GetSize() == size )
            return bitmap;
    }

    // If we don't have one, create it using one of the available resources.
    // Here we try to find the exact match (ideal) or the next larger size
    // (downscaling results in better appearance than upscaling, generally
    // speaking).
    const wxSize sizeDef = GetDefaultSize();
    for ( size_t i = 0; i < m_resourceInfos.size(); ++i )
    {
        const ResourceInfo& info = m_resourceInfos[i];

        const wxSize sizeThis = sizeDef*info.scale;

        // Use this bitmap if it's the first one bigger than the requested size.
        if ( sizeThis.y >= size.y )
            return AddBitmap(info, sizeThis, size);
    }

    // We have to upscale some bitmap because we don't have any bitmaps larger
    // than the requested size. Try to find one which can be upscaled using an
    // integer factor.
    const size_t i = GetIndexToUpscale(size);

    const ResourceInfo& info = m_resourceInfos[i];

    return AddBitmap(info, sizeDef*info.scale, size);
}

// ============================================================================
// wxBitmapBundle::FromResources() implementation
// ============================================================================

/* static */
wxBitmapBundle wxBitmapBundle::FromResources(const wxString& name)
{
    // First of all, find all resources starting with this name.
    RCEnumCallbackData data(name);

    if ( !::EnumResourceNames(NULL, // this HMODULE
                              RT_RCDATA,
                              EnumRCBitmaps,
                              reinterpret_cast<LONG_PTR>(&data)) )
    {
        const DWORD err = ::GetLastError();
        if ( err != NO_ERROR && err != ERROR_RESOURCE_TYPE_NOT_FOUND )
        {
            wxLogSysError(wxS("Couldn't enumerate RCDATA resources"));
        }
    }

    ResourceInfos& resourceInfos = data.resourceInfos;

    if ( resourceInfos.empty() )
        return wxBitmapBundle();

    // We need to load the base bitmap to get the default size anyhow, so do it
    // now and also check that it works.
    wxBitmap bitmap(name, wxBITMAP_TYPE_PNG_RESOURCE);
    if ( !bitmap.IsOk() )
        return wxBitmapBundle();

    // Sort the resources in the order of increasing sizes to simplify the code
    // of wxBitmapBundleImplRC::GetBitmap().
    std::sort(resourceInfos.begin(), resourceInfos.end(), ScaleComparator());

    return wxBitmapBundle(new wxBitmapBundleImplRC(resourceInfos, bitmap));
}

#ifdef wxHAS_SVG

/* static */
wxBitmapBundle wxBitmapBundle::FromSVGResource(const wxString& name, const wxSize& sizeDef)
{
    // Currently we hardcode RCDATA resource type as this is what is usually
    // used for the embedded images. We could allow specifying the type as part
    // of the name in the future (e.g. "type:name" or something like this) if
    // really needed.
    wxCharBuffer svgData = wxCharBuffer::CreateOwned(wxLoadUserResource(name, RT_RCDATA, NULL, wxGetInstance()));

    if ( !svgData.data() )
    {
        wxLogError(wxS("SVG image \"%s\" not found, check ")
                   wxS("that the resource file contains \"RCDATA\" ")
                   wxS("resource with this name."),
                   name);

        return wxBitmapBundle();
    }

    return wxBitmapBundle::FromSVG(svgData.data(), sizeDef);
}

#endif // wxHAS_SVG
