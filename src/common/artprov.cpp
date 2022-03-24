/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/artprov.cpp
// Purpose:     wxArtProvider class
// Author:      Vaclav Slavik
// Modified by:
// Created:     18/03/2002
// Copyright:   (c) Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/artprov.h"

#ifndef WX_PRECOMP
    #include "wx/list.h"
    #include "wx/log.h"
    #include "wx/hashmap.h"
    #include "wx/image.h"
    #include "wx/module.h"
    #include "wx/window.h"
#endif

// ===========================================================================
// implementation
// ===========================================================================

#include "wx/listimpl.cpp"
WX_DECLARE_LIST(wxArtProvider, wxArtProvidersList);
WX_DEFINE_LIST(wxArtProvidersList)

// ----------------------------------------------------------------------------
// Cache class - stores already requested bitmaps
// ----------------------------------------------------------------------------

WX_DECLARE_EXPORTED_STRING_HASH_MAP(wxBitmap, wxArtProviderBitmapsHash);
WX_DECLARE_EXPORTED_STRING_HASH_MAP(wxBitmapBundle, wxArtProviderBitmapBundlesHash);
WX_DECLARE_EXPORTED_STRING_HASH_MAP(wxIconBundle, wxArtProviderIconBundlesHash);

class WXDLLEXPORT wxArtProviderCache
{
public:
    bool GetBitmap(const wxString& full_id, wxBitmap* bmp);
    void PutBitmap(const wxString& full_id, const wxBitmap& bmp)
        { m_bitmapsHash[full_id] = bmp; }

    bool GetBitmapBundle(const wxString& full_id, wxBitmapBundle* bmpbndl);
    void PutBitmapBundle(const wxString& full_id, const wxBitmapBundle& bmpbndl)
        { m_bitmapsBundlesHash[full_id] = bmpbndl; }

    bool GetIconBundle(const wxString& full_id, wxIconBundle* bmp);
    void PutIconBundle(const wxString& full_id, const wxIconBundle& iconbundle)
        { m_iconBundlesHash[full_id] = iconbundle; }

    void Clear();

    static wxString ConstructHashID(const wxArtID& id,
                                    const wxArtClient& client,
                                    const wxSize& size);

    static wxString ConstructHashID(const wxArtID& id,
                                    const wxArtClient& client);

private:
    wxArtProviderBitmapsHash m_bitmapsHash;                 // cache of wxBitmaps
    wxArtProviderBitmapBundlesHash m_bitmapsBundlesHash;    // cache of wxBitmaps
    wxArtProviderIconBundlesHash m_iconBundlesHash;         // cache of wxIconBundles
};

bool wxArtProviderCache::GetBitmap(const wxString& full_id, wxBitmap* bmp)
{
    wxArtProviderBitmapsHash::iterator entry = m_bitmapsHash.find(full_id);
    if ( entry == m_bitmapsHash.end() )
    {
        return false;
    }
    else
    {
        *bmp = entry->second;
        return true;
    }
}

bool wxArtProviderCache::GetBitmapBundle(const wxString& full_id, wxBitmapBundle* bmpbndl)
{
    wxArtProviderBitmapBundlesHash::iterator entry = m_bitmapsBundlesHash.find(full_id);
    if ( entry == m_bitmapsBundlesHash.end() )
    {
        return false;
    }
    else
    {
        *bmpbndl = entry->second;
        return true;
    }
}


bool wxArtProviderCache::GetIconBundle(const wxString& full_id, wxIconBundle* bmp)
{
    wxArtProviderIconBundlesHash::iterator entry = m_iconBundlesHash.find(full_id);
    if ( entry == m_iconBundlesHash.end() )
    {
        return false;
    }
    else
    {
        *bmp = entry->second;
        return true;
    }
}

void wxArtProviderCache::Clear()
{
    m_bitmapsHash.clear();
    m_iconBundlesHash.clear();
}

