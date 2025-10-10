/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/artmac.cpp
// Purpose:     wxArtProvider instance with native Mac stock icons
// Author:      Alan Shouls
// Created:     2006-10-30
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/artprov.h"

#ifndef WX_PRECOMP
    #include "wx/image.h"
#endif

#include "wx/osx/private.h"

// ----------------------------------------------------------------------------
// wxMacArtProvider
// ----------------------------------------------------------------------------

class wxMacArtProvider : public wxArtProvider
{
protected:
#if wxOSX_USE_COCOA_OR_CARBON
    virtual wxIconBundle CreateIconBundle(const wxArtID& id,
                                          const wxArtClient& client) override;
#endif
    virtual wxBitmapBundle CreateBitmapBundle(const wxArtID& id,
                                  const wxArtClient& client,
                                  const wxSize& size) override;
};

/* static */ void wxArtProvider::InitNativeProvider()
{
    PushBack(new wxMacArtProvider);
}

#if wxOSX_USE_COCOA_OR_CARBON

// under cocoa we still have access to the old native icons, so map them

// ----------------------------------------------------------------------------
// helper macros
// ----------------------------------------------------------------------------

#define CREATE_STD_ICON(iconId) \
    { \
        wxIconBundle icon(wxT(iconId), wxBITMAP_TYPE_ICON_RESOURCE); \
        return icon; \
    }

// Macro used in CreateBitmap to get wxICON_FOO icons:
#define ART_MSGBOX(artId, iconId) \
    if ( id == artId ) \
    { \
        CREATE_STD_ICON(#iconId) \
    }

// ----------------------------------------------------------------------------
// CreateIconBundle
// ----------------------------------------------------------------------------

static wxIconBundle wxMacArtProvider_CreateIconBundle(const wxArtID& id)
{
    ART_MSGBOX(wxART_ERROR,       wxICON_ERROR)
    ART_MSGBOX(wxART_INFORMATION, wxICON_INFORMATION)
    ART_MSGBOX(wxART_WARNING,     wxICON_WARNING)
    ART_MSGBOX(wxART_QUESTION,    wxICON_QUESTION)

    ART_MSGBOX(wxART_FOLDER,      wxICON_FOLDER)
    ART_MSGBOX(wxART_FOLDER_OPEN, wxICON_FOLDER_OPEN)
    ART_MSGBOX(wxART_NORMAL_FILE, wxICON_NORMAL_FILE)
    ART_MSGBOX(wxART_EXECUTABLE_FILE, wxICON_EXECUTABLE_FILE)

    ART_MSGBOX(wxART_CDROM,       wxICON_CDROM)
    ART_MSGBOX(wxART_FLOPPY,      wxICON_FLOPPY)
    ART_MSGBOX(wxART_HARDDISK,    wxICON_HARDDISK)
    ART_MSGBOX(wxART_REMOVABLE,   wxICON_REMOVABLE)
    ART_MSGBOX(wxART_PRINT,       wxICON_PRINT)

    ART_MSGBOX(wxART_DELETE,      wxICON_DELETE)

    ART_MSGBOX(wxART_GO_BACK,     wxICON_GO_BACK)
    ART_MSGBOX(wxART_GO_FORWARD,  wxICON_GO_FORWARD)
    ART_MSGBOX(wxART_GO_HOME,     wxICON_GO_HOME)

    ART_MSGBOX(wxART_HELP_SETTINGS, wxICON_HELP_SETTINGS)
    ART_MSGBOX(wxART_HELP_PAGE,   wxICON_HELP_PAGE)
    ART_MSGBOX(wxART_HELP_FOLDER,   wxICON_HELP_FOLDER)

    return wxNullIconBundle;
}


wxIconBundle wxMacArtProvider::CreateIconBundle(const wxArtID& id, const wxArtClient& client)
{
    // On the Mac folders in lists are always drawn closed, so if an open
    // folder icon is asked for we will ask for a closed one in its place
    if ( client == wxART_LIST && id == wxART_FOLDER_OPEN )
        return wxMacArtProvider_CreateIconBundle(wxART_FOLDER);

    return wxMacArtProvider_CreateIconBundle(id);
}

#endif

// ----------------------------------------------------------------------------
// CreateBitmapBundle
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// helper macros
// ----------------------------------------------------------------------------

#define ART_BITMAP(artId, symbolname) \
    if ( id == artId ) \
    { \
        return wxOSXCreateSystemBitmapBundle(symbolname, size); \
    }

static wxBitmapBundle wxMacArtProvider_CreateBitmapBundle(const wxArtID& id, const wxSize& size)
{
    ART_BITMAP(wxART_ERROR,         "xmark.circle" )
    ART_BITMAP(wxART_INFORMATION,   "info.circle" )
    ART_BITMAP(wxART_WARNING,       "exclamationmark.triangle" )
    ART_BITMAP(wxART_QUESTION,      "questionmark.circle" )

    ART_BITMAP(wxART_FOLDER,         "folder")
    ART_BITMAP(wxART_FOLDER_OPEN,    "folder")
    ART_BITMAP(wxART_NORMAL_FILE,    "document")
    ART_BITMAP(wxART_EXECUTABLE_FILE,"app")

    ART_BITMAP(wxART_CDROM,          "opticalsdrive")
    ART_BITMAP(wxART_FLOPPY,         "opticalsdrive")
    ART_BITMAP(wxART_HARDDISK,       "internaldrive")
    ART_BITMAP(wxART_REMOVABLE,      "externaldrive")
    ART_BITMAP(wxART_PRINT,          "printer")

    ART_BITMAP(wxART_DELETE,         "minus.circle")

    ART_BITMAP(wxART_GO_BACK,        "arrow.backward.circle")
    ART_BITMAP(wxART_GO_FORWARD,     "arrow.fordward.circle")
    ART_BITMAP(wxART_GO_HOME,        "house")

    ART_BITMAP(wxART_HELP_SETTINGS,  "gear.badge.questionmark")
    ART_BITMAP(wxART_HELP_PAGE,      "questionmark.text.page")
    ART_BITMAP(wxART_HELP_FOLDER,    "questionmark.folder")

    return wxBitmapBundle();
}

wxBitmapBundle wxMacArtProvider::CreateBitmapBundle(const wxArtID& id, const wxArtClient& client, const wxSize& size )
{
    // On the Mac folders in lists are always drawn closed, so if an open
    // folder icon is asked for we will ask for a closed one in its place
    if ( client == wxART_LIST && id == wxART_FOLDER_OPEN )
        return wxMacArtProvider_CreateBitmapBundle(wxART_FOLDER, size);

    return wxMacArtProvider_CreateBitmapBundle(id, size);
}

// ----------------------------------------------------------------------------
// wxArtProvider::GetNativeDIPSizeHint()
// ----------------------------------------------------------------------------

/*static*/
wxSize wxArtProvider::GetNativeDIPSizeHint(const wxArtClient& client)
{
    if ( client == wxART_TOOLBAR )
    {
        // See http://developer.apple.com/documentation/UserExperience/Conceptual/AppleHIGuidelines/XHIGIcons/chapter_15_section_9.html:
        // "32 x 32 pixels is the recommended size"
        return wxSize(32, 32);
    }
    else if ( client == wxART_BUTTON || client == wxART_MENU )
    {
        // Mac UI doesn't use any images in either buttons or menus in
        // general but the code using wxArtProvider can use wxART_BUTTON to
        // find the icons of a roughly appropriate size for the buttons and
        // 16x16 seems to be the best choice for this kind of use
        return wxSize(16, 16);
    }

    return wxDefaultSize;
}

