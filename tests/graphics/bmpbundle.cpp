///////////////////////////////////////////////////////////////////////////////
// Name:        tests/graphics/bmpbundle.cpp
// Purpose:     wxBitmapBundle unit test
// Author:      Vadim Zeitlin
// Created:     2021-09-27
// Copyright:   (c) 2021 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#include "wx/bmpbndl.h"

#include "asserthelper.h"

// ----------------------------------------------------------------------------
// tests
// ----------------------------------------------------------------------------

TEST_CASE("BitmapBundle::Create", "[bmpbundle]")
{
    wxBitmapBundle b;
    CHECK( !b.IsOk() );
    CHECK( b.GetDefaultSize() == wxDefaultSize );

    b = wxBitmap(16, 16);
    CHECK( b.IsOk() );
    CHECK( b.GetDefaultSize() == wxSize(16, 16) );
}

TEST_CASE("BitmapBundle::FromBitmaps", "[bmpbundle]")
{
    wxVector<wxBitmap> bitmaps;
    bitmaps.push_back(wxBitmap(16, 16));
    bitmaps.push_back(wxBitmap(24, 24));

    wxBitmapBundle b = wxBitmapBundle::FromBitmaps(bitmaps);
    REQUIRE( b.IsOk() );
    CHECK( b.GetDefaultSize() == wxSize(16, 16) );

    CHECK( b.GetBitmap(wxDefaultSize ).GetSize() == wxSize(16, 16) );
    CHECK( b.GetBitmap(wxSize(16, 16)).GetSize() == wxSize(16, 16) );
    CHECK( b.GetBitmap(wxSize(20, 20)).GetSize() == wxSize(20, 20) );
    CHECK( b.GetBitmap(wxSize(24, 24)).GetSize() == wxSize(24, 24) );
}