/* static */ wxString
wxArtProviderCache::ConstructHashID(const wxArtID& id,
                                    const wxArtClient& client)
{
    return id + wxT('-') + client;
}


/* static */ wxString
wxArtProviderCache::ConstructHashID(const wxArtID& id,
                                    const wxArtClient& client,
                                    const wxSize& size)
{
    return ConstructHashID(id, client) + wxT('-') +
            wxString::Format(wxT("%d-%d"), size.x, size.y);
}

// ----------------------------------------------------------------------------
// wxBitmapBundleImplArt: uses art provider to get the bitmaps
// ----------------------------------------------------------------------------

namespace
{

class wxBitmapBundleImplArt : public wxBitmapBundleImpl
{
public:
    wxBitmapBundleImplArt(const wxArtID& id,
                          const wxArtClient& client,
                          const wxSize& size)
        : m_artId(id),
          m_artClient(client),
          m_sizeDefault(GetValidSize(id, client, size))
    {
    }

    virtual wxSize GetDefaultSize() const wxOVERRIDE
    {
        return m_sizeDefault;
    }

    virtual wxSize GetPreferredBitmapSizeAtScale(double scale) const wxOVERRIDE
    {
        // We have no preferred sizes.
        return m_sizeDefault*scale;
    }

    virtual wxBitmap GetBitmap(const wxSize& size) wxOVERRIDE
    {
        return wxArtProvider::GetBitmap(m_artId, m_artClient, size);
    }

private:
    static wxSize GetValidSize(const wxArtID& id,
                               const wxArtClient& client,
                               const wxSize& size)
    {
        // If valid size is provided, just use it.
        if ( size != wxDefaultSize )
            return size;

        // Otherwise, try to get the size we'd use without creating a bitmap
        // immediately.
        const wxSize sizeHint = wxArtProvider::GetSizeHint(client);
        if ( sizeHint != wxDefaultSize )
            return sizeHint;

        // If we really have to, do create a bitmap just to get its size. Note
        // we need the size in logical pixels here, it will be scaled later if
        // necessary, so use GetDIPSize() and not GetSize().
        const wxBitmap bitmap = wxArtProvider::GetBitmap(id, client);
        if ( bitmap.IsOk() )
            return bitmap.GetDIPSize();

        // We really need some default size, so just return this hardcoded
        // value if all else fails -- what else can we do.
        return wxSize(16, 16);
    }

    const wxArtID m_artId;
    const wxArtClient m_artClient;
    const wxSize m_sizeDefault;

    wxDECLARE_NO_COPY_CLASS(wxBitmapBundleImplArt);
};

} // anonymous namespace

// ============================================================================
// wxArtProvider class
// ============================================================================

wxIMPLEMENT_ABSTRACT_CLASS(wxArtProvider, wxObject);

wxArtProvidersList *wxArtProvider::sm_providers = NULL;
wxArtProviderCache *wxArtProvider::sm_cache = NULL;

// ----------------------------------------------------------------------------
// wxArtProvider ctors/dtor
// ----------------------------------------------------------------------------

wxArtProvider::~wxArtProvider()
{
    Remove(this);
}

// ----------------------------------------------------------------------------
// wxArtProvider operations on provider stack
// ----------------------------------------------------------------------------

/*static*/ void wxArtProvider::CommonAddingProvider()
{
    if ( !sm_providers )
    {
        sm_providers = new wxArtProvidersList;
        sm_cache = new wxArtProviderCache;
    }

    sm_cache->Clear();
}

/*static*/ void wxArtProvider::Push(wxArtProvider *provider)
{
    CommonAddingProvider();
    sm_providers->Insert(provider);
}

/*static*/ void wxArtProvider::PushBack(wxArtProvider *provider)
{
    CommonAddingProvider();
    sm_providers->Append(provider);
}

/*static*/ bool wxArtProvider::Pop()
{
    wxCHECK_MSG( sm_providers, false, wxT("no wxArtProvider exists") );
    wxCHECK_MSG( !sm_providers->empty(), false, wxT("wxArtProviders stack is empty") );

    delete sm_providers->GetFirst()->GetData();
    sm_cache->Clear();
    return true;
}

