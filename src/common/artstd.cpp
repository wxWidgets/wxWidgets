/////////////////////////////////////////////////////////////////////////////
// Name:        artstd.cpp
// Purpose:     stock wxArtProvider instance with default wxWin art
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

#include "wx/artprov.h"
#include "wx/module.h"

// ----------------------------------------------------------------------------
// wxDefaultArtProvider
// ----------------------------------------------------------------------------

class wxDefaultArtProvider : public wxArtProvider
{
protected:
    virtual wxBitmap CreateBitmap(const wxArtDomain& domain, 
                                  const wxArtID& id, const wxSize& size);
};

#define BEGIN_DOMAIN(domainId) if ( domain == domainId ) {
#define END_DOMAIN()           }
#define ART_ID(artId, xpmRc)   if ( id == artId ) return wxBitmap(xpmRc##_xpm);

// ----------------------------------------------------------------------------
// wxDefaultArtProviderModule
// ----------------------------------------------------------------------------

class wxDefaultArtProviderModule: public wxModule
{
public:
    bool OnInit() 
    {
        wxArtProvider::PushProvider(new wxDefaultArtProvider);
        return TRUE; 
    }
    void OnExit() {}

    DECLARE_DYNAMIC_CLASS(wxDefaultArtProviderModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxDefaultArtProviderModule, wxModule)


// ----------------------------------------------------------------------------
// XPMs with the art
// ----------------------------------------------------------------------------

// XPM hack: make the arrays const
#define static static const

#if wxUSE_HTML
#include "../../art/wxhtml/addbookm.xpm"
#include "../../art/wxhtml/delbookm.xpm"
#include "../../art/wxhtml/navig.xpm"
#include "../../art/wxhtml/settings.xpm"
#include "../../art/wxhtml/book.xpm"
#include "../../art/wxhtml/folder.xpm"
#include "../../art/wxhtml/page.xpm"
#endif // wxUSE_HTML

#include "../../art/browser/back.xpm"
#include "../../art/browser/forward.xpm"
#include "../../art/browser/up.xpm"
#include "../../art/browser/down.xpm"
#include "../../art/browser/toparent.xpm"

#include "../../art/toolbar/fileopen.xpm"
#include "../../art/toolbar/print.xpm"

#include "../../art/framicon/help.xpm"

#undef static

// ----------------------------------------------------------------------------
// CreateBitmap routine
// ----------------------------------------------------------------------------

wxBitmap wxDefaultArtProvider::CreateBitmap(const wxArtDomain& domain, 
                                            const wxArtID& id,
                                            const wxSize& size)
{
#if wxUSE_HTML
    BEGIN_DOMAIN(wxART_WXHTML)
        ART_ID(wxART_ADD_BOOKMARK,     addbookm)
        ART_ID(wxART_DEL_BOOKMARK,     delbookm)
        ART_ID(wxART_NAVIG_PANEL,      navig)
        ART_ID(wxART_HELP_SETTINGS,    settings)
        ART_ID(wxART_HELP_BOOK,        book)
        ART_ID(wxART_HELP_FOLDER,      folder)
        ART_ID(wxART_HELP_PAGE,        page)
    END_DOMAIN()
#endif // wxUSE_HTML

    BEGIN_DOMAIN(wxART_BROWSER_TOOLBAR)
        ART_ID(wxART_GO_BACK,          back)
        ART_ID(wxART_GO_FORWARD,       forward)
        ART_ID(wxART_GO_UP,            up)
        ART_ID(wxART_GO_DOWN,          down)
        ART_ID(wxART_GO_TO_PARENT,     toparent)
    END_DOMAIN()

    BEGIN_DOMAIN(wxART_TOOLBAR)
        ART_ID(wxART_FILE_OPEN,        fileopen)
        ART_ID(wxART_PRINT,            print)
    END_DOMAIN()

    BEGIN_DOMAIN(wxART_FRAME_ICON)
        ART_ID(wxART_HELP,             help)
    END_DOMAIN()

    return wxNullBitmap;
}
