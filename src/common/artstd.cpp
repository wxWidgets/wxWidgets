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
    virtual wxBitmap CreateBitmap(const wxArtID& id, const wxArtClient& client,
                                  const wxSize& size);
};

#define ART(artId, xpmRc) \
    if ( id == artId ) return wxBitmap(xpmRc##_xpm);
    

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
    #include "../../art/htmsidep.xpm"
    #include "../../art/htmoptns.xpm"
    #include "../../art/htmbook.xpm"
    #include "../../art/htmfoldr.xpm"
    #include "../../art/htmpage.xpm"
#endif // wxUSE_HTML

#include "../../art/addbookm.xpm"
#include "../../art/delbookm.xpm"
#include "../../art/back.xpm"
#include "../../art/forward.xpm"
#include "../../art/up.xpm"
#include "../../art/down.xpm"
#include "../../art/toparent.xpm"
#include "../../art/fileopen.xpm"
#include "../../art/print.xpm"
#include "../../art/helpicon.xpm"
#include "../../art/tipicon.xpm"
#include "../../art/home.xpm"
#include "../../art/repview.xpm"
#include "../../art/listview.xpm"
#include "../../art/new_dir.xpm"
#include "../../art/folder.xpm"
#include "../../art/dir_up.xpm"
#include "../../art/exefile.xpm"
#include "../../art/deffile.xpm"

#undef static

// ----------------------------------------------------------------------------
// CreateBitmap routine
// ----------------------------------------------------------------------------

wxBitmap wxDefaultArtProvider::CreateBitmap(const wxArtID& id,
                                            const wxArtClient& client,
                                            const wxSize& size)
{
#if wxUSE_HTML
    ART(wxART_HELP_SIDE_PANEL,                     htmsidep)
    ART(wxART_HELP_SETTINGS,                       htmoptns)
    ART(wxART_HELP_BOOK,                           htmbook)
    ART(wxART_HELP_FOLDER,                         htmfoldr)
    ART(wxART_HELP_PAGE,                           htmpage)
#endif // wxUSE_HTML
    ART(wxART_ADD_BOOKMARK,                        addbookm)
    ART(wxART_DEL_BOOKMARK,                        delbookm)
    ART(wxART_GO_BACK,                             back)
    ART(wxART_GO_FORWARD,                          forward)
    ART(wxART_GO_UP,                               up)
    ART(wxART_GO_DOWN,                             down)
    ART(wxART_GO_TO_PARENT,                        toparent)
    ART(wxART_GO_HOME,                             home)
    ART(wxART_FILE_OPEN,                           fileopen)
    ART(wxART_PRINT,                               print)
    ART(wxART_HELP,                                helpicon)
    ART(wxART_TIP,                                 tipicon)
    ART(wxART_REPORT_VIEW,                         repview)
    ART(wxART_LIST_VIEW,                           listview)
    ART(wxART_NEW_DIR,                             new_dir)
    ART(wxART_FOLDER,                              folder)
    ART(wxART_GO_DIR_UP,                           dir_up)
    ART(wxART_EXECUTABLE_FILE,                     exefile)
    ART(wxART_NORMAL_FILE,                         deffile)

    return wxNullBitmap;
}