/*static*/ bool wxArtProvider::Remove(wxArtProvider *provider)
{
    wxCHECK_MSG( sm_providers, false, wxT("no wxArtProvider exists") );

    if ( sm_providers->DeleteObject(provider) )
    {
        sm_cache->Clear();
        return true;
    }

    return false;
}

/*static*/ bool wxArtProvider::Delete(wxArtProvider *provider)
{
    // provider will remove itself from the stack in its dtor
    delete provider;

    return true;
}

/*static*/ void wxArtProvider::CleanUpProviders()
{
    if ( sm_providers )
    {
        while ( !sm_providers->empty() )
            delete *sm_providers->begin();

        wxDELETE(sm_providers);
        wxDELETE(sm_cache);
    }
}

// ----------------------------------------------------------------------------
// wxArtProvider: retrieving bitmaps/icons
// ----------------------------------------------------------------------------

#if WXWIN_COMPATIBILITY_3_0
void wxArtProvider::RescaleBitmap(wxBitmap& bmp, const wxSize& sizeNeeded)
{
    return wxBitmap::Rescale(bmp, sizeNeeded);
}
#endif // WXWIN_COMPATIBILITY_3_0

void
wxArtProvider::RescaleOrResizeIfNeeded(wxBitmap& bmp, const wxSize& sizeNeeded)
{
    if ( sizeNeeded == wxDefaultSize )
        return;

    int bmp_w = bmp.GetWidth();
    int bmp_h = bmp.GetHeight();

    if ( bmp_w == sizeNeeded.x && bmp_h == sizeNeeded.y )
        return;

#if wxUSE_IMAGE
    if ((bmp_h <= sizeNeeded.x) && (bmp_w <= sizeNeeded.y))
    {
        // the caller wants default size, which is larger than
        // the image we have; to avoid degrading it visually by
        // scaling it up, paste it into transparent image instead:
        wxPoint offset((sizeNeeded.x - bmp_w)/2, (sizeNeeded.y - bmp_h)/2);
        wxImage img = bmp.ConvertToImage();
        img.Resize(sizeNeeded, offset);
        bmp = wxBitmap(img);
    }
    else // scale (down or mixed, but not up)
#endif // wxUSE_IMAGE
    {
        wxBitmap::Rescale(bmp, sizeNeeded);
    }
}

/*static*/ wxBitmap wxArtProvider::GetBitmap(const wxArtID& id,
                                             const wxArtClient& client,
                                             const wxSize& size)
{
    // safety-check against writing client,id,size instead of id,client,size:
    wxASSERT_MSG( client.Last() == wxT('C'), wxT("invalid 'client' parameter") );

    wxCHECK_MSG( sm_providers, wxNullBitmap, wxT("no wxArtProvider exists") );

    wxString hashId = wxArtProviderCache::ConstructHashID(id, client, size);

    wxBitmap bmp;
    if ( !sm_cache->GetBitmap(hashId, &bmp) )
    {
        for (wxArtProvidersList::compatibility_iterator node = sm_providers->GetFirst();
             node; node = node->GetNext())
        {
            wxArtProvider* const provider = node->GetData();
            bmp = provider->CreateBitmap(id, client, size);
            if ( bmp.IsOk() )
                break;

            const wxBitmapBundle& bb = provider->CreateBitmapBundle(id, client, size);
            if ( bb.IsOk() )
            {
                bmp = bb.GetBitmap(size);
                break;
            }
        }

        wxSize sizeNeeded = size;
        if ( !bmp.IsOk() )
        {
            // no bitmap created -- as a fallback, try if we can find desired
            // icon in a bundle
            wxIconBundle iconBundle = DoGetIconBundle(id, client);
            if ( iconBundle.IsOk() )
            {
                if ( sizeNeeded == wxDefaultSize )
                    sizeNeeded = GetNativeSizeHint(client);

                wxIcon icon(iconBundle.GetIcon(sizeNeeded));
                if ( icon.IsOk() )
                {
                    // this icon may be not of the correct size, it will be
                    // rescaled below in such case
                    bmp.CopyFromIcon(icon);
                }
            }
        }

        // resize the bitmap if necessary
        if ( bmp.IsOk() )
        {
            RescaleOrResizeIfNeeded(bmp, sizeNeeded);
        }

        sm_cache->PutBitmap(hashId, bmp);
    }

    return bmp;
}

