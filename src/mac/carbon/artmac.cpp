/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/artmac.cpp
// Purpose:     wxArtProvider instance with native Mac stock icons
// Author:      Alan Shouls
// Created:     2006-10-30
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
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
    #include "wx/image.h"
#endif

#include "wx/artprov.h"
#include "wx/image.h"

// ----------------------------------------------------------------------------
// wxMacArtProvider
// ----------------------------------------------------------------------------

class wxMacArtProvider : public wxArtProvider
{
protected:
    virtual wxBitmap CreateBitmap(const wxArtID& id, const wxArtClient& client,
                                  const wxSize& size);
    virtual wxIconBundle CreateIconBundle(const wxArtID& id,
                                          const wxArtClient& client);
};

/* static */ void wxArtProvider::InitNativeProvider()
{
    wxArtProvider::Push(new wxMacArtProvider);
}

// ----------------------------------------------------------------------------
// helper macros
// ----------------------------------------------------------------------------

#define CREATE_STD_ICON(iconId, xpmRc) \
    { \
        wxIconBundle icon(_T(iconId), wxBITMAP_TYPE_ICON_RESOURCE); \
        return icon; \
    }

// Macro used in CreateBitmap to get wxICON_FOO icons:
#define ART_MSGBOX(artId, iconId, xpmRc) \
    if ( id == artId ) \
    { \
        CREATE_STD_ICON(#iconId, xpmRc) \
    }

static wxIconBundle wxMacArtProvider_CreateIconBundle(const wxArtID& id)
{
    ART_MSGBOX(wxART_ERROR,       wxICON_ERROR,       error)
    ART_MSGBOX(wxART_INFORMATION, wxICON_INFORMATION, info)
    ART_MSGBOX(wxART_WARNING,     wxICON_WARNING,     warning)
    ART_MSGBOX(wxART_QUESTION,    wxICON_QUESTION,    question)

    ART_MSGBOX(wxART_FOLDER,      wxICON_FOLDER,      folder)
    ART_MSGBOX(wxART_FOLDER_OPEN, wxICON_FOLDER_OPEN, folder_open)
    ART_MSGBOX(wxART_NORMAL_FILE, wxICON_NORMAL_FILE, deffile)

    return wxNullIconBundle;
}

// ----------------------------------------------------------------------------
// CreateIconBundle
// ----------------------------------------------------------------------------

wxIconBundle wxMacArtProvider::CreateIconBundle(const wxArtID& id, const wxArtClient& client)
{
    // On the Mac folders in lists are always drawn closed, so if an open
    // folder icon is asked for we will ask for a closed one in its place
    if ( client == wxART_LIST && id == wxART_FOLDER_OPEN )
        return wxMacArtProvider_CreateIconBundle(wxART_FOLDER);

    return wxMacArtProvider_CreateIconBundle(id);
}

// ----------------------------------------------------------------------------
// CreateBitmap
// ----------------------------------------------------------------------------

wxBitmap wxMacArtProvider::CreateBitmap(const wxArtID& id,
                                        const wxArtClient& client,
                                        const wxSize& reqSize)
{
    wxIconBundle ic(CreateIconBundle(id, client));
    if (ic.IsOk())
    {
        wxIcon theIcon(ic.GetIcon(reqSize));
        return wxBitmap(theIcon);
    }

    return wxNullBitmap;
}
