/////////////////////////////////////////////////////////////////////////////
// Name:        artprov.h
// Purpose:     wxArtProvider class
// Author:      Vaclav Slavik
// Modified by:
// Created:     18/03/2002
// RCS-ID:      $Id$
// Copyright:   (c) Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ARTPROV_H_
#define _WX_ARTPROV_H_

#ifdef __GNUG__
#pragma interface "artprov.h"
#endif

#include "wx/string.h"
#include "wx/bitmap.h"
#include "wx/icon.h"

class WXDLLEXPORT wxArtProvidersList;
class WXDLLEXPORT wxArtProviderCache;

// ----------------------------------------------------------------------------
// Types
// ----------------------------------------------------------------------------

typedef wxString wxArtClient;
typedef wxString wxArtID;

// ----------------------------------------------------------------------------
// Art clients
// ----------------------------------------------------------------------------

#define wxART_TOOLBAR                       _T("toolbar_C")
#define wxART_MENU                          _T("menu_C")
#define wxART_FRAME_ICON                    _T("frame_icon_C")

#define wxART_CMN_DIALOG                    _T("cmn_dialog_C")
#define wxART_HELP_BROWSER                  _T("help_browser_C")

#define wxART_OTHER                         _T("other_C")

// ----------------------------------------------------------------------------
// Art IDs
// ----------------------------------------------------------------------------

#define wxART_ADD_BOOKMARK             _T("add_bookmark")
#define wxART_DEL_BOOKMARK             _T("del_bookmark")
#define wxART_HELP_SIDE_PANEL          _T("help_side_panel")
#define wxART_HELP_SETTINGS            _T("help_settings")
#define wxART_HELP_BOOK                _T("help_book")
#define wxART_HELP_FOLDER              _T("help_folder")
#define wxART_HELP_PAGE                _T("help_page")
#define wxART_GO_BACK                  _T("go_back")
#define wxART_GO_FORWARD               _T("go_forward")
#define wxART_GO_UP                    _T("go_up")
#define wxART_GO_DOWN                  _T("go_down")
#define wxART_GO_TO_PARENT             _T("go_to_parent")
#define wxART_FILE_OPEN                _T("file_open")
#define wxART_PRINT                    _T("print")
#define wxART_HELP                     _T("help")
#define wxART_TIP                      _T("tip")

// ----------------------------------------------------------------------------
// wxArtProvider class
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxArtProvider : public wxObject
{
public:
    // Add new provider to the top of providers stack.
    static void PushProvider(wxArtProvider *provider);

    // Remove latest added provider and delete it.
    static bool PopProvider();

    // Remove provider. The provider must have been added previously!
    // The provider is _not_ deleted.
    static bool RemoveProvider(wxArtProvider *provider);
    
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

    // Destroy caches & all providers
    static void CleanUpProviders();

protected:
    // Derived classes must override this method to create requested 
    // art resource. This method is called only once per instance's
    // lifetime for each requested wxArtID.
    virtual wxBitmap CreateBitmap(const wxArtID& WXUNUSED(id),
                                  const wxArtClient& WXUNUSED(client),
                                  const wxSize& WXUNUSED(size))
    {
        wxFAIL_MSG(_T("pure virtual method wxArtProvider::CreateBitmap called!"));
        return wxNullBitmap;
    }

private:
    // list of providers:
    static wxArtProvidersList *sm_providers;
    // art resources cache (so that CreateXXX is not called that often):
    static wxArtProviderCache *sm_cache;

    DECLARE_ABSTRACT_CLASS(wxArtProvider)
};


#endif // _WX_ARTPROV_H_