/*static*/
wxBitmapBundle wxArtProvider::GetBitmapBundle(const wxArtID& id,
                                              const wxArtClient& client,
                                              const wxSize& size)
{
    // safety-check against writing client,id,size instead of id,client,size:
    wxASSERT_MSG( client.Last() == wxT('C'), wxT("invalid 'client' parameter") );

    wxCHECK_MSG( sm_providers, wxNullBitmap, wxT("no wxArtProvider exists") );

    wxString hashId = wxArtProviderCache::ConstructHashID(id, client, size);

    wxBitmapBundle bitmapbundle; // (DoGetIconBundle(id, client));

    if ( !sm_cache->GetBitmapBundle(hashId, &bitmapbundle) )
    {
        for (wxArtProvidersList::compatibility_iterator node = sm_providers->GetFirst();
             node; node = node->GetNext())
        {
            wxArtProvider* const provider = node->GetData();
            bitmapbundle = provider->CreateBitmapBundle(id, client, size);
            if ( bitmapbundle.IsOk() )
                break;

            // Try creating the bundle from individual bitmaps returned by the
            // provider because they can be available in more than one size,
            // i.e. it's better to return a custom bundle returning them in the
            // size closest to the requested one rather than a simple bundle
            // just containing the single bitmap in the specified size.
            //
            // Note that we do this here rather than outside of this loop
            // because we consider that a simple bitmap defined in a higher
            // priority provider should override a bitmap bundle defined in a
            // lower priority one: even if this means that the bitmap will be
            // scaled, at least we'll be using the expected bitmap rather than
            // potentially using a bitmap of a different style.
            if ( provider->CreateBitmap(id, client, size).IsOk() )
            {
                bitmapbundle = wxBitmapBundle::FromImpl(new wxBitmapBundleImplArt(id, client, size));
                break;
            }
        }

        sm_cache->PutBitmapBundle(hashId, bitmapbundle);
    }

    return bitmapbundle;
}

/*static*/
wxIconBundle wxArtProvider::GetIconBundle(const wxArtID& id, const wxArtClient& client)
{
    wxIconBundle iconbundle(DoGetIconBundle(id, client));

    if ( iconbundle.IsOk() )
    {
        return iconbundle;
    }
    else
    {
        // fall back to single-icon bundle
        return wxIconBundle(GetIcon(id, client));
    }
}

/*static*/
wxIconBundle wxArtProvider::DoGetIconBundle(const wxArtID& id, const wxArtClient& client)
{
    // safety-check against writing client,id,size instead of id,client,size:
    wxASSERT_MSG( client.Last() == wxT('C'), wxT("invalid 'client' parameter") );

    wxCHECK_MSG( sm_providers, wxNullIconBundle, wxT("no wxArtProvider exists") );

    wxString hashId = wxArtProviderCache::ConstructHashID(id, client);

    wxIconBundle iconbundle;
    if ( !sm_cache->GetIconBundle(hashId, &iconbundle) )
    {
        for (wxArtProvidersList::compatibility_iterator node = sm_providers->GetFirst();
             node; node = node->GetNext())
        {
            iconbundle = node->GetData()->CreateIconBundle(id, client);
            if ( iconbundle.IsOk() )
                break;
        }

        sm_cache->PutIconBundle(hashId, iconbundle);
    }

    return iconbundle;
}

