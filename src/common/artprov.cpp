/////////////////////////////////////////////////////////////////////////////
// Name:        artprov.cpp
// Purpose:     wxArtProvider class
// Author:      Vaclav Slavik
// Modified by:
// Created:     18/03/2002
// RCS-ID:      $Id$
// Copyright:   (c) Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "artprov.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/list.h"
#endif

#include "wx/artprov.h"
#include "wx/hashmap.h"
#include "wx/module.h"


// ===========================================================================
// implementation
// ===========================================================================

#include "wx/listimpl.cpp"
WX_DECLARE_LIST(wxArtProvider, wxArtProvidersList);
WX_DEFINE_LIST(wxArtProvidersList);

// ----------------------------------------------------------------------------
// Cache class - stores already requested bitmaps
// ----------------------------------------------------------------------------

WX_DECLARE_STRING_HASH_MAP(wxBitmap, wxArtProviderBitmapsHash);

class WXDLLEXPORT wxArtProviderCache
{
public:
    bool GetBitmap(const wxString& full_id, wxBitmap* bmp);
    void PutBitmap(const wxString& full_id, const wxBitmap& bmp)
        { m_bitmapsHash[full_id] = bmp; }

    void Clear();
    
    static wxString ConstructHashID(const wxArtDomain& domain, 
                                    const wxArtID& id,
                                    const wxSize& size);

private:
    wxArtProviderBitmapsHash m_bitmapsHash;
};

bool wxArtProviderCache::GetBitmap(const wxString& full_id, wxBitmap* bmp)
{
    wxArtProviderBitmapsHash::iterator entry = m_bitmapsHash.find(full_id);
    if ( entry == m_bitmapsHash.end() )
    {
        return FALSE;
    }
    else
    {
        *bmp = entry->second;
        return TRUE;
    }
}

void wxArtProviderCache::Clear()
{
    m_bitmapsHash.clear();
}

/*static*/ wxString wxArtProviderCache::ConstructHashID(
                                    const wxArtDomain& domain, 
                                    const wxArtID& id, const wxSize& size)
{
    wxString str;
    str.Printf(wxT("%s-%s-%i-%i"), domain.c_str(), id.c_str(), size.x, size.y);
    return str;
}


// ----------------------------------------------------------------------------
// wxArtProvider class
// ----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxArtProvider, wxObject)

wxArtProvidersList *wxArtProvider::sm_providers = NULL;
wxArtProviderCache *wxArtProvider::sm_cache = NULL;

/*static*/ void wxArtProvider::PushProvider(wxArtProvider *provider)
{
    if ( !sm_providers )
    {
        sm_providers = new wxArtProvidersList;
        sm_providers->DeleteContents(TRUE);
        sm_cache = new wxArtProviderCache;
    }

    sm_providers->Insert(provider);
}

/*static*/ bool wxArtProvider::PopProvider()
{
    wxCHECK_MSG( sm_providers, FALSE, _T("no wxArtProvider exists") );
    wxCHECK_MSG( sm_providers->GetCount() > 0, FALSE, _T("wxArtProviders stack is empty") );

    sm_providers->DeleteNode(sm_providers->GetFirst());
    sm_cache->Clear();
    return TRUE;
}

/*static*/ bool wxArtProvider::RemoveProvider(wxArtProvider *provider)
{
    wxCHECK_MSG( sm_providers, FALSE, _T("no wxArtProvider exists") );

    if ( sm_providers->DeleteObject(provider) )
    {
        sm_cache->Clear();
        return TRUE;
    }
    
    return FALSE;
}

/*static*/ void wxArtProvider::CleanUpProviders()
{
    wxDELETE(sm_providers);
    wxDELETE(sm_cache);
}

/*static*/ wxBitmap wxArtProvider::GetBitmap(const wxArtDomain& domain,
                                             const wxArtID& id,
                                             const wxSize& size)
{
    wxCHECK_MSG( sm_providers, wxNullBitmap, _T("no wxArtProvider exists") );

    wxString hashId = wxArtProviderCache::ConstructHashID(domain, id, size);

    wxBitmap bmp;
    if ( !sm_cache->GetBitmap(hashId, &bmp) )
    {
        for (wxArtProvidersList::Node *node = sm_providers->GetFirst();
             node; node = node->GetNext())
        {
            bmp = node->GetData()->CreateBitmap(domain, id, size);
            if ( bmp.Ok() )
                break;
        }
        sm_cache->PutBitmap(hashId, bmp);
    }

    return bmp;
}

/*static*/ wxIcon wxArtProvider::GetIcon(const wxArtDomain& domain, 
                                         const wxArtID& id,
                                         const wxSize& size)
{
    wxCHECK_MSG( sm_providers, wxNullIcon, _T("no wxArtProvider exists") );

    wxBitmap bmp = GetBitmap(domain, id, size);
    if ( bmp.Ok() )
    {
        wxIcon icon;
        icon.CopyFromBitmap(bmp);
        return icon;
    }
    else
    {
        return wxNullIcon;
    }
}



class wxArtProviderModule: public wxModule
{
public:
    bool OnInit() { return TRUE; }
    void OnExit() { wxArtProvider::CleanUpProviders(); }

    DECLARE_DYNAMIC_CLASS(wxArtProviderModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxArtProviderModule, wxModule)
