/////////////////////////////////////////////////////////////////////////////
// Name:        helpview.h
// Purpose:     HelpView application
//              A standalone viewer for wxHTML Help (.htb) files
// Author:      Vaclav Slavik, Julian Smart
// Modified by:
// Created:     2002-07-09
// RCS-ID:      $Id$
// Copyright:   (c) 2002 Vaclav Slavik, Julian Smart and others
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "help.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/image.h"
#include "wx/wxhtml.h"
#include "wx/fs_zip.h"
#include "wx/log.h"
#include "wx/artprov.h"
#include "wx/filedlg.h"

#include "helpview.h"

class AlternateArtProvider : public wxArtProvider
{
protected:
    virtual wxBitmap CreateBitmap(const wxArtID& id, const wxArtClient& client,
                                  const wxSize& size);
};

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------


IMPLEMENT_APP(hvApp)


bool hvApp::OnInit()
{
#ifdef __WXMOTIF__
    delete wxLog::SetActiveTarget(new wxLogStderr); // So dialog boxes aren't used
#endif

    wxArtProvider::PushProvider(new AlternateArtProvider);
  
    wxInitAllImageHandlers();
    wxFileSystem::AddHandler(new wxZipFSHandler);

    SetVendorName("wxWindows");
    SetAppName("wxHTMLHelp"); 
    wxConfig::Get(); // create an instance

    help = new wxHtmlHelpController(
        wxHF_DEFAULT_STYLE|wxHF_FLAT_TOOLBAR|wxHF_OPEN_FILES
        );
    
    help->SetTitleFormat(wxT("%s"));
    if (argc < 2) {
        if (!OpenBook(help))
            return FALSE;
    }

    for (int i = 1; i < argc; i++)
        help -> AddBook(argv[i]);

#ifdef __WXMOTIF__
    delete wxLog::SetActiveTarget(new wxLogGui);
#endif

    help -> DisplayContents();

    return TRUE;
}


int hvApp::OnExit()
{
    delete help;
    delete wxConfig::Set(NULL);

    return 0;
}

bool hvApp::OpenBook(wxHtmlHelpController* controller)
{
    wxString s = wxFileSelector(_("Open help file"),
        wxGetCwd(),
        wxEmptyString,
        wxEmptyString,
        _(
"Help books (*.htb)|*.htb|Help books (*.zip)|*.zip|\
HTML Help Project (*.hhp)|*.hhp"),
    wxOPEN | wxFILE_MUST_EXIST,
    NULL);

    if (!s.IsEmpty())
    {
        wxString ext = s.Right(4).Lower();
        if (ext == _T(".zip") || ext == _T(".htb") || ext == _T(".hhp"))
        {
            wxBusyCursor bcur;
            controller->AddBook(s);
            return TRUE;
        }
    }
    return FALSE;
}

/*
 * Art provider class
 */

// ---------------------------------------------------------------------
// helper macros
// ---------------------------------------------------------------------

// Standard macro for getting a resource from XPM file:
#define ART(artId, xpmRc) \
    if ( id == artId ) return wxBitmap(xpmRc##_xpm);

// Compatibility hack to use wxApp::GetStdIcon of overriden by the user
#if WXWIN_COMPATIBILITY_2_2
    #define GET_STD_ICON_FROM_APP(iconId) \
        if ( client == wxART_MESSAGE_BOX ) \
        { \
            wxIcon icon = wxTheApp->GetStdIcon(iconId); \
            if ( icon.Ok() ) \
            { \
                wxBitmap bmp; \
                bmp.CopyFromIcon(icon); \
                return bmp; \
            } \
        }
#else
    #define GET_STD_ICON_FROM_APP(iconId)
#endif

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
        GET_STD_ICON_FROM_APP(iconId) \
        CREATE_STD_ICON(#iconId, xpmRc) \
    }

// ---------------------------------------------------------------------
// XPMs with the art
// ---------------------------------------------------------------------

// XPM hack: make the arrays const
//#define static static const

#include "bitmaps/helpback.xpm"
#include "bitmaps/helpbook.xpm"
#include "bitmaps/helpdown.xpm"
#include "bitmaps/helpforward.xpm"
#include "bitmaps/helpoptions.xpm"
#include "bitmaps/helppage.xpm"
#include "bitmaps/helpsidepanel.xpm"
#include "bitmaps/helpup.xpm"
#include "bitmaps/helpuplevel.xpm"
#include "bitmaps/helpicon.xpm"
#include "bitmaps/helpopen.xpm"

//#undef static

// ---------------------------------------------------------------------
// CreateBitmap routine
// ---------------------------------------------------------------------

wxBitmap AlternateArtProvider::CreateBitmap(const wxArtID& id,
                                            const wxArtClient& client,
                                            const wxSize& WXUNUSED(size))
{
    ART(wxART_HELP_SIDE_PANEL,                     helpsidepanel)
    ART(wxART_HELP_SETTINGS,                       helpoptions)
    ART(wxART_HELP_BOOK,                           helpbook)
    ART(wxART_HELP_FOLDER,                         helpbook)
    ART(wxART_HELP_PAGE,                           helppage)
    //ART(wxART_ADD_BOOKMARK,                        addbookm)
    //ART(wxART_DEL_BOOKMARK,                        delbookm)
    ART(wxART_GO_BACK,                             helpback)
    ART(wxART_GO_FORWARD,                          helpforward)
    ART(wxART_GO_UP,                               helpup)
    ART(wxART_GO_DOWN,                             helpdown)
    ART(wxART_GO_TO_PARENT,                        helpuplevel)
    ART(wxART_FILE_OPEN,                           helpopen)
    if (client == wxART_HELP_BROWSER)
    {
        ART(wxART_FRAME_ICON,                          helpicon)
    }

    //ART(wxART_GO_HOME,                             home)

    // Any wxWindows icons not implemented here
    // will be provided by the default art provider.
    return wxNullBitmap;
}