/*static*/ wxIcon wxArtProvider::GetIcon(const wxArtID& id,
                                         const wxArtClient& client,
                                         const wxSize& size)
{
    wxBitmap bmp = GetBitmap(id, client, size);

    if ( !bmp.IsOk() )
        return wxNullIcon;

    wxIcon icon;
    icon.CopyFromBitmap(bmp);
    return icon;
}

/* static */
wxArtID wxArtProvider::GetMessageBoxIconId(int flags)
{
    switch ( flags & wxICON_MASK )
    {
        default:
            wxFAIL_MSG(wxT("incorrect message box icon flags"));
            wxFALLTHROUGH;

        case wxICON_ERROR:
            return wxART_ERROR;

        case wxICON_INFORMATION:
            return wxART_INFORMATION;

        case wxICON_WARNING:
            return wxART_WARNING;

        case wxICON_QUESTION:
            return wxART_QUESTION;
    }
}

#if WXWIN_COMPATIBILITY_3_0
/*static*/ wxSize wxArtProvider::GetSizeHint(const wxArtClient& client,
                                         bool platform_dependent)
{
    return platform_dependent ? GetNativeSizeHint(client) : GetSizeHint(client);
}
#endif // WXWIN_COMPATIBILITY_3_0

/*static*/ wxSize wxArtProvider::GetDIPSizeHint(const wxArtClient& client)
{
    wxArtProvidersList::compatibility_iterator node = sm_providers->GetFirst();
    if (node)
        return node->GetData()->DoGetSizeHint(client);

    return GetNativeDIPSizeHint(client);
}

/*static*/
wxSize wxArtProvider::GetSizeHint(const wxArtClient& client, wxWindow* win)
{
    return wxWindow::FromDIP(GetDIPSizeHint(client), win);
}

wxSize wxArtProvider::DoGetSizeHint(const wxArtClient& client)
{
    return GetNativeDIPSizeHint(client);
}

/*static*/
wxSize wxArtProvider::GetNativeSizeHint(const wxArtClient& client, wxWindow* win)
{
    return wxWindow::FromDIP(GetNativeDIPSizeHint(client), win);
}

#ifndef wxHAS_NATIVE_ART_PROVIDER_IMPL
/*static*/
wxSize wxArtProvider::GetNativeDIPSizeHint(const wxArtClient& WXUNUSED(client))
{
    // rather than returning some arbitrary value that doesn't make much
    // sense (as 2.8 used to do), tell the caller that we don't have a clue:
    return wxDefaultSize;
}

/*static*/
void wxArtProvider::InitNativeProvider()
{
}
#endif // !wxHAS_NATIVE_ART_PROVIDER_IMPL


/* static */
bool wxArtProvider::HasNativeProvider()
{
#ifdef __WXGTK20__
    return true;
#else
    return false;
#endif
}

// ----------------------------------------------------------------------------
// deprecated wxArtProvider methods
// ----------------------------------------------------------------------------

#if WXWIN_COMPATIBILITY_2_8
/* static */ void wxArtProvider::Insert(wxArtProvider *provider)
{
    PushBack(provider);
}
#endif // WXWIN_COMPATIBILITY_2_8

// ============================================================================
// wxArtProviderModule
// ============================================================================

class wxArtProviderModule: public wxModule
{
public:
    bool OnInit() wxOVERRIDE
    {
        // The order here is such that the native provider will be used first
        // and the standard one last as all these default providers add
        // themselves to the bottom of the stack.
        wxArtProvider::InitNativeProvider();
#if wxUSE_ARTPROVIDER_TANGO
        wxArtProvider::InitTangoProvider();
#endif // wxUSE_ARTPROVIDER_TANGO
#if wxUSE_ARTPROVIDER_STD
        wxArtProvider::InitStdProvider();
#endif // wxUSE_ARTPROVIDER_STD
        return true;
    }
    void OnExit() wxOVERRIDE
    {
        wxArtProvider::CleanUpProviders();
    }

    wxDECLARE_DYNAMIC_CLASS(wxArtProviderModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxArtProviderModule, wxModule);
