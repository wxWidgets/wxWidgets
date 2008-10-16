/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/artmsw.cpp
// Purpose:     stock wxArtProvider instance with native MSW stock icons
// Author:      Vaclav Slavik
// Modified by:
// Created:     2008-10-15
// RCS-ID:      $Id$
// Copyright:   (c) Vaclav Slavik, 2008
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
#include "wx/msw/wrapwin.h"


// ----------------------------------------------------------------------------
// wxWindowsArtProvider
// ----------------------------------------------------------------------------

class wxWindowsArtProvider : public wxArtProvider
{
protected:
    virtual wxBitmap CreateBitmap(const wxArtID& id, const wxArtClient& client,
                                  const wxSize& size);
};

static wxBitmap CreateFromStdIcon(const char *iconName)
{
    wxIcon icon(iconName);
    wxBitmap bmp;
    bmp.CopyFromIcon(icon);
    return bmp;
}

wxBitmap wxWindowsArtProvider::CreateBitmap(const wxArtID& id,
                                            const wxArtClient& WXUNUSED(client),
                                            const wxSize& WXUNUSED(size))
{
    // handle message box icons specially (wxIcon ctor treat these names
    // as special cases via wxICOResourceHandler::LoadIcon):
    if ( id == wxART_ERROR )
        return CreateFromStdIcon("wxICON_ERROR");
    else if ( id == wxART_INFORMATION )
        return CreateFromStdIcon("wxICON_INFORMATION");
    else if ( id == wxART_WARNING )
        return CreateFromStdIcon("wxICON_WARNING");
    else if ( id == wxART_QUESTION )
        return CreateFromStdIcon("wxICON_QUESTION");

    // for anything else, fall back to generic provider:
    return wxNullBitmap;
}

// ----------------------------------------------------------------------------
// wxArtProvider::InitNativeProvider()
// ----------------------------------------------------------------------------

/*static*/ void wxArtProvider::InitNativeProvider()
{
    Push(new wxWindowsArtProvider);
}

// ----------------------------------------------------------------------------
// wxArtProvider::GetNativeSizeHint()
// ----------------------------------------------------------------------------

/*static*/
wxSize wxArtProvider::GetNativeSizeHint(const wxArtClient& client)
{
    if ( client == wxART_TOOLBAR )
    {
        return wxSize(24, 24);
    }
    else if ( client == wxART_MENU )
    {
        return wxSize(16, 16);
    }
    else if ( client == wxART_FRAME_ICON )
    {
        return wxSize(::GetSystemMetrics(SM_CXSMICON),
                      ::GetSystemMetrics(SM_CYSMICON));
    }
    else if ( client == wxART_CMN_DIALOG ||
              client == wxART_MESSAGE_BOX )
    {
        return wxSize(::GetSystemMetrics(SM_CXICON),
                      ::GetSystemMetrics(SM_CYICON));
    }
    else if (client == wxART_BUTTON)
    {
        return wxSize(16, 16);
    }

    return wxDefaultSize;
}
