/////////////////////////////////////////////////////////////////////////////
// Name:        wx/artprov.h
// Purpose:     wxArtProvider class
// Author:      Vaclav Slavik
// Modified by:
// Created:     18/03/2002
// Copyright:   (c) Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ARTPROV_H_
#define _WX_ARTPROV_H_

#include "wx/string.h"
#include "wx/bitmap.h"
#include "wx/icon.h"
#include "wx/iconbndl.h"

class WXDLLIMPEXP_FWD_CORE wxArtProvidersList;
class WXDLLIMPEXP_FWD_CORE wxArtProviderCache;
class wxArtProviderModule;

// ----------------------------------------------------------------------------
// Types
// ----------------------------------------------------------------------------

typedef wxString wxArtClient;
typedef wxString wxArtID;

#define wxART_MAKE_CLIENT_ID_FROM_STR(id)  ((id) + wxASCII_STR("_C"))
#define wxART_MAKE_ART_ID_FROM_STR(id)     (id)

#define wxART_MAKE_CLIENT_ID(id) \
  extern WXDLLIMPEXP_DATA_CORE(const wxArtClient) id;
#define wxART_MAKE_ART_ID(id) \
  extern WXDLLIMPEXP_DATA_CORE(const wxArtID) id;

#include "wx/artids.h"
#undef wxART_MAKE_ART_ID
#undef wxART_MAKE_CLIENT_ID

// ----------------------------------------------------------------------------
// wxArtProvider class
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxArtProvider : public wxObject
{
public:
    // Dtor removes the provider from providers stack if it's still on it
    virtual ~wxArtProvider();

    // Does this platform implement native icons theme?
    static bool HasNativeProvider();

    // Add new provider to the top of providers stack (i.e. the provider will
    // be queried first of all).
    static void Push(wxArtProvider *provider);

    // Add new provider to the bottom of providers stack (i.e. the provider
    // will be queried as the last one).
    static void PushBack(wxArtProvider *provider);

#if WXWIN_COMPATIBILITY_2_8
    // use PushBack(), it's the same thing
    static wxDEPRECATED( void Insert(wxArtProvider *provider) );
#endif

    // Remove latest added provider and delete it.
    static bool Pop();

    // Remove provider from providers stack but don't delete it.
    static bool Remove(wxArtProvider *provider);

    // Delete the given provider and remove it from the providers stack.
    static bool Delete(wxArtProvider *provider);


    // Query the providers for bitmap with given ID and return it. Return
    // wxNullBitmap if no provider provides it.
    static wxBitmap GetBitmap(const wxArtID& id,
                              const wxArtClient& client = wxART_OTHER,
                              const wxSize& size = wxDefaultSize);

    // Query the providers for icon with given ID and return it. Return
    // wxNullIcon if no provider provides it.
    static wxIcon GetIcon(const wxArtID& id,
                          const wxArtClient& client = wxART_OTHER,
                          const wxSize& size = wxDefaultSize);

    // Helper used by GetMessageBoxIcon(): return the art id corresponding to
    // the standard wxICON_INFORMATION/WARNING/ERROR/QUESTION flags (only one
    // can be set)
    static wxArtID GetMessageBoxIconId(int flags);

    // Helper used by several generic classes: return the icon corresponding to
    // the standard wxICON_INFORMATION/WARNING/ERROR/QUESTION flags (only one
    // can be set)
    static wxIcon GetMessageBoxIcon(int flags)
    {
        return GetIcon(GetMessageBoxIconId(flags), wxART_MESSAGE_BOX);
    }

    // Query the providers for iconbundle with given ID and return it. Return
    // wxNullIconBundle if no provider provides it.
    static wxIconBundle GetIconBundle(const wxArtID& id,
                                      const wxArtClient& client = wxART_OTHER);

    // Gets native size for given 'client' or wxDefaultSize if it doesn't
    // have native equivalent
    static wxSize GetNativeSizeHint(const wxArtClient& client);

    // Get the size hint of an icon from a specific wxArtClient, queries
    // the topmost provider if platform_dependent = false
    static wxSize GetSizeHint(const wxArtClient& client, bool platform_dependent = false);

    // Rescale bitmap (used internally if requested size is other than the available).
    static void RescaleBitmap(wxBitmap& bmp, const wxSize& sizeNeeded);

protected:
    friend class wxArtProviderModule;
#if wxUSE_ARTPROVIDER_STD
    // Initializes default provider
    static void InitStdProvider();
#endif // wxUSE_ARTPROVIDER_STD
    // Initializes Tango-based icon provider
#if wxUSE_ARTPROVIDER_TANGO
    static void InitTangoProvider();
#endif // wxUSE_ARTPROVIDER_TANGO
    // Initializes platform's native provider, if available (e.g. GTK2)
    static void InitNativeProvider();
    // Destroy caches & all providers
    static void CleanUpProviders();

    // Get the default size of an icon for a specific client
    virtual wxSize DoGetSizeHint(const wxArtClient& client)
    {
        return GetSizeHint(client, true);
    }

    // Derived classes must override CreateBitmap or CreateIconBundle
    // (or both) to create requested art resource. This method is called
    // only once per instance's lifetime for each requested wxArtID.
    virtual wxBitmap CreateBitmap(const wxArtID& WXUNUSED(id),
                                  const wxArtClient& WXUNUSED(client),
                                  const wxSize& WXUNUSED(size))
    {
        return wxNullBitmap;
    }

    virtual wxIconBundle CreateIconBundle(const wxArtID& WXUNUSED(id),
                                          const wxArtClient& WXUNUSED(client))
    {
        return wxNullIconBundle;
    }

private:
    static void CommonAddingProvider();
    static wxIconBundle DoGetIconBundle(const wxArtID& id,
                                        const wxArtClient& client);

private:
    // list of providers:
    static wxArtProvidersList *sm_providers;
    // art resources cache (so that CreateXXX is not called that often):
    static wxArtProviderCache *sm_cache;

    wxDECLARE_ABSTRACT_CLASS(wxArtProvider);
};


#if !defined(__WXUNIVERSAL__) && \
    ((defined(__WXGTK__) && defined(__WXGTK20__)) || defined(__WXMSW__) || \
     defined(__WXMAC__))
  // *some* (partial) native implementation of wxArtProvider exists; this is
  // not the same as wxArtProvider::HasNativeProvider()!
  #define wxHAS_NATIVE_ART_PROVIDER_IMPL
#endif

#endif // _WX_ARTPROV_H_
