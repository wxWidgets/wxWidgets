///////////////////////////////////////////////////////////////////////////////
// Name:        tests/graphics/iconbundle.cpp
// Purpose:     wxIconBundle unit test
// Author:      Vadim Zeitlin
// Created:     2026-11-08
// Copyright:   (c) 2026 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#include "wx/iconbndl.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/bitmap.h"
    #include "wx/icon.h"
    #include "wx/settings.h"
#endif // WX_PRECOMP

#include "asserthelper.h"

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

namespace
{

wxIcon MakeIcon(int width, int height)
{
    wxIcon icon;
    icon.CopyFromBitmap(wxBitmap(width, height));
    return icon;
}

wxIcon MakeIcon(int size)
{
    return MakeIcon(size, size);
}

// Return the size of the icon returned by GetIcon() or wxDefaultSize if it
// didn't return any valid icon.
wxSize
GetIconSize(const wxIconBundle& bundle, const wxSize& size, int flags)
{
    const wxIcon icon = bundle.GetIcon(size, flags);
    return icon.IsOk() ? icon.GetSize() : wxDefaultSize;
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// tests
// ----------------------------------------------------------------------------

TEST_CASE("IconBundle::AddIcon", "[iconbundle]")
{
    wxIconBundle b;
    CHECK( b.IsEmpty() );
    CHECK( b.GetIconCount() == 0 );

    b.AddIcon(MakeIcon(16));
    b.AddIcon(MakeIcon(32));
    CHECK( !b.IsEmpty() );
    CHECK( b.GetIconCount() == 2 );
    CHECK( b.GetIconByIndex(0).GetSize() == wxSize(16, 16) );
    CHECK( b.GetIconByIndex(1).GetSize() == wxSize(32, 32) );

    // Adding an icon of the same size as an existing one replaces it.
    b.AddIcon(MakeIcon(16));
    CHECK( b.GetIconCount() == 2 );
}

TEST_CASE("IconBundle::GetIcon", "[iconbundle]")
{
    const int NONE = wxIconBundle::FALLBACK_NONE;
    const int LARGER = wxIconBundle::FALLBACK_NEAREST_LARGER;

    wxIconBundle b;

    SECTION("Empty")
    {
        CHECK( GetIconSize(b, wxSize(32, 32), NONE) == wxDefaultSize );
        CHECK( GetIconSize(b, wxSize(32, 32), LARGER) == wxDefaultSize );
    }

    SECTION("Exact")
    {
        b.AddIcon(MakeIcon(16));
        b.AddIcon(MakeIcon(32));
        b.AddIcon(MakeIcon(48));

        CHECK( GetIconSize(b, wxSize(32, 32), NONE) == wxSize(32, 32) );
        CHECK( GetIconSize(b, wxSize(32, 32), LARGER) == wxSize(32, 32) );
        CHECK( b.GetIcon(32, NONE).GetSize() == wxSize(32, 32) );
        CHECK( b.GetIconOfExactSize(wxSize(48, 48)).GetSize() == wxSize(48, 48) );

        CHECK( GetIconSize(b, wxSize(24, 24), NONE) == wxDefaultSize );
        CHECK( !b.GetIconOfExactSize(wxSize(24, 24)).IsOk() );
    }

    SECTION("NearestLarger")
    {
        b.AddIcon(MakeIcon(16));
        b.AddIcon(MakeIcon(64));
        b.AddIcon(MakeIcon(48));

        // The closest larger icon is preferred, even if there is a closer
        // smaller one.
        CHECK( GetIconSize(b, wxSize(20, 20), LARGER) == wxSize(48, 48) );
        CHECK( GetIconSize(b, wxSize(32, 32), LARGER) == wxSize(48, 48) );
        CHECK( GetIconSize(b, wxSize(56, 56), LARGER) == wxSize(64, 64) );
    }

    SECTION("NearestSmaller")
    {
        // If there is no larger icon, the closest smaller one must be
        // returned, independently of the order of the icons in the bundle.
        SECTION("Ascending")
        {
            b.AddIcon(MakeIcon(16));
            b.AddIcon(MakeIcon(24));
            b.AddIcon(MakeIcon(32));
        }

        SECTION("Descending")
        {
            b.AddIcon(MakeIcon(32));
            b.AddIcon(MakeIcon(24));
            b.AddIcon(MakeIcon(16));
        }

        SECTION("Mixed")
        {
            b.AddIcon(MakeIcon(24));
            b.AddIcon(MakeIcon(32));
            b.AddIcon(MakeIcon(16));
        }

        CHECK( GetIconSize(b, wxSize(48, 48), LARGER) == wxSize(32, 32) );
        CHECK( GetIconSize(b, wxSize(48, 48), NONE) == wxDefaultSize );
    }

    SECTION("NonSquare")
    {
        // An icon is only considered to be larger if both of its dimensions
        // are, so 64x16 icon is never chosen over 48x48 one here.
        b.AddIcon(MakeIcon(64, 16));
        b.AddIcon(MakeIcon(48));

        CHECK( GetIconSize(b, wxSize(32, 32), LARGER) == wxSize(48, 48) );

        // But it can be chosen if it's the closest to the requested size and
        // there is no larger icon.
        CHECK( GetIconSize(b, wxSize(56, 24), LARGER) == wxSize(64, 16) );
    }
}

TEST_CASE("IconBundle::GetIcon::System", "[iconbundle]")
{
    const int SYSTEM = wxIconBundle::FALLBACK_SYSTEM;
    const int LARGER = wxIconBundle::FALLBACK_NEAREST_LARGER;

    // Use the same window as GetIcon() itself does to get the system icon
    // size, as it may depend on the DPI.
    wxWindow* const win = wxApp::GetMainTopWindow();
    const wxSize sizeSys(wxSystemSettings::GetMetric(wxSYS_ICON_X, win),
                         wxSystemSettings::GetMetric(wxSYS_ICON_Y, win));
    if ( sizeSys.x <= 0 || sizeSys.y <= 0 )
    {
        WARN("Skipping test as system icon size is unknown.");
        return;
    }

    INFO("System icon size is " << sizeSys);

    wxIconBundle b;
    b.AddIcon(MakeIcon(2*sizeSys.x, 2*sizeSys.y));
    b.AddIcon(MakeIcon(sizeSys.x, sizeSys.y));

    const wxSize sizeOther = sizeSys + wxSize(1, 1);

    // Icon of the system size is returned if the requested size is not found,
    // even if there is a closer larger icon...
    CHECK( GetIconSize(b, sizeOther, SYSTEM) == sizeSys );
    CHECK( GetIconSize(b, sizeOther, SYSTEM | LARGER) == sizeSys );

    // ... which is returned if FALLBACK_SYSTEM is not specified.
    CHECK( GetIconSize(b, sizeOther, LARGER) == 2*sizeSys );

    // And exact match is still preferred to the icon of the system size.
    CHECK( GetIconSize(b, 2*sizeSys, SYSTEM) == 2*sizeSys );
}
