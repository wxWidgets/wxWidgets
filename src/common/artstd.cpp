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

#ifndef WX_PRECOMP
    #if WXWIN_COMPATIBILITY_2_2
        #include "wx/app.h"
    #endif
#endif

#include "wx/artprov.h"

// ----------------------------------------------------------------------------
// wxDefaultArtProvider
// ----------------------------------------------------------------------------

class wxDefaultArtProvider : public wxArtProvider
{
protected:
    virtual wxBitmap CreateBitmap(const wxArtID& id, const wxArtClient& client,
                                  const wxSize& size);
};

// ----------------------------------------------------------------------------
// helper macros
// ----------------------------------------------------------------------------

// Standard macro for getting a resource from XPM file:
#define ART(artId, xpmRc) \
    if ( id == artId ) return wxBitmap(xpmRc##_xpm);

// There are two ways of getting the standard icon: either via XPMs or via
// wxIcon ctor. This depends on the platform:
#if defined(__WXUNIVERSAL__)
    #define CREATE_STD_ICON(iconId, xpmRc) return wxNullBitmap;
#elif defined(__WXGTK__) || defined(__WXMOTIF__)
    #define CREATE_STD_ICON(iconId, xpmRc) return wxBitmap(xpmRc##_xpm);
#else
    #define CREATE_STD_ICON(iconId, xpmRc) \
        { \
            wxIcon icon(_T(iconId)); \
            wxBitmap bmp; \
            bmp.CopyFromIcon(icon); \
            return bmp; \
        }
#endif

// Macro used in CreateBitmap to get wxICON_FOO icons:
#define ART_MSGBOX(artId, iconId, xpmRc) \
    if ( id == artId ) \
    { \
        CREATE_STD_ICON(#iconId, xpmRc) \
    }

// ----------------------------------------------------------------------------
// wxArtProvider::InitStdProvider
// ----------------------------------------------------------------------------

/*static*/ void wxArtProvider::InitStdProvider()
{
    wxArtProvider::PushProvider(new wxDefaultArtProvider);
}

#if !defined(__WXGTK20__) || defined(__WXUNIVERSAL__)
/*static*/ void wxArtProvider::InitNativeProvider()
{
}
#endif


// ----------------------------------------------------------------------------
// XPMs with the art
// ----------------------------------------------------------------------------

// XPM hack: make the arrays const
#define static static const

#if defined(__WXGTK__)
    #include "../../art/gtk/info.xpm"
    #include "../../art/gtk/error.xpm"
    #include "../../art/gtk/warning.xpm"
    #include "../../art/gtk/question.xpm"
#elif defined(__WXMOTIF__)
    #include "../../art/motif/info.xpm"
    #include "../../art/motif/error.xpm"
    #include "../../art/motif/warning.xpm"
    #include "../../art/motif/question.xpm"
#endif

#if wxUSE_HTML
    #include "../../art/htmsidep.xpm"
    #include "../../art/htmoptns.xpm"
    #include "../../art/htmbook.xpm"
    #include "../../art/htmfoldr.xpm"
    #include "../../art/htmpage.xpm"
#endif // wxUSE_HTML

#include "../../art/missimg.xpm"
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
#include "../../art/tick.xpm"
#include "../../art/cross.xpm"

#undef static

// ----------------------------------------------------------------------------
// CreateBitmap routine
// ----------------------------------------------------------------------------

wxBitmap wxDefaultArtProvider::CreateBitmap(const wxArtID& id,
                                            const wxArtClient& WXUNUSED(client),
                                            const wxSize& WXUNUSED(size))
{
    // wxMessageBox icons:
    ART_MSGBOX(wxART_ERROR,       wxICON_ERROR,       error)
    ART_MSGBOX(wxART_INFORMATION, wxICON_INFORMATION, info)
    ART_MSGBOX(wxART_WARNING,     wxICON_WARNING,     warning)
    ART_MSGBOX(wxART_QUESTION,    wxICON_QUESTION,    question)

    // standard icons:
#if wxUSE_HTML
    ART(wxART_HELP_SIDE_PANEL,                     htmsidep)
    ART(wxART_HELP_SETTINGS,                       htmoptns)
    ART(wxART_HELP_BOOK,                           htmbook)
    ART(wxART_HELP_FOLDER,                         htmfoldr)
    ART(wxART_HELP_PAGE,                           htmpage)
#endif // wxUSE_HTML
    ART(wxART_MISSING_IMAGE,                       missimg)
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
    ART(wxART_TICK_MARK,                           tick)
    ART(wxART_CROSS_MARK,                          cross)

    return wxNullBitmap;
}
